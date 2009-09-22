/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GLTEXTURE_H
#define AX_GLTEXTURE_H

namespace Axon { namespace Render {

	class GLtexture : public Texture {
	public:
		GLtexture();
		virtual ~GLtexture();

		GLuint getObject() const;
		GLenum getTarget() const;

		// implement Asset
		virtual bool doInit(const String& name, intptr_t arg);

		// implement Texture
		virtual void initialize(TexFormat format, int width, int height, InitFlags flags = 0);
		virtual void getSize(int& width, int& height, int& depth);
		virtual void getSize(int& width, int& height);
		virtual void setSize(int width, int height, int depth);
		virtual TexFormat getFormat();
		virtual void uploadSubTexture(const Rect& rect, const void* pixels, TexFormat format = TexFormat::AUTO);
#if 0
		virtual void uploadSubTextureImmediately(const Rect& rect, const void* pixels);
#endif
		virtual void setClampMode(ClampMode clampmode);
		virtual void setFilterMode(FilterMode filtermode);
		virtual void saveToFile(const String& filename);
		virtual void generateMipmap();
		virtual void setBorderColor(const Rgba& color);
		virtual void setHardwareShadowMap(bool enable);

		void copyFramebuffer(const Rect& r);
		void setDepthTextureMode(GLenum mode);

		static void initFactory();
		static void finalizeFactory();

	private:
		bool loadFile2D();
		void loadFileCUBE();
		void loadMemory2D();

	private:
		InitFlags m_createFlags;
		String m_name;
		TexFormat m_format;
		GLenum m_glformat;
		GLuint m_object;
		bool m_dataLoaded;		// Is data have loaded?
		int m_width;
		int m_height;
		int m_depth;
		uint_t m_mipmapLevels;
		void* m_compressedData;
		bool m_isLoadFromMemory;
		uint_t m_videoMemoryUsed;
	};


}} // namespace Axon::Render

#endif // AX_GLTEXTURE_H
