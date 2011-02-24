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
	string TechniqueGeoFill = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// features typedef
#define F_SHADOWED			M_FEATURE0
#define F_DIRECTION_LIGHT	M_FEATURE1
#define F_SKY_LIGHT			M_FEATURE2
#define F_ENV_LIGHT			M_FEATURE3

AX_BEGIN_PC
	float4 s_lightPos : PREG0; // (xyz)*invR, invR
	float4 s_lightColor : PREG1; // = float4(1,1,1,1);
	float4 s_skyColor : PREG2; // = float4(0.5,0.5,0.5,0.5);
	float4 s_envColor : PREG3; // = float4(0,0,0,0);
	float4x4 s_splitRanges : PREG4;
AX_END_PC

float4x4 GetSplitRanges()
{ return s_splitRanges; }


struct LightVertexOut
{
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

LightVertexOut VP_main(ChunkVertex IN)
{
	LightVertexOut OUT;

	float3 worldpos = VP_modelToWorld(IN.position);

	OUT.hpos = VP_worldToClip(worldpos);

	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.viewDir.xyz = worldpos - g_cameraPos.xyz;
	OUT.viewDir.w = OUT.hpos.w;

//	OUT.viewDir.xyz /= OUT.viewDir.w;

	return OUT;
}

half4 FP_main(LightVertexOut IN) : COLOR
{
	half4 OUT = 0;

	// get gbuffer
	DeferredData data = GB_Input(IN.viewDir, IN.screenTc);

	half3 L = s_lightPos.xyz;
	half3 N = data.normal;
	half3 E = normalize(-IN.viewDir.xyz);

	half NdotL = saturate(dot(N, L));

	half3 R = 2 * NdotL * N - L;
	//	half3 R = reflect(L, N);
	half RdotE = saturate(dot(E, R));

#if F_DIRECTION_LIGHT
	OUT.xyz = s_lightColor.xyz * NdotL;
	OUT.w = pow(RdotE, data.shiness) * NdotL/* * s_lightColor.w*/;
	OUT *= getShadow(data.worldPos, data.viewDepth);
#endif

#if F_SKY_LIGHT
	OUT.xyz += lerp(s_skyColor.xyz * 0.5, s_skyColor.xyz, N.z*0.5+0.5);
#endif

#if F_ENV_LIGHT
	OUT.xyz += s_envColor.xyz;
#endif

	OUT.xyz = OUT.xyz * data.diffuse + OUT.w * data.specular;
	return OUT;
}



technique main
{
    pass p0 {
        VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();
    }
}

