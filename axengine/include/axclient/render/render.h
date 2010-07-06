/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_ALL_H
#define AX_RENDER_ALL_H

AX_BEGIN_NAMESPACE

enum ElementType {
	ElementType_PointList,
	ElementType_LineList,
	ElementType_TriList,
	ElementType_TriStrip
};

struct SamplerStateDesc {
	enum ClampMode {
		CM_Repeat,
		CM_Clamp,
		CM_Border // only used in engine internal
	};

	enum FilterMode {
		FM_Nearest,
		FM_Linear,
		FM_LinearMipmap,
		FM_Trilinear,
		FM_Anisotropic
	};

	enum BorderColor {
		BM_Zero, BM_One
	};

	SamplerStateDesc() : clampMode(CM_Repeat), filterMode(FM_Trilinear), borderMode(BM_Zero), maxAnisotropy(0) {}

	size_t hash() const
	{
		return intValue;
	}

	operator size_t() const { return hash(); }

	bool operator==(const SamplerStateDesc &rhs) const
	{
		return intValue == rhs.intValue;
	}

	union {
		struct {
			ClampMode clampMode : 8;
			FilterMode filterMode : 8;
			BorderColor borderMode : 8;
			int maxAnisotropy : 8;
		};
		int intValue;
	};
};

struct DepthStencilStateDesc {
	enum CompareFunc {
		CompareFunc_Never,
		CompareFunc_Less,
		CompareFunc_Equal,
		CompareFunc_LessEqual,
		CompareFunc_Greater,
		CompareFunc_NotEqual,
		CompareFunc_GreaterEqual,
		CompareFunc_Always
	};

	enum StencilOp {
		StencilOp_Keep,
		StencilOp_Zero,
		StencilOp_Replace,
		StencilOp_IncrementSat,
		StencilOp_DecrementSat,
		StencilOp_Invert,
		StencilOp_Increment,
		StencilOp_Decrement
	};

	DepthStencilStateDesc()
	{
		depthEnable = true;
		depthWritable = true;
		depthFunc = CompareFunc_LessEqual;

		stencilEnable = false;
		stencilReadMask = 0xff;
		stencilWriteMask = 0xff;

		stencilFailOp = StencilOp_Keep;
		stencilDepthFailOp = StencilOp_Keep;
		stencilPassOp = StencilOp_Keep;
		stencilFunc = CompareFunc_Never;

		stencilFailOpBackface = stencilFailOp;
		stencilDepthFailOpBackface = stencilDepthFailOp;
		stencilPassOpBackface = stencilPassOp;
		stencilFuncBackface = stencilFunc;
	}

	bool depthEnable;
	bool depthWritable;
	CompareFunc depthFunc;
	bool stencilEnable;
	byte_t stencilReadMask;
	byte_t stencilWriteMask;

	StencilOp stencilFailOp;
	StencilOp stencilDepthFailOp;
	StencilOp stencilPassOp;
	CompareFunc stencilFunc;

	StencilOp stencilFailOpBackface;
	StencilOp stencilDepthFailOpBackface;
	StencilOp stencilPassOpBackface;
	CompareFunc stencilFuncBackface;
};

struct RasterizerStateDesc {
	enum FillMode {
		FillMode_Wireframe,
		FillMode_Solid
	};

	enum CullMode {
		CullMode_Front,
		CullMode_Back,
		CullMode_None
	};

	RasterizerStateDesc()
	{
		fillMode = FillMode_Solid;
		cullMode = CullMode_Back;

		frontCounterClockwise = false;
		depthBias = 0;
		depthBiasClamp = 0;
		slopeScaledDepthBias = 0;
		depthClipEnable = false;
		scissorEnable = true;
		multisampleEnable = false;
		antialiasedLineEnable = false;
	}

	FillMode fillMode;
	CullMode cullMode;
	bool frontCounterClockwise;
	int depthBias;
	float depthBiasClamp;
	float slopeScaledDepthBias;
	bool depthClipEnable;
	bool scissorEnable;
	bool multisampleEnable;
	bool antialiasedLineEnable;
};

struct BlendStateDesc {
	enum BlendFactor {
		BlendFactor_Zero,
		BlendFactor_One,
		BlendFactor_SrcColor,
		BlendFactor_OneMinusSrcColor,
		BlendFactor_DstColor,
		BlendFactor_OneMinusDstColor,
		BlendFactor_SrcAlpha,
		BlendFactor_OneMinusSrcAlpha,
		BlendFactor_SrcAlphaSaturate
	};

	enum BlendOp {
		BlendOp_Add, BlendOp_Subtract, BlendOp_RevSubtract, BlendOp_Min, BlendOp_Max
	};

	BlendStateDesc()
	{
		alphaToCoverageEnable = false;
		independentBlendEnable = false;

		blendEnable = false;
		srcBlend = BlendFactor_One;
		destBlend = BlendFactor_Zero;
		blendOp = BlendOp_Add;

		srcBlendAlpha = srcBlend;
		destBlendAlpha = destBlend;
		blendOpAlpha = blendOp;

		renderTargetWriteMask = 0xff;
	}

	bool alphaToCoverageEnable;
	bool independentBlendEnable;

	bool blendEnable;
	BlendFactor srcBlend;
	BlendFactor destBlend;
	BlendOp blendOp;
	BlendFactor srcBlendAlpha;
	BlendFactor destBlendAlpha;
	BlendOp blendOpAlpha;
	byte_t renderTargetWriteMask;
};

struct RenderClearer {
	Rgba color;
	float depth;
	int stencil;
	bool isClearColor : 1;
	bool isClearDepth : 1;
	bool isClearStencil : 1;

	RenderClearer() : color(Rgba::Black), depth(1.0f), stencil(0), isClearColor(false), isClearDepth(false), isClearStencil(false)
	{}

	void clearDepth(bool enable, float ref = 1.0f)
	{
		isClearDepth = enable;
		depth = ref;
	}

	void clearColor(bool enable, Rgba ref = Rgba::Zero)
	{
		isClearColor = enable;
		color = ref;
	}

	void clearStencil(bool enable, int ref)
	{
		isClearStencil = enable;
		stencil = ref;
	}
};


AX_END_NAMESPACE

template <typename T> static inline T *GetPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::const_pointer GetPtrHelper(const Wrapper &p) { return p.get(); }
template <typename Wrapper> static inline typename Wrapper::pointer GetPtrHelper(Wrapper &p) { return p.get(); }

#define AX_DECLARE_DATA(Class) \
	typedef Class DataClass; \
	Class *d_func() { return reinterpret_cast<Class *>( GetPtrHelper(m_data)); } \
	const Class *d_func() const { return reinterpret_cast<const Class *>( GetPtrHelper(m_data)); }

#define AX_DECLARE_BACKEND(Class) \
	typedef Class BackendClass; \
	Class *b_func() { return reinterpret_cast<Class *>( GetPtrHelper(m_backend)); } \
	const Class *b_func() const { return reinterpret_cast<const Class *>( GetPtrHelper(m_backend)); }

#define AX_DATA \
	DataClass *d = d_func();

#define AX_CONSTDATA \
	const DataClass *d = d_func();

#define AX_BACKEND \
	BackendClass *b = b_func();

#define AX_CONST_BACKEND \
	const BackendClass *b = b_func();


#include "query.h"
#include "sampler.h"
#include "texture.h"
#include "textureatlas.h"
#include "materialdecl.h"
#include "shader.h"
#include "material.h"
#include "camera.h"
#include "target.h"
#include "interaction.h"
#include "primitive.h"
#include "render_entity.h"
#include "light.h"
#include "fog.h"
#include "terrain.h"
#include "driver.h"
#include "font.h"
#include "wind.h"
#include "timeofday.h"
#include "outdoorenv.h"
#include "render_world.h"
#include "queue.h"
#include "render_system.h"

AX_BEGIN_NAMESPACE

extern RenderFrame *g_renderFrame;
extern ShaderMacro g_shaderMacro;
extern Uniforms g_uniforms;
extern IRenderDriver *g_renderDriver;

AX_END_NAMESPACE

#endif // end guardian

