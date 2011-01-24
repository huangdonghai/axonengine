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
	// technique
	string TechniqueGeoFill = "gfill";
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

ShadowVertexOut VP_zpass(MeshVertex IN)
{
	ShadowVertexOut OUT;
	OUT.hpos = VP_modelToClip(IN, IN.position);
	OUT.diffuseTc = IN.streamTc.xy;
	return OUT;
}

half4 FP_zpass(ShadowVertexOut IN) : COLOR
{
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

Gbuffer FP_gpass(VertexOut IN)
{
	Gbuffer OUT=(Gbuffer)0;

	half3 detail = 0;
#if G_HAVE_DETAIL
	detail = tex2D(g_detailMap, IN.streamTc.xy * g_detailScale).xyz - 0.5;
#endif

#if G_HAVE_DIFFUSE
    OUT.albedo.xyz = tex2D(g_diffuseMap, IN.streamTc.xy).xyz;
#else
	OUT.albedo.xyz = half3(1, 1, 1);
#endif
	OUT.albedo.xyz += detail;
	OUT.albedo.xyz *= IN.color.rgb;

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

	half3 N;

#if NO_NORMALMAPPING
	OUT.normal.xyz = normalize(IN.normal);
#else
	OUT.normal.xyz = FP_GetNormal(IN.normal, IN.tangent, IN.binormal, IN.streamTc.xy);
#endif
	OUT.normal.xyz = OUT.normal.xyz * 0.5f + 0.5f;

	OUT.misc.w = g_matShiness;

	half3 spec;
#if G_HAVE_SPECULAR
	spec = tex2D(g_specularMap, IN.streamTc.xy).xyz + detail;
#else
#if G_HAVE_NORMAL
	spec = tex2D(g_normalMap, IN.streamTc.xy).a + detail;
#else
	spec = OUT.albedo.xyz;
#endif
#endif
	OUT.albedo.w = Rgb2Lum(spec);

#if G_HAVE_EMISSION
	OUT.accum.xyz = tex2D(g_emissionMap, IN.streamTc.xy).xyz;
#endif

#if S_DECAL || S_ALPHATEST
	OUT.accum.a = alpha;
#else
	OUT.accum.a = 1;
#endif

	OUT.accum = OUT.normal;

	return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR0
{
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
	detail = tex2D(g_detailMap, IN.streamTc.xy * g_detailScale) - 0.5;
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

technique gfill {
    pass p0 {
        VertexShader = compile VP_2_0 OutputMeshVertex();
		PixelShader = compile FP_2_0 FP_gpass();
    }
}

technique shadowgen {
    pass p0 {
        VertexShader = compile VP_2_0 VP_zpass();
		PixelShader = compile FP_2_0 FP_zpass();
    }
}


technique main {
    pass p0 {
        VertexShader = compile VP_2_0 OutputMeshVertex();
        PixelShader = compile FP_2_0 FP_main();

#if 0
	    DEPTHTEST = true;
		DEPTHMASK_MAIN;
		CULL_ENABLED;
#if S_DECAL
		BLEND_BLEND;
#else
		BLEND_NONE;
#endif
#endif
    }
}

/***************************** eof ***/
