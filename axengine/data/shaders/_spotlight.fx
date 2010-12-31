/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



// draw point light to light buffer

#include "common.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// features typedef
#define F_MASKFRONT G_FEATURE0
#define F_SPECULAR	G_FEATURE1
#define F_PROJECTOR	G_FEATURE2
#define F_SHADOWED	G_FEATURE3

float4x4	s_shadowMatrix;

float4x4	s_lightMatrix;
float4		s_lightCenter;		// (xyz)*invR, invR
float4		s_lightColor = float4(1,1,1,1);

float2 s_pixelOffsets[4] = {
	{ -0.5, -0.5 },
	{ -0.5, 0.5 },
	{ 0.5, -0.5 },
	{ 0.5, 0.5 },
};

float2 s_texelOffsets[4]
<
	string pixelToTexel = "s_pixelOffsets";
>;

struct ShadowVertexOut {
    float4	hpos	: POSITION;
	float4	screenTc: TEXCOORD0;
	float4	viewDir	: TEXCOORD1;
};

half getShadow(float3 worldpos, float depth)
{
#if F_SHADOWED
	static const float bias = 2.0 / 65536.0;
	static const float delta = 0.000001;

	float4 shadowPos = mul(s_shadowMatrix, float4(worldpos,1));

	shadowPos /= shadowPos.w;

	// fix alias, sub a viewspace bias
	shadowPos.z -= depth * delta;

	half shadow = 0;

#if 0
	shadow = Shadow2D(g_diffuseMap, shadowPos);
	return shadow;
#endif

	UNROLL
	for(int i = 0; i < 4; i++) {
		float4 tc = float4(shadowPos.xy + s_texelOffsets[i], shadowPos.zw);
		shadow += tex2Dproj(g_diffuseMap, tc).r;
	}
	shadow *= 0.25f;
	return shadow;
#else
	return 1;
#endif
}

ShadowVertexOut VP_main(VertexIn IN) {
	ShadowVertexOut OUT;

	float3 worldpos = VP_modelToWorld(IN, IN.position);

	OUT.hpos = VP_worldToClip(worldpos);

	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.viewDir.xyz = worldpos - g_cameraPos.xyz;
	OUT.viewDir.w = OUT.hpos.w;

//	OUT.viewDir.xyz /= OUT.viewDir.w;

	return OUT;
}

half4 FP_main(ShadowVertexOut IN) : COLOR {
	half4 result = 0;

	// get gbuffer
	half4 gbuffer = tex2Dproj(g_geoBuffer, IN.screenTc);

	half3 worldpos = g_cameraPos.xyz + IN.viewDir.xyz / IN.viewDir.w * gbuffer.a;

	half4 lightPos = mul(s_lightMatrix, worldpos);

	lightPos.xy /= lightPos.z;

	half3 L = s_lightCenter.xyz - worldpos.xyz;

	L = normalize(L);
	half3 N = gbuffer.xyz;
	half3 E = normalize(-IN.viewDir.xyz);

	half NdotL = saturate(dot(N, L));
//	return NdotL.xxxx;

	half3 R = 2 * NdotL * N - L;
	half RdotE = saturate(dot(E, R));

	result.xyz = NdotL;;

#if 1//F_SPECULAR
	result.w = pow(RdotE, 10) * NdotL;
#endif

	return result * getShadow(worldpos, gbuffer.a);
}



technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
#if 0//F_MASKFRONT
	    DEPTHTEST = true;
		CULL_BACK;
#else
		DEPTHTEST = false;
		CULL_FRONT;
#endif
		DEPTHMASK = false;

		BLEND_ADD;
    }
}

