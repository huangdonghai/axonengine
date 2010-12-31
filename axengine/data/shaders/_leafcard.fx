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
	string TechniqueZpass = "zpass";
	string TechniqueShadowGen = "shadowgen";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// UN-TWEAKABLES

static const float c_alpharef = 0.3;

float3 ComputeWorldpos(VertexIn IN, bool extrude = false) {
	float3 posWorld;
	float3 pivot = VP_modelToWorld(IN, IN.tangent);
	float3 corner = (IN.tangent - IN.position) * VP_getInstanceParam(IN)[SCALE];

#if G_OPENGL
	float angleIndex = IN.color.b * 255;
#else
	float angleIndex = IN.color.r * 255;
#endif
	float2 vLeafRockAndRustle = g_leafAngles[angleIndex].xy;

	float3x3 rot = RotationMatrix_angles(/*g_cameraAngles[PITCH] + */vLeafRockAndRustle.y, /*g_cameraAngles[YAW] + */vLeafRockAndRustle.x);

	float3 forward = pivot * g_cameraPos.w - g_cameraPos.xyz;
	forward = normalize(forward);

	float3 up = float3(0,0,1);
	float3 left = normalize(cross(up,forward));
	up = cross(forward,left);

	float3x3 rot2 = float3x3(forward,left,up);

	rot = mul(rot, rot2);

	corner = mul(corner, rot);
	posWorld = pivot - corner;

	float3 offset = posWorld - VP_getModelPos(IN);
	offset = WindEffect(IN, offset, IN.st2);
	posWorld = offset + VP_getModelPos(IN);

	if (extrude) {
		// extrude for shadow gen
//		float3 cameraDir = posWorld * g_cameraPos.w - g_cameraPos.xyz;
//		cameraDir = normalize(cameraDir);
		posWorld += forward * length(corner) * 0.25;
	}

	return posWorld;
}

struct FrondVertexOut {
	float4 hpos			: POSITION;
	float2 diffuseTc	: TEXCOORD0;
};

FrondVertexOut VP_zpass(VertexIn IN) {
	FrondVertexOut OUT;

	float3 posWorld = ComputeWorldpos(IN);

	OUT.hpos = VP_worldToClip(posWorld);
	OUT.diffuseTc = IN.st;

	return OUT;
}

half4 FP_zpass(FrondVertexOut IN) : COLOR {
	half4 Cd = tex2D(g_diffuseMap, IN.diffuseTc);
	clip(Cd.a - c_alpharef);
	return 1;
}

GpassOut VP_gpass(VertexIn IN) {
	GpassOut OUT = (GpassOut)0;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal);
	float3 posWorld = ComputeWorldpos(IN);

	float4 posClip = VP_worldToClip(posWorld);
	OUT.hpos = posClip;

	OUT.screenTc = Clip2Screen(posClip);

	OUT.streamTc = VP_computeStreamTc(IN);

	return OUT;
}

half4 FP_gpass(GpassOut IN) : COLOR {
	half4 Cd = tex2D(g_diffuseMap, IN.streamTc.xy);
	clip(Cd.a - c_alpharef);

	half3 N;

	N = normalize(IN.normal);
	return half4(N,IN.screenTc.w);
}


FrondVertexOut VP_shadowgen(VertexIn IN) {
	FrondVertexOut OUT;
	float3 posWorld = ComputeWorldpos(IN, true);

	OUT.hpos = VP_worldToClip(posWorld);
	OUT.diffuseTc = IN.st;

	return OUT;
}

half4 FP_shadowgen(FrondVertexOut IN) : COLOR {
	half4 Cd = tex2D(g_diffuseMap, IN.diffuseTc);
	clip(Cd.a - c_alpharef);
	return 1;
}




/*********** Generic Vertex Shader ******/

VertexOut VP_main(VertexIn IN) {
	VertexOut OUT = (VertexOut)0;

	OUT.color = IN.color.a;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal);

#if !NO_NORMALMAPPING
	OUT.tangent = N_modelToWorld(IN, IN.tangent);
	OUT.binormal = N_modelToWorld(IN, IN.binormal);
#endif

	float3 posWorld = ComputeWorldpos(IN);

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
	half4 map = tex2D(g_diffuseMap, IN.streamTc.xy);
#else
	half4 map = half4(1, 1, 1, 1);
#endif
	clip(map.a - c_alpharef);

	lps.Cd = map.xyz * IN.color.xyz;
	lps.Ca.xyz = map.xyz * 0.5f;
	lps.Ca.w = 1;

	half3 N;

#if NO_NORMALMAPPING
	N = normalize(IN.normal);
#else
	half3 normal = GetNormal(g_normalMap, IN.streamTc.xy).xyz;

	half3x3 axis = half3x3(IN.tangent,IN.binormal, IN.normal);
	N = mul(normal, axis);
	N = normalize(N);
#endif
	lps.normal = N;

#if 0 // !G_DISABLE_SPECULAR
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

technique zpass {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_gpass();
		FRAGMENTPROGRAM = compile FP_2_0 FP_gpass();
#if 0
		DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_NONE;
		BLEND_NONE;
#endif
	}
}

technique shadowgen {
	pass p0 {
		VERTEXPROGRAM = compile VP_2_0 VP_shadowgen();
		FRAGMENTPROGRAM = compile FP_2_0 FP_shadowgen();
#if 0
		DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_NONE;
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
		CULL_NONE;
		BLEND_NONE;
#endif
	}
}

/***************************** eof ***/
