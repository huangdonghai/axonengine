/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

/*------------------------------------------------------------------------------ 
.TGA file reader 
------------------------------------------------------------------------------*/ 

#define MAXCOLORS 16384 

/* Definitions for image types. */ 
#define TGA_Null 0 /* no image data */ 
#define TGA_Map 1 /* Uncompressed, color-mapped images. */ 
#define TGA_RGB 2 /* Uncompressed, RGB images. */ 
#define TGA_Mono 3 /* Uncompressed, black and white images. */ 
#define TGA_RLEMap 9 /* Runlength encoded color-mapped images. */ 
#define TGA_RLERGB 10 /* Runlength encoded RGB images. */ 
#define TGA_RLEMono 11 /* Compressed, black and white images. */ 
#define TGA_CompMap 32 /* Compressed color-mapped data, using Huffman, Delta, and runlength encoding. */ 
#define TGA_CompMap4 33 /* Compressed color-mapped data, using Huffman, Delta, and runlength encoding.  4-pass quadtree-type process. */ 

/* Definitions for interleave flag. */ 
#define TGA_IL_None 0 /* non-interleaved. */ 
#define TGA_IL_Two 1 /* two-way (even/odd) interleaving */ 
#define TGA_IL_Four 2 /* four way interleaving */ 
#define TGA_IL_Reserved 3 /* reserved */ 

/* Definitions for origin flag */ 
#define TGA_O_UPPER 0 /* Origin in lower left-hand corner. */ 
#define TGA_O_LOWER 1 /* Origin in upper left-hand corner. */ 

AX_BEGIN_NAMESPACE


	struct TgaHeader { 
		byte_t 	id_length, colormap_type, image_type; 
		ushort_t colormap_index, colormap_length; 
		byte_t colormap_size; 
		ushort_t x_origin, y_origin, width, height; 
		byte_t pixel_size, attributes; 
	}; 

	void Image::writeTGA(const String &filename, const byte_t *pixels, int width, int height) { 
		TgaHeader header; 
		byte_t *filebuf; 
		byte_t *d; 
		int i; 
		int isize = width*height*4; 
		int fsize = isize + 18;	// 18 is targa header size 
	 
		filebuf = new byte_t[fsize]; 
		header.id_length = 0; 
		header.colormap_type = 0; 
		header.image_type = 2; 
		header.colormap_index = 0; 
		header.colormap_length = 0; 
		header.colormap_size = 0; 
		header.x_origin = 0; 
		header.y_origin = 0; 
		header.width = width; 
		header.height = height; 
		header.pixel_size = 32; 
		header.attributes = 0; 
	 
		/* write targa header */ 
		d = filebuf; 
		*d++ = header.id_length; 
		*d++ = header.colormap_type; 
		*d++ = header.image_type; 
		*d++ = header.colormap_index & 0xFF; 
		*d++ = header.colormap_index >> 8; 
		*d++ = header.colormap_length & 0xFF; 
		*d++ = header.colormap_length >> 8; 
		*d++ = header.colormap_size; 
		*d++ = header.x_origin & 0xFF; 
		*d++ = header.x_origin >> 8; 
		*d++ = header.y_origin & 0xFF; 
		*d++ = header.y_origin >> 8; 
		*d++ = header.width & 0xFF; 
		*d++ = header.width >> 8; 
		*d++ = header.height & 0xFF; 
		*d++ = header.height >> 8; 
		*d++ = header.pixel_size; 
		*d++ = header.attributes; 
	 
		for (i = 0; i < isize; i += 4) { 
			d[i  ] = pixels[i]; 
			d[i+1] = pixels[i+1]; 
			d[i+2] = pixels[i+2]; 
			d[i+3] = pixels[i+3]; 
		} 
	 
		g_fileSystem->writeFile(filename, filebuf, fsize); 
	 
		delete [](filebuf); 
	} 
	 
	void WriteTGA24(const String &filename, const byte_t *pixels, int width, int height) { 
		TgaHeader header; 
		byte_t *filebuf; 
		byte_t *d; 
		int i; 
		int isize = width*height*3; 
		int fsize = isize + 18;	// 18 is targa header size 
	 
		filebuf = new byte_t[fsize]; 
		header.id_length = 0; 
		header.colormap_type = 0; 
		header.image_type = 2; 
		header.colormap_index = 0; 
		header.colormap_length = 0; 
		header.colormap_size = 0; 
		header.x_origin = 0; 
		header.y_origin = 0; 
		header.width = width; 
		header.height = height; 
		header.pixel_size = 24; 
		header.attributes = 0; 
	 
		/* write targa header */ 
		d = filebuf; 
		*d++ = header.id_length; 
		*d++ = header.colormap_type; 
		*d++ = header.image_type; 
		*d++ = header.colormap_index & 0xFF; 
		*d++ = header.colormap_index >> 8; 
		*d++ = header.colormap_length & 0xFF; 
		*d++ = header.colormap_length >> 8; 
		*d++ = header.colormap_size; 
		*d++ = header.x_origin & 0xFF; 
		*d++ = header.x_origin >> 8; 
		*d++ = header.y_origin & 0xFF; 
		*d++ = header.y_origin >> 8; 
		*d++ = header.width & 0xFF; 
		*d++ = header.width >> 8; 
		*d++ = header.height & 0xFF; 
		*d++ = header.height >> 8; 
		*d++ = header.pixel_size; 
		*d++ = header.attributes; 
	 
		for (i = 0; i < isize; i += 3) { 
			d[i  ] = pixels[i  ]; 
			d[i+1] = pixels[i+1]; 
			d[i+2] = pixels[i+2]; 
		} 
	 
	#if 0
		Printf("write file %s\n", filename.c_str());
		FILE *hf = fopen(filename,"wb");
		if (!hf)
			return;
		fwrite(filebuf, fsize,1,hf);
		fclose(hf);
	#else
		g_fileSystem->writeFile(filename, filebuf, fsize); 
	#endif
	 
		delete[](filebuf); 
	}

	bool
	Image::loadFile_tga(const String &filename) {
		// clear first
		clear();

		int w, h, x, y; 
		int realrow, truerow, baserow; 
		int i, temp1, temp2; 
		byte_t *dst; 
		int dstbpp; 
		int pixel_size; 
		byte_t r, g, b, a, j, k, l; 
		int map_idx; 
		TgaHeader header; 
		int mapped; 
		int rlencoded; 
		int RLE_count; 
		int RLE_flag; 
		byte_t *ColorMap; 
		byte_t *data; 
		byte_t *pdata; 
		int size; 
		int interleave; 
		int origin; 
		byte_t tmp[2]; 

		m_format = TexFormat::AUTO; 

		/* load file */ 
		size_t fsize = g_fileSystem->readFile(filename, (void**)&data); 

		if (!data || !fsize) { 
			return false; 
		} 

		pdata = data; 

		header.id_length = *pdata++; 
		header.colormap_type = *pdata++; 
		header.image_type = *pdata++; 

		tmp[0] = pdata[0]; 
		tmp[1] = pdata[1]; 
		header.colormap_index = LittleShort(*((short *)tmp)); 
		pdata+=2; 
		tmp[0] = pdata[0]; 
		tmp[1] = pdata[1]; 
		header.colormap_length = LittleShort(*((short *)tmp)); 
		pdata+=2; 
		header.colormap_size = *pdata++; 
		header.x_origin = LittleShort(*((short *)pdata)); 
		pdata+=2; 
		header.y_origin = LittleShort(*((short *)pdata)); 
		pdata+=2; 
		header.width = LittleShort(*((short *)pdata)); 
		pdata+=2; 
		header.height = LittleShort(*((short *)pdata)); 
		pdata+=2; 
		header.pixel_size = *pdata++; 
		header.attributes = *pdata++; 

		if (header.id_length) 
			pdata += header.id_length; 

		/* validate TGA type */ 
		switch (header.image_type) { 
		case TGA_Map: 
		case TGA_RGB: 
		case TGA_Mono: 
		case TGA_RLEMap: 
		case TGA_RLERGB: 
		case TGA_RLEMono: 
			break; 
		default: 
			Errorf("LoadTGA(%s): Only type 1 (map), 2 (RGB), 3 (mono), 9 (RLEmap), 10 (RLERGB), 11 (RLEmono) TGA images supported\n", filename.c_str()); 
			return false; 
		} 

		/* validate color depth */ 
		switch (header.pixel_size) { 
		case 8:
			m_format = TexFormat::L8;
			dstbpp = 1; 
			break; 
		case 15:
		case 16:
		case 24:
			if (m_loadFlags & Image::ExpandAlpha) {
				m_format = TexFormat::BGRX8;
				dstbpp = 4;
			} else {
				m_format = TexFormat::BGR8;
				dstbpp = 3;
			}
			break;
		case 32:
			m_format = TexFormat::BGRA8;
			dstbpp = 4;
			break;
		default:
			Errorf("LoadTGA(%s): Only 8, 15, 16, 24 or 32 bit images (with colormaps) supported\n", filename.c_str());
			return false;
		}

		r = g = b= a = l = 0; 

		/* if required, read the color map information. */ 
		ColorMap = NULL; 
		mapped = (header.image_type == TGA_Map || header.image_type == TGA_RLEMap || header.image_type == TGA_CompMap || header.image_type == TGA_CompMap4) && header.colormap_type == 1; 
		if (mapped) { 
			/* validate colormap size */ 
			switch (header.colormap_size) { 
			case 8: 
			case 15: 
			case 16: 
			case 32: 
			case 24: 
				break; 
			default: 
				Errorf("LoadTGA(%s): Only 8, 15, 16, 24 or 32 bit colormaps supported\n", filename.c_str()); 
				return false; 
			} 

			temp1 = header.colormap_index; 
			temp2 = header.colormap_length; 
			if ((temp1 + temp2 + 1) >= MAXCOLORS) { 
				g_fileSystem->freeFile(data); 
				return false; 
			} 
			ColorMap = new byte_t[MAXCOLORS * 4]; 
			map_idx = 0; 
			for (i = temp1; i < temp1 + temp2; ++i, map_idx += 4) { 
				/* read appropriate number of uint8s, break into rgb & put in map. */ 
				switch (header.colormap_size) { 
				case 8:	/* grey scale, read and triplicate. */ 
					r = g = b = *pdata++; 
					a = 255; 
					break; 
				case 15:	/* 5 bits each of red green and blue. */ 
					/* watch Uint8 order. */ 
					j = *pdata++; 
					k = *pdata++; 
					l = ((uint_t) k << 8) + j; 
					r = (byte_t) (((k & 0x7C) >> 2) << 3); 
					g = (byte_t) ((((k & 0x03) << 3) + ((j & 0xE0) >> 5)) << 3); 
					b = (byte_t) ((j & 0x1F) << 3); 
					a = 255; 
					break; 
				case 16:	/* 5 bits each of red green and blue, 1 alpha bit. */ 
					/* watch Uint8 order. */ 
					j = *pdata++; 
					k = *pdata++; 
					l = ((uint_t) k << 8) + j; 
					r = (byte_t) (((k & 0x7C) >> 2) << 3); 
					g = (byte_t) ((((k & 0x03) << 3) + ((j & 0xE0) >> 5)) << 3); 
					b = (byte_t) ((j & 0x1F) << 3); 
					a = (k & 0x80) ? 255 : 0; 
					break; 
				case 24:	/* 8 bits each of blue, green and red. */ 
					b = *pdata++; 
					g = *pdata++; 
					r = *pdata++; 
					a = 255; 
					l = 0; 
					break; 
				case 32:	/* 8 bits each of blue, green, red and alpha. */ 
					b = *pdata++; 
					g = *pdata++; 
					r = *pdata++; 
					a = *pdata++; 
					l = 0; 
					break; 
				} 
				ColorMap[map_idx + 0] = r; 
				ColorMap[map_idx + 1] = g; 
				ColorMap[map_idx + 2] = b; 
				ColorMap[map_idx + 3] = a; 
			} 
		} 

		/* check run-length encoding. */ 
		rlencoded = header.image_type == TGA_RLEMap || header.image_type == TGA_RLERGB || header.image_type == TGA_RLEMono; 
		RLE_count = 0; 
		RLE_flag = 0; 

		w = header.width; 
		h = header.height; 

		m_width = w; 
		m_height = h; 

		size = w * h * 4; 
		byte_t *pixels = new byte_t[size]; 
		memset(pixels, 0, size); 

		/* read the Targa file body and convert to portable format. */ 
		pixel_size = header.pixel_size; 
		origin = (header.attributes & 0x20) >> 5; 
		interleave = (header.attributes & 0xC0) >> 6; 
		truerow = 0; 
		baserow = 0; 
		for (y = 0; y < h; y++) { 
			realrow = truerow; 
			if (origin == TGA_O_UPPER) 
	//		if (origin == TGA_O_LOWER)		// HACK: why tga image is fliped? 
				realrow = h - realrow - 1; 

			dst = pixels + realrow * w * dstbpp; 

			for (x = 0; x < w; x++) { 
				/* check if run length encoded. */ 
				if (rlencoded) { 
					if (!RLE_count) { 
						/* have to restart run. */ 
						i = *pdata++; 
						RLE_flag = (i & 0x80); 
						if (!RLE_flag) { 
							/* stream of unencoded pixels. */ 
							RLE_count = i + 1; 
						} else { 
							/* single pixel replicated. */ 
							RLE_count = i - 127; 
						} 
						/* decrement count & get pixel. */ 
						--RLE_count; 
					} else { 
						/* have already read count & (at least) first pixel. */ 
						--RLE_count; 
						if (RLE_flag) 
							/* replicated pixels. */ 
							goto PixEncode; 
					} 
				} 

				/* read appropriate number of uint8s, break into RGB. */ 
				switch (pixel_size) { 
				case 8:	/* grey scale, read and triplicate. */ 
					r = g = b = l = *pdata++; 
					a = 255; 
					break; 
				case 15:	/* 5 bits each of red green and blue. */ 
					/* watch Uint8 order. */ 
					j = *pdata++; 
					k = *pdata++; 
					l = ((uint_t) k << 8) + j; 
					r = (byte_t) (((k & 0x7C) >> 2) << 3); 
					g = (byte_t) ((((k & 0x03) << 3) + ((j & 0xE0) >> 5)) << 3); 
					b = (byte_t) ((j & 0x1F) << 3); 
					a = 255; 
					break; 
				case 16:	/* 5 bits each of red green and blue, 1 alpha bit. */ 
					/* watch Uint8 order. */ 
					j = *pdata++; 
					k = *pdata++; 
					l = ((uint_t) k << 8) + j; 
					r = (byte_t) (((k & 0x7C) >> 2) << 3); 
					g = (byte_t) ((((k & 0x03) << 3) + ((j & 0xE0) >> 5)) << 3); 
					b = (byte_t) ((j & 0x1F) << 3); 
					//a = (k & 0x80) ? 255 : 0; 
					/* TODO */ 
					a = 255; 
					break; 
				case 24:	/* 8 bits each of blue, green and red. */ 
					b = *pdata++; 
					g = *pdata++; 
					r = *pdata++; 
					a = 255; 
					l = 0; 
					break; 
				case 32:	/* 8 bits each of blue, green, red and alpha. */ 
					b = *pdata++; 
					g = *pdata++; 
					r = *pdata++; 
					a = *pdata++; 
					l = 0; 
					break; 
				default: 
					Errorf("Illegal pixel_size '%d' in file '%s'\n", filename.c_str()); 
					return false; 
				} 

	PixEncode: 
				if (mapped) {
					if (m_format == TexFormat::BGR8) {
						map_idx = l * 4;
						*dst++ = ColorMap[map_idx + 2];
						*dst++ = ColorMap[map_idx + 1];
						*dst++ = ColorMap[map_idx + 0];
						continue;
					} 
					if (m_format == TexFormat::BGRA8 || m_format == TexFormat::BGRX8) { 
						map_idx = l * 4; 
						*dst++ = ColorMap[map_idx + 2]; 
						*dst++ = ColorMap[map_idx + 1]; 
						*dst++ = ColorMap[map_idx + 0]; 
						*dst++ = ColorMap[map_idx + 3];
						continue;
					} 
				} else { 
					if (m_format == TexFormat::BGR8) { 
						*dst++ = b; 
						*dst++ = g; 
						*dst++ = r; 
						continue;
					} 
					if (m_format == TexFormat::BGRA8 || m_format == TexFormat::BGRX8) { 
						*dst++ = b; 
						*dst++ = g; 
						*dst++ = r; 
						*dst++ = a; 
						continue;
					} 
				} 
			} 

			if (interleave == TGA_IL_Four) { 
				truerow += 4; 
			} else if (interleave == TGA_IL_Two) { 
				truerow += 2; 
			} else { 
				truerow++; 
			} 
			if (truerow >= h) 
				truerow = ++baserow; 
		} 

		if (mapped) { 
			delete[] ColorMap; 
		} 

		g_fileSystem->freeFile(data);

		m_datas.push_back(DataBuffer(pixels));

		return m_dataPresent = true; 
	}

	void Image::saveFile_tga(const String &filename) {
		if (m_format == TexFormat::BGR8) {
			WriteTGA24(filename, getData(0), m_width, m_height);

		} else if (m_format == TexFormat::BGRA8) {
			Image::writeTGA(filename, getData(0), m_width, m_height);
		}

	}


AX_END_NAMESPACE
