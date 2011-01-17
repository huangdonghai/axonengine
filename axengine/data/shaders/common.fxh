/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

// material samplers
#define DECL_SAMPLER(type, name) \
	texture name##_tex; \
	type name = sampler_state { texture = <name##_tex>; };

// material
DECL_SAMPLER(sampler2D, g_diffuseMap);
DECL_SAMPLER(sampler2D, g_normalMap);
DECL_SAMPLER(sampler2D, g_specularMap);
DECL_SAMPLER(sampler2D, g_emissionMap);
DECL_SAMPLER(sampler2D, g_detailMap);
DECL_SAMPLER(sampler2D, g_detailNormalMap);
DECL_SAMPLER(sampler2D, g_opacitMap);
DECL_SAMPLER(sampler2D, g_displacementMap);
DECL_SAMPLER(sampler2D, g_envMap);
// terrain
DECL_SAMPLER(sampler2D, g_terrainColor);
DECL_SAMPLER(sampler2D, g_terrainNormal);
DECL_SAMPLER(sampler2D, g_layerAlpha);
// engine
DECL_SAMPLER(sampler2D, g_reflectionMap);
DECL_SAMPLER(sampler2D, g_lightMap);
DECL_SAMPLER(sampler2D, g_shadowMap);
// global
DECL_SAMPLER(sampler2D, g_rt1);
DECL_SAMPLER(sampler2D, g_lightBuffer);
DECL_SAMPLER(sampler2D, g_sceneColor);

#ifdef G_D3D
#	define for if (0) else for
#	pragma warning(disable:3205) // warning X3205: conversion from larger type to smaller, possible loss of data
#	pragma pack_matrix(row_major)
#endif

#include "shared.fxh"
#include "constant.fxh"
#include "mathlib.fxh"

// use DXSAS 0.8 standard
// see http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c_Summer_04/directx/graphics/reference/EffectFileReference/EffectFileFormat/StandardAnnotations/tandardAnnotationScriptingSyntax.asp
// NOTE: up link is broken
// NOTE: not compatibility to dxsas 0.8 now


// zparam, for recover view space z, Zview = 2*f*n/((f+n)-Zbuf(f-n)), where Zbuf is [-1,1]
// if Zbuf is [0,1], acultly we use, the equation should be
//
//				f * n
// Zview = -------------------
//			f - Zbuf(f-n)
// faster than use matrix multiply, but only for perspective projection

#if 0
struct ZrecoverParam {
	float near, far, farXnear, farSUBnear;
};

float ZR_getViewSpace(float zbuf) {
	return g_zrecoverParam.z / (g_zrecoverParam.y - zbuf * g_zrecoverParam.w);
}
//#else
//struct ZrecoverParam {
//	float near, far, -(f-n)/fn, (1/n);
//};

float ZR_getViewSpace(float zbuf) {
	return 1.0 / (zbuf * g_zrecoverParam.z + g_zrecoverParam.w);
}
#endif



// sturct
/* data from application vertex buffer */
struct VertexIn {
    float3 position		: POSITION;
    float4 streamTc		: TEXCOORD0;
	float4 color		: COLOR0;
	float4 normal		: NORMAL;
	float4 tangent		: TEXCOORD6;

	// instance
#if G_GEOMETRY_INSTANCING
	float4 matrixX		: TEXCOORD2;
	float4 matrixY		: TEXCOORD3;
	float4 matrixZ		: TEXCOORD4;
	float4 userDefined	: TEXCOORD5;
#else
	float3 oldPosition	: TEXCOORD2;
#endif
};

/* data passed from vertex shader to pixel shader */
struct VertexOut {
    float4 hpos			: POSITION;
	float4 color		: COLOR;
	float4 screenTc		: TEXCOORD0;
    float4 streamTc		: TEXCOORD1;
	float3 worldPos		: TEXCOORD2;
	float3 normal		: TEXCOORD3;

#if !NO_NORMALMAPPING
	float3 tangent		: TEXCOORD4;
	float3 binormal		: TEXCOORD5;
#endif

#if G_FOG
	float fog			: TEXCOORD6;
#endif
};

struct GpassOut {
	float4 hpos			: POSITION;
	float4 screenTc		: TEXCOORD0;
	float3 normal		: TEXCOORD1;
	float4 streamTc		: TEXCOORD2;

#if !NO_NORMALMAPPING
	float3 tangent		: TEXCOORD3;
	float3 binormal		: TEXCOORD4;
#endif
};

struct Gbuffer {
	half4 normalz		: COLOR0;
	half4 accum			: COLOR1;
	half4 diffuse		: COLOR2;
	half4 specular		: COLOR3;
};

// debug vertex input
struct DebugVertex {
    float3 xyz			: POSITION;
	float4 color		: COLOR0;

	// instance
	float4 matrixX		: TEXCOORD2;
	float4 matrixY		: TEXCOORD3;
	float4 matrixZ		: TEXCOORD4;
	float4 userDefined	: TEXCOORD5;
};


#include "texgen.fxh"
#include "fragment.fxh"
#include "vertex.fxh"
