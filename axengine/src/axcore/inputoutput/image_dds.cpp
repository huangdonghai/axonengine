/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"


// dds格式支持

// 一、读取
// 1. 压缩格式仅支持DXT1、3、5，以及其他所有非压缩格式
//    DXT1、3、5 都转化为 Image::RGBA8/Image::BGRA8
// 2. 所有单通道文件都转化为 Image::L8， 双通道都认为是Image::LA8（因为在保存的时候信息丢失，DDS不支持Image::A8之类的格式），

// 二、保存
// 1. 浮点类型 和 DXT类型 原样保存
// 2. RGB 类型的：带通道的一律压缩保存为DXT5，不带通道的压缩保存为 DXT1 （因为 DXT3 和 DXT5 大小一样，质量不如 DXT5 好）

// 三、其他
// 1. 对 Cubemap 的支持：从文件读取时，支持全部格式，可以带任意张Cubemap图；
//						 写入到文件时，只能要么使用全部6张Cubemap图，要么不使用Cubemap；
// 2. 对 Mipmap 的支持：从文件读取时，支持任意张Mipmap；
//						写入到文件时，只能要么使用全部的Mipmap，要么不用Mipmap；

//Those 4 were added on 20040516 to make
//the written dds files more standard compliant
#define DDS_CAPS 0x00000001L
#define DDS_HEIGHT 0x00000002L
#define DDS_WIDTH 0x00000004L
#define DDS_RGB 0x00000040L
#define DDS_PIXELFORMAT 0x00001000L
#define DDS_LUMINANCE 0x00020000L

#define DDS_ALPHAPIXELS 0x00000001L
#define DDS_ALPHA 0x00000002L
#define DDS_FOURCC 0x00000004L
#define DDS_PITCH 0x00000008L
#define DDS_COMPLEX 0x00000008L
#define DDS_TEXTURE 0x00001000L
#define DDS_MIPMAPCOUNT 0x00020000L
#define DDS_LINEARSIZE 0x00080000L
#define DDS_VOLUME 0x00200000L
#define DDS_MIPMAP 0x00400000L
#define DDS_DEPTH 0x00800000L

#define DDS_CUBEMAP 0x00000200L
#define DDS_CUBEMAP_POSITIVEX 0x00000400L
#define DDS_CUBEMAP_NEGATIVEX 0x00000800L
#define DDS_CUBEMAP_POSITIVEY 0x00001000L
#define DDS_CUBEMAP_NEGATIVEY 0x00002000L
#define DDS_CUBEMAP_POSITIVEZ 0x00004000L
#define DDS_CUBEMAP_NEGATIVEZ 0x00008000L
#define DDS_CUBEMAP_ALL 0x0000FC00L

#define CUBEMAP_SIDES 6
#define DDS_DXT_BLOCK_SIZE 16

AX_BEGIN_NAMESPACE

	const uint_t FOURCC_DXT1 = AX_MAKEFOURCC('D','X','T','1');
	const uint_t FOURCC_DXT3 = AX_MAKEFOURCC('D','X','T','3');
	const uint_t FOURCC_DXT5 = AX_MAKEFOURCC('D','X','T','5');
	
	enum DdsFormatType {
		DDS_TYPE_ARGB,
		DDS_TYPE_DXT,
		DDS_TYPE_FLOAT,
	};

	struct DdsHeader {
		byte_t Signature[4];

		uint_t Size1;				// size of the structure (minus MagicNum)
		uint_t Flags1; 			// determines what fields are valid
		uint_t Height; 			// height of surface to be created
		uint_t Width;				// width of input surface
		uint_t LinearSize; 		// Formless late-allocated optimized surface size
		uint_t Depth;				// Depth if a volume texture
		uint_t MipMapCount;		// number of mip-map levels requested
		uint_t AlphaBitDepth;		// depth of alpha buffer requested

		uint_t NotUsed[10];

		uint_t Size2;				// size of structure
		uint_t Flags2;				// pixel format flags
		uint_t FourCC;				// (FOURCC code)
		uint_t RGBBitCount;		// how many bits per pixel
		uint_t RBitMask;			// mask for red bit
		uint_t GBitMask;			// mask for green bits
		uint_t BBitMask;			// mask for blue bits
		uint_t ABitMask;			// mask for alpha channel

		uint_t ddsCaps1, ddsCaps2, ddsCaps3, ddsCaps4; // direct draw surface capabilities
		uint_t TextureStage;
	};

	enum DdsPixFormat {
		PF_ARGB,
		PF_RGB,
		PF_LUMINANCE,
		PF_LUMINANCE_ALPHA,

		PF_R16F,
		PF_G16R16F,
		PF_A16B16G16R16F,
		PF_R32F,
		PF_G32R32F,
		PF_A32B32G32R32F,

		PF_DXT1,
		PF_DXT3,
		PF_DXT5,

		// not support
		//PF_DXT2,
		//PF_DXT4,
		//PF_A16B16G16R16,
		//PF_3DC,
		//PF_ATI1N,
		//PF_RXGB, //Doom3 normal maps
		//PF_UNKNOWN = 0xFF
	};

	// 除了早期从 DdsHeader 中判断格式，计算参数之外，其他部分的函数都使用从 DdsReadParams 中传递过来的参数
	struct DdsReadParams {
		DdsPixFormat format;
		uint_t formatType;
		int loadFlag;
		TexFormat outFormat;

		byte_t *inData;
		uint_t inDataSize;			// current org data block's size
		uint_t inDataSizeAll;

		byte_t *outData;
		uint_t outDataSize;

		uint_t width,height,depth;
		uint_t linearSize; 		// Formless late-allocated optimized surface size

		uint_t inbpp;		// byte per pixel
		uint_t inbpl;		// byte per line
		uint_t inbpd;		// byte per page(depth)

		uint_t outbpp;		// byte per pixel
		uint_t outbpl;		// byte per line
		uint_t outbpd;		// byte per page(depth)
	};

	static void Color565To888(ushort_t c565, Rgba *out) {
		out->b = c565 & 0x1f;
		out->g = (c565 & 0x7E0) >> 5;
		out->r = (c565 & 0xF800) >> 11;

		out->r <<= 3;
		out->g <<= 2;
		out->b <<= 3;

		out->r |= out->r >> 5;
		out->g |= out->g >> 6;
		out->b |= out->b >> 5;
	}

	static bool DecompressDXT1(DdsReadParams *pParams) {
		Rgba colours[4];
		byte_t *pInData = pParams->inData;
		colours[0].a = 0xFF;
		colours[1].a = 0xFF;
		colours[2].a = 0xFF;

		for (int z = 0; z < (int)pParams->depth; z++) {
			for (int y = 0; y < (int)pParams->height; y += 4) {
				for (int x = 0; x < (int)pParams->width; x += 4) {
					ushort_t color_0 = LittleShort(*(ushort_t*)pInData);
					ushort_t color_1 = LittleShort(*(ushort_t*)(pInData+2));
					uint_t bitmask = LittleInt(*(uint_t*)(pInData+4));
					pInData += 8;

					Color565To888(color_0, colours);
					Color565To888(color_1, colours + 1);

					if (color_0 > color_1) {
						// Four-color block: derive the other two colors.
						// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
						// These 2-bit codes correspond to the 2-bit fields 
						// stored in the 64-bit block.
						colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
						colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
						colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;

						colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
						colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
						colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
						colours[3].a = 0xFF;
					} else { 
						// Three-color block: derive the other color.
						// 00 = color_0,  01 = color_1,  10 = color_2,
						// 11 = transparent.
						// These 2-bit codes correspond to the 2-bit fields 
						// stored in the 64-bit block. 
						colours[2].b = (colours[0].b + colours[1].b) / 2;
						colours[2].g = (colours[0].g + colours[1].g) / 2;
						colours[2].r = (colours[0].r + colours[1].r) / 2;
						colours[3].a = 0x00;
					}

					for (int j = 0, k = 0; j < 4; j++) {
						for (int i = 0; i < 4; i++, k++) {
							if (((x + i) < (int)pParams->width) && ((y + j) < (int)pParams->height)) {
								uint_t Select = (bitmask & (0x03 << k*2)) >> k*2;
								uint_t Offset = z * pParams->outbpd + (y + j) * pParams->outbpl + (x + i) * pParams->outbpp;
								*(Rgba*)(pParams->outData + Offset) = colours[Select];
							}
						}
					}
				}
			}
		}
		pParams->inData = pInData;

		return true;
	}

	static bool DecompressDXT3(DdsReadParams *pParams) {
		Rgba colours[4];
		byte_t *pInData = pParams->inData;

		for (int z = 0; z < (int)pParams->depth; z++) {
			for (int y = 0; y < (int)pParams->height; y += 4) {
				for (int x = 0; x < (int)pParams->width; x += 4) {

					byte_t *pAlpha = pInData;
					pInData += 8;
					ushort_t color_0 = LittleShort(*(ushort_t*)pInData);
					ushort_t color_1 = LittleShort(*(ushort_t*)(pInData+2));
					uint_t bitmask = LittleInt(*(uint_t*)(pInData+4));
					pInData += 8;

					Color565To888(color_0, colours);
					Color565To888(color_1, colours + 1);

					// Four-color block: derive the other two colors.    
					// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
					// These 2-bit codes correspond to the 2-bit fields 
					// stored in the 64-bit block.
					colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
					colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
					colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;

					colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
					colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
					colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;

					for (int k = 0, j = 0; j < 4; j++) {
						for (int i = 0; i < 4; i++, k++) {

							byte_t alpha = *pAlpha;
							if (i & 1){
								pAlpha++;
								alpha >>= 4;
							}
							if (((x + i) < (int)pParams->width) && ((y + j) < (int)pParams->height)) {
								uint_t Select = (bitmask & (0x03 << k*2)) >> k*2;
								uint_t Offset = z * pParams->outbpd + (y + j) * pParams->outbpl + (x + i) * pParams->outbpp;
								*(Rgba*)(pParams->outData + Offset) = colours[Select];

								alpha &= 0xf;
								alpha |= alpha << 4;
								*(pParams->outData + Offset + 3) = alpha;
							}
						}
					}// for (int k = 0, j = 0; j < 4; j++) {
				}
			}
		}
		pParams->inData = pInData;

		return true;
	}

	static bool DecompressDXT5(DdsReadParams *pParams) {
		Rgba colours[4];
		byte_t 		alphas[8];
		byte_t *pInData = pParams->inData;

		for (int z = 0; z < (int)pParams->depth; z++) {
			for (int y = 0; y < (int)pParams->height; y += 4) {
				for (int x = 0; x < (int)pParams->width; x += 4) {

					alphas[0] = pInData[0];
					alphas[1] = pInData[1];

					byte_t *alphamask = pInData + 2;
					pInData += 8;

					ushort_t color_0 = LittleShort(*(ushort_t*)pInData);
					ushort_t color_1 = LittleShort(*(ushort_t*)(pInData+2));
					uint_t bitmask = LittleInt(*(uint_t*)(pInData+4));
					pInData += 8;

					Color565To888(color_0, colours);
					Color565To888(color_1, colours + 1);

					// Four-color block: derive the other two colors.    
					// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
					// These 2-bit codes correspond to the 2-bit fields 
					// stored in the 64-bit block.
					colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
					colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
					colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;

					colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
					colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
					colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;

					// 8-alpha or 6-alpha block?    
					if (alphas[0] > alphas[1]) {    
						// 8-alpha block:  derive the other six alphas.    
						// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
						alphas[2] = (6 * alphas[0] + 1 * alphas[1] + 3) / 7;	// bit code 010
						alphas[3] = (5 * alphas[0] + 2 * alphas[1] + 3) / 7;	// bit code 011
						alphas[4] = (4 * alphas[0] + 3 * alphas[1] + 3) / 7;	// bit code 100
						alphas[5] = (3 * alphas[0] + 4 * alphas[1] + 3) / 7;	// bit code 101
						alphas[6] = (2 * alphas[0] + 5 * alphas[1] + 3) / 7;	// bit code 110
						alphas[7] = (1 * alphas[0] + 6 * alphas[1] + 3) / 7;	// bit code 111
					} else {
						// 6-alpha block.
						// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
						alphas[2] = (4 * alphas[0] + 1 * alphas[1] + 2) / 5;	// Bit code 010
						alphas[3] = (3 * alphas[0] + 2 * alphas[1] + 2) / 5;	// Bit code 011
						alphas[4] = (2 * alphas[0] + 3 * alphas[1] + 2) / 5;	// Bit code 100
						alphas[5] = (1 * alphas[0] + 4 * alphas[1] + 2) / 5;	// Bit code 101
						alphas[6] = 0x00;										// Bit code 110
						alphas[7] = 0xFF;										// Bit code 111
					}

					uint_t bits = (alphamask[0]) | (alphamask[1] << 8) | (alphamask[2] << 16);

					for (int k = 0, j = 0; j < 4; j++) {
						if (j == 2)
							bits = (alphamask[3]) | (alphamask[4] << 8) | (alphamask[5] << 16);
						for (int i = 0; i < 4; i++, k++) {

							if (((x + i) < (int)pParams->width) && ((y + j) < (int)pParams->height)) {
								uint_t Select = (bitmask & (0x03 << k*2)) >> k*2;
								uint_t Offset = z * pParams->outbpd + (y + j) * pParams->outbpl + (x + i) * pParams->outbpp;
								*(Rgba*)(pParams->outData + Offset) = colours[Select];
								*(pParams->outData + Offset + 3) = alphas[bits & 0x07];
							}
							bits >>= 3;
						}
					}
				}
			}
		}
		pParams->inData = pInData;

		return true;
	}

	static void ChangeHeaderIntOrder(DdsHeader *pHeader){
		pHeader->Size1 = LittleInt(pHeader->Size1);
		pHeader->Size2 = LittleInt(pHeader->Size2);
		pHeader->Flags1 = LittleInt(pHeader->Flags1);
		pHeader->Flags2 = LittleInt(pHeader->Flags2);
		pHeader->Width = LittleInt(pHeader->Width);
		pHeader->Height = LittleInt(pHeader->Height);
		pHeader->Depth = LittleInt(pHeader->Depth);
		pHeader->ddsCaps1 = LittleInt(pHeader->ddsCaps1);
		pHeader->ddsCaps2 = LittleInt(pHeader->ddsCaps2);
		pHeader->ddsCaps3 = LittleInt(pHeader->ddsCaps3);
		pHeader->ddsCaps4 = LittleInt(pHeader->ddsCaps4);
		pHeader->RBitMask = LittleInt(pHeader->RBitMask);
		pHeader->GBitMask = LittleInt(pHeader->GBitMask);
		pHeader->BBitMask = LittleInt(pHeader->BBitMask);
		pHeader->ABitMask = LittleInt(pHeader->ABitMask);
		pHeader->RGBBitCount = LittleInt(pHeader->RGBBitCount);
		pHeader->LinearSize = LittleInt(pHeader->LinearSize);
		pHeader->MipMapCount = LittleInt(pHeader->MipMapCount);
		pHeader->AlphaBitDepth = LittleInt(pHeader->AlphaBitDepth);
		pHeader->TextureStage = LittleInt(pHeader->TextureStage);
	}

	static bool CheckHeaderIsValid(DdsHeader *pHeader){
		if (*(uint_t*)pHeader->Signature != AX_MAKEFOURCC('D', 'D', 'S', ' '))
			return false;
		//if (Strnicmp((const char*)pHeader->Signature, "DDS ", 4))
		//	return false;
		//note that if Size1 is "DDS " this is not a valid dds file according
		//to the file spec. Some broken tool out there seems to produce files
		//with this value in the size field, so we support reading them...
		if (pHeader->Size1 != 124)
			return false;
		if (pHeader->Size2 != 32)
			return false;
		if (pHeader->Width == 0 || pHeader->Height == 0)
			return false;

		return true;
	}

	static bool GetFormatAndBlockSize(DdsHeader *pHeader, DdsReadParams *pParams) {
		if (pHeader->Flags2 & DDS_FOURCC) {
			uint_t size1 = ((pHeader->Width + 3)/4) * ((pHeader->Height + 3)/4) * pHeader->Depth;
			uint_t size2 = pHeader->Width * pHeader->Height * pHeader->Depth;
			switch (pHeader->FourCC){
			case AX_MAKEFOURCC('D','X','T','1'):
				pParams->format = PF_DXT1;
				pParams->formatType = DDS_TYPE_DXT;
				pParams->inDataSize = size1 * 8;
				break;
			case AX_MAKEFOURCC('D','X','T','3'):
				pParams->format = PF_DXT3;
				pParams->formatType = DDS_TYPE_DXT;
				pParams->inDataSize = size1 * 16;
				break;
			case AX_MAKEFOURCC('D','X','T','5'):
				pParams->format = PF_DXT5;
				pParams->formatType = DDS_TYPE_DXT;
				pParams->inDataSize = size1 * 16;
				break;
			case AX_MAKEFOURCC('o', '\0', '\0', '\0'):
				pParams->format = PF_R16F;
				pParams->formatType = DDS_TYPE_FLOAT;
				pParams->inDataSize = size2 * 2;
				break;
			case AX_MAKEFOURCC('p', '\0', '\0', '\0'):
				pParams->format = PF_G16R16F;
				pParams->formatType = DDS_TYPE_FLOAT;
				pParams->inDataSize = size2 * 4;
				break;
			case AX_MAKEFOURCC('q', '\0', '\0', '\0'):
				pParams->format = PF_A16B16G16R16F;
				pParams->formatType = DDS_TYPE_FLOAT;
				pParams->inDataSize = size2 * 8;
				break;
			case AX_MAKEFOURCC('r', '\0', '\0', '\0'):
				pParams->format = PF_R32F;
				pParams->formatType = DDS_TYPE_FLOAT;
				pParams->inDataSize = size2 * 4;
				break;
			case AX_MAKEFOURCC('s', '\0', '\0', '\0'):
				pParams->format = PF_G32R32F;
				pParams->formatType = DDS_TYPE_FLOAT;
				pParams->inDataSize = size2 * 8;
				break;
			case AX_MAKEFOURCC('t', '\0', '\0', '\0'):
				pParams->format = PF_A32B32G32R32F;
				pParams->formatType = DDS_TYPE_FLOAT;
				pParams->inDataSize = size2 * 16;
				break;
			default:
				return false; 
			}
		} else {
			pParams->formatType = DDS_TYPE_ARGB;
			// This dds texture isn't compressed so write out ARGB or luminance format
			if (pHeader->Flags2 & DDS_LUMINANCE) {
				if (pHeader->Flags2 & DDS_ALPHAPIXELS) {
					pParams->format = PF_LUMINANCE_ALPHA;
				} else {
					pParams->format = PF_LUMINANCE;
				}
			} else if (pHeader->Flags2 & DDS_RGB) {
				if (pHeader->Flags2 & DDS_ALPHAPIXELS) {
					pParams->format = PF_ARGB;
				} else {
					pParams->format = PF_RGB;
				}
			} else {
				return false;
			}
			pParams->inDataSize = (pHeader->Width * pHeader->Height * pHeader->Depth * (pHeader->RGBBitCount >> 3));
		}
		return true;
	}

	static uint_t GetBytePerPixel(DdsPixFormat format, uint_t bitCount) {
		switch (format) {
			case PF_LUMINANCE:
			case PF_LUMINANCE_ALPHA:
			case PF_ARGB:
				return bitCount / 8;
			case PF_R16F:
				return 2;
			case PF_RGB:	// PF_RXGB
				return 3;
			//case PF_A16B16G16R16:
			case PF_A16B16G16R16F:
			case PF_G32R32F:
				return 8;
			case PF_A32B32G32R32F:
				return 16;
			default:// PF_G16R16F  PF_R32F dxt
				return 4;
		}
	}

	static uint_t GetCompFactor(DdsPixFormat format) {
		//This doesn't work for images which first mipmap (= the image itself) has width or height < 4
		//if (header.Flags1 & DDS_LINEARSIZE) {
		//	CompFactor = (Width * Height * Depth * Bpp) / header.LinearSize;
		//}
		switch (format){
		//This is officially 6, we have 8 here because DXT1 may contain alpha
		case PF_DXT1:		return 8;
		//case PF_DXT2:
		case PF_DXT3:
		//case PF_DXT4:
		case PF_DXT5:		return 4;
		//This is officially 4 for 3dc, but that's bullshit :) There's no alpha data in 3dc images
		//case PF_RXGB:
		//case PF_3DC:		return 3;
		//case PF_ATI1N:		return 2;
		default:			return 1;
		}
	}

	static void RepairLinearSize(DdsHeader *pHeader, DdsReadParams *pParams) {
		if (!(pHeader->ddsCaps2 & DDS_VOLUME))
			pParams->depth = 1;

		switch (pParams->format){
		case PF_ARGB:
		case PF_RGB:
		case PF_LUMINANCE:
		case PF_LUMINANCE_ALPHA:
			//don't use the iCompFormatToBpp() function because this way
			//argb images with only 8 bits (eg. a1r2g3b2) work as well
			pParams->linearSize = std::max<uint_t>(1,pParams->width) * std::max<uint_t>(1,pParams->height) * (pHeader->RGBBitCount / 8);
			break;

		case PF_DXT1:
			pParams->linearSize = ((pParams->width+3)/4) * ((pParams->height+3)/4) * 8;
			break;

		case PF_DXT3:
		case PF_DXT5:
		//case PF_RXGB:
			pParams->linearSize = ((pParams->width+3)/4) * ((pParams->height+3)/4) * 16;
			break;

		//case PF_A16B16G16R16:
		case PF_R16F:
		case PF_G16R16F:
		case PF_A16B16G16R16F:
		case PF_R32F:
		case PF_G32R32F:
		case PF_A32B32G32R32F:
			pParams->linearSize = std::max<uint_t>(1,pParams->width) * std::max<uint_t>(1,pParams->height) * pParams->inbpp;
			break;
		default:
			AX_NO_DEFAULT;
			break;
		}

		pHeader->Flags1 |= DDS_LINEARSIZE;
		pParams->linearSize *= pParams->depth;
	}

	static uint_t GetMipmapLinearSize(DdsHeader *pHeader, DdsReadParams *pParams, uint_t factor) {

		if (!(pHeader->Flags1 & DDS_LINEARSIZE))
			return pParams->linearSize >> 1;

		else if (pParams->formatType == DDS_TYPE_FLOAT){
				//pParams->outbpp = channels;
			return pParams->width * pParams->height * pParams->depth * pParams->inbpp;

		//} else if (pParams->format == PF_A16B16G16R16){
		//	pParams->linearSize = pParams->width * pParams->height * pParams->depth * pParams->inbpp;
		} else if (pParams->formatType == DDS_TYPE_DXT) {
				//compressed format
				uint_t minW = (pParams->width + 3) & ~3;
				uint_t minH = (pParams->height + 3) & ~3;
				return (minW * minH * pParams->depth * pParams->inbpp) / factor;
		} // else
		//don't use Bpp to support argb images with less than 32 bits
		return pParams->width * pParams->height * pParams->depth * (pHeader->RGBBitCount / 8);
	}

	static void GetBitsFromMask(uint_t mask, byte_t *shiftLeft, byte_t *shiftRight) {
		if (mask == 0) {
			*shiftLeft = *shiftRight = 0;
			return;
		}

		int i;
		for (i = 0; i < 32; i++, mask >>= 1) {
			if (mask & 1)
				break;
		}
		*shiftRight = i;

		// Temp is preserved, so use it again:
		for (i = 0; i < 32; i++, mask >>= 1) {
			if (!(mask & 1))
				break;
		}
		if (i <= 8) {
			*shiftLeft = 8 - i;
		}else{
			*shiftLeft = 0;
			*shiftRight += i - 8;
		}
	}

	static bool DecompressARGB(DdsHeader *pHeader, DdsReadParams *pParams) {
		byte_t RedL,RedR,GreenL,GreenR,BlueL,BlueR,AlphaL,AlphaR;
		GetBitsFromMask(pHeader->RBitMask, &RedL, &RedR);
		GetBitsFromMask(pHeader->GBitMask, &GreenL, &GreenR);
		GetBitsFromMask(pHeader->BBitMask, &BlueL, &BlueR);
		GetBitsFromMask(pHeader->ABitMask, &AlphaL, &AlphaR);

		pParams->inbpp = pHeader->RGBBitCount / 8;
	#if 0
		int count = pParams->inDataSize / pParams->inbpp;
	#else
		int count = pParams->outDataSize / pParams->outbpp;
		byte_t *inptr = pParams->inData;
		pParams->inData += pParams->linearSize;
		byte_t *outptr = pParams->outData;
	#endif
		switch (pParams->outbpp){
		case 1:
			for (int i = 0; i < count; i++) {
				uint_t pixel = LittleInt(*(uint_t*)inptr);
				inptr += pParams->inbpp;
				*outptr++ = ((pixel & pHeader->RBitMask) >> RedR) << RedL;
			}
			break;
		case 2:
			for (int i = 0; i < count; i++) {
				uint_t pixel = LittleInt(*(uint_t*)inptr);
				inptr += pParams->inbpp;
				*outptr++ = ((pixel & pHeader->RBitMask) >> RedR) << RedL;
				byte_t alpha = ((pixel & pHeader->ABitMask) >> AlphaR) << AlphaL;
				if (AlphaL >= 7)
					alpha = alpha? 0xff : 0x00;
				else if (AlphaL >= 4)
					alpha |= alpha >> 4;
				 *outptr++ = alpha;
			}
			break;
		case 3:
			for (int i = 0; i < count; i++) {
				uint_t pixel = LittleInt(*(uint_t*)inptr);
				inptr += pParams->inbpp;
				*outptr++ = ((pixel & pHeader->BBitMask) >> BlueR) << BlueL;
				*outptr++ = ((pixel & pHeader->GBitMask) >> GreenR) << GreenL;
				*outptr++ = ((pixel & pHeader->RBitMask) >> RedR) << RedL;
			}
			break;
		case 4:
			for (int i = 0; i < count; i++) {
				uint_t pixel = LittleInt(*(uint_t*)inptr);
				inptr += pParams->inbpp;
				*outptr++ = ((pixel & pHeader->BBitMask) >> BlueR) << BlueL;
				*outptr++ = ((pixel & pHeader->GBitMask) >> GreenR) << GreenL;
				*outptr++ = ((pixel & pHeader->RBitMask) >> RedR) << RedL;

				if (pParams->outFormat == TexFormat::BGRX8) {
					*outptr++ = 255;
					continue;
				}
				byte_t alpha = ((pixel & pHeader->ABitMask) >> AlphaR) << AlphaL;
				if (AlphaL >= 7)
					alpha = alpha? 0xff : 0x00;
				else if (AlphaL >= 4)
					alpha |= alpha >> 4;
				*outptr++ = alpha;
			}
			break;
		default:	AX_NO_DEFAULT;	break;
		}
		return true;
	}

	static bool DecompressDdsData(DdsHeader *pHeader, DdsReadParams *pParams) {
		switch (pParams->format){
		case PF_ARGB:
		case PF_RGB:
		case PF_LUMINANCE:
		case PF_LUMINANCE_ALPHA:
			return DecompressARGB(pHeader, pParams);
		case PF_DXT1:
			if (pParams->loadFlag & Image::NoCompressed)
				return DecompressDXT1(pParams);
			else{
				for (uint_t i=0; i<pParams->outDataSize; i+=8) {
					*(ushort_t*)(pParams->outData + i)		= LittleShort(*(ushort_t*)(pParams->inData + i));
					*(ushort_t*)(pParams->outData + i + 2)	= LittleShort(*(ushort_t*)(pParams->inData + i + 2));
					*(uint_t*)(pParams->outData + i + 4)		= LittleInt(*(uint_t*)(pParams->inData + i + 4));
				}
				pParams->inData += pParams->outDataSize;
			}
			break;
		case PF_DXT3:
		case PF_DXT5:
			if (pParams->loadFlag & Image::NoCompressed) {
				switch (pParams->format){
				case PF_DXT3:	return DecompressDXT3(pParams);
				case PF_DXT5:	return DecompressDXT5(pParams);
				default: AX_NO_DEFAULT; return true;
				}
			} else {
				for (uint_t i=0; i<pParams->outDataSize; i+=16) {
					*(uint_t*)(pParams->outData + i + 0)	= *(uint_t*)(pParams->inData + i + 0);
					*(uint_t*)(pParams->outData + i + 4)	= *(uint_t*)(pParams->inData + i + 4);
					*(ushort_t*)(pParams->outData + i + 8)	= LittleShort(*(ushort_t*)(pParams->inData + i + 8));
					*(ushort_t*)(pParams->outData + i + 10)	= LittleShort(*(ushort_t*)(pParams->inData + i + 10));
					*(uint_t*)(pParams->outData + i + 12)	= LittleInt(*(uint_t*)(pParams->inData + i + 12));
				}
				pParams->inData += pParams->outDataSize;
			}
			break;
			// else memcpy, so run to next case
		//case PF_A16B16G16R16:
		case PF_R32F:
		case PF_G32R32F:
		case PF_A32B32G32R32F:
			for (uint_t i=0; i<pParams->outDataSize; i+=4)
				*(uint_t*)(pParams->outData + i)	= LittleInt(*(uint_t*)(pParams->inData + i));
			pParams->inData += pParams->outDataSize;
			break;
		case PF_R16F:
		case PF_G16R16F:
		case PF_A16B16G16R16F:
			for (uint_t i=0; i<pParams->outDataSize; i+=2)
				*(ushort_t*)(pParams->outData + i)	= LittleShort(*(ushort_t*)(pParams->inData + i));
			pParams->inData += pParams->outDataSize;
			break;
			
		default:	AX_NO_DEFAULT;		return true;
		}
		return true;
	}

	static void GetOutFormat(DdsReadParams *pParams){
		switch (pParams->formatType){
		case DDS_TYPE_ARGB:
			switch (pParams->format) {
			case PF_ARGB:
				pParams->outFormat = TexFormat::BGRA8;	pParams->outbpp = 4;
				break;
			case PF_RGB:
				if (pParams->loadFlag & Image::ExpandAlpha) {
					pParams->outFormat = TexFormat::BGRX8;	pParams->outbpp = 4;
				} else {
					pParams->outFormat = TexFormat::BGR8;	pParams->outbpp = 3;
				}
				break;
			case PF_LUMINANCE:
				pParams->outFormat = TexFormat::L8;	pParams->outbpp = 1;
				break;
			case PF_LUMINANCE_ALPHA:
				pParams->outFormat = TexFormat::LA8;	pParams->outbpp = 2;
				break;
			default:
				AX_NO_DEFAULT;
				break;
			}
			break;

		case DDS_TYPE_DXT:
			if (pParams->loadFlag & Image::NoCompressed) {
				pParams->outFormat = TexFormat::BGRA8;
				pParams->outbpp = 4;
			} else {
				switch (pParams->format){
				case PF_DXT1:	pParams->outFormat = TexFormat::DXT1;	break;
				case PF_DXT3:	pParams->outFormat = TexFormat::DXT3;	break;
				case PF_DXT5:	pParams->outFormat = TexFormat::DXT5;	break;
				default: AX_NO_DEFAULT; break;
				}
			}
			break;
		case DDS_TYPE_FLOAT:
			switch (pParams->format) {
			case PF_R32F:
				pParams->outFormat = TexFormat::R32F;	pParams->outbpp = 4;
				break;
			case PF_G32R32F:
				pParams->outFormat = TexFormat::RG32F;	pParams->outbpp = 8;
				break;
			case PF_A32B32G32R32F:
				pParams->outFormat = TexFormat::RGBA32F;	pParams->outbpp = 16;
				break;
			case PF_R16F:
				pParams->outFormat = TexFormat::R16F;	pParams->outbpp = 2;
				break;
			case PF_G16R16F:
				pParams->outFormat = TexFormat::RG16F;	pParams->outbpp = 4;
				break;
			case PF_A16B16G16R16F:
				pParams->outFormat = TexFormat::RGBA16F;	pParams->outbpp = 8;
				break;
			default:
				AX_NO_DEFAULT;
				break;
			}
			break;

		default:
			AX_NO_DEFAULT;
			break;
		}
	}

	static bool AllocImage(DdsHeader *pHeader, DdsReadParams *pParams){
		if (pParams->loadFlag & Image::NoCompressed || pParams->outFormat.isByte()) {
			pParams->outbpl = pParams->outbpp * pParams->width;
			pParams->outbpd = pParams->outbpl * pParams->height;
			pParams->outDataSize = pParams->outbpd * pParams->depth;
		} else if (pHeader->Flags1 & DDS_LINEARSIZE) {
			pParams->outDataSize = pParams->linearSize;
		} else {
			pParams->outbpl = pParams->width * pHeader->RGBBitCount / 8;
			pParams->outDataSize = pParams->outbpl * pParams->height * pParams->depth;
		}

		AX_ASSERT(pParams->outDataSize > 0);
		pParams->outData = new byte_t[pParams->outDataSize];
		if (!pParams->outData)
			return false;
		memset(pParams->outData, 0, pParams->outDataSize);
		return true;
	}

	static void CheckRGBOrder(DdsReadParams *pParams){
		if (!(pParams->loadFlag & Image::RgbOrder))
			return;

		int channels = 0;
		if (pParams->outFormat == TexFormat::BGRA8)
			channels = 4;
		else if (pParams->outFormat == TexFormat::BGR8)
			channels = 3;
		else
			return;

		uint_t pixelCount = pParams->height * pParams->width * pParams->depth;
		byte_t *pBuf = pParams->outData;
		for (uint_t i=0; i<pixelCount; i++) {
			byte_t t = pBuf[0];
			pBuf[0] = pBuf[2];
			pBuf[2] = t;
			pBuf += channels;
		}
	}

	static bool Load_dds_data(DdsHeader *pHeader, DdsReadParams *pParams, Image::DataBufferSeq &dataBufs){
		if (!AllocImage(pHeader, pParams))
			return false;

		if (!DecompressDdsData(pHeader, pParams))
			return false;

		CheckRGBOrder(pParams);

		dataBufs.push_back(shared_array<byte_t>(pParams->outData));

		//Read mipmaps
		uint_t factor = GetCompFactor(pParams->format);

		uint_t lastLinearSize = pParams->linearSize;
		for (int i = 0; i < (int)pHeader->MipMapCount - 1; i++) {
			pParams->depth = std::max<uint_t>(1, pParams->depth / 2);
			pParams->width = std::max<uint_t>(1, pParams->width / 2);
			pParams->height = std::max<uint_t>(1, pParams->height / 2);

			pParams->linearSize = GetMipmapLinearSize(pHeader, pParams, factor);

			if (!AllocImage(pHeader, pParams))
				return false;

			if (!DecompressDdsData(pHeader, pParams))
				return false;

			CheckRGBOrder(pParams);

			dataBufs.push_back(shared_array<byte_t>(pParams->outData));
		}

		pParams->linearSize = lastLinearSize;
		return true;
	}

	bool Image::loadFile_dds(const String &filename){
		// clear first
		clear();

		m_realName = filename;

		DdsReadParams params;
		params.inDataSizeAll = (uint_t)g_fileSystem->readFile(filename, (void**)&params.inData);
		if (!params.inData)
			return false;

		// auto free file data
		class TempFreeFile {
			byte_t *mpFileData;
		public:
			TempFreeFile(byte_t *pFileData):mpFileData(pFileData){}
			~TempFreeFile(){ g_fileSystem->freeFile(mpFileData); }
		};
		TempFreeFile t(params.inData);

		DdsHeader header;
		header = *(DdsHeader*)params.inData;
		params.inData += sizeof(DdsHeader);
		ChangeHeaderIntOrder(&header);
		if (header.Depth == 0)
			header.Depth = 1;

		if (!CheckHeaderIsValid(&header))
			return false;

		if (!GetFormatAndBlockSize(&header, &params)){
			Errorf("Load DDS(%s): Only suport type DXT1,DXT3,DXT5\n", m_realName.c_str());
			return false;
		}

		params.loadFlag = m_loadFlags;
		params.inbpp = GetBytePerPixel(params.format, header.RGBBitCount);

		m_width = header.Width;
		m_height = header.Height;
		m_depth = header.Depth;
		params.width = header.Width;
		params.height = header.Height;
		params.depth = header.Depth;
		params.linearSize = header.LinearSize;

		// Microsoft bug, they're not following their own documentation.
		if (!(header.Flags1 & (DDS_LINEARSIZE | DDS_PITCH))|| header.LinearSize == 0) {
			header.Flags1 |= DDS_LINEARSIZE;
			params.linearSize = params.inDataSize;
		}

		if (!(header.Flags1 & DDS_MIPMAPCOUNT) || header.MipMapCount == 0)
			header.MipMapCount = 1;

		GetOutFormat(&params);

		if (header.ddsCaps1 & DDS_COMPLEX && header.ddsCaps2 & DDS_CUBEMAP) {
			int CubemapDirections[CUBEMAP_SIDES] = {
				DDS_CUBEMAP_POSITIVEX,
				DDS_CUBEMAP_NEGATIVEX,
				DDS_CUBEMAP_POSITIVEY,
				DDS_CUBEMAP_NEGATIVEY,
				DDS_CUBEMAP_POSITIVEZ,
				DDS_CUBEMAP_NEGATIVEZ
			};

			for (int i = 0; i < CUBEMAP_SIDES; i++) {
				AX_ASSERT(header.ddsCaps2 & CubemapDirections[i]);
				if (header.ddsCaps2 & CubemapDirections[i]) {
					params.width = header.Width;
					params.height = header.Height;
					params.depth = header.Depth;
					if (!Load_dds_data(&header, &params, m_datas))
						return false;
				}
			}
		} else {
			if (params.depth > 1) 
				RepairLinearSize(&header, &params);

			if (!Load_dds_data(&header, &params, m_datas))
				return false;
		}

		if ((params.loadFlag & Image::RgbOrder)){
			if (params.outFormat == TexFormat::BGRA8)
				params.outFormat = TexFormat::RGBA8;
			else if (params.outFormat == TexFormat::BGR8)
				params.outFormat = TexFormat::RGB8;
		}
		m_format = params.outFormat;
		m_dataPresent = true;

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// write

	struct DdsWriteParams {
		TexFormat orgFormat;
		DdsPixFormat outFormat;
		uint_t formatType;

		bool needCompress;
		int loadflags;

		const byte_t *inData;
		uint_t inDataSize;			// current org data block's size

		byte_t *outData;
		uint_t outDataSize;
		File *outfp;

		uint_t width,height,depth;
		uint_t linearSize; 		// Formless late-allocated optimized surface size

		bool bFast;
	};


	static uint_t GetMipmapCount(uint_t length){
		AX_ASSERT(length > 0);
		int count = 0;
		while (length > 0) {
			length >>= 1;
			count++;
		}
		return count;
	}

	static bool FillHeader(DdsHeader *pHeader, DdsWriteParams *pParams, uint_t width, uint_t height, uint_t depth, uint_t dataCount, TexFormat format){
		memset(pParams, 0, sizeof(DdsWriteParams));
		memset(pHeader, 0, sizeof(DdsHeader));
		pHeader->Flags1 = DDS_LINEARSIZE | DDS_MIPMAPCOUNT | DDS_WIDTH | DDS_HEIGHT | DDS_CAPS | DDS_PIXELFORMAT;
		pHeader->Flags2 = DDS_FOURCC;

		*(uint_t*)pHeader->Signature = AX_MAKEFOURCC('D', 'D', 'S', ' ');
		pHeader->Size1 = 124;
		pHeader->Size2 = 32;

		pHeader->Width = width;
		pHeader->Height = height;

		if (depth == 0)
			depth = 1;
		pHeader->Depth = depth;
		if (depth > 1) {
			pHeader->ddsCaps2 |= DDS_VOLUME;
			pHeader->Flags1 |= DDS_DEPTH;
		}

		DdsPixFormat outFormat;
		switch (format) {
		case TexFormat::DXT1:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','1');		outFormat = PF_DXT1; break;
		case TexFormat::DXT3:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','3');		outFormat = PF_DXT3; break;
		case TexFormat::DXT5:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','5');		outFormat = PF_DXT5; break;

		case TexFormat::RGB8:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','1');		outFormat = PF_DXT1;	pParams->needCompress = true; break;
		case TexFormat::RGBA8:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','5');		outFormat = PF_DXT5;	pParams->needCompress = true; break;
		case TexFormat::RGBX8:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','1');		outFormat = PF_DXT1;	pParams->needCompress = true; break;
		case TexFormat::BGR8:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','1');		outFormat = PF_DXT1;	pParams->needCompress = true; break;
		case TexFormat::BGRA8:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','5');		outFormat = PF_DXT5;	pParams->needCompress = true; break;
		case TexFormat::BGRX8:		pHeader->FourCC = AX_MAKEFOURCC('D','X','T','1');		outFormat = PF_DXT1;	pParams->needCompress = true; break;

		case TexFormat::R16F:		pHeader->FourCC = AX_MAKEFOURCC('o', '\0', '\0', '\0');	outFormat = PF_R16F; break;
		case TexFormat::RG16F:		pHeader->FourCC = AX_MAKEFOURCC('p', '\0', '\0', '\0');	outFormat = PF_G16R16F; break;
		case TexFormat::RGBA16F:	pHeader->FourCC = AX_MAKEFOURCC('q', '\0', '\0', '\0');	outFormat = PF_A16B16G16R16F; break;
		case TexFormat::R32F:		pHeader->FourCC = AX_MAKEFOURCC('r', '\0', '\0', '\0');	outFormat = PF_R32F; break;
		case TexFormat::RG32F:		pHeader->FourCC = AX_MAKEFOURCC('s', '\0', '\0', '\0');	outFormat = PF_G32R32F; break;
		case TexFormat::RGBA32F:	pHeader->FourCC = AX_MAKEFOURCC('t', '\0', '\0', '\0');	outFormat = PF_A32B32G32R32F; break;

		case TexFormat::LA8:
			pHeader->Flags2 |= DDS_ALPHAPIXELS;	
			outFormat = PF_LUMINANCE_ALPHA;
			pHeader->Flags2 |= DDS_LUMINANCE;
			pHeader->Flags2 &= ~DDS_FOURCC;
			pHeader->RBitMask = 0xff;
			pHeader->ABitMask = 0xff00;
			break;
		case TexFormat::L8:			
		case TexFormat::A8:
			outFormat = PF_LUMINANCE;
			pHeader->Flags2 |= DDS_LUMINANCE;
			pHeader->Flags2 &= ~DDS_FOURCC;
			pHeader->RBitMask = 0xff;
			break;

		default:
			return false;
		}

		switch (outFormat) {
		case PF_DXT1:		pHeader->LinearSize = (((pHeader->Width + 3)/4) * ((pHeader->Height + 3)/4)) * 8 * pHeader->Depth;	pHeader->RGBBitCount = 4; pParams->formatType = DDS_TYPE_DXT; break;
		case PF_DXT3:
		case PF_DXT5:		pHeader->LinearSize = (((pHeader->Width + 3)/4) * ((pHeader->Height + 3)/4)) * 16 * pHeader->Depth; pHeader->RGBBitCount = 8; pParams->formatType = DDS_TYPE_DXT; break;
		case PF_R16F:			pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 2; pHeader->RGBBitCount = 16; pParams->formatType = DDS_TYPE_FLOAT; break;
		case PF_G16R16F:		pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 4; pHeader->RGBBitCount = 32; pParams->formatType = DDS_TYPE_FLOAT; break;
		case PF_A16B16G16R16F:	pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 8; pHeader->RGBBitCount = 64; pParams->formatType = DDS_TYPE_FLOAT; break;
		case PF_R32F:			pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 4; pHeader->RGBBitCount = 32; pParams->formatType = DDS_TYPE_FLOAT; break;
		case PF_G32R32F:		pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 8; pHeader->RGBBitCount = 64; pParams->formatType = DDS_TYPE_FLOAT; break;
		case PF_A32B32G32R32F:	pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 16; pHeader->RGBBitCount = 128; pParams->formatType = DDS_TYPE_FLOAT; break;
		case PF_LUMINANCE_ALPHA:pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 2; pHeader->RGBBitCount = 16; pParams->formatType = DDS_TYPE_ARGB; break;
		case PF_LUMINANCE:		pHeader->LinearSize = pHeader->Width * pHeader->Height * pHeader->Depth * 1; pHeader->RGBBitCount = 8; pParams->formatType = DDS_TYPE_ARGB; break;
		default: break;
		}
		pParams->orgFormat = format;
		pParams->outFormat = outFormat;
		pParams->width = pHeader->Width;
		pParams->height = pHeader->Height;
		pParams->depth = pHeader->Depth;

		pHeader->ddsCaps1 |= DDS_TEXTURE;
		pHeader->MipMapCount = 1;

		AX_ASSERT(dataCount > 0);
		if (dataCount > 1) {
			pHeader->ddsCaps1 |= DDS_COMPLEX;
			uint_t abstractMipmapCount = GetMipmapCount(std::max(width,height));
			if (dataCount == 6) {
				if (pParams->loadflags & Image::Cubemap) {
					pHeader->ddsCaps2 |= DDS_CUBEMAP_ALL;
				} else {
					pHeader->MipMapCount = abstractMipmapCount;
					pHeader->ddsCaps1 |= DDS_MIPMAP;
				}
			}else if (dataCount == abstractMipmapCount) {
				pHeader->MipMapCount = abstractMipmapCount;
				pHeader->ddsCaps1 |= DDS_MIPMAP;
			}else if (dataCount == abstractMipmapCount * 6){
				pHeader->MipMapCount = abstractMipmapCount;
				pHeader->ddsCaps1 |= DDS_MIPMAP;
				pHeader->ddsCaps2 |= DDS_CUBEMAP_ALL;			
			}else{
				// 错误！如果使用Mipmap或者Cubemap，必须使用全部的Mipmap或者Cubemap，不全的暂时不支持
				return false;
			}
		}
		return true;
	}

	static void ChangeFormatToNormal(DdsWriteParams *pParams, byte_t** ppNewMem){
		uint_t pixelCount = pParams->width * pParams->height * pParams->depth;
		pParams->inDataSize = pixelCount * sizeof(Rgba);

		if (pParams->orgFormat == TexFormat::RGBA8) {
			return;
		}

		Rgba *pBuffer = new Rgba[pixelCount];
		*ppNewMem = (byte_t*)pBuffer;

		switch (pParams->orgFormat)
		{
		case TexFormat::RGB8:	
			for (uint_t i=0; i<pixelCount; i++) {
				pBuffer[i].r = pParams->inData[i * 3];
				pBuffer[i].g = pParams->inData[i * 3 + 1];
				pBuffer[i].b = pParams->inData[i * 3 + 2];
				pBuffer[i].a = 255;
			}
			break;
		//case Image::RGBA8:	break;
		case TexFormat::RGBX8:
			for (uint_t i=0; i<pixelCount; i++) {
				pBuffer[i].r = pParams->inData[i * 4];
				pBuffer[i].g = pParams->inData[i * 4 + 1];
				pBuffer[i].b = pParams->inData[i * 4 + 2];
				pBuffer[i].a = 255;
			}
			break;
		case TexFormat::BGR8:
			for (uint_t i=0; i<pixelCount; i++) {
				pBuffer[i].r = pParams->inData[i * 3 + 2];
				pBuffer[i].g = pParams->inData[i * 3 + 1];
				pBuffer[i].b = pParams->inData[i * 3];
				pBuffer[i].a = 255;
			}
			break;
		case TexFormat::BGRA8:
			for (uint_t i=0; i<pixelCount; i++) {
				pBuffer[i].r = pParams->inData[i * 4 + 2];
				pBuffer[i].g = pParams->inData[i * 4 + 1];
				pBuffer[i].b = pParams->inData[i * 4];
				pBuffer[i].a = pParams->inData[i * 4 + 3];
			}
			break;
		case TexFormat::BGRX8:
			for (uint_t i=0; i<pixelCount; i++) {
				pBuffer[i].r = pParams->inData[i * 4 + 2];
				pBuffer[i].g = pParams->inData[i * 4 + 1];
				pBuffer[i].b = pParams->inData[i * 4];
				pBuffer[i].a = 255;
			}
			break;
		default: AX_NO_DEFAULT; break;
		}

		pParams->inData = *ppNewMem;
	}

	static void GetBlock(Rgba *block, DdsWriteParams *pParams, const byte_t *data) {
		uint_t i = 0;
		Rgba fill = *(Rgba*)data;
		for (uint_t y=0; y < 4; y++) {
			for (uint_t x=0; x < 4; x++) {
				if (data < pParams->inData + pParams->inDataSize)
					block[i++] = *(Rgba*)(data + x * sizeof(Rgba));
				else
					block[i++] = fill;
			}
			data += pParams->width * sizeof(Rgba);
		}
	}

	static uint_t Distance(Rgba *c1, Rgba *c2) {
		int dr = c1->r - c2->r;
		int dg = c1->g - c2->g;
		int db = c1->b - c2->b;
		return (dr * dr + dg * dg + db * db);
	}

	static ushort_t ColorRGBAto565(Rgba *c) {
		return ((c->r >> 3) << 11) | ((c->g >> 2) << 5) | (c->b >> 3);
	}

	static const byte_t mask5 = 0xf8;
	static const byte_t mask6 = 0xfc;
	static const byte_t add5 = 2;
	static const byte_t add6 = 1;
	static const int INSET_SHIFT = 4; // inset the bounding box with (range >> shift)

	template< int C_SHIFT, int C_DELTA, int C_MASK >
	void GetMinMax(byte_t *pMin, byte_t *pMax) {
		if (*pMin > *pMax) {
			std::swap(pMin, pMax);
		}
		byte_t min2 = (*pMin & C_MASK) + (*pMin >> C_SHIFT);
		if (min2 > *pMin + C_DELTA) {
			*pMin -= C_DELTA * 4;
			*pMin = (*pMin & C_MASK) + (*pMin >> C_SHIFT);
		}else{
			*pMin = min2;
		}

		byte_t max2 = (*pMax & C_MASK) + (*pMax >> C_SHIFT);
		if (max2 < *pMax - C_DELTA) {
			*pMax += C_DELTA * 4;
			*pMax = (*pMax & C_MASK) + (*pMax >> C_SHIFT);
		}else{
			*pMax = max2;
		}
	}
	#if 0
	static void ChooseEndpoints2(ColorRGBA *block, ColorRGBA *minColor, ColorRGBA *maxColor) {
		minColor->r = minColor->g = minColor->b = 255;
		maxColor->r = maxColor->g = maxColor->b = 0;

		for (int i = 0; i < 16; i++) {
			if (block[i].r < minColor->r) { minColor->r = block[i].r; }
			if (block[i].g < minColor->g) { minColor->g = block[i].g; }
			if (block[i].b < minColor->b) { minColor->b = block[i].b; }
			if (block[i].r > maxColor->r) { maxColor->r = block[i].r; }
			if (block[i].g > maxColor->g) { maxColor->g = block[i].g; }
			if (block[i].b > maxColor->b) { maxColor->b = block[i].b; }
		}

		Uint8 inset[3];
		inset[0] = (maxColor->r - minColor->r) >> INSET_SHIFT;
		inset[1] = (maxColor->g - minColor->g) >> INSET_SHIFT;
		inset[2] = (maxColor->b - minColor->b) >> INSET_SHIFT;

		minColor->r = (minColor->r + inset[0] <= 255) ? minColor->r + inset[0] : 255;
		minColor->g = (minColor->g + inset[1] <= 255) ? minColor->g + inset[1] : 255;
		minColor->b = (minColor->b + inset[2] <= 255) ? minColor->b + inset[2] : 255;
		maxColor->r = (maxColor->r >= inset[0]) ? maxColor->r - inset[0] : 0;
		maxColor->g = (maxColor->g >= inset[1]) ? maxColor->g - inset[1] : 0;
		maxColor->b = (maxColor->b >= inset[2]) ? maxColor->b - inset[2] : 0;

		GetMinMax< 5, add5, mask5 >(&minColor->r, &maxColor->r);
		GetMinMax< 6, add6, mask6 >(&minColor->g, &maxColor->g);
		GetMinMax< 5, add5, mask5 >(&minColor->b, &maxColor->b);
	}
	#endif
	static void ChooseEndpoints(Rgba *block, Rgba *rgba1, Rgba *rgba2) {
		uint_t farthest = 0;
		int pos1 = 0, pos2 = 0;

		for (int i = 0; i < 16; i++) {
			for (int j = i+1; j < 16; j++) {
				uint_t d = Distance(&block[i], &block[j]);
				if (d > farthest) {
					farthest = d;
					pos1 = i;
					pos2 = j;
				}
			}
		}
		*rgba1 = block[pos1];
		*rgba2 = block[pos2];

		GetMinMax< 5, add5, mask5 >(&rgba1->r, &rgba2->r);
		GetMinMax< 6, add6, mask6 >(&rgba1->g, &rgba2->g);
		GetMinMax< 5, add5, mask5 >(&rgba1->b, &rgba2->b);
	}

	static uint_t GenBitMask(Rgba *c1, Rgba *c2, int numCols, Rgba *block) {
		byte_t mask[16];
		Rgba colours[4];

		colours[0] = *c1; colours[1] = *c2;
		if (numCols == 3) {
			colours[2].r = (colours[0].r + colours[1].r) / 2;
			colours[2].g = (colours[0].g + colours[1].g) / 2;
			colours[2].b = (colours[0].b + colours[1].b) / 2;
		}
		else {  // NumCols == 4
			colours[2].r = (2 * colours[0].r + colours[1].r + 1) / 3;
			colours[2].g = (2 * colours[0].g + colours[1].g + 1) / 3;
			colours[2].b = (2 * colours[0].b + colours[1].b + 1) / 3;
			colours[3].r = (colours[0].r + 2 * colours[1].r + 1) / 3;
			colours[3].g = (colours[0].g + 2 * colours[1].g + 1) / 3;
			colours[3].b = (colours[0].b + 2 * colours[1].b + 1) / 3;
		}

		for (int i=0; i < DDS_DXT_BLOCK_SIZE; i++) {
			if (numCols == 3) {  // Test to see if we have 1-bit transparency
				if (block[i].a < 128) {
					mask[i] = 3;  // Transparent
					continue;
				}
			}

			// If no transparency, try to find which colour is the closest.
			uint_t closest = UINT_MAX;
			for (int j=0; j < numCols; j++) {
				uint_t d = Distance(&block[i], &colours[j]);
				if (d < closest) {
					closest = d;
					mask[i] = j;
				}
			}
		}

		uint_t bitMask = 0;
		for (int i=0; i < DDS_DXT_BLOCK_SIZE; i++) {
			AX_ASSERT(mask[i] < 4);
			bitMask |= mask[i] << ((i) * 2);
		}
		return bitMask;
	}

	void ChooseAlphaEndpoints(Rgba *block, byte_t *high, byte_t *low) {
		*high = block[0].a;
		*low = block[0].a;

		bool flip = false;
		for (int i=0; i < DDS_DXT_BLOCK_SIZE; i++) {
			byte_t a = block[i].a;
			if (a == 0 || a == 255) // use 0, 255 as endpoints
				flip = true;
			else if (a < *low)
				*low = a;
			else if (a > *high)
				*high = a;
		}

		if (flip) {
			byte_t t = *high;
			*high = *low;
			*low = t;
		}
	}

	void GenAlphaBitMask(byte_t a0, byte_t a1, Rgba *block, byte_t *outMask) {
		byte_t alphas[8];
		byte_t mask[16];

		alphas[0] = a0;
		alphas[1] = a1;

		// 8-alpha or 6-alpha block?
		if (a0 > a1) {
			// 8-alpha block:  derive the other six alphas.
			// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
			alphas[2] = (6 * alphas[0] + 1 * alphas[1] + 3) / 7;	// bit code 010
			alphas[3] = (5 * alphas[0] + 2 * alphas[1] + 3) / 7;	// bit code 011
			alphas[4] = (4 * alphas[0] + 3 * alphas[1] + 3) / 7;	// bit code 100
			alphas[5] = (3 * alphas[0] + 4 * alphas[1] + 3) / 7;	// bit code 101
			alphas[6] = (2 * alphas[0] + 5 * alphas[1] + 3) / 7;	// bit code 110
			alphas[7] = (1 * alphas[0] + 6 * alphas[1] + 3) / 7;	// bit code 111
		}
		else {
			// 6-alpha block.
			// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
			alphas[2] = (4 * alphas[0] + 1 * alphas[1] + 2) / 5;	// Bit code 010
			alphas[3] = (3 * alphas[0] + 2 * alphas[1] + 2) / 5;	// Bit code 011
			alphas[4] = (2 * alphas[0] + 3 * alphas[1] + 2) / 5;	// Bit code 100
			alphas[5] = (1 * alphas[0] + 4 * alphas[1] + 2) / 5;	// Bit code 101
			alphas[6] = 0x00;										// Bit code 110
			alphas[7] = 0xFF;										// Bit code 111
		}

		for (int i=0; i < DDS_DXT_BLOCK_SIZE; i++) {
			int closest = INT_MAX;
			for (int j=0; j < 8; j++) {
				int d = block[i].a - alphas[j];
				d *= d;
				if (d < closest) {
					closest = d;
					mask[i] = j;
					if (d == 0)
						break;
				}
			}
		}

		// First three bytes.
		outMask[0] = (mask[0]) | (mask[1] << 3) | ((mask[2] & 0x03) << 6);
		outMask[1] = ((mask[2] & 0x04) >> 2) | (mask[3] << 1) | (mask[4] << 4) | ((mask[5] & 0x01) << 7);
		outMask[2] = ((mask[5] & 0x06) >> 1) | (mask[6] << 2) | (mask[7] << 5);

		// Second three bytes.
		outMask[3] = (mask[8]) | (mask[9] << 3) | ((mask[10] & 0x03) << 6);
		outMask[4] = ((mask[10] & 0x04) >> 2) | (mask[11] << 1) | (mask[12] << 4) | ((mask[13] & 0x01) << 7);
		outMask[5] = ((mask[13] & 0x06) >> 1) | (mask[14] << 2) | (mask[15] << 5);
	}

	void DXT1CheckOrder(Rgba *block, Rgba *c1, Rgba *c2, bool *pHasAlpha){
		// check & change order
		bool hasAlpha = false;
		for (uint_t i=0 ; i < DDS_DXT_BLOCK_SIZE; i++) {
			if (block[i].a < 128) {
				hasAlpha = true;
				break;
			}
		}
		*pHasAlpha = hasAlpha;

		bool bigger = ColorRGBAto565(c1) > ColorRGBAto565(c2);
		bool needSwap = (hasAlpha && bigger) || (!hasAlpha && !bigger);
		if (needSwap)
			std::swap(*c1, *c2);
	}

	//NV_ERROR_CODE nvDXTReadCallback(void *buffer, size_t count, void * userData)
	//{
	//	std::istream *s = static_cast<std::istream*>(userData);
	//	s->read(static_cast<char*>(buffer), (std::streamsize)count);
	//	return s->good() ? NV_OK : NV_READ_FAILED;
	//}
	#if 0
	NV_ERROR_CODE nvDXTWriteCallback(const void *buffer, size_t count, const MIPMapData * mipMapData, void * userData) {
		if (mipMapData)
		{
			// means a MIP map
			DdsWriteParams *pParams = static_cast<DdsWriteParams*>(userData);
			pParams->outfp->Write(static_cast<const char*>(buffer), count);
		}
		else
		{
			// otherwise file header data
		}

		return NV_OK;
	}


	void NvDxtCompress(DdsWriteParams *pParams) {
		nvCompressionOptions options;
		//options.SetQuality(kQualityFastest, 200);
		options.UseExisitingMIPMaps();
		options.user_data = pParams;

		NV_ERROR_CODE ret = nvDDS::nvDXTcompress(	pParams->inData,
													pParams->width,
													pParams->height,
													pParams->width * sizeof(ColorRGBA),
													nvRGBA,
													&options,
													nvDXTWriteCallback,
													0 );

		AX_ASSERT(ret == NV_OK);
	}
	#endif

	void MyDxt1Compress(DdsWriteParams *pParams) {
		Rgba block[DDS_DXT_BLOCK_SIZE];
		byte_t *pWriteBuf = pParams->outData;
		const byte_t *pReadBuf = pParams->inData;

		for (uint_t z=0; z < pParams->depth; z++) {
			for (uint_t y=0; y < pParams->height; y += 4) {
				for (uint_t x=0; x < pParams->width; x += 4) {

					uint_t offset = y * pParams->width + x;
					GetBlock(block, pParams, pReadBuf + offset * sizeof(Rgba));

					Rgba rgba1, rgba2;
					bool hasAlpha;
					ChooseEndpoints(block, &rgba1, &rgba2);
					DXT1CheckOrder(block, &rgba1, &rgba2, &hasAlpha);
					*(ushort_t*)(pWriteBuf + 0)	= LittleShort(ColorRGBAto565(&rgba1));
					*(ushort_t*)(pWriteBuf + 2)	= LittleShort(ColorRGBAto565(&rgba2));

					uint_t bitMask = (hasAlpha)?	GenBitMask(&rgba1, &rgba2, 3, block)
						: GenBitMask(&rgba1, &rgba2, 4, block);
					*(uint_t*)(pWriteBuf + 4)	= LittleInt(bitMask);
					pWriteBuf += 8;
				}
			}
			pReadBuf += pParams->width * pParams->height * sizeof(Rgba);
		}
	}

	void MyDxt3Compress(DdsWriteParams *pParams) {
		Rgba block[DDS_DXT_BLOCK_SIZE];
		byte_t *pWriteBuf = pParams->outData;
		const byte_t *pReadBuf = pParams->inData;

		for (uint_t z=0; z < pParams->depth; z++) {
			for (uint_t y=0; y < pParams->height; y += 4) {
				for (uint_t x=0; x < pParams->width; x += 4) {

					uint_t offset = y * pParams->width + x;
					GetBlock(block, pParams, pReadBuf + offset * sizeof(Rgba));

					for (int i=0; i < DDS_DXT_BLOCK_SIZE; i+=2) {
						*pWriteBuf++ = (((block[i+1].a >> 4) << 4) | (block[i].a >> 4));
					}

					Rgba rgba1, rgba2;
					ChooseEndpoints(block, &rgba1, &rgba2);
					ushort_t c1 = ColorRGBAto565(&rgba1);
					ushort_t c2 = ColorRGBAto565(&rgba2);
					if (c1 < c2) {
						std::swap(c1, c2);
						std::swap(rgba1, rgba2);
					}
					*(ushort_t*)(pWriteBuf + 0)	= LittleShort(c1);
					*(ushort_t*)(pWriteBuf + 2)	= LittleShort(c2);

					uint_t bitMask = GenBitMask(&rgba1, &rgba2, 4, block);
					*(uint_t*)(pWriteBuf + 4)	= LittleInt(bitMask);
					pWriteBuf += 8;
				}
			}
			pReadBuf += pParams->width * pParams->height * sizeof(Rgba);
		}
	}

	void MyDxt5Compress(DdsWriteParams *pParams) {
		Rgba block[DDS_DXT_BLOCK_SIZE];
		byte_t *pWriteBuf = pParams->outData;
		const byte_t *pReadBuf = pParams->inData;

		for (uint_t z=0; z < pParams->depth; z++) {
			for (uint_t y=0; y < pParams->height; y += 4) {
				for (uint_t x=0; x < pParams->width; x += 4) {

					uint_t offset = y * pParams->width + x;
					GetBlock(block, pParams, pReadBuf + offset * sizeof(Rgba));

					byte_t a0, a1, mask[6];
					ChooseAlphaEndpoints(block, &a0, &a1);
					GenAlphaBitMask(a0, a1, block, mask);
					*pWriteBuf++ = a0;
					*pWriteBuf++ = a1;
					*pWriteBuf++ = mask[0];
					*pWriteBuf++ = mask[1];
					*pWriteBuf++ = mask[2];
					*pWriteBuf++ = mask[3];
					*pWriteBuf++ = mask[4];
					*pWriteBuf++ = mask[5];

					Rgba rgba1, rgba2;
					ChooseEndpoints(block, &rgba1, &rgba2);
					ushort_t c1 = ColorRGBAto565(&rgba1);
					ushort_t c2 = ColorRGBAto565(&rgba2);
					if (c1 < c2) {
						std::swap(c1, c2);
						std::swap(rgba1, rgba2);
					}
					*(ushort_t*)(pWriteBuf + 0)	= LittleShort(c1);
					*(ushort_t*)(pWriteBuf + 2)	= LittleShort(c2);

					uint_t bitMask = GenBitMask(&rgba1, &rgba2, 4, block);
					*(uint_t*)(pWriteBuf + 4)	= LittleInt(bitMask);
					pWriteBuf += 8;
				}
			}
			pReadBuf += pParams->width * pParams->height * sizeof(Rgba);
		}
	}

	void Compress(DdsWriteParams *pParams) {
		byte_t *pNewMem = NULL;
		ChangeFormatToNormal(pParams, &pNewMem);

		//CALC_FUNC_RUN_TIME;

		if (1 || pParams->bFast) {
			switch (pParams->outFormat) {
			case PF_DXT1:
				MyDxt1Compress(pParams);
				break;
			case PF_DXT3:
				MyDxt3Compress(pParams);
				break;
			case PF_DXT5:
				MyDxt5Compress(pParams);
				break;
			default: AX_NO_DEFAULT; break;
			}

			pParams->inData = NULL;
			delete pNewMem;

			pParams->outfp->write(pParams->outData, pParams->linearSize);
		} else {
			// disabled for linux
	//		NvDxtCompress(pParams);
		}
	}

	void CopyData(DdsWriteParams *pParams){
		switch (pParams->outFormat)
		{
		case PF_DXT1:
			for (uint_t i=0; i<pParams->linearSize; i+=8) {
				*(ushort_t*)(pParams->outData + i)		= LittleShort(*(ushort_t*)(pParams->inData + i));
				*(ushort_t*)(pParams->outData + i + 2)	= LittleShort(*(ushort_t*)(pParams->inData + i + 2));
				*(uint_t*)(pParams->outData + i + 4)	= LittleInt(*(uint_t*)(pParams->inData + i + 4));
			}
			break;
		case PF_DXT3:
		case PF_DXT5:		
			for (uint_t i=0; i<pParams->linearSize; i+=16) {
				*(uint_t*)(pParams->outData + i + 0)	= *(uint_t*)(pParams->inData + i + 0);
				*(uint_t*)(pParams->outData + i + 4)	= *(uint_t*)(pParams->inData + i + 4);
				*(ushort_t*)(pParams->outData + i + 8)	= LittleShort(*(ushort_t*)(pParams->inData + i + 8));
				*(ushort_t*)(pParams->outData + i + 10)	= LittleShort(*(ushort_t*)(pParams->inData + i + 10));
				*(uint_t*)(pParams->outData + i + 12)	= LittleInt(*(uint_t*)(pParams->inData + i + 12));
			}
			break;
		case PF_R16F:			
		case PF_G16R16F:		
		case PF_A16B16G16R16F:	
			for (uint_t i=0; i<pParams->linearSize; i+=2) {
				*(ushort_t*)(pParams->outData + i)		= LittleShort(*(ushort_t*)(pParams->inData + i));
			}
			break;
		case PF_R32F:			
		case PF_G32R32F:		
		case PF_A32B32G32R32F:	
			for (uint_t i=0; i<pParams->linearSize; i+=4) {
				*(uint_t*)(pParams->outData + i)		= LittleInt(*(uint_t*)(pParams->inData + i));
			}
			break;
		case PF_LUMINANCE_ALPHA:
		case PF_LUMINANCE:		
			memcpy(pParams->outData, pParams->inData, pParams->linearSize);
			break;
		default: AX_NO_DEFAULT; break;
		}
		pParams->outfp->write(pParams->outData, pParams->linearSize);
	}

	void Image::saveFile_dds(const String &filename, bool bFast) {
		File *fp = g_fileSystem->openFileWrite(filename);

		DdsHeader header;
		DdsWriteParams params;
		FillHeader(&header, &params, m_width, m_height, m_depth, (uint_t)m_datas.size(), m_format);
		params.outfp = fp;
		params.bFast = bFast;

		ChangeHeaderIntOrder(&header);
		fp->write(&header, sizeof(DdsHeader));
		ChangeHeaderIntOrder(&header);

		uint_t cubemapCount = (header.ddsCaps2 & DDS_CUBEMAP_ALL)? 6 : 1;
		AX_ASSERT(cubemapCount * header.MipMapCount == (uint_t)m_datas.size());
		params.outData = new byte_t[header.LinearSize];
		for (uint_t i=0; i<cubemapCount; i++) {
			params.linearSize = header.LinearSize;
			params.width = header.Width;
			params.height = header.Height;
			params.depth = header.Depth;
			for (uint_t j=0; j<header.MipMapCount; j++) {
				params.inData = m_datas[ i * header.MipMapCount + j ].get();
				if (!params.needCompress){
					CopyData(&params);
				} else {
					Compress(&params);
				}

				params.width = std::max<uint_t>(1, params.width  / 2);
				params.height = std::max<uint_t>(1, params.height / 2);
				params.depth = std::max<uint_t>(1, params.depth  / 2);
				if (params.formatType == DDS_TYPE_DXT) {
					uint_t minW = (params.width + 3) & ~3;
					uint_t minH = (params.height + 3) & ~3;
					params.linearSize = (minW * minH * params.depth) * header.RGBBitCount / 8;
				} else {
					params.linearSize = params.width * params.height * params.depth * header.RGBBitCount / 8;
				}
			}
		}
		delete[] params.outData;

		fp->close();
		delete fp;
	}

AX_END_NAMESPACE
