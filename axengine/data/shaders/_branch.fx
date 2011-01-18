/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"
#include "light.fxh"
#include "wind.fxh"

float Script : STANDARDSGLOBAL <
	string UIWidget = "none";
	string ScriptClass = "object";
	string ScriptOrder = "standard";
	string ScriptOutput = "color";
	string Script = "Technique=Technique?main;";

	// technique
	string TechniqueGeoFill = "gpass";
	string TechniqueShadowGen = "shadowgen";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// UN-TWEAKABLES

const int TESTCONST = 1;

float4 VP_zpass(VertexIn IN) : POSITION {
	float3 offset = VP_modelRotateScale(IN, IN.position);
	offset = WindEffect(IN, offset, IN.st2);
	float3 posWorld = offset + VP_getModelPos(IN);

	return VP_worldToClip(posWorld);
}

half4 FP_zpass(float4 IN : POSITION) : COLOR {
	return 1;
}

GpassOut VP_gpass(VertexIn IN) {
	GpassOut OUT = (GpassOut)0;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal);
#if !NO_NORMALMAPPING
	OUT.tangent = N_modelToWorld(IN, IN.tangent);
	OUT.binormal = N_modelToWorld(IN, IN.binormal);
#endif
	float3 offset = VP_modelRotateScale(IN, IN.position);
	offset = WindEffect(IN, offset, IN.st2);
	float3 posWorld = offset + VP_getModelPos(IN);

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
	N = GetNormal(g_normalMap, IN.streamTc.xy);

	half3x3 axis = half3x3(IN.tangent,IN.binormal, IN.normal);
	N = mul(N, axis);
	N = normalize(N);
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

	// compute wind
	float3 offset = VP_modelRotateScale(IN, IN.position);
	offset = WindEffect(IN, offset, IN.st2);
	float3 posWorld = offset + VP_getModelPos(IN);
	OUT.worldPos = posWorld;

	float4 posClip = VP_worldToClip(posWorld);
	OUT.hpos = posClip;

	OUT.screenTc = Clip2Screen(posClip);

	VP_final(IN, OUT);

	return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	LightParams lps;
	lps.worldpos = IN.worldPos;

	lps.calcSpecular = false;
	lps.screenTc = IN.screenTc;

#if G_HAVE_DIFFUSE
	lps.Cd = tex2D(g_diffuseMap, IN.streamTc.xy).xyz;
#else
	lps.Cd = half3(1, 1, 1);
#endif

	lps.Ca.xyz = lps.Cd;
	lps.Ca.w = 0.2f;

	half3 N;

#if NO_NORMALMAPPING
	lps.normal = normalize(IN.normal);
#else
	half3 normal = GetNormal(g_normalMap, IN.streamTc.xy);

	half3x3 axis = half3x3(IN.tangent,IN.binormal, IN.normal);
	N = mul(normal, axis);
	lps.normal = normalize(N);
#endif

#if 0 //!G_DISABLE_SPECULAR
	lps.calcSpecular = true;
	lps.viewDir = normalize(IN.eyevec);
	lps.shiness = 20;
#else
	lps.calcSpecular = false;
#endif

#if G_HAVE_SPECULAR
	lps.Cs = tex2D(g_specularMap, IN.streamTc.xy).xyz;
#else
	lps.Cs = Dif2Spec(lps.Cd);
#endif

#if G_FOG
	lps.fog = IN.fog;
#else
	lps.fog = 1;
#endif
	half3 final = LT_calcAllLights(lps);
	return half4(final, 1);
}



//------------------------------------------------------------------------------
// TECHNIQUES
//------------------------------------------------------------------------------

technique gpass {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_gpass();
		FRAGMENTPROGRAM = compile FP_2_0 FP_gpass();
#if 0
		DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_ENABLED;
		BLEND_NONE;
#endif
	}
}

technique shadowgen {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_zpass();
		FRAGMENTPROGRAM = compile FP_2_0 FP_zpass();
#if 0
		DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_ENABLED;
		BLEND_NONE;
#endif
	}
}

technique main {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
#if 0
		DEPTHTEST = true;
		DEPTHMASK_MAIN;
		CULL_ENABLED;
		BLEND_NONE;
#endif
	}
}

/***************************** eof ***/
