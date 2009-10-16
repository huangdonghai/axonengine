/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

// include freetype2 header
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

// FIXME: make windows happy
#undef GetCharWidth
#undef createFont


#define AX_DEFAULT_FONTSIZE 14

namespace {
	using namespace Axon;
	using namespace Axon::Render;

	FT_Library gFT_Library;

	const char* gFontPath = "fonts";
	const char* gFontDefFileExt = ".font";

	const float FONT_SIZE_SCALE = 1.f / 64;
	const int FONT_METRIC_SHIFT_BITS = 6;

	const wchar_t DEFAULT_CHAR = L'?';

//------------------------------------------------------------------------------
// class Manager
//------------------------------------------------------------------------------

	class Manager {
	public:
		Manager();
		~Manager();

		void initialize();
		void finalize();
		BufInfo getFontFileBuf(const String filename);

		FontRp load(const String& name, int w, int h);
		void deleteFont(Font* font);

	private:
		bool m_initialized;
		typedef Dict<String, BufInfo,hash_pathname,equal_pathname> FileBufDict;
		FileBufDict m_fontFileBufs;
		typedef Dict<FixedString, Font*> FontDict;
		FontDict m_fontDict;
	};

	Manager::Manager()
		: m_initialized(false)
	{
		initialize();
	}

	Manager::~Manager()
	{
		finalize();
	}

	void Manager::initialize()
	{
		if (m_initialized)
			Errorf("Manager::initialize: already initialized");

		Printf("..initializing FreeType library...\n");
		FT_Error error = FT_Init_FreeType(&gFT_Library);

		if (error) {
			Errorf("Manager::initialize: cann't initialize freetype2 library");
		}

		m_initialized = true;
	}

	void Manager::finalize()
	{}


	BufInfo Manager::getFontFileBuf(const String name)
	{
		FileBufDict::iterator it;

		it = m_fontFileBufs.find(name);

		if (it != m_fontFileBufs.end())
			return it->second;

		String filename = gFontPath;
		filename += "/" + name;
		void* buf;
		size_t size;

		size = g_fileSystem->readFile(filename, &buf);

		if (!size || !buf) {
			Errorf("Manager::getFontFileBuf: cann't read font file '%s'", filename.c_str());
		}

		BufInfo buf_info;
		buf_info.buf = buf;
		buf_info.size = size;

		m_fontFileBufs[name] = buf_info;

		return buf_info;
	}

	FontRp Manager::load( const String& name, int w, int h )
	{
		FixedString key = Font::normalizeKey(name, w, h);

		FontDict::const_iterator it = m_fontDict.find(key);
		if (it != m_fontDict.end()) {
			it->second->addref();
			return it->second;
		}

		Font* result = new Font();
		result->doInit(name, w, h);

		m_fontDict[key] = result;
		result->setKey(key);

		return result;
	}

	void Manager::deleteFont( Font* font )
	{
		// TODO: wait for delete
	}

	static Manager* s_fontManager = 0;
}

namespace Axon { namespace Render {


	//------------------------------------------------------------------------------
	// class FontFace
	//------------------------------------------------------------------------------

	FontFace::FontFace(const FaceDef* def, int nWidth, int nHeight)
		: m_def(def)
	{
		BufInfo info = s_fontManager->getFontFileBuf(def->filename);

		FT_Error error = FT_New_Memory_Face(gFT_Library,
			(FT_Byte*)info.buf,	
			(FT_Long)info.size,	
			0,					
			&m_ftface);

		if (error == FT_Err_Unknown_File_Format) {
			Errorf("FontFace::FontFace: font file '%s' unknown format", def->filename.c_str());
			return;
		} else if (error) {
			Errorf("FontFace::FontFace: cann't read font file '%s'", def->filename.c_str());
			return;
		}

		FT_Set_Pixel_Sizes(m_ftface, def->scale.x * (float)nWidth, def->scale.y * (float)nHeight);

		m_faceInfo.width = m_ftface->size->metrics.x_ppem;
		m_faceInfo.height = m_ftface->size->metrics.y_ppem;
		m_faceInfo.maxWidth = m_ftface->size->metrics.max_advance >> FONT_METRIC_SHIFT_BITS;
		m_faceInfo.maxHeight = m_ftface->size->metrics.height >> FONT_METRIC_SHIFT_BITS;
		m_faceInfo.ascender = m_ftface->size->metrics.ascender >> FONT_METRIC_SHIFT_BITS;
		m_faceInfo.descender = m_ftface->size->metrics.descender >> FONT_METRIC_SHIFT_BITS;
		m_faceInfo.viewHeight = 0;
		m_faceInfo.heightAdjust = 0;

		if (m_def->antialias) {
			m_loadflags = FT_LOAD_NO_BITMAP;
			m_loadflagsWithRender = FT_LOAD_NO_BITMAP | FT_LOAD_RENDER;
		} else {
			m_loadflags = FT_LOAD_DEFAULT | FT_LOAD_TARGET_MONO;
			m_loadflagsWithRender = m_loadflags | FT_LOAD_RENDER;
		}

	}

	FontFace::~FontFace() {
	}

	FaceInfo FontFace::getFaceInfo() {
		return m_faceInfo;
	}

	void FontFace::setViewHeight(int view_height, int maxAscender) {
		m_faceInfo.viewHeight = view_height;
//		m_faceInfo.heightAdjust =(view_height - m_faceInfo.height) >> 1;
		m_faceInfo.heightAdjust = maxAscender - m_faceInfo.ascender;
	}

	bool FontFace::getGlyphInfo(wchar_t ch, GlyphInfo& ginfo) {
		FT_UInt glyph_index = 0;

		glyph_index = FT_Get_Char_Index(m_ftface, ch);

		if (!glyph_index)
			return false;

		FT_Error error = FT_Load_Glyph(m_ftface, glyph_index, m_loadflags);

		if (error)
			return false;

		ginfo.advance = m_ftface->glyph->metrics.horiAdvance >> FONT_METRIC_SHIFT_BITS;
		ginfo.width = m_ftface->glyph->metrics.width >> FONT_METRIC_SHIFT_BITS;
		ginfo.bearing = m_ftface->glyph->metrics.horiBearingX >> FONT_METRIC_SHIFT_BITS;

		return true;
	}

	bool FontFace::getCharBitmap(wchar_t ch, int width, int height, byte_t* data) {
		FT_UInt glyph_index = 0;

		glyph_index = FT_Get_Char_Index(m_ftface, ch);

		if (!glyph_index)
			return false;

		FT_Error error = FT_Load_Glyph(m_ftface, glyph_index, m_loadflagsWithRender);

		if (error)
			return false;

		if (m_ftface->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
			if (m_def->antialias) {
				error = FT_Render_Glyph(m_ftface->glyph, FT_RENDER_MODE_NORMAL);
			} else {
				error = FT_Render_Glyph(m_ftface->glyph, FT_RENDER_MODE_MONO);
			}

			if (error)
				return false;
		}

		int h_start =(m_ftface->glyph->metrics.height - m_ftface->glyph->metrics.horiBearingY) >> FONT_METRIC_SHIFT_BITS;
		int w_start;
		memset(data, 0, width * height);

		int h, w, wr, hr;
		FT_Bitmap& bitmap = m_ftface->glyph->bitmap;
		w_start = m_ftface->glyph->bitmap_left;
		w_start = Font::ATLAS_PAD;
		h_start = m_faceInfo.height - m_ftface->glyph->bitmap_top + m_faceInfo.heightAdjust + Font::ATLAS_PAD;

		if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
			for (h = 0, hr = h_start; h < bitmap.rows; h++, hr++) {
				if (hr >= height)
					break;

				for (w = 0, wr = w_start; w < bitmap.width; w++, wr++) {
					if (wr >= width)
						break;
					byte_t b = bitmap.buffer[h*bitmap.pitch+w/8];
					data[hr*width+wr] =(b & (1<<(7-w%8))) ? 255 : 0;
				}
			}
		} else if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY){
			for (h = 0, hr = h_start; h < bitmap.rows; h++, hr++) {
				if (hr >= height)
					break;

				for (w = 0, wr = w_start; w < bitmap.width; w++, wr++) {
					if (wr >= width)
						break;
					data[hr*width+wr] = bitmap.buffer[h*bitmap.pitch+w];
				}
			}
		} else {
			return false;
		}

		return true;
	}


	//------------------------------------------------------------------------------
	// class Font
	//------------------------------------------------------------------------------

	Font::~Font()
	{}

	bool Font::doInit(const String& name, int w, int h)
	{
		m_name = name;
#if 0
		if (!File::havePath(name))
			m_name = "fonts/" + name;
		parseSize(arg, m_width, m_height);
#else
		m_name = name;
		m_width = w;
		m_height = h;
#endif

		if (m_width == 0) m_width = AX_DEFAULT_FONTSIZE;
		if (m_height == 0) m_height = AX_DEFAULT_FONTSIZE;

		if (!parseFontDef())
			return false;

		// init cached glyph size to 0
		::memset(m_glyphInfos, 0xFF, sizeof(m_glyphInfos));

		size_t i;
		int maxWidth = 0;
		int maxHeight = 0;
		int maxViewHeight = 0;
		int maxAscender = 0;
		for (i=0; i<m_faceDefs.size(); i++) {
			FontFace* face = new FontFace(&m_faceDefs[i], m_width, m_height);
			m_fontFaces.push_back(face);

			FaceInfo faceinfo = face->getFaceInfo();
			if (faceinfo.maxWidth > maxWidth)
				maxWidth = faceinfo.maxWidth;
			if (faceinfo.maxHeight > maxHeight)
				maxHeight = faceinfo.maxHeight;
			if (faceinfo.height > maxViewHeight)
				maxViewHeight = faceinfo.height;

			maxAscender = std::max(maxAscender, faceinfo.ascender);
		}

		m_width = maxWidth + ATLAS_PAD * 2;
		m_height = maxHeight + ATLAS_PAD * 2;

		for (i=0; i<m_fontFaces.size(); i++) {
			m_fontFaces[i]->setViewHeight(maxViewHeight, maxAscender);
		}

		// init default char
		m_getCharWidth(DEFAULT_CHAR);
		if (m_glyphInfos[DEFAULT_CHAR].advance == 0xFF) {
			// not find default char, fire error
			Errorf("Font::doInit: no DEFAULT_CHAR '%c' glyph", DEFAULT_CHAR);
		}

		// init texture pool
		m_texPool = new TextureAtlas;
		m_texPool->initialize(m_name, m_width, m_height, TEXTURE_SIZE, TexFormat::A8);
		m_texPool->setTextureFilterMode(Texture::FM_Linear);

		// test
		newFrame();

		// set key
		StringUtil::sprintf(m_key, "%s_%d_%d", m_name.c_str(), m_width, m_height);

		return true;
	}

	bool Font::parseFontDef()
	{
		String fname = m_name + ".font";
		char* fbuf;
		size_t fsize;

		fsize = g_fileSystem->readFile(fname, (void**)&fbuf);
		if (!fsize || !fbuf)
			return false;

		TiXmlDocument doc;

		doc.Parse(fbuf, NULL, TIXML_ENCODING_UTF8);
		g_fileSystem->freeFile(fbuf);

		if (doc.Error()) {
			Debugf(_("%s: error parse %s in line %d - %s"), __func__
				, m_name.c_str()
				, doc.ErrorRow()
				, doc.ErrorDesc());
			return false;
		}

		const TiXmlElement* root = doc.FirstChildElement("font");

		// no root
		if (!root)
			return false;

		const TiXmlElement* elem = nullptr;
		for (elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
			if (elem->ValueTStr() != "face")
				continue;

			const char* file = elem->Attribute("file");
			const char* aa = elem->Attribute("antialias");
			const char* scale = elem->Attribute("scale");
			if (!file || !aa) return false;

			FaceDef def;
			def.filename = file;
			def.antialias = true;

			if (aa) {
				def.antialias = atoi(aa) != 0;
			}

			if (!scale) return false;

			int v = sscanf(scale, "%f %f", &def.scale.x, &def.scale.y);

			if (v != 2)
				return false;

			m_faceDefs.push_back(def);
		}

		return true;
	}

	String Font::getName()
	{
		return m_name;
	}

	uint_t Font::getStringWidth(const WString& string)
	{
		uint_t width = 0;

		size_t i;
		for (i = 0; i < string.size(); i++) {
			width += m_getCharWidth(string[i]);
		}
		return width;
	}

	uint_t Font::getWidth()
	{
		return m_width;
	}

	uint_t Font::getHeight()
	{
		return m_height;
	}

	#define BLINK_DIVISOR 75

	uint_t Font::m_getCharWidth(wchar_t ch)
	{
		GlyphInfo& ginfo = m_glyphInfos[ch];

		// if have cached, return
		if (ginfo.advance != 0xFF)
			return ginfo.advance;

		// cached glyph info
		size_t i;
		for (i = 0; i < m_fontFaces.size(); i++) {
			bool has = m_fontFaces[i]->getGlyphInfo(ch, ginfo);
			if (has) {
				ginfo.face = (byte_t)i;
				break;
			}
		}

		// not found, set to default
		if (ginfo.advance == 0xFF) {
			ginfo = m_glyphInfos[DEFAULT_CHAR];
			ginfo.defaulted = true;
		} else {
			ginfo.defaulted = false;
		}

		return ginfo.advance;
	}

	size_t Font::updateTexture(const wchar_t* str)
	{
		size_t i;
		wchar_t c;
		byte_t* data = (byte_t*)Alloca(m_width*m_height);

		m_texPool->newFrame();

		for (i=0; *str; str++, i++) {
			c = *str;

			if (!m_texPool->isChunkResident(c)) {
				if (!uploadCharGlyph(c, data)) {
					i++;		// return right len
					break;
				}
			}
		}

		return i;
	}

	void Font::getCharInfo(int id, Texture*& tex, Vector4& tc)
	{
		return m_texPool->getChunkInfo(id, tex, tc);
	}

	const GlyphInfo& Font::getGlyphInfo(wchar_t c)
	{
		return m_glyphInfos[c];
	}

	bool Font::uploadCharGlyph(wchar_t c, byte_t* data)
	{
#if  1
		if (m_glyphInfos[c].advance == 0xFF) {
			m_getCharWidth(c);
		}

		if (m_glyphInfos[c].face == 0xFF)
			return true;
#endif
		if (m_glyphInfos[c].defaulted) {
			m_fontFaces[m_glyphInfos[c].face]->getCharBitmap(DEFAULT_CHAR, m_width, m_height, data);
		} else {
			m_fontFaces[m_glyphInfos[c].face]->getCharBitmap(c, m_width, m_height, data);
		}

		return m_texPool->updateChunk(c, data);
	}

	void Font::newFrame()
	{
		m_texPool->newFrame();
	}

	FontRp Font::load( const String& name, int w, int h )
	{
		return s_fontManager->load(name, w, h);
	}

	void Font::initManager()
	{
		s_fontManager = new Manager();
	}

	void Font::finalizeManager()
	{
		SafeDelete(s_fontManager);
	}

	void Font::deleteThis()
	{
		s_fontManager->deleteFont(this);
	}

	FixedString Font::normalizeKey( const String& name, int w, int h )
	{
		std::stringstream ss;
		ss << name << "_" << w << "x" << h;
		FixedString key = ss.str();
		return key;
	}

}} // namespace Axon::Render


