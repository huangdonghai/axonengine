/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_TEXTURE_H
#define AX_TEXTURE_H

namespace Axon {

	class Texture : public Asset {
	public:
		enum { AssetType = Asset::kTexture };

		enum InitFlag {
			IF_NoMipmap = 1,
			IF_NoDownsample = 2,
			IF_AutoGenMipmap = 4,
			IF_RenderTarget = 8
		};

		typedef Flags_<InitFlag> InitFlags;

		enum TexType {
			TT_2D,
			TT_3D,
			TT_CUBE,
		};

		enum ClampMode {
			CM_Repeat,
			CM_Clamp,
			CM_ClampToEdge,	// only used in engine internal
			CM_ClampToBorder // only used in engine internal
		};


		enum FilterMode {
			FM_Nearest,
			FM_Linear,
			FM_Bilinear,
			FM_Trilinear
		};

		virtual void initialize(TexFormat format, int width, int height, InitFlags flags = 0) = 0;
		virtual void getSize(int& width, int& height, int& depth) = 0;
		virtual void uploadSubTexture(const Rect& rect, const void* pixels, TexFormat format = TexFormat::AUTO) = 0;
		virtual void setClampMode(ClampMode clampmode) = 0;
		virtual void setFilterMode(FilterMode filtermode) = 0;
		virtual void setBorderColor(const Rgba& color) = 0;
		virtual void setHardwareShadowMap(bool enable) = 0;

		virtual void saveToFile(const String& filename) = 0;
		virtual void generateMipmap() = 0;
		virtual TexFormat getFormat() = 0;

	protected:
		virtual ~Texture() {}
	};

AX_DECLARE_REFPTR(Texture);

} // namespace Axon


#endif // end guardian

