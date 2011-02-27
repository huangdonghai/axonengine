/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDERDRIVER_H
#define AX_RENDERDRIVER_H

#define ClassName_RenderDriver "gRenderDriver"

AX_BEGIN_NAMESPACE

struct HitRecord {
	int name;
	float minz;
	float maxz;
};
typedef std::vector<HitRecord> HitRecords;

struct RenderDriverInfo {
	enum DriverType {
		OpenGL, DX9, DX11,
	};

	enum SuggestedFormat {
		SuggestedFormat_DepthStencil,
		SuggestedFormat_DepthStencilTexture,
		SuggestedFormat_SceneColor,
		SuggestedFormat_HdrLightBuffer,
		SuggestedFormat_HdrSceneColor,
		SuggestedFormat_NormalBuffer,
		SuggestedFormat_ShadowMap,
		SuggestedFormat_CubeShadowMap,
		SuggestedFormat_Max
	};

	DriverType driverType;	// opengl, d3d etc...

	inline bool isOpenGL() const { return driverType == OpenGL; }
	inline bool isDX9() const { return driverType == DX9; }
	inline bool isDX11() const { return driverType == DX11; }
	inline bool isDX() const { return !isOpenGL(); }

	// for opengl
	std::string vendor;
	std::string renderer;
	std::string version;
	std::string extension;

	int maxTextureSize;
	int max3DTextureSize;
	int maxCubeMapTextureSize;	// queried from GL

	int maxTextureCoords;		// arb_fragment_program
	int maxTextureImageUnits;	// arb_fragment_program

	bool textureFormatSupports[TexFormat::MAX_NUMBER];
	bool renderTargetFormatSupport[TexFormat::MAX_NUMBER];
	bool autogenMipmapSupports[TexFormat::MAX_NUMBER];

	TexFormat suggestFormats[SuggestedFormat_Max];

	bool transposeMatrix;
	bool support2DArray;
	bool supportCubeArray;
};

class IRenderDriver
{
public:
	// device
	virtual ~IRenderDriver() {}
	virtual bool initialize() = 0;
	virtual void finalize() = 0;

	virtual const ShaderInfo *findShaderInfo(const FixedString &key) = 0;
};

AX_END_NAMESPACE

#endif // AX_RENDERDRIVER_H
