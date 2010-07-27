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
typedef Sequence<HitRecord> HitRecords;

struct RenderDriverInfo {
	enum DriverType {
		OpenGL,
		D3D,
		UNKNOWN
	};

	enum SuggestFormat {
		LDR_COLOR,
		HDR_COLOR,
		SHADOW_MAP,
		SUGGEST_MAX
	};

	DriverType driverType;	// opengl, d3d etc...

	// for opengl
	String vendor;
	String renderer;
	String version;
	String extension;

	int maxTextureSize;
	int max3DTextureSize;
	int maxCubeMapTextureSize;	// queried from GL

	int maxTextureUnits;		// arb_multitexture
	int maxTextureCoords;		// arb_fragment_program
	int maxTextureImageUnits;	// arb_fragment_program

	bool textureFormatSupports[TexFormat::MAX_NUMBER];
	bool renderTargetFormatSupport[TexFormat::MAX_NUMBER];
	bool autogenMipmapSupports[TexFormat::MAX_NUMBER];

	TexFormat suggestFormats[SUGGEST_MAX];

	bool transposeMatrix;
};

class IRenderDriver
{
public:
	// device
	virtual ~IRenderDriver() {}
	virtual void initialize() = 0;
	virtual void finalize() = 0;
};

AX_END_NAMESPACE

#endif // AX_RENDERDRIVER_H
