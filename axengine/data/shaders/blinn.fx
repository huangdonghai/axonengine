/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"
#include "light.fxh"
#include "fog.fxh"

#define S_DECAL G_FEATURE0
#define S_ALPHATEST G_FEATURE1
#define S_TWOSIDES G_FEATURE2

float Script : STANDARDSGLOBAL <
	// sort hint
#if S_DECAL
	int SortHint = SortHint_decal;
#else
	int SortHint = SortHint_opacit;
#endif

	// technique
	string TechniqueZpass = "zpass";
#if S_DECAL
	string TechniqueShadowGen = "";
#else
	string TechniqueShadowGen = "shadowgen";
#endif
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// UN-TWEAKABLES

struct ShadowVertexOut {
	float4 hpos			: POSITION;
	float2 diffuseTc	: TEXCOORD0;
};


ShadowVertexOut VP_zpass(VertexIn IN) {
	ShadowVertexOut OUT;
	OUT.hpos = VP_modelToClip(IN, IN.xyz);
	OUT.diffuseTc = IN.st;
	return OUT;
}

half4 FP_zpass(ShadowVertexOut IN) : COLOR {
#if !S_ALPHATEST
	return 1;
#else
#if G_HAVE_DIFFUSE
	half alpha = tex2D(g_diffuseMap, IN.diffuseTc).a;
#else
	half alpha = 1;
#endif
	if (alpha < 0.5)
		discard;
	return 1;
#endif
}

GpassOut VP_gpass(VertexIn IN) {
	GpassOut OUT = (GpassOut)0;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal);
#if !NO_NORMALMAPPING
	OUT.tangent = N_modelToWorld(IN, IN.tangent);
	OUT.binormal = N_modelToWorld(IN, IN.binormal);
#endif
	float3 posWorld = VP_modelToWorld(IN, IN.xyz);

	float4 posClip = VP_worldToClip(posWorld);
	OUT.hpos = posClip;

	OUT.screenTc = Clip2Screen(posClip);

	OUT.streamTc = VP_computeStreamTc(IN);

	return OUT;
}

half4 FP_gpass(GpassOut IN) : COLOR {
	half3 N;

#if NO_NORMALMAPPING
	N = normalize(IN.normal);
#else
	N = FP_GetNormal(IN.normal, IN.tangent, IN.binormal, IN.streamTc.xy);
#endif

#if S_DECAL || S_ALPHATEST
#if G_HAVE_DIFFUSE
	half alpha = tex2D(g_diffuseMap, IN.streamTc.xy).a;
#else
	half alpha = 1;
#endif
	if (alpha < 0.5)
		discard;
#endif
	return half4(N,IN.screenTc.w);
}

/*********** Generic Vertex Shader ******/

VertexOut VP_main(VertexIn IN) {
    VertexOut OUT = (VertexOut)0;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal);
#if !NO_NORMALMAPPING
	OUT.tangent = N_modelToWorld(IN, IN.tangent);
	OUT.binormal = N_modelToWorld(IN, IN.binormal);
#endif
	float3 posWorld = VP_modelToWorld(IN, IN.xyz);
	OUT.worldPos = posWorld;

	float4 posClip = VP_worldToClip(posWorld);
	OUT.hpos = posClip;

	OUT.screenTc = Clip2Screen(posClip);

#if G_OPENGL
	OUT.color = IN.color;
#else
	OUT.color = IN.color.bgra;
#endif
	OUT.color.rgb *= VP_getInstanceParam(IN).rgb;

	VP_final(IN, OUT);

    return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
#if 0 && G_HAVE_DETAIL_NORMAL
	return tex2D(g_detailNormalMap, IN.streamTc.xy);
#endif

	LightParams lps;

#if G_D3D
	lps = (LightParams)0;
#endif

	lps.worldpos = IN.worldPos;

	lps.calcSpecular = false;
	lps.screenTc = IN.screenTc;

	half3 detail = 0;
#if G_HAVE_DETAIL
	detail = tex2D(g_detailMap, IN.streamTc.xy * g_layerScale) - 0.5;
#endif

#if G_HAVE_DIFFUSE
    lps.Cd = tex2D(g_diffuseMap, IN.streamTc.xy).xyz;
#else
	lps.Cd = half3(1, 1, 1);
#endif
	lps.Cd += detail;
	lps.Cd *= IN.color.rgb;

#if S_DECAL || S_ALPHATEST
#if G_HAVE_DIFFUSE
	half alpha = tex2D(g_diffuseMap, IN.streamTc.xy).a;
#else
	half alpha = 1;
#endif
#endif
#if S_ALPHATEST
	if (alpha < 0.5)
		discard;
#endif

#if G_HAVE_LIGHTMAP
	lps.Cd *= saturate(tex2D(g_lightMap, IN.streamTc.zw).x);
//	return tex2D(g_lightMap, IN.streamTc.zw);
#endif
	lps.Ca.xyz = lps.Cd;
	lps.Ca.w = 0.2f;

	half3 N;

#if NO_NORMALMAPPING
	lps.normal = normalize(IN.normal);
#else
#if 0
	half3 normal = GetNormal(g_normalMap, IN.streamTc.xy).xyz;

	half3x3 axis = half3x3(IN.tangent,IN.binormal, IN.normal);
	N = mul(normal, axis);
	lps.normal = normalize(N);
#else
	lps.normal = FP_GetNormal(IN.normal, IN.tangent, IN.binormal, IN.streamTc.xy);
#endif
#endif

#if !G_DISABLE_SPECULAR
	lps.calcSpecular = true;
	lps.viewDir = normalize(g_cameraPos.xyz - IN.worldPos);
	lps.shiness = 20;
#else
	lps.calcSpecular = false;
#endif

#if G_HAVE_SPECULAR
	lps.Cs = tex2D(g_specularMap, IN.streamTc.xy).xyz + detail;
#else
#if G_HAVE_NORMAL
	lps.Cs = tex2D(g_normalMap, IN.streamTc.xy).a + detail;
#else
	lps.Cs = Dif2Spec(lps.Cd);
#endif
#endif

#if G_FOG
	lps.fog = IN.fog;
#else
	lps.fog = 1;
#endif

	half4 final;
	final.rgb = LT_calcAllLights(lps);

#if G_HAVE_EMISSION
	final.rgb += tex2D(g_emissionMap, IN.streamTc.xy).xyz;
#endif

#if S_DECAL || S_ALPHATEST
	final.a = alpha;
#else
	final.a = 1;
#endif

	return final;
}


//------------------------------------------------------------------------------
// TECHNIQUES
//------------------------------------------------------------------------------

technique zpass {
    pass p0 {
        VertexShader = compile VP_2_0 VP_gpass();
		PixelShader = compile FP_2_0 FP_gpass();

	    DEPTHTEST = true;
#if S_DECAL
		DEPTHMASK = false;
#else
		DEPTHMASK = true;
#endif
		CULL_ENABLED;
		BLEND_NONE;
    }
}

technique shadowgen {
    pass p0 {
        VertexShader = compile VP_2_0 VP_zpass();
		PixelShader = compile FP_2_0 FP_zpass();

	    DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_ENABLED;
		BLEND_NONE;
    }
}


technique main {
    pass p0 {
        VertexShader = compile VP_2_0 VP_main();
        PixelShader = compile FP_2_0 FP_main();

	    DEPTHTEST = true;
		DEPTHMASK_MAIN;
		CULL_ENABLED;
#if S_DECAL
		BLEND_BLEND;
#else
		BLEND_NONE;
#endif
    }
}

/***************************** eof ***/
