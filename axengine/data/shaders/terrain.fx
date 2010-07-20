/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "common.fxh"
#include "fog.fxh"
#include "light.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueZpass = "zpass";
	string TechniqueShadowGen = "shadowGen";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "layer";
> = 0.8;

#define S_VERTICAL_PROJECTION	G_FEATURE0
#define S_FIRST_LAYER			G_FEATURE1

struct TerrainVertexIn {
	float3 xyz			: POSITION;
};

struct TerrainVertexOut {
	float4	hpos		: POSITION;
	float4	screenTc	: TEXCOORD1;
	float2	zoneTC		: TEXCOORD0;
	float3	eyevec		: TEXCOORD4;
	float	fog			: FOG;
};

struct LayerVertexOut {
	float4	hpos		: POSITION;
#if 0
	float2	zoneTC		: TEXCOORD0;
	float2	chunkTC		: TEXCOORD1;
#else
	float4	streamTc	: TEXCOORD0;
#endif
	float4	viewDir		: TEXCOORD1;	// .w store length, for refraction fog
	float3	worldPos	: TEXCOORD2;
	float4	screenTc	: TEXCOORD3;
	float	fog			: TEXCOORD4;
};


#if 0
// terrain parameter
float4	g_terrainRect;
float4	g_zoneRect;		// zone x, y, w, h
float4	g_chunkRect;
float2	g_layerScale;

struct TerrainConst {
	float4	zoneRect;		// zone x, y, w, h
	float4	chunkRect;
};

//AX_DECL_PRIMITIVECONST(TerrainConst);
TerrainConst g_pc;
#define g_zoneRect g_pc.zoneRect
#define g_chunkRect g_pc.chunkRect
#endif

AX_BEGIN_PC
	float4 g_zoneRect : PREG0;
	float4 g_chunkRect : PREG1;
AX_END_PC

//------------------------------------------------------------------------------
// VP_zpass
//------------------------------------------------------------------------------

float4 VP_zpass(TerrainVertexIn IN) : POSITION
{
	return VP_worldToClip(IN.xyz);
}

//------------------------------------------------------------------------------
// FP_zpass
//------------------------------------------------------------------------------

half4 FP_zpass(float4 hpos : POSITION) : COLOR
{
	return 0;
}

//------------------------------------------------------------------------------
// VP_gpass
//------------------------------------------------------------------------------

struct TerrainGpassOut {
	float4 hpos		: POSITION;
	float4 screenTc	: TEXCOORD0;
	float2 zoneTc	: TEXCOORD1;
};

TerrainGpassOut VP_gpass(TerrainVertexIn IN)
{
	TerrainGpassOut OUT;

	OUT.hpos = VP_worldToClip(IN.xyz);
	OUT.screenTc = Clip2Screen(OUT.hpos);
	OUT.zoneTc.xy = (IN.xyz.xy - g_zoneRect.xy) / g_zoneRect.zw;

	return OUT;
}

//------------------------------------------------------------------------------
// FP_gpass
//------------------------------------------------------------------------------

half4 FP_gpass(TerrainGpassOut IN) : COLOR
{
	half4 result;

	result.xyz = GetNormal(g_terrainNormal, IN.zoneTc.xy).xyz;
	result.w = IN.screenTc.w;

	return result;
}


//------------------------------------------------------------------------------
// VP_main
//------------------------------------------------------------------------------

TerrainVertexOut VP_main(TerrainVertexIn IN)
{
	TerrainVertexOut OUT = (TerrainVertexOut)0;

	OUT.zoneTC.xy = (IN.xyz.xy - g_zoneRect.xy) / g_zoneRect.zw;

	OUT.eyevec = g_cameraPos.xyz - IN.xyz;

	OUT.hpos = VP_worldToClip(IN.xyz);

	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.fog = IN.xyz.z;

	return OUT;
}

//------------------------------------------------------------------------------
// FP_main
//------------------------------------------------------------------------------

half4 FP_main(LayerVertexOut IN) : COLOR
{
	half4 result = 1;

	half3 N = GetNormal(g_terrainNormal, IN.streamTc.xy).rgb;

	LightParams lps = (LightParams)0;

	lps.worldpos = IN.worldPos;
	lps.normal = N;
	lps.Cd = tex2D(g_terrainColor, IN.streamTc.xy).xyz;
	lps.Ca.xyz = lps.Cd;
	lps.Ca.w = 0.2f;
	lps.screenTc = IN.screenTc;

#if !G_DISABLE_SPECULAR
	lps.calcSpecular = true;
	lps.viewDir = normalize(IN.viewDir.xyz);
	lps.shiness = 20;
#else
	lps.calcSpecular = false;
#endif

#if G_HAVE_SPECULAR
	lps.Cs = tex2D(g_specularMap, IN.streamTc.xy).xyz;
#else
	lps.Cs = Dif2Spec(lps.Cd);
#endif

	lps.fog = IN.fog;
	result.xyz = LT_calcAllLights(lps);

#if 0
	float d = length(IN.eyevec);
	d = computeHeightfogFactor(g_cameraPos.z - IN.eyevec.z, d);
	d = computeFog(d);
	result.xyz = lerp(g_fogParam.fogColor, result.xyz, d);
#endif

	return result;
}

//------------------------------------------------------------------------------
// VP_layer
//------------------------------------------------------------------------------

LayerVertexOut VP_layer(TerrainVertexIn IN)
{
	LayerVertexOut OUT = (LayerVertexOut)0;

	OUT.streamTc.xy = (IN.xyz.xy - g_zoneRect.xy) / g_zoneRect.zw;
	OUT.streamTc.zw = (IN.xyz.xy - g_chunkRect.xy) / g_chunkRect.zw;
	OUT.worldPos = IN.xyz;
	OUT.viewDir.xyz = g_cameraPos.xyz - OUT.worldPos;
	OUT.viewDir.w = length(OUT.viewDir.xyz);

	OUT.hpos = VP_worldToClip(IN.xyz);
	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.fog = FOG_compute(OUT.hpos);

	return OUT;
}

half4 getSampler(sampler2D smpl, float3 worldpos, half3 normal)
{
#if !S_VERTICAL_PROJECTION	// horizon projection
	float2 tc = worldpos.xy * g_layerScale;
	half4 result = tex2D(smpl, tc);
	return result;
#else
	float4 tc = worldpos.xzyz * g_layerScale.xyxy;
	half4 xproj = tex2D(smpl, tc.xy);
	half4 yproj = tex2D(smpl, tc.zw);

	half2 absnormal = abs(normal.xy);
	half factor = absnormal.x / (absnormal.x + absnormal.y);
//	factor = smoothstep(0, 1, factor);

	return lerp(xproj, yproj, factor);

#endif
}

//------------------------------------------------------------------------------
// FP_layer
//------------------------------------------------------------------------------

half4 FP_layer(LayerVertexOut IN) : COLOR
{
	half3 N = GetNormal(g_terrainNormal, IN.streamTc.xy).rgb;
//	return tex2D(g_terrainNormal, IN.streamTc.xy);

	half4 basecolor = tex2D(g_terrainColor, IN.streamTc.xy);
	half4 detail = getSampler(g_diffuseMap, IN.worldPos, N);

#if G_HAVE_LAYERALPHA
	half alpha = tex2D(g_layerAlpha, IN.streamTc.zw).a;
#else
	half alpha = 1;
#endif
	float dist = IN.viewDir.w;
	alpha *= HardStep(256, 224, dist);

	LightParams lps = (LightParams)0;
	lps.worldpos = IN.worldPos;
	lps.normal = N;
	lps.Cd = basecolor.xyz + detail.xyz - 0.5;

#if S_FIRST_LAYER
	lps.Cd = lerp(basecolor.xyz, lps.Cd, alpha);
#endif

	lps.Ca.xyz = lps.Cd;
	lps.Ca.w = 0.2f;
	lps.calcSpecular = false;
	lps.screenTc = IN.screenTc;

#if NO_NORMALMAPPING
	lps.normal = N;
#else
	half3 T = half3(N.z, -N.y, -N.x);
	half3 B = half3(-N.x, N.z, -N.y);
	half3x3 axis = half3x3(T, B, N);

	half3 normal = getSampler(g_normalMap, IN.worldPos, N).rgb * 2 - 1;

	lps.normal = mul(normal, axis);
#if S_FIRST_LAYER
	lps.normal = lerp(N, lps.normal, alpha);
//	normalize(lps.normal);
#endif

#endif

#if !G_DISABLE_SPECULAR
	lps.calcSpecular = true;
	lps.viewDir = normalize(IN.viewDir.xyz);
	lps.shiness = 20;
#if G_HAVE_SPECULAR
	lps.Cs = getSampler(g_specularMap, IN.worldPos, N).xyz;
#else
	lps.Cs = Dif2Spec(lps.Cd);
#endif

#if S_FIRST_LAYER
	lps.Cs = lerp((half3)(Dif2Spec(basecolor.xyz)), lps.Cs, alpha);
#endif

#else
	lps.calcSpecular = false;
#endif

	lps.fog = IN.fog;

	half4 result;
	result.xyz = LT_calcAllLights(lps);

#if S_FIRST_LAYER
	result.a = 1;
#else
	result.a = alpha;
#endif

	return result;
}



//------------------------------------------------------------------------------
// TECHNIQUES
//------------------------------------------------------------------------------

technique zpass {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_gpass();
		FRAGMENTPROGRAM = compile FP_2_0 FP_gpass();

		DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_ENABLED;
		BLEND_NONE;
	}
}

technique shadowGen {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_zpass();
		FRAGMENTPROGRAM = compile FP_2_0 FP_zpass();

		DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_ENABLED;
		BLEND_NONE;
	}
}


technique main {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_layer();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();

		DEPTHTEST = true;
		DEPTHMASK_MAIN;
		CULL_ENABLED;
		BLEND_NONE;
	}
}

technique layer {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_layer();
		FRAGMENTPROGRAM = compile FP_2_0 FP_layer();

		DEPTHTEST = true;
		DEPTHMASK = false;
		CULL_ENABLED;
#if !S_FIRST_LAYER
		BLEND_BLEND;
#else
		BLEND_NONE;
#endif
	}
}

/***************************** eof ***/
