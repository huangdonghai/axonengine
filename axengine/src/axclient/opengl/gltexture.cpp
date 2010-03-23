/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE
	
	inline GLenum trTexType(Texture::TexType type) {
		switch (type) {
		case Texture::TT_2D:
			return GL_TEXTURE_2D;
		case Texture::TT_3D:
			return GL_TEXTURE_3D;
		case Texture::TT_CUBE:
			return GL_TEXTURE_CUBE_MAP;
		default:
			Errorf("trTexType: bad enum");
			return GL_TEXTURE_2D;
		}
	}

	inline void trTexFormat(TexFormat tex_format, GLenum &dataformat, GLenum &datatype, GLenum &internal_format) {
		switch (tex_format) {
		case TexFormat::L8:
			dataformat = GL_LUMINANCE;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_LUMINANCE8;
			break;

		case TexFormat::LA8:
			dataformat = GL_LUMINANCE_ALPHA;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_LUMINANCE8_ALPHA8;
			break;

		case TexFormat::A8:
			dataformat = GL_ALPHA;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_ALPHA4;
			break;

		case TexFormat::BGR8:
			dataformat = GL_BGR;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_RGB8;
			break;

		case TexFormat::BGRA8:
			dataformat = GL_BGRA;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_RGBA8;
			break;

		case TexFormat::BGRX8:
			dataformat = GL_BGRA;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_RGBA8;
			break;
		case TexFormat::DXT1:
			dataformat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;

		case TexFormat::DXT3:
			dataformat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;

		case TexFormat::DXT5:
			dataformat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			datatype = GL_UNSIGNED_BYTE;
			internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;

		case TexFormat::L16:
			dataformat = GL_LUMINANCE;
			datatype = GL_UNSIGNED_SHORT;
			internal_format = GL_LUMINANCE16;
			break;


		// 16 bits float texture
		case TexFormat::R16F:
			dataformat = GL_RED;
			datatype = GL_HALF_FLOAT_ARB;
			internal_format = GL_R16F;
			break;

		case TexFormat::RG16F:
			dataformat = GL_RG;
			datatype = GL_HALF_FLOAT_ARB;
			internal_format = GL_RG16F;
			break;

		case TexFormat::RGB16F:
			dataformat = GL_RGB;
			datatype = GL_HALF_FLOAT_ARB;
			internal_format = GL_RGB16F;
			break;

		case TexFormat::RGBA16F:
			dataformat = GL_RGBA;
			datatype = GL_HALF_FLOAT_ARB;
			internal_format = GL_RGBA16F_ARB;
			break;


		// 32 bits float texture
		case TexFormat::R32F:
			dataformat = GL_RED;
			datatype = GL_FLOAT;
			internal_format = GL_R32F;
			break;

		case TexFormat::RG32F:
			dataformat = GL_RG;
			datatype = GL_FLOAT;
			internal_format = GL_RG32F;
			break;

		case TexFormat::RGB32F:
			dataformat = GL_RGB;
			datatype = GL_FLOAT;
			internal_format = GL_RGB32F;
			break;

		case TexFormat::RGBA32F:
			dataformat = GL_RGBA;
			datatype = GL_FLOAT;
			internal_format = GL_RGBA32F_ARB;
			break;

		case TexFormat::D16:
			dataformat = GL_DEPTH_COMPONENT;
			datatype = GL_UNSIGNED_SHORT;
			internal_format = GL_DEPTH_COMPONENT16;
			break;

		case TexFormat::D24:
			dataformat = GL_DEPTH_COMPONENT;
			datatype = GL_UNSIGNED_INT;
			internal_format = GL_DEPTH_COMPONENT24;
			break;

		case TexFormat::D32:
			dataformat = GL_DEPTH_COMPONENT;
			datatype = GL_UNSIGNED_INT;
			internal_format = GL_DEPTH_COMPONENT32;
			break;

		case TexFormat::D24S8:
			dataformat = GL_DEPTH_STENCIL;
			datatype = GL_UNSIGNED_INT_24_8_EXT;
			internal_format = GL_DEPTH24_STENCIL8_EXT;
			break;


		default:
			dataformat = 0;
			datatype = 0;
			internal_format = 0;
			//Errorf("bad enum");
		}
	}

	inline TexFormat trInternalFormat(GLenum internalformat) {
		switch (internalformat) {
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return TexFormat::DXT1;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			return TexFormat::DXT3;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return TexFormat::DXT5;
		default:
			return TexFormat::BGRA8;
		}
	}

	inline GLenum trTexClamp(Texture::ClampMode clamp) {
		switch (clamp) {
		default:
		case Texture::CM_Repeat:
			return GL_REPEAT;
		case Texture::CM_Clamp:
			return GL_CLAMP; 
		case Texture::CM_ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		case Texture::CM_ClampToBorder:
			return GL_CLAMP_TO_BORDER;
		}
	}

	inline void trTexFilter(Texture::FilterMode filter, GLenum &min_filter, GLenum &mag_filter) {
		switch (filter) {
		case Texture::FM_Nearest:
			min_filter = GL_NEAREST;
			mag_filter = GL_NEAREST;
			return;
		case Texture::FM_Linear:
			min_filter = GL_LINEAR;
			mag_filter = GL_LINEAR;
			return;
		case Texture::FM_Bilinear:
			min_filter = GL_LINEAR_MIPMAP_NEAREST;
			mag_filter = GL_LINEAR;
			return;
		case Texture::FM_Trilinear:
			min_filter = GL_LINEAR_MIPMAP_LINEAR;
			mag_filter = GL_LINEAR;
			return;
		}

		Errorf("trTexFilter: error enum");
	}


	inline float getBppForInternalFormat(GLenum format) {
		switch (format) {
		case GL_LUMINANCE8:
			return 1;
		case GL_LUMINANCE8_ALPHA8:
			return 2;
		case GL_RGB8:
			return 3;
		case GL_RGBA8:
			return 4;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return .5f;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return 1;
		default:
			Errorf("getBppForInternalFormat: bad enum");
			return 0;
		}
	}

AX_END_NAMESPACE

AX_BEGIN_NAMESPACE

	/*!
		\class GLtexture
	*/

	GLtexture::GLtexture()
		: m_object(0)
	{
		m_format = TexFormat::AUTO;
	}


	GLtexture::~GLtexture()
	{
		if (!m_object)
			return;

		glDeleteTextures(1, &m_object);

#if 0
		g_statistic->decValue(stat_numTextures);
		g_statistic->subValue(stat_textureMemory, m_videoMemoryUsed);
#else
		stat_numTextures.dec();
		stat_textureMemory.sub(m_videoMemoryUsed);
#endif
	}

	bool GLtexture::doInit(const String &name, intptr_t arg) {
		if (!PathUtil::haveDir(name))
			m_name = "textures/" + name;
		else
			m_name = name;

		m_createFlags = arg;
		return loadFile2D();
	}

	bool GLtexture::loadFile2D() {
		GLrender::checkErrors();

		const byte_t *data;
		bool isMipmap;
		int flags = g_renderDriver->getDriverInfo()->caps & IRenderDriver::Info::DXT ? 0 : Image::NoCompressed;

		if (!(m_createFlags & Texture::IF_NoMipmap))
			flags |= Image::Mipmap;

		std::auto_ptr<Image> imagefile(new Image);
		if (!imagefile->loadFile(m_name, flags)) {
			Debugf("GLtexture::loadFile2D: cann't find image file for %s\n", m_name.c_str());
			return false;
		}

		m_width = imagefile->getWidth();
		m_height = imagefile->getHeight();
//		mDesc.format = imagefile->getFormat();
		if (!Math::isPowerOfTwo(m_width) || !Math::isPowerOfTwo(m_height)) {
//			if (!(mDesc.flags & TexFlag_allowNPOT))
				Errorf("GLtexture::loadFile2D: texture %s size isn't power of two", m_name.c_str());
//			else
//				Debugf("GLtexture::loadFile2D: texture %s size isn't power of two\n", mDesc.name.c_str());
		}

		if (m_object == 0)
			glGenTextures(1, &m_object);

		GLenum format, datatype, internalformat;

		m_format = imagefile->getFormat();

		trTexFormat(imagefile->getFormat(), format, datatype, internalformat);

		if (!format) {
			Errorf("OpenGL don't support texture format '%s'", m_format.toString());
		}

		glBindTexture(GL_TEXTURE_2D, m_object);

		if (m_createFlags & IF_NoMipmap || imagefile->getNumMipmapLevels() <= 1)
			isMipmap = false;
		else
			isMipmap = true;

		m_videoMemoryUsed = 0;
		if (!isMipmap) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		int width, height;
		width = m_width;
		height = m_height;
		for (int i = 0; i < imagefile->getNumMipmapLevels(); i++) {
			// no mipmap?
			if (i > 0 && !isMipmap)
				break;

			data = imagefile->getData(i);

			uint_t datasize = imagefile->getFormat().calculateDataSize(width, height);
			m_videoMemoryUsed += datasize;

			if (imagefile->getFormat().isDXTC()) {
				glCompressedTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, datasize, data);
				GLrender::checkErrors();
			} else {
				glTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, format, datatype, data);
			}

			width >>= 1;
			height >>= 1;
			if (width < 1) width = 1;
			if (height < 1) height = 1;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		GLrender::checkErrors();
#if 0
		g_statistic->addValue(stat_textureMemory, m_videoMemoryUsed);
		g_statistic->incValue(stat_numTextures);
#else
		stat_textureMemory.add(m_videoMemoryUsed);
		stat_numTextures.inc();
#endif
		return true;
	}

#if 0
	static char *__cubemapSuffix[] = { "_px", "_nx", "_py", "_ny", "_pz", "_nz" };

	void GLTexture::LoadFileCUBE() {
		if (mResourceId == 0)
			glGenTextures(1, &mResourceId);

		gMainState->BindTexture(0, this);

		if (mDesc.flags & TexFlag_nomipmap) {
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// auto generate mipmap
			glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		}

		//	GLenum clamp = __TexClampMode(mDesc.clampMode);
		GLenum clamp = GL_CLAMP_TO_EDGE;
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, clamp);

		String filename;
		const byte_t *data;
		int i;
		Image imagefile;
		GLenum format, datatype, internalformat;
		Bitfields loadflags = 0;

		if (mDesc.flags & TexFlag_loadRGBERaw)
			loadflags = Image::RgbeRaw;
		else
			loadflags = Image::RgbeFloat;

		loadflags |= Image::NoCompressed;

		// check if have a combined cross image
		if (imagefile.loadFile(mDesc.name + "_cube", loadflags)) {
			struct cubeoffset {
				int x;
				int y;
				int rotate;
				bool flipS;
				bool flipT;
			} cubeoffsets[] = {
				{ 1, 1, 270, false, true },
				{ 1, 3, 270, false, true },
				{ 0, 1, 180, true, false },
				{ 2, 1, 0, true, false },
				{ 1, 0, 270, false, true },
				{ 1, 2, 270, false, true },
			};

			mWidth = imagefile.getWidth();
			mHeight = imagefile.getHeight();
			mDesc.format = imagefile.getFormat();

			mWidth /= 3;
			mHeight /= 4;

			if (mWidth != __GetPOT(mWidth) || mHeight != __GetPOT(mHeight)) {
				if (!(mDesc.flags & TexFlag_allowNPOT))
					Errorf("GLTexture::LoadFileCUBE: texture %s size isn't power of two", mDesc.name.c_str());
				else
					Debugf("GLTexture::LoadFileCUBE: texture %s size isn't power of two\n", mDesc.name.c_str());
			}

			AX_ASSERT(mWidth == mHeight);

			mVideoMemoryUsed = getImageSize(imagefile.getFormat(), mWidth, mHeight) * 6;

			byte_t *tempdata = (byte_t*)malloc(getImageSize(imagefile.getFormat(), mWidth, mHeight));

			Rect rect;
			rect.width = mWidth; rect.height = mHeight;
			__TexFormat(imagefile.getFormat(), format, datatype, internalformat);
			for (i = 0; i < 6; i++) {
				rect.x = cubeoffsets[i].x * mWidth;
				rect.y = cubeoffsets[i].y * mHeight;
				imagefile.writeDataTo(0, rect, cubeoffsets[i].rotate, tempdata, cubeoffsets[i].flipS, cubeoffsets[i].flipT);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, internalformat, mWidth, mHeight, 0, format, datatype, tempdata);
			}

			SafeFree(tempdata);
		} else {
			for (i = 0; i < 6; i++) {
				if (!imagefile.loadFile(mDesc.name + __cubemapSuffix[i])) {
					Errorf("GLTexture::LoadFileCUBE: cann't find image file for %s\n", mDesc.name.c_str());
					return;
				}

				mWidth = imagefile.getWidth();
				mHeight = imagefile.getHeight();
				mDesc.format = imagefile.getFormat();
				data = imagefile.getData(0);

				if (mWidth != __GetPOT(mWidth) || mHeight != __GetPOT(mHeight)) {
					if (!(mDesc.flags & TexFlag_allowNPOT))
						Errorf("GLTexture::LoadFileCUBE: texture %s size isn't power of two", mDesc.name.c_str());
					else
						Debugf("GLTexture::LoadFileCUBE: texture %s size isn't power of two\n", mDesc.name.c_str());
				}

				__TexFormat(imagefile.getFormat(), format, datatype, internalformat);

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, internalformat, mWidth, mHeight, 0, format, datatype, data);
			}

			mVideoMemoryUsed = getImageSize(imagefile.getFormat(), mWidth, mHeight) * 6;
		}

		gMainState->CheckErrors();
	}

#endif

	GLuint GLtexture::getObject() const {
		return m_object;
	}

	GLenum GLtexture::getTarget() const {
#if 0
		return trTexType(mDesc.type);
#else
		return GL_TEXTURE_2D;
#endif
	}

	void GLtexture::initialize(TexFormat format, int width, int height, InitFlags flags) {
		m_format = format;
		m_width = width;
		m_height = height;
		m_createFlags = flags;

		if (m_object == 0)
			glGenTextures(1, &m_object);

		bool mipmap = false;

		if (m_createFlags.isSet(IF_AutoGenMipmap)) {
			mipmap = false;
		}

		GLenum glformat, datatype, internalformat;

		trTexFormat(format, glformat, datatype, internalformat);
		if (!format) {
			Errorf("OpenGL don't support texture format '%s'", m_format.toString());
		}

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_object);

		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, glformat, datatype, nullptr);

		m_videoMemoryUsed = m_format.calculateDataSize(width, height);

		if (mipmap) {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#if 0
		g_statistic->addValue(stat_textureMemory, m_videoMemoryUsed);
		g_statistic->incValue(stat_numTextures);
#else
		stat_textureMemory.add(m_videoMemoryUsed);
		stat_numTextures.inc();
#endif
		GLrender::checkErrors();
	}

	void GLtexture::getSize(int &width, int &height, int &depth) {
		width = m_width;
		height = m_height;
		depth = m_depth;
	}

	void GLtexture::getSize(int &width, int &height) {
		width = m_width;
		height = m_height;
	}

	void GLtexture::setSize(int width, int height, int depth) {
	}

	TexFormat GLtexture::getFormat() {
		return m_format;
	}

	void GLtexture::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat fmt) {
		if (rect.isEmpty()) {
			return;
		}

		glBindTexture(GL_TEXTURE_2D, m_object);

		if (fmt == TexFormat::AUTO) {
			fmt = m_format;
		}

		GLenum format, datatype, internalformat;

		trTexFormat(fmt, format, datatype, internalformat);
		if (!format) {
			Errorf("OpenGL don't support texture format '%s'", m_format.toString());
		}

//		if (m_format.isDXTC()) {
//			return;
//		} else {
			glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.width, rect.height, format, datatype, pixels);
//		}

	//	glThreadState->CheckErrors();
	}
#if 0
	void GLtexture::uploadSubTextureImmediately(const Rect &rect, const void *pixels) {
		GLrender::checkErrors();

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_object);

		GLrender::checkErrors();

		GLenum format, datatype, internalformat;

		trTexFormat(m_format, format, datatype, internalformat);

		int width, height;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		if (m_format.isDXTC()) {
			return;
		} else {
			glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.width, rect.height, format, datatype, pixels);
		}

		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		GLrender::checkErrors();
	}
#endif
	void GLtexture::setClampMode(ClampMode clampmode) {
		GLenum c = trTexClamp(clampmode);
		glBindTexture(GL_TEXTURE_2D, m_object);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, c);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, c);
		GLrender::checkErrors();
	}

	void GLtexture::setFilterMode(FilterMode filtermode) {
		GLenum min_filter, mag_filter;
		trTexFilter(filtermode, min_filter, mag_filter);

		glBindTexture(GL_TEXTURE_2D, m_object);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	}

	void GLtexture::saveToFile(const String &filename) {
		glBindTexture(GL_TEXTURE_2D, getObject());

		int width, height;
		int internal_format;

		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);

		TexFormat texformat = trInternalFormat(internal_format);
		Image result;
		result.initImage(texformat, width, height);
		void *data = Malloc(result.getDataSize(0));

		int level = 0;
		while (1) {
			int imagesize;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);
			if (!width) {
				break;
			}
			if (texformat.isCompressed()) {
				glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &imagesize);
				glGetCompressedTexImage(GL_TEXTURE_2D, level, data);
				result.setData(level, data, imagesize);
			} else {
				glGetTexImage(GL_TEXTURE_2D, level, GL_BGRA, GL_UNSIGNED_BYTE, data);
				result.setData(level, data, imagesize);
			}
			level++;
		}
		Free(data);
		result.saveFile_dds(filename);
	}

	void GLtexture::generateMipmap() {
		glBindTexture(GL_TEXTURE_2D, m_object);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		GLrender::checkErrors();
	}

	void GLtexture::initFactory() {
	}

	void GLtexture::finalizeFactory() {
	}

	void GLtexture::copyFramebuffer(const Rect &r)
	{
		glBindTexture(GL_TEXTURE_2D, getObject());

		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, r.x, r.y, r.width, r.height);
#if 0
		ulonglong_t start = Microseconds();

		ulonglong_t end = Microseconds();
		Printf("GLtexture::copyFramebuffer: %d microseconds\n", end - start);
		GLrender::checkErrors();
#endif
	}

	void GLtexture::setDepthTextureMode(GLenum mode)
	{
		if (!m_format.isDepth()) {
			return;
		}

		glBindTexture(GL_TEXTURE_2D, getObject());
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, mode);
	}

	void GLtexture::setBorderColor(const Rgba &color)
	{
		glBindTexture(GL_TEXTURE_2D, getObject());

		Vector4 fcolor = color.toVector4();

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, fcolor.c_ptr());
	}

	void GLtexture::setHardwareShadowMap(bool enable)
	{
		glBindTexture(GL_TEXTURE_2D, getObject());

		GLenum hsm = GL_NONE;
		if (enable) {
			hsm = GL_COMPARE_R_TO_TEXTURE;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, hsm);
	}

AX_END_NAMESPACE


