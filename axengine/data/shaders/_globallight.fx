/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



// draw point light to light buffer

#include "common.fxh"
#include "shadow.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// features typedef
#define F_SHADOWED			G_FEATURE0
#define F_DIRECTION_LIGHT	G_FEATURE1
#define F_SKY_LIGHT			G_FEATURE2
#define F_ENV_LIGHT			G_FEATURE3

float4 s_lightPos;		// (xyz)*invR, invR
float4 s_lightColor;// = float4(1,1,1,1);
float4 s_skyColor;// = float4(0.5,0.5,0.5,0.5);
float4 s_envColor;// = float4(0,0,0,0);

struct ShadowVertexOut {
    float4	hpos	: POSITION;
	float4	screenTc: TEXCOORD0;
	float4	viewDir	: TEXCOORD1;
};

half getShadow(float3 worldpos, float depth)
{
#if F_SHADOWED
	return SampleShadowCsmAtlas(worldpos, depth);
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

	float3 worldpos = g_cameraPos.xyz + IN.viewDir.xyz / IN.viewDir.w * gbuffer.a;

	half3 L = s_lightPos.xyz;
	half3 N = gbuffer.xyz;
	half3 E = normalize(-IN.viewDir.xyz);

	half NdotL = saturate(dot(N, L));

	half3 R = 2 * NdotL * N - L;
	//	half3 R = reflect(L, N);
	half RdotE = saturate(dot(E, R));

#if F_DIRECTION_LIGHT
	result.xyz = s_lightColor.xyz * NdotL;
	result.w = pow(RdotE, 10) * NdotL * s_lightColor.w;
	result *= getShadow(worldpos, gbuffer.a);
#endif

#if F_SKY_LIGHT
	result.xyz += lerp(s_skyColor.xyz * 0.5, s_skyColor.xyz, N.z*0.5+0.5);
#endif

#if F_ENV_LIGHT
	result.xyz += s_envColor.xyz;
#endif

	return result * 0.25;
}



technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_3_0 VP_main();
		FRAGMENTPROGRAM = compile FP_3_0 FP_main();

		DEPTHTEST = false;
		CULL_FRONT;
		DEPTHMASK = false;
		BLEND_ADD;
    }
}

