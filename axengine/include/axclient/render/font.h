/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_FONT_H
#define AX_RENDER_FONT_H

struct FT_FaceRec_;
typedef struct FT_FaceRec_* FT_Face;

namespace Axon { namespace Render {

	struct DrawText;

	struct GlyphInfo {
		byte_t advance;
		byte_t width;
		sbyte_t bearing;
		byte_t face;
		byte_t defaulted;	// not found glyph for this char, so defaulted
	};

	struct TextQuad {
		Vector3 origin;
		Vector3 s_vector;
		Vector3 t_vector;
		float width;
		float height;

		Vector3 getPos(float s, float t, const Vector2& scale) const {
			return origin + s_vector * s * scale.x + t_vector * t * scale.y;
		}
	};

	//------------------------------------------------------------------------------
	// class FontFace
	//------------------------------------------------------------------------------

	struct FaceDef {
		String filename;
		bool antialias;
		Vector2 scale;
	};
	typedef Sequence<FaceDef>	FaceDefs;

	struct FaceInfo {
		int width;
		int height;
		int maxWidth;
		int maxHeight;
		int ascender;
		int descender;
		int viewHeight;
		int heightAdjust;
	};

	class FontFace {
	public:
		FontFace(const FaceDef* def, int nWidth, int nHeight);
		~FontFace();

		FaceInfo getFaceInfo();
		void setViewHeight(int view_height, int maxAscender);
		bool getGlyphInfo(wchar_t ch, GlyphInfo& ginfo);
		bool getCharBitmap(wchar_t ch, int width, int height, byte_t* data);

	private:
		const FaceDef* m_def;
		FT_Face m_ftface;
		FaceInfo m_faceInfo;
		int m_loadflags;
		int m_loadflagsWithRender;
	};

	//------------------------------------------------------------------------------
	// class Font
	//------------------------------------------------------------------------------

	struct BufInfo {
		void* buf;
		size_t size;
	};

	AX_DECLARE_REFPTR(Font);

	class AX_API Font : public RefObject {
	public:
		enum {
			MAX_CHARS = 65536,
			ATLAS_PAD = 2,
			TEXTURE_SIZE = 512
		};

		static inline intptr_t makeSize(int w, int h) {
			AX_ASSERT(w < std::numeric_limits<short>::max());
			AX_ASSERT(h < std::numeric_limits<short>::max());
			return (w << 16) + h;
		}

		static inline void parseSize(intptr_t param, int& w, int& h) {
			w = (param >> 16) & 0xffff;
			h = param & 0xffff;
		}

		Font() {}
		~Font();

		// implement Asset
		virtual bool doInit(const String& name, intptr_t arg);

		String getName();
		uint_t getStringWidth(const WString& string);
		uint_t getWidth();
		uint_t getHeight();
		void newFrame();
		size_t updateTexture(const wchar_t* str);
		void getCharInfo(int id, Texture*& tex, Vector4& tc);
		const GlyphInfo& getGlyphInfo(wchar_t c);

		// management
		static FontPtr load(const String& name, int w, int h);
		static void initManager();
		static void finalizeManager();

	protected:
		bool parseFontDef();
		uint_t m_getCharWidth(wchar_t ch);
		Vector2 drawString(Rgba color, Vector2 xy, const wchar_t* str, size_t len, Vector2 scale, bool italic = false);
		bool uploadCharGlyph(wchar_t c, byte_t* data);

	private:
		String m_name;
		String m_key;
		FaceDefs m_faceDefs;
		Sequence<FontFace*>	m_fontFaces;
		TextureAtlas* m_texPool;
		GlyphInfo m_glyphInfos[MAX_CHARS];
		int m_width, m_height;
	};



}} // namespace Axon::Render

#endif // AX_RENDER_FONT_H
