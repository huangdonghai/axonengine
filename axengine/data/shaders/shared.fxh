/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



// some constant definition shared between shader and engine

// why Cg and hlsl don't support enum type?

#ifdef __cplusplus
#define AX_SHADERCONST(name, value) const int name = value;
#else
#define AX_SHADERCONST(name, value) static const int name = value;
#endif

#ifdef __cplusplus
namespace SHADER {
#endif

// render hardware
AX_SHADERCONST(Hardware_generic	, 0)
AX_SHADERCONST(Hardware_nv_sm2	, 1)
AX_SHADERCONST(Hardware_nv_sm3	, 2)
AX_SHADERCONST(Hardware_nv_sm4	, 3)
AX_SHADERCONST(Hardware_ati_sm2	, 5)
AX_SHADERCONST(Hardware_ati_sm3	, 6)
AX_SHADERCONST(Hardware_ati_sm4	, 7)
AX_SHADERCONST(Hardware_xbox360	, 8)
AX_SHADERCONST(Hardware_ps3		, 9)

// tc class
AX_SHADERCONST(TcSlot_diffuse	, 0)
AX_SHADERCONST(TcSlot_bump		, 1)
AX_SHADERCONST(TcSlot_specular	, 2)
AX_SHADERCONST(TcSlot_env		, 3)
AX_SHADERCONST(TcSlot_number	, 4)

// tex gen types
AX_SHADERCONST(TexGen_default		, 0)
AX_SHADERCONST(TexGen_baseTc		, 1)
AX_SHADERCONST(TexGen_lightmapTc	, 2)
AX_SHADERCONST(TexGen_vertex		, 3)
AX_SHADERCONST(TexGen_normal		, 4)
AX_SHADERCONST(TexGen_reflect		, 5)
AX_SHADERCONST(TexGen_refract		, 6)
AX_SHADERCONST(TexGen_sphereMap		, 7)

// vertex deform
AX_SHADERCONST(VertexDeform_none	, 0)
AX_SHADERCONST(VertexDeform_wave	, 1)
AX_SHADERCONST(VertexDeform_normal	, 2)
AX_SHADERCONST(VertexDeform_bulge	, 3)
AX_SHADERCONST(VertexDeform_move	, 4)
AX_SHADERCONST(VertexDeform_sprite	, 5)
AX_SHADERCONST(VertexDeform_spriteZ	, 6)
AX_SHADERCONST(VertexDeform_flare	, 7)

// light type
AX_SHADERCONST(LightType_invalid	, 0)
AX_SHADERCONST(LightType_global		, 1)
AX_SHADERCONST(LightType_spot		, 2)
AX_SHADERCONST(LightType_point		, 3)

// render type
AX_SHADERCONST(RenderType_none		, 0)
AX_SHADERCONST(RenderType_color		, 1)
AX_SHADERCONST(RenderType_depth		, 2)
AX_SHADERCONST(RenderType_copycolor	, 3)
AX_SHADERCONST(RenderType_zbuffer	, 4)

// camera type
AX_SHADERCONST(CameraType_current	, 0)
AX_SHADERCONST(CameraType_reflect	, 1)

// shader quality
AX_SHADERCONST(ShaderQuality_low	, 0)	// basic, shader mode 2.0, not use per-pixel lighting
AX_SHADERCONST(ShaderQuality_middle	, 1)	// support pixel-pixel lighting, framebuffer, render to texture, shadow
AX_SHADERCONST(ShaderQuality_high	, 2)	// support float texture for HDR, shader mode 3.0

// sort hint
AX_SHADERCONST(SortHint_opacit,		0)
AX_SHADERCONST(SortHint_decal,		1)
AX_SHADERCONST(SortHint_underWater, 2)
AX_SHADERCONST(SortHint_water,		3)
AX_SHADERCONST(SortHint_aboveWater, 4)

// shader macro version
AX_SHADERCONST(SHADERMACRO_VERSION, 2)

#ifdef __cplusplus
} // namespace SHADER
#endif


// declare shader macro AX_DECL_MACRO(name, bits)
#define AX_SHADERMACRO_DEFS \
	AX_DECL_MACRO(G_OPENGL					, 1) \
	AX_DECL_MACRO(G_D3D						, 1) \
	AX_DECL_MACRO(G_REFLECTION				, 1) \
	AX_DECL_MACRO(G_HDR						, 1) \
	AX_DECL_MACRO(G_SHADER_QUALITY			, 4) \
	AX_DECL_MACRO(G_HARDWARE				, 4) \
	AX_DECL_MACRO(G_DEBUG					, 4) \
	AX_DECL_MACRO(G_MODELMATRIX_IDENTITY	, 1) \
	AX_DECL_MACRO(G_GEOMETRY_INSTANCING		, 1) \
	\
				/* sampler */ \
	AX_DECL_MACRO(G_HAVE_DIFFUSE			, 1) \
	AX_DECL_MACRO(G_HAVE_NORMAL				, 1) \
	AX_DECL_MACRO(G_HAVE_SPECULAR			, 1) \
	AX_DECL_MACRO(G_HAVE_ENVMAP				, 1) \
	AX_DECL_MACRO(G_HAVE_DETAIL				, 1) \
	AX_DECL_MACRO(G_HAVE_DETAIL_NORMAL		, 1) \
	AX_DECL_MACRO(G_HAVE_OPACIT				, 1) \
	AX_DECL_MACRO(G_HAVE_EMISSION			, 1) \
	AX_DECL_MACRO(G_HAVE_DISPLACEMENT		, 1) \
	\
	AX_DECL_MACRO(G_HAVE_LAYERALPHA			, 1) \
	AX_DECL_MACRO(G_HAVE_LIGHTMAP			, 1) \
	\
				/* enable flags */ \
	AX_DECL_MACRO(G_DISABLE_SPECULAR		, 1) \
	 \
				/* fog */ \
	AX_DECL_MACRO(G_FOG						, 1) \
				/* texgen */ \
	AX_DECL_MACRO(G_BASETC_ANIM				, 1) \
	\
		/* user customize*/ \
	AX_DECL_MACRO(G_FEATURE0				, 1) \
	AX_DECL_MACRO(G_FEATURE1				, 1) \
	AX_DECL_MACRO(G_FEATURE2				, 1) \
	AX_DECL_MACRO(G_FEATURE3				, 1) \
	AX_DECL_MACRO(G_FEATURE4				, 1) \
	AX_DECL_MACRO(G_FEATURE5				, 1) \
	AX_DECL_MACRO(G_FEATURE6				, 1) \
	AX_DECL_MACRO(G_FEATURE7				, 1) \
	AX_DECL_MACRO(G_LITERAL0				, 4) \
	AX_DECL_MACRO(G_LITERAL1				, 4) \
	AX_DECL_MACRO(G_LITERAL2				, 4) \
	AX_DECL_MACRO(G_LITERAL3				, 4) \
	AX_DECL_MACRO(G_LITERAL4				, 4) \
	AX_DECL_MACRO(G_LITERAL5				, 4) \
	AX_DECL_MACRO(G_LITERAL6				, 4) \
	AX_DECL_MACRO(G_LITERAL7				, 4)

#define SCENECONST_BUF 0
#define SCENECONST_REG 8
#define INTERACTIONCONST_BUF 1
#define INTERACTIONCONST_REG 64
#define PRIMITIVECONST_BUF 2
#define PRIMITIVECONST_REG 80

#if G_D3D
#	if G_DX11
#		define AX_DECL_BUFFER(decl, obj, reg, buf) cbuffer cb##buf : register(b##buf) { decl obj; };
#		define AX_DECL_PRIMITIVECONST(decl) cbuffer cbPrimitiveConst : register(b5) { decl g_pc; };
#		define AX_BEGIN_PARAMETER cbuffer cbPrimitiveConst : register(b5) {
#		define PC0 packoffset(c0)
#		define PC1 packoffset(c1)
#		define AX_END_PARAMETER }
#	else
#		define AX_DECL_BUFFER(decl, obj, reg, buf) shared decl obj : register(c##reg);
#		define AX_DECL_PRIMITIVECONST(decl) decl g_pc : register(vs, c50) : register(ps, c24);
#		define AX_BEGIN_PARAMETER
#		define PC0 register(vs, c50) : register(ps, c24)
#		define PC1 register(vs, c51) : register(ps, c25)
#		define AX_END_PARAMETER

#		define SCR0 register(c8)

#		define ICR0 register(c64)

#		define PCR0 register(c80)
#	endif
#else
#	define AX_DECL_BUFFER(decl, obj, reg, buf) decl obj : BUFFER[buf];
#	define AX_DECL_PRIMITIVECONST(decl) decl g_pc : BUFFER[5];
#	define AX_BEGIN_PARAMETER
#	define PC0 BUFFER[5][0]
#	define PC1 BUFFER[5][16]
#	define AX_END_PARAMETER
#endif

#ifndef __cplusplus

struct GlobalConst {
	float time;
	float4 cameraPos;
	float4 fogParams;
	float4 waterFogParams;
};

struct VS_GlobalConst {
	float4x4 viewProjMatrix;
	float4x4 viewProjNoTranslate;
	float3x3 cameraAxis;
	float3 cameraAngles;
	float4 sceneSize;
	float4x4 windMatrices[3];
	float4 leafAngles[8];
};

struct PS_GlobalConst {
	float4x4 shadowMatrix;
	float4x4 csmOffsetScales;
	float4 globalLightPos;
	float4 globalLightColor;
	float4 skyColor;
	float4 exposure;
};

struct VS_InteractionConst {
	float4x4 texMatrix;
	float3x4 modelMatrix;
	float4 instanceParam;
};

struct PS_InteractionConst {
	float3 matDiffuse;
	float3 matSpecular;
	float matShiness;
	float2 layerScale;
};

AX_DECL_BUFFER(GlobalConst, g_gc, 4, 0);
AX_DECL_BUFFER(VS_GlobalConst, g_vgc, 8, 1);
AX_DECL_BUFFER(PS_GlobalConst, g_pgc, 8, 2);
AX_DECL_BUFFER(VS_InteractionConst, g_vic, 42, 3);
AX_DECL_BUFFER(PS_InteractionConst, g_pic, 20, 4);

#endif // __cplusplus

#define AX_GLOBAL_UNIFORMS \
	AX_UNIFORM(g_gc, time) \
	AX_UNIFORM(g_gc, cameraPos) \
	AX_UNIFORM(g_gc, fogParams) \
	AX_UNIFORM(g_gc, waterFogParams) \
	\
	AX_UNIFORM(g_vgc, viewProjMatrix) \
	AX_UNIFORM(g_vgc, viewProjNoTranslate) \
	AX_UNIFORM(g_vgc, cameraAxis) \
	AX_UNIFORM(g_vgc, cameraAngles) \
	AX_UNIFORM(g_vgc, sceneSize) \
	AX_UNIFORM(g_vgc, windMatrices) \
	AX_UNIFORM(g_vgc, leafAngles) \
	\
	AX_UNIFORM(g_pgc, shadowMatrix) \
	AX_UNIFORM(g_pgc, csmOffsetScales) \
	AX_UNIFORM(g_pgc, globalLightPos) \
	AX_UNIFORM(g_pgc, globalLightColor) \
	AX_UNIFORM(g_pgc, skyColor) \
	AX_UNIFORM(g_pgc, exposure) \
	\
	AX_UNIFORM(g_vic, modelMatrix) \
	AX_UNIFORM(g_vic, instanceParam) \
	AX_UNIFORM(g_vic, texMatrix) \
	\
	AX_UNIFORM(g_pic, matDiffuse) \
	AX_UNIFORM(g_pic, matSpecular) \
	AX_UNIFORM(g_pic, matShiness) \
	AX_UNIFORM(g_pic, layerScale) \

#define AX_SCENE_UNIFORMS \
	/* both vs and ps use */ \
	AX_UNIFORM(float, float, g_time) \
	AX_UNIFORM(float4, Vector4, g_cameraPos) \
	AX_UNIFORM(float4, Vector4, g_fogParams) \
	AX_UNIFORM(float4, Vector4, g_waterFogParams) \
	\
	/* vs use */ \
	AX_UNIFORM(float4x4, Matrix4, g_viewProjMatrix) \
	AX_UNIFORM(float4x4, Matrix4, g_viewProjNoTranslate) \
	AX_UNIFORM(float3x3, Matrix3, g_cameraAxis) \
	AX_UNIFORM(float3, Vector3, g_cameraAngles) \
	AX_UNIFORM(float4, Vector4, g_sceneSize) \
	AX_ARRAY_UNIFORM(float4x4, Matrix4, g_windMatrices, 3) \
	AX_ARRAY_UNIFORM(float4, Vector4, g_leafAngles, 8) \
	\
	/* ps use */ \
	AX_UNIFORM(float4, Vector4, g_globalLightPos) \
	AX_UNIFORM(float4, Vector4,	g_globalLightColor) \
	AX_UNIFORM(float4, Vector4,	g_skyColor) \
	AX_UNIFORM(float4, Vector4,	g_exposure) \

#define AX_IA_UNIFORMS \
	AX_UNIFORM(float3x4, Matrix, g_modelMatrix) \
	AX_UNIFORM(float4, Vector4,	g_instanceParam) \
	AX_UNIFORM(float4x4, Matrix4, g_baseTcMatrix) \
	AX_UNIFORM(float4, Vector4,	g_terrainRect) \
	AX_UNIFORM(float4, Vector4,	g_zoneRect) \
	AX_UNIFORM(float4, Vector4,	g_chunkRect) \
	\
	AX_UNIFORM(float3, Vector3,	g_matDiffuse) \
	AX_UNIFORM(float3, Vector3,	g_matSpecular) \
	AX_UNIFORM(float, float, g_matShiness) \
	AX_UNIFORM(float2, Vector2,	g_layerScale) \
	/*shadow parameter*/ \
	AX_UNIFORM(float4x4, Matrix4, g_shadowMatrix) \
	AX_UNIFORM(float4x4, Matrix4, g_csmOffsetScales) \

#define AX_SAMPLER_UNIFORMS \
	AX_TEXTURE_UNIFORM(sampler2D, g_sceneDepth) \
	AX_TEXTURE_UNIFORM(sampler2D, g_lightBuffer) \
	AX_TEXTURE_UNIFORM(sampler2D, g_lightMap) \
	AX_TEXTURE_UNIFORM(sampler2D, g_shadowMap) \

#define AX_UNIFORM_DEFS															\
	/* both vs and ps use */ \
	AX_UNIFORM(float,		float,		g_time,						1, 1)		\
	AX_UNIFORM(float4,		Vector4,	g_cameraPos,				2, 2)		\
	AX_UNIFORM(float4,		Vector4,	g_fogParams,				3, 3)		\
	AX_UNIFORM(float4,		Vector4,	g_waterFogParams,			4, 4)		\
	\
	/* vs use */ \
	AX_UNIFORM(float3x4,	Matrix,		g_modelMatrix,				8, 0)		\
	AX_UNIFORM(float4,		Vector4,	g_instanceParam,			11, 0)		\
	AX_UNIFORM(float4x4,	Matrix4,	g_viewProjMatrix,			12, 0)		\
	AX_UNIFORM(float4x4,	Matrix4,	g_viewProjNoTranslate,		16, 0)		\
	AX_UNIFORM(float3x3,	Matrix3,	g_cameraAxis,				20, 0)		\
	AX_UNIFORM(float3,		Vector3,	g_cameraAngles,				24, 0)		\
	AX_UNIFORM(float4,		Vector4,	g_terrainRect,				25, 0)		\
	AX_UNIFORM(float4,		Vector4,	g_zoneRect,					26, 0)		\
	AX_UNIFORM(float4,		Vector4,	g_chunkRect,				27, 0)		\
	AX_UNIFORM(float4,		Vector4,	g_sceneSize,				28, 0)		\
	AX_UNIFORM(float4x4,	Matrix4,	g_baseTcMatrix,				32, 0)		\
	AX_ARRAY_UNIFORM(float4x4,	Matrix4, g_windMatrices,	3,		36, 0)		\
	AX_ARRAY_UNIFORM(float4,	Vector4, g_leafAngles,		8,		48, 0)		\
	\
	/* ps use */ \
	AX_UNIFORM(float3,		Vector3,	g_matDiffuse,				0, 6)		\
	AX_UNIFORM(float3,		Vector3,	g_matSpecular,				0, 7)		\
	AX_UNIFORM(float,		float,		g_matShiness,				0, 8)		\
	AX_UNIFORM(float4,		Vector4,	g_globalLightPos,			0, 9)		\
	AX_UNIFORM(float4,		Vector4,	g_globalLightColor,			0, 10)		\
	AX_UNIFORM(float4,		Vector4,	g_skyColor,					0, 11)		\
	AX_UNIFORM(float4,		Vector4,	g_exposure,					0, 12)		\
	AX_UNIFORM(float2,		Vector2,	g_layerScale,				0, 13)		\
	/*shadow parameter*/ \
	AX_UNIFORM(float4x4,	Matrix4,	g_shadowMatrix,				0, 18)		\
	AX_UNIFORM(float4x4,	Matrix4,	g_csmOffsetScales,			0, 22)		\
	\
	\
	AX_TEXTURE_UNIFORM(sampler2D,	g_sceneDepth)		\
	AX_TEXTURE_UNIFORM(sampler2D,	g_lightBuffer)		\
	AX_TEXTURE_UNIFORM(sampler2D,	g_lightMap)			\
	AX_TEXTURE_UNIFORM(sampler2D,	g_shadowMap)		\


// float4x4    g_amWindMatrices[NUM_WIND_MATRICES] REG(21);// houses all of the wind matrices shared by all geometry types
// float4      g_avLeafAngles[MAX_NUM_LEAF_ANGLES]; // each element: .x = rock angle, .y = rustle angle
// each element is a float4, even though only a float2 is needed, to facilitate
// fast uploads on all platforms (one call to upload whole array)
// float4      g_vLeafAngleScalars;                 // each tree model has unique scalar values: .x = rock scalar, .y = rustle scalar
// float       g_fWindMatrixOffset REG(6);                 // keeps two instances of the same tree model from using the same wind matrix (range: [0,NUM_WIND_MATRICES])
