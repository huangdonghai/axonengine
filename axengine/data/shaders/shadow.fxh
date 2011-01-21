/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


float4 g_shadowPixelOffsets[2] = {
	{ -0.5, -0.5, -0.5, 0.5 },
	{ 0.5, -0.5, 0.5, 0.5 }
};

float4 g_shadowTexelOffsets[2]
<
	string pixelToTexel = "g_shadowPixelOffsets";
>;

float4 s_splitRanges[4];
#if 0
= {
	{ 0,	0.5,	0.5,	1 },
	{ 0.5,	0.5,	1,		1 },
	{ 0,	0,		0.5,	0.5 },
	{ 0.5,	0,		1,		0.5 },
};
#endif

#define g_shadowMatrix g_texMatrix

half SampleShadow(float3 worldpos, float depth)
{
	static const float bias = 2.0 / 65536.0;
	static const float delta = 0.000002;

	float4 shadowPos = mul(g_shadowMatrix, float4(worldpos,1));

	shadowPos /= shadowPos.w;

	// fix alias, sub a viewspace bias
	shadowPos.z -= depth * delta;
	half shadow = 0;

	UNROLL
	for (int i = 0; i < 2; i++) {
		float4 tc = float4(shadowPos.xy + g_shadowTexelOffsets[i].xy, shadowPos.zw);
		shadow += tex2Dproj(g_shadowMap, tc).r;
		tc = float4(shadowPos.xy + g_shadowTexelOffsets[i].zw, shadowPos.zw);
		shadow += tex2Dproj(g_shadowMap, tc).r;
	}
	shadow *= 0.25f;
	return shadow;
}

half SampleShadowCsmAtlas(float3 worldpos, float depth)
{
	static const float bias = 2.0 / 65536.0;
	static const float delta = 0.000002;

	float4 shadowPos = mul(g_shadowMatrix, float4(worldpos,1));

	shadowPos /= shadowPos.w;

	float2 newPos = shadowPos.xy;

	float4 b = newPos.x > s_splitRanges[0] && newPos.x < s_splitRanges[2] && newPos.y > s_splitRanges[1] && newPos.y < s_splitRanges[3];
	if (dot(b,b) == 0)
		return 1;

	float4 os;
	for (int i = 3; i >= 0; i--) {
		if (b[i]) {
			os = g_csmOffsetScales[i];
		}
	}
	newPos = newPos * os.xy + os.zw;

	shadowPos.xy = newPos;

	// fix alias, sub a viewspace bias
	shadowPos.z -= depth * delta;
	half shadow = 0;

	UNROLL
	for (int i = 0; i < 2; i++) {
		float4 tc = float4(shadowPos.xy + g_shadowTexelOffsets[i].xy, shadowPos.zw);
		shadow += tex2Dproj(g_shadowMap, tc).r;
		tc = float4(shadowPos.xy + g_shadowTexelOffsets[i].zw, shadowPos.zw);
		shadow += tex2Dproj(g_shadowMap, tc).r;
	}
	shadow *= 0.25f;

	return shadow;
}