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
	AX_TEXTURE_UNIFORM(sampler2D,	g_lightMap)		\
	AX_TEXTURE_UNIFORM(sampler2D,	g_shadowMap)		\


// float4x4    g_amWindMatrices[NUM_WIND_MATRICES] REG(21);// houses all of the wind matrices shared by all geometry types
// float4      g_avLeafAngles[MAX_NUM_LEAF_ANGLES]; // each element: .x = rock angle, .y = rustle angle
// each element is a float4, even though only a float2 is needed, to facilitate
// fast uploads on all platforms (one call to upload whole array)
// float4      g_vLeafAngleScalars;                 // each tree model has unique scalar values: .x = rock scalar, .y = rustle scalar
// float       g_fWindMatrixOffset REG(6);                 // keeps two instances of the same tree model from using the same wind matrix (range: [0,NUM_WIND_MATRICES])
