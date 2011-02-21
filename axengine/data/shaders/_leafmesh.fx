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
	string TechniqueGeoFill = "zpass";
	string TechniqueShadowGen = "shadowgen";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// UN-TWEAKABLES

struct ZpassVertexOut {
	float4 hpos			: POSITION;
	float2 diffuseTc	: TEXCOORD0;
};

ZpassVertexOut VP_zpass(MeshVertex IN) {
	ZpassVertexOut OUT;

#if G_OPENGL
	float angleIndex = IN.color.b * 255;
#else
	float angleIndex = IN.color.r * 255;
#endif
	float2 vLeafRockAndRustle = g_leafAngles[angleIndex].xy;

	// vPosition stores the leaf mesh geometry, not yet put into place at position vOffset.
	// leaf meshes rock and rustle, which requires rotations on two axes (rustling is not
	// useful on leaf mesh geometry)
	float3x3 matRockRustle = RotationMatrix_xAxis(vLeafRockAndRustle.x); // rock

	float3 local = IN.position - IN.tangent;
	local = mul(matRockRustle, local) + IN.tangent;

	float3 offset = VP_modelRotateScale(IN, local);
	offset = WindEffect(IN, offset, IN.st2);
	float3 posWorld = offset + VP_getModelPos(IN);

	OUT.hpos = VP_worldToClip(posWorld);
	OUT.diffuseTc = IN.st;
	return OUT;
}

half4 FP_zpass(ZpassVertexOut IN) : COLOR {
	half4 Cd = tex2D(g_diffuseMap, IN.diffuseTc);
	clip(Cd.a - 0.5);
	return Cd;
}

GpassOut VP_gpass(MeshVertex IN) {
	GpassOut OUT = (GpassOut)0;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal);
#if !NO_NORMALMAPPING
	OUT.tangent = N_modelToWorld(IN, IN.tangent);
	OUT.binormal = N_modelToWorld(IN, IN.binormal);
#endif
#if G_OPENGL
	float angleIndex = IN.color.b * 255;
#else
	float angleIndex = IN.color.r * 255;
#endif
	float2 vLeafRockAndRustle = g_leafAngles[angleIndex].xy;

	// vPosition stores the leaf mesh geometry, not yet put into place at position vOffset.
	// leaf meshes rock and rustle, which requires rotations on two axes (rustling is not
	// useful on leaf mesh geometry)
	float3x3 matRockRustle = RotationMatrix_xAxis(vLeafRockAndRustle.x); // rock

	float3 local = IN.position - IN.tangent;
	local = mul(matRockRustle, local) + IN.tangent;

	float3 offset = VP_modelRotateScale(IN, local);
	offset = WindEffect(IN, offset, IN.st2);
	float3 posWorld = offset + VP_getModelPos(IN);

	float4 posClip = VP_worldToClip(posWorld);
	OUT.hpos = posClip;

	OUT.screenTc = Clip2Screen(posClip);

	OUT.streamTc = VP_computeStreamTc(IN);

	return OUT;
}

half4 FP_gpass(GpassOut IN) : COLOR {
	half4 Cd = tex2D(g_diffuseMap, IN.streamTc.xy);
	clip(Cd.a - 0.5);

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

VertexOut VP_main(MeshVertex IN) {
	VertexOut OUT = (VertexOut)0;

	OUT.color = IN.color.a;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal);

#if !NO_NORMALMAPPING
	OUT.tangent = N_modelToWorld(IN, IN.tangent);
	OUT.binormal = N_modelToWorld(IN, IN.binormal);
#endif

#if G_OPENGL
	float angleIndex = IN.color.b * 255;
#else
	float angleIndex = IN.color.r * 255;
#endif
	float2 vLeafRockAndRustle = g_leafAngles[angleIndex].xy;

	// vPosition stores the leaf mesh geometry, not yet put into place at position vOffset.
	// leaf meshes rock and rustle, which requires rotations on two axes (rustling is not
	// useful on leaf mesh geometry)
	float3x3 matRockRustle = RotationMatrix_xAxis(vLeafRockAndRustle.x); // rock

	float3 local = IN.position - IN.tangent;
	local = mul(matRockRustle, local) + IN.tangent;

	float3 offset = VP_modelRotateScale(IN, local);
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

#if M_DIFFUSE
	half4 Cd = tex2D(g_diffuseMap, IN.streamTc.xy);
#else
	half4 Cd = 1;
#endif
	lps.Cd = Cd.xyz * IN.color.a;
	lps.Ca.xyz = Cd.xyz;
	lps.Ca.w = 1;

	clip(Cd.a - 0.5);

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

#if M_SPECULAR
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

technique zpass {
	pass p0 {
		VertexShader = compile VS_3_0 VP_gpass();
		PixelShader = compile PS_3_0 FP_gpass();
	}
}

technique shadowgen {
	pass p0 {
		VertexShader = compile VS_3_0 VP_zpass();
		PixelShader = compile PS_3_0 FP_zpass();
	}
}

technique main {
	pass p0 {
		VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();
	}
}

/***************************** eof ***/
