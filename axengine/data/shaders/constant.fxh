/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#define IS_SET(x) defined(x) && (x != 0)

#if !G_OPENGL
#	define VP_2_0	vs_2_0
#	define FP_2_0	ps_2_0
#	define VP_3_0	vs_3_0
#	define FP_3_0	ps_3_0
#else	// D3D
#	define VP_2_0	arbvp1
#	define FP_2_0	arbfp1
#	define VP_3_0	glslv
#	define FP_3_0	glslf
#endif

// let hlsl use Cg/GLSL like matrix mode
#if G_D3D
#endif

// effect state for d3d/opengl portability
#if G_D3D
#	define VERTEXPROGRAM		VertexShader
#	define FRAGMENTPROGRAM		PixelShader
#	define DEPTHTEST			ZEnable
#	define DEPTHMASK			ZWriteEnable
#	define BLEND_NONE			AlphaBlendEnable = false
#	define BLEND_ADD			AlphaBlendEnable = true; SrcBlend = ONE; DestBlend = ONE
#	define BLEND_FILTER			AlphaBlendEnable = true; SrcBlend = ZERO; DestBlend = SRCCOLOR
#	define BLEND_BLEND			AlphaBlendEnable = true; SrcBlend = SRCALPHA; DestBlend = INVSRCALPHA
#	define CULL_NONE			CullMode = NONE
//#	define CULL_FRONT			CullMode = CW;
//#	define CULL_BACK			CullMode = CCW;
#	define CULL_ENABLED			CULL_BACK

#	if G_REFLECTION
#		define DEPTHMASK_MAIN	ZWriteEnable = true
#		define CULL_FRONT		CullMode = CCW
#		define CULL_BACK		CullMode = CW
#	else
#		define DEPTHMASK_MAIN	ZWriteEnable = true
#		define CULL_FRONT		CullMode = CW
#		define CULL_BACK		CullMode = CCW
#	endif

#if 0
#	define DEPTHFUNC_LEQUAL
#	define DEPTHFUNC_EQUAL
#	define DEPTHFUNC_GREATER
#	define DEPTHFUNC_LESS
#endif

#else // G_D3D
#	define VERTEXPROGRAM		VertexProgram
#	define FRAGMENTPROGRAM		FragmentProgram
#	define DEPTHTEST			DepthTestEnable
#	define DEPTHMASK			DepthMask
#	define BLEND_NONE			BlendEnable = false
#	define BLEND_ADD			BlendEnable = true; BlendFunc = { One, One }
#	define BLEND_FILTER			BlendEnable = true; BlendFunc = { Zero, SrcColor }
#	define BLEND_BLEND			BlendEnable = true; BlendFunc = { SrcAlpha, OneMinusSrcAlpha }
#	define CULL_NONE			CullFaceEnable = false
#	define CULL_FRONT			CullFaceEnable = true; FrontFace = CW
#	define CULL_BACK			CullFaceEnable = true 
#	define CULL_ENABLED			CullFaceEnable = true

#	if G_REFLECTION
#		define DEPTHMASK_MAIN DepthMask = true
#	else
#		define DEPTHMASK_MAIN DepthMask = true
#	endif

#endif // G_D3D

/*
	custom state

	depthWrites = true | false;
	depthTest = true | false;
*/


// sampler state
#if G_D3D
#	define FILTER_POINT				MinFilter = POINT; MagFilter = POINT; MipFilter = NONE
#	define FILTER_LINEAR			MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = NONE
#	define FILTER_MIPMAP_POINT		MinFilter = POINT; MagFilter = POINT; MipFilter = POINT
#	define FILTER_MIPMAP_LINEAR		MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = POINT
#	define FILTER_TRILINEAR			MinFilter = LINEAR; MagFilter = LINEAR; MipFilter = LINEAR
#	define CLAMP_REPEAT				AddressU = WRAP;	AddressV = WRAP
#	define CLAMP_EDGE				AddressU = CLAMP;	AddressV = CLAMP
#	define CLAMP_BORDER				AddressU = BORDER;	AddressV = BORDER
#else
#	define FILTER_POINT				MinFilter = NEAREST; MagFilter = NEAREST
#	define FILTER_LINEAR			MinFilter = LINEAR; MagFilter = LINEAR
#	define FILTER_MIPMAP_POINT		MinFilter = NEAREST_MIPMAP_NEAREST; MagFilter = NEAREST
#	define FILTER_MIPMAP_LINEAR		MinFilter = LINEAR_MIPMAP_NEAREST; MagFilter = LINEAR
#	define FILTER_TRILINEAR			MinFilter = LINEAR_MIPMAP_LINEAR; MagFilter = LINEAR
#	define CLAMP_REPEAT				WrapS = REPEAT;	WrapT = REPEAT
#	define CLAMP_EDGE				WrapS = ClampToEdge;	WrapT = ClampToEdge
#	define CLAMP_BORDER				WrapS = ClampToBorder;	WrapT = ClampToBorder
#endif

// pragma
#if G_D3D
#define UNROLL	[unroll]
#define LOOP	[loop]
#else
#define UNROLL
#define LOOP
#endif

#if G_D3D
//#define AX_UNIFORM(shadertype, axtype, name, vsreg, psreg) shadertype name : register(vs, c##vsreg) : register(ps, c##psreg);
//#define AX_ARRAY_UNIFORM(shadertype, axtype, name, num, vsreg, psreg) shadertype name[num] : register(vs, c##vsreg) : register(ps, c##psreg);
#define AX_TEXTURE_UNIFORM(shadertype, name) texture name##_tex; shadertype name = sampler_state { Texture = <name##_tex>; };
#else
//#define AX_UNIFORM(shadertype, axtype, name, vsreg, psreg) shadertype name : register(vs, c##vsreg) : register(ps, c##psreg);
//#define AX_ARRAY_UNIFORM(shadertype, axtype, name, num, vsreg, psreg) shadertype name[num] : register(vs, c##vsreg) : register(ps, c##psreg);
#define AX_TEXTURE_UNIFORM(shadertype, name) texture name##_tex; shadertype name = sampler_state { Texture = <name##_tex>; };
#endif

#if 0
AX_UNIFORM_DEFS
#else
#define g_time g_gc.time
#define g_cameraPos g_gc.cameraPos
#define g_fogParams g_gc.fogParams
#define g_waterFogParams g_gc.waterFogParams

#define g_viewProjMatrix g_vgc.viewProjMatrix
#define g_viewProjNoTranslate g_vgc.viewProjNoTranslate
#define g_cameraAxis g_vgc.cameraAxis
#define g_cameraAngles g_vgc.cameraAngles
#define g_sceneSize g_vgc.sceneSize
#define g_windMatrices g_vgc.windMatrices
#define g_leafAngles g_vgc.leafAngles

#define g_shadowMatrix g_pgc.shadowMatrix
#define g_csmOffsetScales g_pgc.csmOffsetScales
#define g_globalLightPos g_pgc.globalLightPos
#define g_globalLightColor g_pgc.globalLightColor
#define g_skyColor g_pgc.skyColor
#define g_exposure g_pgc.exposure

#define g_modelMatrix g_vic.modelMatrix
#define g_instanceParam g_vic.instanceParam
#define g_texMatrix g_vic.texMatrix

#define g_matDiffuse g_pic.matDiffuse
#define g_matSpecular g_pic.matSpecular
#define g_matShiness g_pic.matShiness
#define g_layerScale g_pic.layerScale
AX_SAMPLER_UNIFORMS
#endif

// render hardware
#define HARDWARE_GENERIC	0
#define HARDWARE_NV_SM2		0
#define HARDWARE_NV_SM3		0
#define HARDWARE_NV_SM4		0
#define HARDWARE_ATI_SM2	0
#define HARDWARE_ATI_SM3	0
#define HARDWARE_ATI_SM4	0
#define HARDWARE_XBOX360	0
#define HARDWARE_PS3		0

// shader debug mode
#define DEBUG_NONE			0
#define DEBUG_DIFFUSEMAP	1
#define DEBUG_NORMALMAP		2
#define DEBUG_SPECULARMAP	3
#define DEBUG_AMBIENT		4
#define DEBUG_DIFFUSE		5
#define DEBUG_SPECULAR		6
#define DEBUG_FOG			7

#if NOT_IN_ENGINE
// G_OPENGL
// G_D3D
// G_SHADER_QUALITY
#define G_MODELMATRIX_IDENTITY	0
#define G_GEOMETRY_INSTANCING	0
#define G_HAVE_DIFFUSE	1
#define G_HAVE_NORMAL	1
#define G_HAVE_SPECULAR	0
// G_HAVE_ENVMAP
// G_HAVE_DETAIL
// G_HAVE_SSS
// G_HAVE_OPACIT
// G_HAVE_SELFILLUM
// G_HAVE_DISPLACEMENT
// G_HAVE_CUSTOM1
// G_HAVE_CUSTOM2
#define G_REFLECTION 1
#define G_HAVE_LAYERALPHA	1
#define G_HAVE_LIGHTMAP	1
#define G_DISABLE_SPECULAR 0
#define G_FOG			1
#define G_BASETC_ANIM	0
#define G_MAIN_TEXGEN TexGen_vertex
#define G_FEATURE0		1
#define G_FEATURE1		1
#define G_FEATURE2		1
#define G_FEATURE3		1
#define G_FEATURE4		0
#define G_FEATURE5		0
#define G_FEATURE6		0
#define G_FEATURE7		0
#define G_LITERAL0		0
#define G_LITERAL1		0
#define G_LITERAL2		0
#define G_LITERAL3		0
#define G_LITERAL4		0
#define G_LITERAL5		0
#define G_LITERAL6		0
#define G_LITERAL7		0
#endif

#if (!G_HAVE_NORMAL && !G_HAVE_DETAIL_NORMAL)
#	define NO_NORMALMAPPING	1
#else
#	define NO_NORMALMAPPING	0
#endif
