/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

static TextureManager *s_textureManager;

//--------------------------------------------------------------------------
// class Texture
//--------------------------------------------------------------------------

Texture::Texture()
{
//	m_needFreeLink.setOwner(this);
//	m_needGenMipmapLink.setOwner(this);
}

Texture::~Texture()
{}

void Texture::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	if (g_renderDriver->isInRenderingThread()) {
		uploadSubTextureIm(rect, pixels, format);
		return;
	}

	s_textureManager->uploadSubTexture(this, rect, pixels, format);
}

void Texture::generateMipmap()
{
	if (g_renderDriver->isInRenderingThread()) {
		generateMipmapIm();
		return;
	}
	s_textureManager->generateMipmap(this);
}

void Texture::deleteThis()
{
	s_textureManager->freeTexture(this);
}

TexturePtr Texture::load(const String &name, InitFlags flags/*=0*/)
{
	return s_textureManager->loadTexture(name, flags);
}

TexturePtr Texture::create( const String &debugname, TexFormat format, int width, int height, InitFlags flags /*= 0*/ )
{
	return s_textureManager->createTexture(debugname, format, width, height, flags);
}

bool Texture::isExist(const String &name)
{
	return s_textureManager->isExist(name);
}

void Texture::initManager()
{ /* do nothing */ }

void Texture::finalizeManager()
{ /* do nothing */ }

FixedString Texture::normalizeKey(const String &name)
{
	FixedString key;

	if (!PathUtil::haveDir(name))
		key = "textures/" + name;
	else
		key = name;

	key = PathUtil::removeExt(key);

	return key;
}

void Texture::texlist_f( const CmdArgs &args )
{
	s_textureManager->texlist_f(args);
}

//--------------------------------------------------------------------------
// class TextureManager
//--------------------------------------------------------------------------

TextureManager::TextureManager()
{
	if (s_textureManager) {
		Errorf("TextureManager already instanced");
		return;
	}

	s_textureManager = this;
}

TextureManager::~TextureManager()
{
	s_textureManager = 0;
}

TexturePtr TextureManager::loadTexture(const String &texname, Texture::InitFlags flags/*=0*/)
{
	FixedString key = Texture::normalizeKey(texname);

	// find if already loaded
	TextureDict::const_iterator it = m_textureDict.find(key);
	if (it != m_textureDict.end()) {
		Texture *result = it->second;
#if 0
		result->m_needFreeLink.removeFromList();
#else
		if (result->m_needFreeLink.isLinked())
			m_needFreeHead.erase(result);
#endif
		result->incref();
		return result;
	}

	if (!isExist(key))
		return 0;

	// create a new texture object
	TexturePtr tex = createObject();

	if (g_renderDriver->isInRenderingThread()) {
		tex->initialize(key, flags);
	} else {
		// pending to render thread
		LoadCmd cmd;
		cmd.texture = tex.get();
		cmd.texName = key;
		cmd.initFlags = flags;
		cmd.format = 0;
		cmd.width = 0;
		cmd.height = 0;

		m_loadCmdList.push_back(cmd);
	}

	// add to hash table
	m_textureDict[key] = tex.get();
	tex->m_key = key;

	return tex;
}

TexturePtr TextureManager::createTexture(const String &debugname, TexFormat format, int width, int height, Texture::InitFlags flags /*= 0*/)
{
	std::stringstream ss;
	ss << "_" << debugname << "$" << g_system->generateId();

	FixedString key = Texture::normalizeKey(ss.str());

	// create object
	TexturePtr tex = createObject();

	if (g_renderDriver->isInRenderingThread()) {
		// init immedially
		tex->initialize(format, width, height, flags);
	} else {
		// pending to render thread
		LoadCmd cmd;
		cmd.texture = tex.get();
		cmd.initFlags = flags;
		cmd.format = format;
		cmd.width = width;
		cmd.height = height;

		m_loadCmdList.push_back(cmd);
	}

	m_textureDict[key] = tex.get();
	tex->m_key = key;

	return tex;
}

bool TextureManager::isExist(const FixedString &key)
{
	ExistDict::const_iterator it = m_existDict.find(key);
	if (it != m_existDict.end())
		return it->second;

	String filename = key.toString() + ".dds";

	bool result = g_fileSystem->isFileExist(filename);
	m_existDict[key] = result;

	return result;
}

void TextureManager::uploadSubTexture(Texture *tex, const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	// calculate pixel size
	if (format == TexFormat::AUTO) {
		format = tex->getFormat();
	}

	if (format == TexFormat::AUTO) {
		Errorf("unknown texture format");
		return;
	}

	size_t size = format.calculateDataSize(rect.width, rect.height);

	UploadCmd cmd;

	cmd.texture = tex;
	cmd.rect = rect;
	cmd.format = format;

	// clone pixel
	void *clonedPixels = g_renderQueue->allocType<byte_t>(size);
	memcpy(clonedPixels, pixels, size);
	cmd.pixel = clonedPixels;

	m_uploadCmdList.push_back(cmd);
}

void TextureManager::generateMipmap(Texture *tex)
{
#if 0
	tex->m_needGenMipmapLink.addToEnd(m_needGenMipmapHead);
#else
	m_needGenMipmapHead.push_back(tex);
#endif
}

void TextureManager::freeTexture(Texture *tex)
{
#if 0
	tex->m_needFreeLink.addToEnd(m_needFreeHead);
#else
	m_needFreeHead.push_back(tex);
#endif
}

void TextureManager::texlist_f(const CmdArgs &args)
{
	Printf("List texture(s):\n");

	int count = 0;
	TextureDict::const_iterator it = m_textureDict.begin();
	for (; it != m_textureDict.end(); ++it) {
		Texture *tex = it->second;
		if (!tex) {
			continue;
		}

		int width, height, depth;
		tex->getSize(width, height, depth);
		Printf("%4d %4d %4d %8s %s\n",tex->getref(), width, height, tex->getFormat().toString(), tex->m_key.c_str());
		count++;
	}

	Printf("total %d texture(s)\n", count);
}

//---------------------------------------------------------------------------

Dict<FixedString, HardwareTexture*> HardwareTexture::ms_texDict;
List<HardwareTexture*> HardwareTexture::ms_asioList;

HardwareTexture::HardwareTexture(const FixedString &key)
{
	setKey(key);

	ms_texDict[key] = this;

	m_asioRead.filename = key;
	m_asioRead.syncCounter.incref();
	g_fileSystem->queAsioRead(&m_asioRead);

	ms_asioList.push_back(this);
}


HardwareTexture::HardwareTexture( const FixedString &key, TexFormat format, int width, int height )
{
	setKey(key);

	ms_texDict[key] = this;

	m_handle = g_renderApi->createTexture2D(format, width, height, Texture2::IF_NoMipmap);
}

HardwareTexture::~HardwareTexture()
{
	ms_texDict.erase(getKey());

	if (m_handle)
		g_renderApi->deleteTexture2D(m_handle);
}

void HardwareTexture::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format)
{
	if (!m_handle) return;

	g_renderApi->uploadSubTexture(m_handle, rect, pixels, format);
}

void HardwareTexture::generateMipmap()
{
	if (!m_handle) return;

	g_renderApi->generateMipmap(m_handle);
}

FixedString HardwareTexture::normalizeKey( const String &name )
{
	String key;

	if (!PathUtil::haveDir(name))
		key = "textures/" + name;
	else
		key = name;

	key = PathUtil::removeExt(key);

	return key;
}

HardwareTexturePtr HardwareTexture::findTexture(const FixedString &key)
{
	Dict<FixedString, HardwareTexture*>::const_iterator it = ms_texDict.find(key);

	if (it != ms_texDict.end())
		return it->second;

	return 0;
}

HardwareTexturePtr HardwareTexture::createTexture(const String &debugname, TexFormat format, int width, int height)
{
	std::stringstream ss;
	ss << "_" << debugname << "$" << g_system->generateId();

	FixedString key = ss.str();

	HardwareTexture *result = new HardwareTexture(key, format, width, height);

	return result;
}

TextureBackend::TextureBackend(TextureData *data) : RenderBackend(data), m_data(data)
{}

TextureBackend::~TextureBackend()
{}

void TextureBackend::sync()
{
	m_samplerState = m_data->m_samplerState;
}

void TextureBackend::init(const String &name)
{
	m_hardwareRes = HardwareTexture::findTexture(name);
}

void TextureBackend::initUnique(const String &debugname, TexFormat format, int width, int height)
{
	m_hardwareRes = HardwareTexture::createTexture(debugname, format, width, height);
}

void TextureBackend::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format)
{
	m_hardwareRes->uploadSubTexture(rect, pixels, format);
}

void TextureBackend::generateMipmap()
{
	m_hardwareRes->generateMipmap();
}

TextureData::TextureData()
{
}

TextureData::TextureData(const String &name)
{
	m_backend = new TextureBackend(this);

	queueCmd1(m_backend, &TextureBackend::init).call(name);
}

TextureData::TextureData(const String &debugname, TexFormat format, int width, int height)
{
	m_backend = new TextureBackend(this);

	queueCmd4(m_backend, &TextureBackend::initUnique).call(debugname, format, width, height);
}

TextureData::~TextureData()
{

}

void TextureData::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	if (!m_backend) return;

	if (rect.isEmpty())
		return;

	if (format == TexFormat::AUTO) {
		Errorf("unknown texture format");
		return;
	}

	size_t size = format.calculateDataSize(rect.width, rect.height);
	// clone pixel
	void *clonedPixels = g_renderQueue->allocType<byte_t>(size);
	memcpy(clonedPixels, pixels, size);

	queueCmd3(m_backend, &TextureBackend::uploadSubTexture).call(rect, clonedPixels, format);
}

void TextureData::generateMipmap()
{
	queueCmd0(m_backend, &TextureBackend::generateMipmap).call();
}

void TextureData::setClampMode(SamplerState::ClampMode clampmode)
{
	m_samplerState.clampMode = clampmode;
	checkedSync();
}

void TextureData::setFilterMode(SamplerState::FilterMode filtermode)
{
	m_samplerState.filterMode = filtermode;
	checkedSync();
}

void TextureData::setBorderColor(SamplerState::BorderColor bordercolor)
{
	m_samplerState.borderMode = bordercolor;
	checkedSync();
}


Texture2::Texture2()
{}

Texture2::Texture2(const String &name)
{
	m_data = new TextureData(name);
}

Texture2::Texture2(const String &debugname, TexFormat format, int width, int height)
{
	m_data = new TextureData(debugname, format, width, height);
}

Texture2::~Texture2()
{}

void Texture2::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	if (!m_data) return;
	m_data->uploadSubTexture(rect, pixels, format);
}

void Texture2::generateMipmap()
{
	if (!m_data) return;
	m_data->generateMipmap();
}

void Texture2::setClampMode(SamplerState::ClampMode clampmode)
{
	if (!m_data) return;
	m_data->setClampMode(clampmode);
}

void Texture2::setFilterMode(SamplerState::FilterMode filtermode)
{
	if (!m_data) return;
	m_data->setFilterMode(filtermode);
}

void Texture2::setBorderColor(SamplerState::BorderColor bordercolor)
{
	if (!m_data) return;
	m_data->setBorderColor(bordercolor);
}

AX_END_NAMESPACE

