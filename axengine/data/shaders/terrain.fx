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

#define S_VERTICAL_PROJECTION	M_FEATURE0
#define S_VERTICAL_PROJECTION1	M_FEATURE1
#define S_VERTICAL_PROJECTION2	M_FEATURE2
#define S_VERTICAL_PROJECTION3	M_FEATURE3

struct TerrainVertexIn {
	float3 position		: POSITION;
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

AX_BEGIN_PC
	float4 g_zoneRect : PREG0;
	float4 g_chunkRect : PREG1;
AX_END_PC

//------------------------------------------------------------------------------

float4 VP_zpass(TerrainVertexIn IN) : POSITION
{
	return VP_worldToClip(IN.position);
}

//------------------------------------------------------------------------------

half4 FP_zpass(float4 hpos : POSITION) : COLOR
{
	return 0;
}

//------------------------------------------------------------------------------

struct TerrainGpassOut {
	float4 hpos		: POSITION;
	float4 screenTc	: TEXCOORD0;
	float2 zoneTc	: TEXCOORD1;
};

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

#if M_SPECULAR
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

LayerVertexOut VP_layer(TerrainVertexIn IN)
{
	LayerVertexOut OUT = (LayerVertexOut)0;

	OUT.streamTc.xy = (IN.position.xy - g_zoneRect.xy) / g_zoneRect.zw;
	OUT.streamTc.zw = (IN.position.xy - g_chunkRect.xy) / g_chunkRect.zw;
	OUT.worldPos = IN.position;
	OUT.viewDir.xyz = g_cameraPos.xyz - OUT.worldPos;
	OUT.viewDir.w = length(OUT.viewDir.xyz);

	OUT.hpos = VP_worldToClip(IN.position);
	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.fog = FOG_compute(OUT.hpos);

	return OUT;
}

half4 getSampler(sampler2D smpl, float3 worldpos, half3 normal, int layer = 0)
{
	bool4 verticalProjection = {M_FEATURE0,M_FEATURE1,M_FEATURE2,M_FEATURE3};

	if (!verticalProjection[layer]) {
		// horizon projection
		float2 tc = worldpos.xy * g_detailScale[layer];
		half4 result = tex2D(smpl, tc);
		return result;
	} else {
		float4 tc = worldpos.xzyz * g_detailScale[layer];
		half4 xproj = tex2D(smpl, tc.xy);
		half4 yproj = tex2D(smpl, tc.zw);

		half2 absnormal = abs(normal.xy);
		half factor = absnormal.x / (absnormal.x + absnormal.y);

		return lerp(xproj, yproj, factor);
	}
}

//------------------------------------------------------------------------------

GBufferOut FP_layer(LayerVertexOut IN)
{
	GBufferData OUT=(GBufferData)0;

	half3 N = GetNormal(g_terrainNormal, IN.streamTc.xy).rgb;
	half3 basecolor = tex2D(g_terrainColor, IN.streamTc.xy).rgb;
	half3 diffuse = basecolor;
	half3 normal = N;

	bool4 m_details = bool4(M_DETAIL, M_DETAIL1, M_DETAIL2, M_DETAIL3);
	bool4 m_detailnormals = bool4(M_DETAIL_NORMAL, M_DETAIL_NORMAL1, M_DETAIL_NORMAL2, M_DETAIL_NORMAL3);
	bool4 m_layeralpha = bool4(M_LAYERALPHA, M_LAYERALPHA1, M_LAYERALPHA2, M_LAYERALPHA3);

	static const sampler2D m_detailMaps[4] = { g_detailMap, g_detailMap1, g_detailMap2, g_detailMap3 };
	static const sampler2D m_detailNormalMaps[4] = { g_detailNormalMap, g_detailNormalMap1, g_detailNormalMap2, g_detailNormalMap3 };
	static const sampler2D m_layerMaps[4] = { g_layerAlpha, g_layerAlpha1, g_layerAlpha2, g_layerAlpha3 };

	UNROLL
	for (int i = 0; i < M_NUM_LAYERS; i++) {
		half alpha = 1;
		if (m_layeralpha[i]) {
			alpha = tex2D(m_layerMaps[i], IN.streamTc.zw).a;
		}
		float dist = IN.viewDir.w;
		alpha *= smoothstep(256, 224, dist);

		if (m_details[i]) {
			half3 detail = getSampler(m_detailMaps[i], IN.worldPos, N, i).rgb;
			detail = basecolor.xyz + detail.xyz - 0.5;
			diffuse = lerp(diffuse, detail, alpha);
		}
#if !NO_NORMALMAPPING
		if (m_detailnormals[i]) {
			half3 detailNormal = getSampler(m_detailNormalMaps[i], IN.worldPos, N, i).rgb * 2 - 1;
			half3 T = half3(N.z, -N.y, -N.x);
			half3 B = half3(-N.x, N.z, -N.y);
			half3x3 axis = half3x3(T, B, N);
			detailNormal = mul(detailNormal, axis);
			normal = lerp(normal, detailNormal, alpha);
		}
#endif
	}

	OUT.normal = normal;
	OUT.diffuse = diffuse;
	OUT.specular = diffuse;
	OUT.shiness = g_matShiness;

	return GB_Output(OUT);
}



//------------------------------------------------------------------------------
// TECHNIQUES
//------------------------------------------------------------------------------

technique GeoFill
{
	pass p0 {
		VertexShader = compile VS_3_0 VP_layer();
		PixelShader = compile PS_3_0 FP_layer();
	}
}

technique ShadowGen
{
	pass p0 {
		VertexShader = compile VS_3_0 VP_zpass();
		PixelShader = compile PS_3_0 FP_zpass();
	}
}


technique Main
{
	pass p0 {
		VertexShader = compile VS_3_0 VP_layer();
		PixelShader = compile PS_3_0 FP_main();
	}
}

