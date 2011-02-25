/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#define IS_SET(x) defined(x) && (x != 0)

// effect state for d3d/opengl portability
#if G_D3D
#	if G_DX11
#		define technique technique11
#		define VS_3_0	vs_4_0
#		define PS_3_0	ps_4_0
#	else // not dx11
#		define VS_3_0	vs_3_0
#		define PS_3_0	ps_3_0
#	endif
#else // not G_D3D
#	define VS_3_0	glslv
#	define PS_3_0	glslf
#endif // G_D3D

// pragma
#if G_D3D
#define UNROLL	[unroll]
#define LOOP	[loop]
#else
#define UNROLL
#define LOOP
#endif

#include "sceneconst.fxh"
#include "interactionconst.fxh"

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
