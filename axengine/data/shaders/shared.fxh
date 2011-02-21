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

// shader macro version
AX_SHADERCONST(SHADERMACRO_VERSION, 2)

#ifdef __cplusplus
} // namespace SHADER
#endif


// declare shader macro AX_DECL_MACRO(name, bits)
#define AX_SHADERMACRO_DEFS \
	AX_DECL_MACRO(G_OPENGL					, 1) \
	AX_DECL_MACRO(G_D3D						, 1) \
	AX_DECL_MACRO(G_DX11					, 1) \
	AX_DECL_MACRO(G_REFLECTION				, 1) \
	AX_DECL_MACRO(G_HDR						, 1) \
	AX_DECL_MACRO(G_SHADER_QUALITY			, 3) \
	AX_DECL_MACRO(G_DEBUG					, 3) \
	AX_DECL_MACRO(G_GEOMETRY_INSTANCING		, 1) \
	\
				/* enable flags */ \
	AX_DECL_MACRO(G_DISABLE_SPECULAR		, 1) \
				/* fog */ \
	AX_DECL_MACRO(G_FOG						, 1) \


// declare shader macro AX_DECL_MACRO(name, bits)
#define AX_MATERIALMACRO_DEFS \
	AX_DECL_MACRO(M_NUM_LAYERS				, 3) \
	\
	AX_DECL_MACRO(M_DIFFUSE					, 1) \
	AX_DECL_MACRO(M_NORMAL					, 1) \
	AX_DECL_MACRO(M_SPECULAR				, 1) \
	AX_DECL_MACRO(M_ENVMAP					, 1) \
	AX_DECL_MACRO(M_EMISSION				, 1) \
	AX_DECL_MACRO(M_DISPLACEMENT			, 1) \
	\
	AX_DECL_MACRO(M_LIGHTMAP				, 1) \
	\
	AX_DECL_MACRO(M_DETAIL					, 1) \
	AX_DECL_MACRO(M_DETAIL_NORMAL			, 1) \
	AX_DECL_MACRO(M_LAYERALPHA				, 1) \
	AX_DECL_MACRO(M_DETAIL1					, 1) \
	AX_DECL_MACRO(M_DETAIL_NORMAL1			, 1) \
	AX_DECL_MACRO(M_LAYERALPHA1				, 1) \
	AX_DECL_MACRO(M_DETAIL2					, 1) \
	AX_DECL_MACRO(M_DETAIL_NORMAL2			, 1) \
	AX_DECL_MACRO(M_LAYERALPHA2				, 1) \
	AX_DECL_MACRO(M_DETAIL3					, 1) \
	AX_DECL_MACRO(M_DETAIL_NORMAL3			, 1) \
	AX_DECL_MACRO(M_LAYERALPHA3				, 1) \
				/* texgen */ \
	AX_DECL_MACRO(M_TEXANIM					, 1) \
	\
		/* user customize*/ \
	AX_DECL_MACRO(M_FEATURE0				, 1) \
	AX_DECL_MACRO(M_FEATURE1				, 1) \
	AX_DECL_MACRO(M_FEATURE2				, 1) \
	AX_DECL_MACRO(M_FEATURE3				, 1) \
	AX_DECL_MACRO(M_FEATURE4				, 1) \
	AX_DECL_MACRO(M_FEATURE5				, 1) \
	AX_DECL_MACRO(M_FEATURE6				, 1) \
	AX_DECL_MACRO(M_FEATURE7				, 1)

#define SCENECONST_BUF 0
#define INTERACTIONCONST_BUF 1
#define PRIMITIVECONST_BUF 2

#define SCENECONST_REG 8
#define INTERACTIONCONST_REG 64
#define PRIMITIVECONST_REG 80
#define PRIMITIVECONST_COUNT 16


// for dx/hlsl
#if G_D3D
#	if G_DX11

#		define AX_BEGIN_CBUFFER(name, reg) cbuffer name : register(b##reg) {
#		define AX_END_CBUFFER }

#		define AX_BEGIN_PC cbuffer cbPrimitiveConst : register(b2) {
#		define AX_END_PC }

#		define SREG 0
#		define IREG 1
#		define PREG 2

#		define SREG0 packoffset(c0)
#		define SREG1 packoffset(c1)
#		define SREG2 packoffset(c2)
#		define SREG3 packoffset(c3)
#		define SREG4 packoffset(c4)
#		define SREG5 packoffset(c5)
#		define SREG6 packoffset(c6)
#		define SREG7 packoffset(c7)
#		define SREG8 packoffset(c8)
#		define SREG9 packoffset(c9)
#		define SREG10 packoffset(c10)
#		define SREG11 packoffset(c11)
#		define SREG12 packoffset(c12)
#		define SREG13 packoffset(c13)
#		define SREG14 packoffset(c14)
#		define SREG15 packoffset(c15)
#		define SREG16 packoffset(c16)
#		define SREG17 packoffset(c17)
#		define SREG18 packoffset(c18)
#		define SREG19 packoffset(c19)
#		define SREG20 packoffset(c20)
#		define SREG21 packoffset(c21)
#		define SREG22 packoffset(c22)
#		define SREG23 packoffset(c23)
#		define SREG24 packoffset(c24)
#		define SREG25 packoffset(c25)
#		define SREG26 packoffset(c26)
#		define SREG27 packoffset(c27)
#		define SREG28 packoffset(c28)
#		define SREG29 packoffset(c29)
#		define SREG30 packoffset(c30)
#		define SREG31 packoffset(c31)
#		define SREG32 packoffset(c32)
#		define SREG33 packoffset(c33)
#		define SREG34 packoffset(c34)
#		define SREG35 packoffset(c35)
#		define SREG36 packoffset(c36)
#		define SREG37 packoffset(c37)
#		define SREG38 packoffset(c38)
#		define SREG39 packoffset(c39)
#		define SREG40 packoffset(c40)
#		define SREG41 packoffset(c41)
#		define SREG42 packoffset(c42)
#		define SREG43 packoffset(c43)
#		define SREG44 packoffset(c44)
#		define SREG45 packoffset(c45)
#		define SREG46 packoffset(c46)
#		define SREG47 packoffset(c47)
#		define SREG48 packoffset(c48)
#		define SREG49 packoffset(c49)
#		define SREG50 packoffset(c50)
#		define SREG51 packoffset(c51)
#		define SREG52 packoffset(c52)
#		define SREG53 packoffset(c53)
#		define SREG54 packoffset(c54)
#		define SREG55 packoffset(c55)

#		define IREG0 packoffset(c0)
#		define IREG1 packoffset(c1)
#		define IREG2 packoffset(c2)
#		define IREG3 packoffset(c3)
#		define IREG4 packoffset(c4)
#		define IREG5 packoffset(c5)
#		define IREG6 packoffset(c6)
#		define IREG7 packoffset(c7)
#		define IREG8 packoffset(c8)
#		define IREG9 packoffset(c9)
#		define IREG10 packoffset(c10)
#		define IREG11 packoffset(c11)
#		define IREG12 packoffset(c12)
#		define IREG13 packoffset(c13)
#		define IREG14 packoffset(c14)
#		define IREG15 packoffset(c15)

#		define PREG0 packoffset(c0)
#		define PREG1 packoffset(c1)
#		define PREG2 packoffset(c2)
#		define PREG3 packoffset(c3)
#		define PREG4 packoffset(c4)
#		define PREG5 packoffset(c5)
#		define PREG6 packoffset(c6)
#		define PREG7 packoffset(c7)
#		define PREG8 packoffset(c8)
#		define PREG9 packoffset(c9)
#		define PREG10 packoffset(c10)
#		define PREG11 packoffset(c11)
#		define PREG12 packoffset(c12)
#		define PREG13 packoffset(c13)
#		define PREG14 packoffset(c14)
#		define PREG15 packoffset(c15)

#	else // G_DX11

#		define AX_BEGIN_CBUFFER(name, reg)
#		define AX_END_CBUFFER

#		define AX_BEGIN_PC
#		define AX_END_PC

#		define SREG 8
#		define IREG 64
#		define PREG 80

#		define SREG0 register(c8)
#		define SREG1 register(c9)
#		define SREG2 register(c10)
#		define SREG3 register(c11)
#		define SREG4 register(c12)
#		define SREG5 register(c13)
#		define SREG6 register(c14)
#		define SREG7 register(c15)
#		define SREG8 register(c16)
#		define SREG9 register(c17)
#		define SREG10 register(c18)
#		define SREG11 register(c19)
#		define SREG12 register(c20)
#		define SREG13 register(c21)
#		define SREG14 register(c22)
#		define SREG15 register(c23)
#		define SREG16 register(c24)
#		define SREG17 register(c25)
#		define SREG18 register(c26)
#		define SREG19 register(c27)
#		define SREG20 register(c28)
#		define SREG21 register(c29)
#		define SREG22 register(c30)
#		define SREG23 register(c31)
#		define SREG24 register(c32)
#		define SREG25 register(c33)
#		define SREG26 register(c34)
#		define SREG27 register(c35)
#		define SREG28 register(c36)
#		define SREG29 register(c37)
#		define SREG30 register(c38)
#		define SREG31 register(c39)
#		define SREG32 register(c40)
#		define SREG33 register(c41)
#		define SREG34 register(c42)
#		define SREG35 register(c43)
#		define SREG36 register(c44)
#		define SREG37 register(c45)
#		define SREG38 register(c46)
#		define SREG39 register(c47)
#		define SREG40 register(c48)
#		define SREG41 register(c49)
#		define SREG42 register(c50)
#		define SREG43 register(c51)
#		define SREG44 register(c52)
#		define SREG45 register(c53)
#		define SREG46 register(c54)
#		define SREG47 register(c55)
#		define SREG48 register(c56)
#		define SREG49 register(c57)
#		define SREG50 register(c58)
#		define SREG51 register(c59)
#		define SREG52 register(c60)
#		define SREG53 register(c61)
#		define SREG54 register(c62)
#		define SREG55 register(c63)

#		define IREG0 register(c64)
#		define IREG1 register(c65)
#		define IREG2 register(c66)
#		define IREG3 register(c67)
#		define IREG4 register(c68)
#		define IREG5 register(c69)
#		define IREG6 register(c70)
#		define IREG7 register(c71)
#		define IREG8 register(c72)
#		define IREG9 register(c73)
#		define IREG10 register(c74)
#		define IREG11 register(c75)
#		define IREG12 register(c76)
#		define IREG13 register(c77)
#		define IREG14 register(c78)
#		define IREG15 register(c79)

#		define PREG0 register(c80)
#		define PREG1 register(c81)
#		define PREG2 register(c82)
#		define PREG3 register(c83)
#		define PREG4 register(c84)
#		define PREG5 register(c85)
#		define PREG6 register(c86)
#		define PREG7 register(c87)
#		define PREG8 register(c88)
#		define PREG9 register(c89)
#		define PREG10 register(c90)
#		define PREG11 register(c91)
#		define PREG12 register(c92)
#		define PREG13 register(c93)
#		define PREG14 register(c94)
#		define PREG15 register(c95)

#	endif
#endif

// for opengl/Cg
#if G_OPENGL
#	define AX_BEGIN_CBUFFER(name, reg)
#	define AX_END_CBUFFER

#	define AX_BEGIN_PC
#	define AX_END_PC

#	define SREG 0
#	define IREG 1
#	define PREG 2

#	define SREG0 BUFFER[0][0]
#	define SREG1 BUFFER[0][16]
#	define SREG2 BUFFER[0][32]
#	define SREG3 BUFFER[0][48]
#	define SREG4 BUFFER[0][64]
#	define SREG5 BUFFER[0][80]
#	define SREG6 BUFFER[0][96]
#	define SREG7 BUFFER[0][112]
#	define SREG8 BUFFER[0][128]
#	define SREG9 BUFFER[0][144]
#	define SREG10 BUFFER[0][160]
#	define SREG11 BUFFER[0][176]
#	define SREG12 BUFFER[0][192]
#	define SREG13 BUFFER[0][208]
#	define SREG14 BUFFER[0][224]
#	define SREG15 BUFFER[0][240]
#	define SREG16 BUFFER[0][256]
#	define SREG17 BUFFER[0][272]
#	define SREG18 BUFFER[0][288]
#	define SREG19 BUFFER[0][304]
#	define SREG20 BUFFER[0][320]
#	define SREG21 BUFFER[0][336]
#	define SREG22 BUFFER[0][352]
#	define SREG23 BUFFER[0][368]
#	define SREG24 BUFFER[0][384]
#	define SREG25 BUFFER[0][400]
#	define SREG26 BUFFER[0][416]
#	define SREG27 BUFFER[0][432]
#	define SREG28 BUFFER[0][448]
#	define SREG29 BUFFER[0][464]
#	define SREG30 BUFFER[0][480]
#	define SREG31 BUFFER[0][496]
#	define SREG32 BUFFER[0][512]
#	define SREG33 BUFFER[0][528]
#	define SREG34 BUFFER[0][544]
#	define SREG35 BUFFER[0][560]
#	define SREG36 BUFFER[0][576]
#	define SREG37 BUFFER[0][592]
#	define SREG38 BUFFER[0][608]
#	define SREG39 BUFFER[0][624]
#	define SREG40 BUFFER[0][640]
#	define SREG41 BUFFER[0][656]
#	define SREG42 BUFFER[0][672]
#	define SREG43 BUFFER[0][688]
#	define SREG44 BUFFER[0][704]
#	define SREG45 BUFFER[0][720]
#	define SREG46 BUFFER[0][736]
#	define SREG47 BUFFER[0][752]
#	define SREG48 BUFFER[0][768]
#	define SREG49 BUFFER[0][784]
#	define SREG50 BUFFER[0][800]
#	define SREG51 BUFFER[0][816]
#	define SREG52 BUFFER[0][832]
#	define SREG53 BUFFER[0][848]
#	define SREG54 BUFFER[0][864]
#	define SREG55 BUFFER[0][880]

#	define IREG0 BUFFER[1][0]
#	define IREG1 BUFFER[1][16]
#	define IREG2 BUFFER[1][32]
#	define IREG3 BUFFER[1][48]
#	define IREG4 BUFFER[1][64]
#	define IREG5 BUFFER[1][80]
#	define IREG6 BUFFER[1][96]
#	define IREG7 BUFFER[1][112]
#	define IREG8 BUFFER[1][128]
#	define IREG9 BUFFER[1][144]
#	define IREG10 BUFFER[1][160]
#	define IREG11 BUFFER[1][176]
#	define IREG12 BUFFER[1][192]
#	define IREG13 BUFFER[1][208]
#	define IREG14 BUFFER[1][224]
#	define IREG15 BUFFER[1][240]

#	define PREG0 BUFFER[2][0]
#	define PREG1 BUFFER[2][16]
#	define PREG2 BUFFER[2][32]
#	define PREG3 BUFFER[2][48]
#	define PREG4 BUFFER[2][64]
#	define PREG5 BUFFER[2][80]
#	define PREG6 BUFFER[2][96]
#	define PREG7 BUFFER[2][112]
#	define PREG8 BUFFER[2][128]
#	define PREG9 BUFFER[2][144]
#	define PREG10 BUFFER[2][160]
#	define PREG11 BUFFER[2][176]
#	define PREG12 BUFFER[2][192]
#	define PREG13 BUFFER[2][208]
#	define PREG14 BUFFER[2][224]
#	define PREG15 BUFFER[2][240]
#endif

// for c++
#ifdef __cplusplus
#	define AX_BEGIN_CBUFFER(name, reg)
#	define AX_END_CBUFFER

#	define AX_BEGIN_PC
#	define AX_END_PC

#	define SREG0 0
#	define SREG1 1
#	define SREG2 2
#	define SREG3 3
#	define SREG4 4
#	define SREG5 5
#	define SREG6 6
#	define SREG7 7
#	define SREG8 8
#	define SREG9 9
#	define SREG10 10
#	define SREG11 11
#	define SREG12 12
#	define SREG13 13
#	define SREG14 14
#	define SREG15 15
#	define SREG16 16
#	define SREG17 17
#	define SREG18 18
#	define SREG19 19
#	define SREG20 20
#	define SREG21 21
#	define SREG22 22
#	define SREG23 23
#	define SREG24 24
#	define SREG25 25
#	define SREG26 26
#	define SREG27 27
#	define SREG28 28
#	define SREG29 29
#	define SREG30 30
#	define SREG31 31
#	define SREG32 32
#	define SREG33 33
#	define SREG34 34
#	define SREG35 35
#	define SREG36 36
#	define SREG37 37
#	define SREG38 38
#	define SREG39 39
#	define SREG40 40
#	define SREG41 41
#	define SREG42 42
#	define SREG43 43
#	define SREG44 44
#	define SREG45 45
#	define SREG46 46
#	define SREG47 47
#	define SREG48 48
#	define SREG49 49
#	define SREG50 50
#	define SREG51 51
#	define SREG52 52
#	define SREG53 53
#	define SREG54 54
#	define SREG55 55

#	define IREG0 0
#	define IREG1 1
#	define IREG2 2
#	define IREG3 3
#	define IREG4 4
#	define IREG5 5
#	define IREG6 6
#	define IREG7 7
#	define IREG8 8
#	define IREG9 9
#	define IREG10 10
#	define IREG11 11
#	define IREG12 12
#	define IREG13 13
#	define IREG14 14
#	define IREG15 15

#	define PREG0 0
#	define PREG1 1
#	define PREG2 2
#	define PREG3 3
#	define PREG4 4
#	define PREG5 5
#	define PREG6 6
#	define PREG7 7
#	define PREG8 8
#	define PREG9 9
#	define PREG10 10
#	define PREG11 11
#	define PREG12 12
#	define PREG13 13
#	define PREG14 14
#	define PREG15 15

#else

#	define AX_ITEM(stype, ctype, name, reg) stype name : reg;
#	define AX_ARRAY(stype, ctype, name,  n, reg) stype name[n] : reg;

#endif

