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
#define F_MASKFRONT		G_FEATURE0
#define F_SPECULAR		G_FEATURE1
#define F_PROJECTOR		G_FEATURE2
#define F_SHADOWED		G_FEATURE3
#define F_SPOTLIGHT		G_FEATURE4
#define F_BOXFALLOFF	G_FEATURE5

float4x4	s_lightMatrix;
float4		s_lightPos;		// (xyz)*invR, invR
float4		s_lightColor = float4(1,1,1,1);

struct ShadowVertexOut {
    float4	hpos	: POSITION;
	float4	screenTc: TEXCOORD0;
	float4	viewDir	: TEXCOORD1;
};

half getShadow(float3 worldpos, float depth)
{
#if F_SHADOWED
	return SampleShadow(worldpos,depth);
#else
	return 1;
#endif
}

ShadowVertexOut VP_main(VertexIn IN) {
	ShadowVertexOut OUT;

	float3 worldpos = VP_modelToWorld(IN, IN.xyz);

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
	half4 gbuffer = tex2Dproj(g_sceneDepth, IN.screenTc);

	float3 worldpos = g_cameraPos.xyz + IN.viewDir.xyz / IN.viewDir.w * gbuffer.a;

#if F_PROJECTOR || F_SPOTLIGHT
	float4 projTc = mul(s_lightMatrix, float4(worldpos,1));
	projTc.xy /= projTc.z;

//	projTc.xyz = abs(projTc.xyz);
//	projTc.xy *= projTc.xy > projTc.yx;

	half falloff = saturate(1.0f - dot(projTc.xyz,projTc.xyz));
//	return falloff;

	float3 lightPos = s_lightPos.xyz - worldpos;
#else
	float3 lightPos = (s_lightPos.xyz - worldpos.xyz) * s_lightPos.w;
	half falloff = saturate(1.0f - dot(lightPos.xyz, lightPos.xyz));
#endif

	half3 L = normalize(lightPos.xyz);
	half3 N = gbuffer.xyz;
	half3 E = normalize(-IN.viewDir.xyz);

	half NdotL = saturate(dot(N, L));
//	return NdotL.xxxx;

	half3 R = 2 * NdotL * N - L;
//	half3 R = reflect(L, N);
	half RdotE = saturate(dot(E, R));

	result.xyz = s_lightColor.xyz * NdotL;

#if 1//F_SPECULAR
	result.w = pow(RdotE, 10) * NdotL * s_lightColor.w;
#endif

	return result * falloff * getShadow(worldpos, gbuffer.a) * 0.25;
}



technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_3_0 VP_main();
		FRAGMENTPROGRAM = compile FP_3_0 FP_main();
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

