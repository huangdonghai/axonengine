/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#define IS_SET(x) defined(x) && (x != 0)

#if !G_OPENGL
#	define VP_2_0	vs_3_0
#	define FP_2_0	ps_3_0
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
/*
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
*/
#else // G_D3D
#	define VERTEXPROGRAM		VertexProgram
#	define FRAGMENTPROGRAM		FragmentProgram
/*
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
*/
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

#include "sceneconst.fxh"
#include "interactionconst.fxh"
#if 0
AX_SAMPLER_UNIFORMS
#endif
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
#define G_GEOMETRY_INSTANCING	1
#define G_REFLECTION 0
#define G_DISABLE_SPECULAR 0
#define G_FOG			1
#define G_MAIN_TEXGEN TexGen_vertex
#define G_CUBE_SHADOWMAP	1

#define M_NUM_LAYERS	4
#define M_DIFFUSE		1
#define M_NORMAL		1
#define M_SPECULAR		1
#define M_ENVMAP		1
#define M_EMISSION		1
#define M_DETAIL		1
#define M_DETAIL_NORMAL	1
#define M_LAYERALPHA	1
#define M_DETAIL1		1
#define M_DETAIL_NORMAL1	1
#define M_LAYERALPHA1	1
#define M_DETAIL2		1
#define M_DETAIL_NORMAL2	1
#define M_LAYERALPHA2	1
#define M_DETAIL3		1
#define M_DETAIL_NORMAL3	1
#define M_LAYERALPHA3	1
// M_SSS
// M_OPACIT
// M_SELFILLUM
// M_DISPLACEMENT
// M_CUSTOM1
// M_CUSTOM2
#define M_LIGHTMAP	1
#define M_TEXANIM	0
#define M_FEATURE0		1
#define M_FEATURE1		1
#define M_FEATURE2		1
#define M_FEATURE3		1
#define M_FEATURE4		0
#define M_FEATURE5		0
#define M_FEATURE6		0
#define M_FEATURE7		0
#endif

#if (!M_NORMAL && !M_DETAIL_NORMAL)
#	define NO_NORMALMAPPING	1
#else
#	define NO_NORMALMAPPING	0
#endif
