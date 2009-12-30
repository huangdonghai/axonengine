/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

	Image::Image()
		: m_format(TexFormat::AUTO)
		, m_dataPresent(false)
		, m_loadFlags(0)
		, m_width(0)
		, m_height(0)
		, m_depth(0)
	{}

	Image::~Image() {
		clear();
	}

	// IO & processing
	bool Image::loadFile(const String& filename, int flags) {
		clear();

		m_loadFlags = flags;
		if (!(flags & RgbeFloat) || !(flags & RgbeHalf)) {
			flags |= RgbeRaw;
		}
		m_dataPresent = false;

		if (loadFile_dds(filename + ".dds")) {
			m_dataPresent = true;
		}

		return m_dataPresent;
	}

	bool Image::loadFileByType(const String& filename, FileType filetype, int flags) {
		m_loadFlags = flags;
		if (!(flags & RgbeFloat) || !(flags & RgbeHalf)) {
			flags |=RgbeRaw;
		}

		bool loaded = false;

		if (filetype == DDS) {
			loaded = loadFile_dds(filename);
		} else if (filetype == TGA) {
			loaded = loadFile_tga(filename);
		} else if (filetype == HDR) {
			loaded = loadFile_hdr(filename);
		}

		m_dataPresent = loaded;

		if (!loaded) {
			return false;
		}

		// check if need generate mipmaps
		if (flags & Mipmap && m_format.isByte()) {
			generateMipmaps();
		}

		return true;
	}


	Image* Image::resize(int width, int height) {
		if (!m_dataPresent)
			return nullptr;

		if (!m_format.isByte()) {
			Debugf("%s: only support byte format", __func__);
			return nullptr;
		}

		AX_ASSERT(width > 0 && height > 0);
		AX_ASSERT(m_format.getNumComponents() <= 4);

		// horizon scale
		Image* temp = new Image();
		temp->initImage(m_format, width, m_height);

		float xscale = (float)width / (float)m_width;
		float yscale = (float)height / (float)m_height;

		if (xscale < 1.0f) {
			float fwidth = 1.0f / xscale * 0.5f;

			for (int y = 0; y < m_height; y++) {
				for (int x = 0; x < width; x++) {
					float center = x / xscale;
					float left = ceilf(center - fwidth);
					float right = floorf(center + fwidth);

					int c[4]; c[0] = c[1] = c[2] = c[3] = 0;
					int count = 0;
					for (int j = left; j <= right; j++) {
						int n_j = Math::mapToRange(j, 0, m_width);
						const byte_t* pixel = getPixel(0, j, y);

						for (int k = 0; k < m_format.getNumComponents(); k++) {
							c[k] += pixel[k];
						}
						count++;
					}

					byte_t bc[4];
					for (int k = 0; k < m_format.getNumComponents(); k++) {
						bc[k] = c[k] / count;
					}

					temp->setPixel(x, y, bc);
				}
			}
		} else {
		}

		Image* result = new Image;
		result->initImage(m_format, width, height);

		if (yscale < 1.0f) {
			float fwidth = 1.0f / yscale * 0.5f;
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					float center = y / yscale;
					float low = ceilf(center - fwidth);
					float high = floorf(center + fwidth);
					int c[4]; c[0] = c[1] = c[2] = c[3] = 0;
					int count = 0;

					for (int j = low; j <= high; j++) {
						int n_j = Math::mapToRange(j, 0, m_height);
						const byte_t* pixel = temp->getPixel(0, x, j);

						for (int k = 0; k < m_format.getNumComponents(); k++) {
							c[k] += pixel[k];
						}
						count++;
					}

					byte_t bc[4];
					for (int k = 0; k < m_format.getNumComponents(); k++) {
						bc[k] = c[k] / count;
					}

					result->setPixel(x, y, bc);
				}
			}
		}

		delete temp;
		return result;
	}

	Image* Image::getFloatHeightmap(float max_height) {
		AX_ASSERT(m_dataPresent);
		AX_ASSERT(m_format == TexFormat::L16);

		Image* outimage = new Image;
		outimage->initImage(TexFormat::R32F, m_width, m_height, nullptr);

		const ushort_t* indata = (ushort_t*)getData(0);
		float* outdata = (float*)outimage->getWritablePointer(0);

		for (int height = 0; height < m_height; height++) {
			for (int width = 0; width < m_width; width++) {
				*outdata = ((*indata / (float)0xffff) * 2.0f - 1.0f) * max_height;
				indata++;
				outdata++;
			}
		}

		return outimage;
	}

	void Image::generateMipmaps() {
		if (!m_dataPresent)
			return;

		if (!m_format.isByte()) {
			Errorf(_("Image::generateMipmaps: Only byte texture can generate mipmap levels, %s isn't a byte texture"), m_realName.c_str());
			return;
		}

		if (m_datas.size() > 1)
			m_datas.resize(1);

		int i, j, k;
		byte_t* in;
		byte_t* out;
		int width = m_width;
		int height = m_height;
		int bpp = m_format.calculateDataSize(1, 1);

		while (1) {
			if (width == 1 && height == 1)
				break;

			int stride = width*bpp;
			int last_width = width;
			int last_height = height;

			width >>= 1;
			height >>= 1;

			if (width < 1) width = 1;
			if (height < 1) height = 1;

			in = m_datas[m_datas.size()-1].get();
			out = new byte_t[ width*height*bpp];
			m_datas.push_back(DataBuffer(out));

			for (i=0; i<height; i++) {
				for (j=0; j<width; j++, out+=bpp) {
					for (k=0; k<bpp; k++) {
						if (last_width < 2) {
							out[k] = (in[k] + in[stride+k]) >> 1;
						} else if (last_height < 2) {
							out[k] = (in[k] + in[k+bpp]) >> 1;
						} else {
							out[k] = (in[k] + in[k+bpp] + in[stride+k] + in[stride+k+bpp]) >> 2;
						}
					}
					if (last_width < 2)
						in += bpp;
					else
						in += bpp*2;
				}
				if (last_height >= 2)
					in+=stride;
			}
		}
	}

	void Image::generateNormalmapMipmaps() {
	}

	void Image::generateHeightmapMipmaps() {
		if (!m_dataPresent)
			return;

		if (m_format != TexFormat::R32F) {
			Errorf(_("Image::generateHeightmapMipmaps: Only R32F texture can generate heightmap mipmap levels, %s isn't a Uint8 texture"), m_realName.c_str());
			return;
		}

		if (m_datas.size() > 1)
			m_datas.resize(1);

		int i, j;
		float* in;
		float* out;
		int width = m_width;
		int height = m_height;
		int numcmpts =  m_format.getNumComponents();

		while (1) {
			if (width == 1 && height == 1)
				break;

			int stride = width;
			int last_width = width;
			int last_height = height;

			width >>= 1;
			height >>= 1;

			if (width < 1) width = 1;
			if (height < 1) height = 1;

			in = (float*)m_datas[m_datas.size()-1].get();
			out = (float*)new byte_t[ width*height*m_format.getBitsPerPixel()/8];
			m_datas.push_back(DataBuffer((byte_t*)out));

			for (i=0; i<height; i++) {
				for (j=0; j<width; j++, out++) {
					*out = *in;
					if (last_width < 2)
						in += numcmpts;
					else
						in += numcmpts*2;
				}
				if (last_height >= 2)
					in+=stride*numcmpts;
			}
		}
	}

	void Image::initImage(TexFormat format, int width, int height, const byte_t* pixel) {
		clear();

		m_format = format;
		m_width = width;
		m_height = height;

		int imagesize = format.calculateDataSize(width, height);
		byte_t* data = new byte_t[imagesize];

		m_datas.push_back(DataBuffer(data));
		m_dataPresent = true;

		if (pixel) {
			setPixel(0, 0, m_width, m_height, pixel);
		}
	}

	void Image::initImage(TexFormat format, int width, int height, bool mipmap) {
		clear();

		m_format = format;
		m_width = width;
		m_height = height;

		int imagesize = format.calculateDataSize(width, height);
		byte_t* data = new byte_t[imagesize];

		m_datas.push_back(DataBuffer(data));
		m_dataPresent = true;

		if (!mipmap) {
			return;
		}

		while (1) {
			if (width == 1 && height == 1)
				break;

			width >>= 1;
			height >>= 1;

			if (width < 1) width = 1;
			if (height < 1) height = 1;

			int imagesize = format.calculateDataSize(width, height);
			byte_t* data = new byte_t[imagesize];

			m_datas.push_back(DataBuffer(data));
		}
	}


	void Image::clear() {
		m_datas.clear();
		m_dataPresent = false;
	}

	void Image::setPixel(int level, const Point& pos, const byte_t* pixel) {
		setPixel(level, pos.x, pos.y, pixel);
	}

	void Image::setPixel(int level, int x, int y, const byte_t* pixel) {
		if (!m_dataPresent) {
			Errorf(_("Image::setPixel: data not present\n"));
		}

		if (m_format.isDXTC()) {
			Errorf(_("Image::setPixel: can do this on compressed image\n"));
		}

		if (level >= getNumMipmapLevels()) {
			Errorf(_("Image::setPixel: level out of bound\n"));
		}

		int bits_per_pixel = m_format.getBitsPerPixel();
		int bytes_per_pixel = bits_per_pixel / 8;
		int level_width = getWidth(level);
		int level_height = getHeight(level);

		AX_ASSERT(bits_per_pixel % 8 == 0);
		AX_ASSERT(x >= 0 && x < level_width);
		AX_ASSERT(y >= 0 && y < level_height);

		byte_t* dst = m_datas[level].get() + (y * level_width + x) * bytes_per_pixel;
		::memcpy(dst, pixel, bytes_per_pixel);
	}

	void Image::setPixel(int level, const Rect& rect, const byte_t* pixel) {
		setPixel(level, rect.x, rect.y, rect.width, rect.height, pixel);
	}

	void Image::setPixel(int level, int x, int y, int width, int height, const byte_t* pixel) {
		if (!m_dataPresent) {
			Errorf(_("Image::setPixel: data not present\n"));
		}

		if (m_format.isDXTC()) {
			Errorf(_("Image::setPixel: can do this on compressed image\n"));
		}

		if (level >= getNumMipmapLevels()) {
			Errorf(_("Image::setPixel: level out of bound\n"));
		}

		int bits_per_pixel = m_format.getBitsPerPixel();
		int bytes_per_pixel = bits_per_pixel / 8;
		int level_width = getWidth(level);
		int level_height = getHeight(level);
		AX_ASSERT(bits_per_pixel % 8 == 0);

		int i, j, px, py;
		for (i = 0, py = y; i < height; i++, py++) {
			px = x;
			byte_t* dst = m_datas[level].get() + (py * level_width + px) * bytes_per_pixel;

			for (j = 0; j < height; j++, px++) {
				AX_ASSERT(px >= 0 && px < level_width >> level);
				AX_ASSERT(py >= 0 && py < level_height >> level);
				::memcpy(dst, pixel, bytes_per_pixel);
				dst += bytes_per_pixel;
			}
		}
	}

	void Image::setPixel(const Point& pos, const byte_t* pixel) {
		setPixel(0, pos, pixel);
	}

	void Image::setPixel(int x, int y, const byte_t* pixel) {
		setPixel(0, x, y, pixel);
	}

	void Image::setPixel(const Rect& rect, const byte_t* pixel) {
		setPixel(0, rect, pixel);
	}

	void Image::setPixel(int x, int y, int width, int height, const byte_t* pixel) {
		setPixel(0, x, y, width, height, pixel);
	}

	const byte_t* Image::getPixel(int level, const Point& pos) const {
		return getPixel(level, pos.x, pos.y);
	}

	// add support compressed format
	const byte_t* Image::getPixel(int level, int x, int y) const {
		if (!m_dataPresent) {
			Errorf(_("Image::SetPixel: data not present\n"));
		}

		if (level >= getNumMipmapLevels()) {
			Errorf(_("Image::SetPixel: level out of bound\n"));
		}

		// get block info
		int blocksize = m_format.getBlockSize();
		int blockdatasize = m_format.getBlockDataSize();

		int level_width = getWidth(level);
		int level_height = getHeight(level);

		x = Math::mapToBound(x, level_width);
		y = Math::mapToBound(y, level_height);

		byte_t* dst = m_datas[level].get() + (y/blocksize * level_width/blocksize + x/blocksize) * blockdatasize;
		return dst;
	}

	ByteSeq Image::getFilteredPixel(int level, const Vector2& st) const {
		return getFilteredPixel(level, st.x, st.y);
	}

	ByteSeq Image::getFilteredPixel(int level, float s, float t) const {
		if (!m_dataPresent) {
			Errorf(_("Image::GetFilteredPixel: data not present\n"));
		}

		if (m_format.isDXTC()) {
			Errorf(_("Image::GetFilteredPixel: can do this on compressed image\n"));
		}

		if (level >= getNumMipmapLevels()) {
			Errorf(_("Image::GetFilteredPixel: level out of bound\n"));
		}

		int bits_per_pixel = m_format.getBitsPerPixel();
		int bytes_per_pixel = bits_per_pixel / 8;
		int level_width = getWidth(level);
		int level_height = getHeight(level);

		s = s * level_width - 0.5f;
		t = t * level_height - 0.5f;

		float s_frac = s - floorf(s);
		float t_frac = s - floorf(t);
		float f00 = (1.0f - s_frac) * (1.0f - t_frac);
		float f01 = s_frac * (1.0f - t_frac);
		float f10 = (1.0f - s_frac) * t_frac;
		float f11 = s_frac * t_frac;

		ByteSeq ret;
		ret.resize(bytes_per_pixel);

		if (m_format.isByte()) {
			const byte_t* p00 = getPixel(level, floorf(s), floorf(t));
			const byte_t* p01 = getPixel(level, ceilf(s), floorf(t));
			const byte_t* p10 = getPixel(level, floorf(s), ceilf(t));
			const byte_t* p11 = getPixel(level, ceilf(s), ceilf(t));
			byte_t* p = &ret[0];

			int i;
			for (i = 0; i < m_format.getNumComponents(); i++) {
				p[i] = p00[i] * f00 + p01[i] * f01 + p10[i] * f10 + p11[i] * f11;
			}
		} else if (m_format.isUShort()) {
			const ushort_t* p00 = (ushort_t*)getPixel(level, floorf(s), floorf(t));
			const ushort_t* p01 = (ushort_t*)getPixel(level, ceilf(s), floorf(t));
			const ushort_t* p10 = (ushort_t*)getPixel(level, floorf(s), ceilf(t));
			const ushort_t* p11 = (ushort_t*)getPixel(level, ceilf(s), ceilf(t));
			ushort_t* p = (ushort_t*)&ret[0];

			int i;
			for (i = 0; i < m_format.getNumComponents(); i++) {
				p[i] = p00[i] * f00 + p01[i] * f01 + p10[i] * f10 + p11[i] * f11;
			}
		} else if (m_format.isFloat()) {
			const float* p00 = (float*)getPixel(level, floorf(s), floorf(t));
			const float* p01 = (float*)getPixel(level, ceilf(s), floorf(t));
			const float* p10 = (float*)getPixel(level, floorf(s), ceilf(t));
			const float* p11 = (float*)getPixel(level, ceilf(s), ceilf(t));
			float* p = (float*)&ret[0];

			int i;
			for (i = 0; i < m_format.getNumComponents(); i++) {
				p[i] = p00[i] * f00 + p01[i] * f01 + p10[i] * f10 + p11[i] * f11;
			}
		} else {
			// shouldn't be here
			AX_ASSERT(0);
		}

		return ret;
	}

	// treat image is tiled, so rect can out of bounds
	Image* Image::readSubImage(int level, const Rect& rect) const {
		if (!m_dataPresent) {
			Errorf(_("Image::readSubImage: data not present\n"));
		}

		if (m_format.isDXTC()) {
			Errorf(_("Image::readSubImage: can do this on compressed image\n"));
		}

		if (level >= getNumMipmapLevels()) {
			Errorf(_("Image::readSubImage: level out of bound\n"));
		}

		Image* image = new Image;
		image->initImage(m_format, rect.width, rect.height);
		image->writeSubImage(this, level, rect, 0, Point(0,0));

		return image;
	}


	// copy block data from another image, source image and dst image must have same format
	// dxt and other compressed image format are support
	void Image::writeSubImage(const Image* src_image, int src_level, const Rect& src_rect, int dst_level, const Point& dst_pos) {
		if (src_image->getFormat() != m_format) {
			Errorf(_("Image::writeSubImage: only same format can do this"));
			return;
		}

		// get block info
		int blocksize = m_format.getBlockSize();
		int blockdatasize = m_format.getBlockDataSize();

		// get source image width and height
		int s_image_width = src_image->getWidth(src_level);
		int s_image_height = src_image->getHeight(src_level);

		// bounds check and params check
		AX_ASSERT(dst_pos.x >= 0);
		AX_ASSERT(dst_pos.y >= 0);
		AX_ASSERT(dst_pos.x + src_rect.width <= getWidth(dst_level));
		AX_ASSERT(dst_pos.y + src_rect.height <= getHeight(dst_level));
		AX_ASSERT(src_rect.x % blocksize == 0);
		AX_ASSERT(src_rect.y % blocksize == 0);
		AX_ASSERT(src_rect.width % blocksize == 0);
		AX_ASSERT(src_rect.height % blocksize == 0);
		AX_ASSERT(s_image_width % blocksize == 0);
		AX_ASSERT(s_image_height % blocksize == 0);
		AX_ASSERT(dst_pos.x % blocksize == 0);
		AX_ASSERT(dst_pos.y % blocksize == 0);


		// mapping source rect to positive value for correct calculating
		Rect src_rect_normalized = src_rect;
		const int sx = Math::mapToBound(src_rect.x, s_image_width);
		const int sy = Math::mapToBound(src_rect.y, s_image_height);
		const int sx_max = sx + src_rect.width;
		const int sy_max = sy + src_rect.height;

		// iterator for each bounded coping
		int copyed_x = sx;
		int coping_x = sx;
		int dest_x = dst_pos.x;
		while (coping_x < sx_max) {
			// get next x axis bounds to width
			coping_x += s_image_width;
			coping_x -= coping_x % s_image_width;
			coping_x = std::min(coping_x, sx_max);

			int dest_y = dst_pos.y;
			int copyed_y = sy;
			int coping_y = sy;
			while (coping_y < sy_max) {
				// get next y axis bounds to height
				coping_y += s_image_height;
				coping_y -= coping_y % s_image_height;
				coping_y = std::min(coping_y, sy_max);

				Rect copy_rect(copyed_x, copyed_y, coping_x-copyed_x, coping_y-copyed_y);
				copy_rect.x = Math::mapToBound(copy_rect.x, s_image_width);
				copy_rect.y = Math::mapToBound(copy_rect.y, s_image_height);

				int d_y, s_y;
				int linewidth = blockdatasize * copy_rect.width / blocksize;
				int block_width = getWidth(dst_level) / blocksize;

				for (s_y = copy_rect.y, d_y = dest_y; s_y < copy_rect.yMax(); s_y+=blocksize, d_y+=blocksize) {
					const byte_t* src = src_image->getPixel(src_level, copy_rect.x, s_y);
					byte_t* dst = m_datas[dst_level].get() + (d_y * block_width + dest_x) / blocksize * blockdatasize;
					memcpy(dst, src, linewidth);
				}

				dest_y += coping_y - copyed_y;
				copyed_y = coping_y;
			}

			dest_x += coping_x - copyed_x;
			copyed_x = coping_x;
		}
	}

	void Image::writeSubImage(const Image* src_image, const Rect& src_rect, const Point& dst_pos) {
		writeSubImage(src_image, 0, src_rect, 0, dst_pos);
	}

	void Image::writeSubImage(const Image* src_image, int src_level, const Rect& src_rect, int dst_level, const Rect& dst_rect, bool need_filter) {
	}

	void Image::writeSubImage(const Image* src_image, const Rect& src_rect, const Rect& dst_rect, bool need_filter) {
		writeSubImage(src_image, 0, src_rect, 0, dst_rect, need_filter);
	}


	// get info
	int Image::getWidth(int level) const {
		if (!m_dataPresent) {
			Errorf(_("Image::getWidth: data not present\n"));
		}

		if (level >= getNumMipmapLevels()) {
//			Errorf(_("Image::getWidth: level out of bound\n"));
		}

		return std::max(1, m_width >> level);
	}

	int Image::getHeight(int level) const {
		if (!m_dataPresent) {
			Errorf(_("Image::getHeight: data not present\n"));
		}

		if (level >= getNumMipmapLevels()) {
//			Errorf(_("Image::getHeight: level out of bound\n"));
		}

		return std::max(1, m_height >> level);
	}

	int Image::getNumMipmapLevels() const {
		if (!m_dataPresent) {
			Errorf(_("Image::getNumMipmapLevels: data not present\n"));
		}

		return (int)m_datas.size();
	}

	TexFormat Image::getFormat() const {
		if (!m_dataPresent) {
			Errorf(_("Image::getFormat: data not present\n"));
		}

		return m_format;
	}

	String Image::getRealName() const {
		if (!m_dataPresent) {
			Errorf(_("Image::getRealName: data not present\n"));
		}

		return m_realName;
	}

	void Image::convertFormatTo(TexFormat format) {
		if (!m_dataPresent) {
			Errorf(_("Image::convertFormatTo: data not present\n"));
		}

		if (format == m_format)
			return;

		int maxLevel = getNumMipmapLevels();
		for (int i = 0; i< maxLevel; i++) {
			int bufLen = convertFormatTo(format, i, NULL);
			byte_t* buf = new byte_t[bufLen];
			convertFormatTo(format, i, buf);
			m_datas[i] = DataBuffer(buf);
		}
	}

	int Image::convertFormatTo(TexFormat format, int level, byte_t* pBuf) const {
		int width = getWidth(level);
		int height = getHeight(level);
		int ret = format.calculateDataSize(width, height);

		if (format == m_format)
			return ret;

		if (pBuf == NULL)
			return ret;

		switch (format) {
		case TexFormat::L8:				// 8 bits luminance, alpha is always 1.0
		case TexFormat::LA8:				// 8 bits luminance & alpha
		case TexFormat::A8:				// 8 bits alpha: font texture use this
		case TexFormat::RGB8:
			Errorf(_("Image::convertFormatTo: run to not write code."));
			break;
		case TexFormat::RGBA8:
			{
				const byte_t* src = m_datas[level].get();
				for (int h = 0; h < height; h++) {
					for (int w = 0; w < width; w++) {
						switch (m_format)
						{
						case TexFormat::L8:				// 8 bits luminance, alpha is always 1.0
						case TexFormat::A8:				// 8 bits alpha: font texture use this
							pBuf[0] = pBuf[1] = pBuf[2] = src[0];
							pBuf[3] = 255;
							src++;
							break;
						case TexFormat::LA8:				// 8 bits luminance & alpha
							pBuf[0] = pBuf[1] = pBuf[2] = src[0];
							pBuf[3] = src[1];
							src += 2;
							break;
						case TexFormat::RGB8:
							pBuf[0] = src[0];
							pBuf[1] = src[1];
							pBuf[2] = src[2];
							pBuf[3] = 255;
							src += 3;
							break;
						case TexFormat::RGBX8:				// rgb: x is reserved not use: 32 bits
							pBuf[0] = src[0];
							pBuf[1] = src[1];
							pBuf[2] = src[2];
							pBuf[3] = 255;
							src += 4;
							break;
						case TexFormat::BGR8:
							pBuf[0] = src[2];
							pBuf[1] = src[1];
							pBuf[2] = src[0];
							pBuf[3] = 255;
							src += 3;
							break;
						case TexFormat::BGRA8:
							pBuf[0] = src[2];
							pBuf[1] = src[1];
							pBuf[2] = src[0];
							pBuf[3] = src[3];
							src += 4;
							break;
						case TexFormat::BGRX8:				// bgr: x is reserved not use: 32 bits
							pBuf[0] = src[2];
							pBuf[1] = src[1];
							pBuf[2] = src[0];
							pBuf[3] = 255;
							src += 4;
							break;
							// compressed image
						case TexFormat::DXT1:
						case TexFormat::DXT3:
						case TexFormat::DXT5:
							Errorf("If need DDS format, please use flag IFLF_no_compressed.");
							break;
							// 16 bits fixed image
						case TexFormat::L16:				// 16 bits int texture: terrain heightmap use this
							pBuf[0] = pBuf[1] = pBuf[2] = src[1];
							pBuf[3] = 255;
							src += 2;
							break;
							// 16 bits float image
						case TexFormat::R16F:				// 16 bits float luminance: alpha is always 1.0
						case TexFormat::RG16F:
						case TexFormat::RGBA16F:
							Errorf("run to not complete code");
							break;
							// 32 bits float image
						case TexFormat::R32F:				// 16 bits float luminance: alpha is always 1.0
						case TexFormat::RG32F:
						case TexFormat::RGBA32F:
						default:
							Errorf("run to not complete code");
							break;
						}
						pBuf += 4;
					}
				}
			}
			break;
		case TexFormat::RGBX8:				// rgb: x is reserved not use: 32 bits
		case TexFormat::BGR8:
			Errorf("Image::ConvertFormatTo: run to not write code.");
			break;
		case TexFormat::BGRA8:
			{
				const byte_t* src = m_datas[level].get();
				for (int h = 0; h < height; h++) {
					for (int w = 0; w < width; w++) {
						switch (m_format)
						{
						case TexFormat::L8:				// 8 bits luminance, alpha is always 1.0
						case TexFormat::A8:				// 8 bits alpha: font texture use this
							pBuf[0] = pBuf[1] = pBuf[2] = src[0];
							pBuf[3] = 255;
							src++;
							break;
						case TexFormat::LA8:				// 8 bits luminance & alpha
							pBuf[0] = pBuf[1] = pBuf[2] = src[0];
							pBuf[3] = src[1];
							src += 2;
							break;
						case TexFormat::BGR8:
							pBuf[0] = src[0];
							pBuf[1] = src[1];
							pBuf[2] = src[2];
							pBuf[3] = 255;
							src += 3;
							break;
						case TexFormat::BGRX8:				// rgb: x is reserved not use: 32 bits
							pBuf[0] = src[0];
							pBuf[1] = src[1];
							pBuf[2] = src[2];
							pBuf[3] = 255;
							src += 4;
							break;
						case TexFormat::RGB8:
							pBuf[0] = src[2];
							pBuf[1] = src[1];
							pBuf[2] = src[0];
							pBuf[3] = 255;
							src += 3;
							break;
						case TexFormat::RGBA8:
							pBuf[0] = src[2];
							pBuf[1] = src[1];
							pBuf[2] = src[0];
							pBuf[3] = src[3];
							src += 4;
							break;
						case TexFormat::RGBX8:				// bgr: x is reserved not use: 32 bits
							pBuf[0] = src[2];
							pBuf[1] = src[1];
							pBuf[2] = src[0];
							pBuf[3] = 255;
							src += 4;
							break;
							// compressed image
						case TexFormat::DXT1:
						case TexFormat::DXT3:
						case TexFormat::DXT5:
							Errorf("If need DDS format, please use flag IFLF_no_compressed.");
							break;
							// 16 bits fixed image
						case TexFormat::L16:				// 16 bits int texture: terrain heightmap use this
							pBuf[0] = pBuf[1] = pBuf[2] = src[1];
							pBuf[3] = 255;
							src += 2;
							break;
							// 16 bits float image
						case TexFormat::R16F:				// 16 bits float luminance: alpha is always 1.0
						case TexFormat::RG16F:
						case TexFormat::RGBA16F:
							Errorf("run to not complete code");
							break;
							// 32 bits float image
						case TexFormat::R32F:				// 16 bits float luminance: alpha is always 1.0
						case TexFormat::RG32F:
						case TexFormat::RGBA32F:
						default:
							Errorf("run to not complete code");
							break;
						}
						pBuf += 4;
					}
				}
			}
			break;
		case TexFormat::BGRX8:				// bgr: x is reserved not use: 32 bits
		// compressed image
		case TexFormat::DXT1:
		case TexFormat::DXT3:
		case TexFormat::DXT5:
		// 16 bits fixed image
		case TexFormat::L16:				// 16 bits int texture: terrain heightmap use this
		// 16 bits float image
		case TexFormat::R16F:				// 16 bits float luminance: alpha is always 1.0
		case TexFormat::RG16F:
		case TexFormat::RGBA16F:
		// 32 bits float image
		case TexFormat::R32F:				// 16 bits float luminance: alpha is always 1.0
		case TexFormat::RG32F:
		case TexFormat::RGBA32F:
		default:
			Errorf("Image::ConvertFormatTo: run to not write code.");
			break;
		}
		return ret;
	}

	const byte_t* Image::getData(int level) const {
		if (!m_dataPresent) {
			Errorf("Image::getData: data not present\n");
		}

		if (level >= s2i(m_datas.size())) {
			Errorf("Image::getData: no this level\n");
		}

		return m_datas[level].get();
	}

	const byte_t* Image::getData(int level, TexFormat format) {
		if (!m_dataPresent) {
			Errorf("Image::getData: data not present\n");
		}

		if (level >= s2i(m_datas.size())) {
			Errorf("Image::getData: no this level\n");
		}

		if (format != m_format) {
			convertFormatTo(format);
		}
		return m_datas[level].get();
	}

	void Image::writeDataTo(int level, byte_t* data) const {
		if (!m_dataPresent) {
			Errorf("Image::writeDataTo: data not present\n");
		}

		if (level >= s2i(m_datas.size())) {
			Errorf("Image::writeDataTo: no this level\n");
		}

		int imageSize = m_format.calculateDataSize(getWidth(level), getHeight(level));
		memcpy(data, m_datas[level].get(), imageSize);
	}

	void Image::writeDataTo(int level, TexFormat format, byte_t* data) const {
		if (!m_dataPresent) {
			Errorf("Image::writeDataTo: data not present\n");
		}

		if (level >= s2i(m_datas.size())) {
			Errorf("Image::WriteDataTo: no this level\n");
		}

		convertFormatTo(format, level, data);
	}

	void Image::writeDataTo(int level, const Rect& rect, int rotate, byte_t* data, bool flipS, bool flipT) const {
		if (!m_dataPresent) {
			Errorf("Image::writeDataTo: data not present\n");
			return;
		}

		if (level >= s2i(m_datas.size())) {
			Errorf("Image::writeDataTo: no this level\n");
			return;
		}

		if (rect.x < 0 || rect.y < 0 || rect.xMax() > m_width || rect.yMax() > m_height) {
			Errorf("Image::writeDataTo: rect is out of bound\n");
			return;
		}

		if (m_format.isDXTC()) {
			Errorf("Image::writeDataTo: DXT format don't support\n");
			return;
		}

		int srcx, srcy, srcx_delta, srcy_delta, srcx_ldelta, srcy_ldelta;
		int srcbpl, dstbpl;	// source bytes per line and dst bytes per line
		int bpp = m_format.calculateDataSize(1, 1);

		srcbpl = bpp * m_width;
		dstbpl = bpp * rect.width;
		if (rotate == 0) {
			srcx = rect.x;
			srcy = rect.y;
			srcx_delta = 1;
			srcy_delta = 0;
			srcx_ldelta = 0;
			srcy_ldelta = 1;
		} else if (rotate == 90) {
			srcx = rect.x;
			srcy = rect.yMax() - 1;
			srcx_delta = 0;
			srcy_delta = -1;
			srcx_ldelta = 1;
			srcy_ldelta = 0;
		} else if (rotate == 180) {
			srcx = rect.xMax() - 1;
			srcy = rect.yMax() - 1;
			srcx_delta = -1;
			srcy_delta = 0;
			srcx_ldelta = 0;
			srcy_ldelta = -1;
		} else if (rotate == 270) {
			srcx = rect.xMax() - 1;
			srcy = rect.y;
			srcx_delta = 0;
			srcy_delta = 1;
			srcx_ldelta = -1;
			srcy_ldelta = 0;
		} else {
			Errorf("Image::writeDataTo: rotate parameter must be one of 0, 90, 180, 270\n");
			return;
		}

		int i, j;
		byte_t* srcdata = m_datas[level].get();

		for (i = 0; i < rect.height; i++) {
			int x = srcx;
			int y = srcy;
			int dstx, dsty;

			if (flipT)
				dsty = rect.height - i - 1;
			else
				dsty = i;

			for (j = 0; j < rect.width; j++) {
				if (flipS)
					dstx = rect.width - j - 1;
				else
					dstx = j;

				memcpy(data + (dsty * rect.width + dstx) * bpp, srcdata + y * srcbpl + x * bpp, bpp);
				x += srcx_delta;
				y += srcy_delta;
			}
			srcx += srcx_ldelta;
			srcy += srcy_ldelta;
		}

		return;
	}

	byte_t* Image::getWritablePointer(int level) {
		if (!m_dataPresent) {
			Errorf("Image::getWritablePointer: data not present\n");
			return NULL;
		}

		if (level >= s2i(m_datas.size())) {
			Errorf("Image::getWritablePointer: no this level\n");
			return NULL;
		}

		return m_datas[level].get();
	}

	int Image::getTotalDataSize() const {
		if (!m_dataPresent)
			return 0;

		int size = 0;
		for (int i = 0; i < getNumMipmapLevels(); i++) {
			size += m_format.calculateDataSize(getWidth(i), getHeight(i));
		}

		return size;
	}

	int Image::getDataSize(int level) const {
		if (!m_dataPresent)
			return 0;

		return m_format.calculateDataSize(getWidth(level), getHeight(level));
	}


	bool Image::loadFile_raw(const String& filename, int width, int height) {
		return false;
	}

	void Image::saveFile_raw(const String& filename) {
		if (!m_dataPresent)
			return;

		if (m_format != TexFormat::L16)
			Errorf("%s: write raw file format isn't L16");

		g_fileSystem->writeFile(filename, m_datas[0].get(), getDataSize(0));
	}

	void Image::setData(int level, const void* data, int size) {
		AX_ASSERT(size == getDataSize(level));

		if (getNumMipmapLevels() <= level) {
			byte_t* newdata = new byte_t[size];
			memcpy(newdata, data, size);

			m_datas.push_back(DataBuffer(newdata));
		} else {
			byte_t* newdata = m_datas[level].get();
			memcpy(newdata, data, size);
		}
	}


AX_END_NAMESPACE

