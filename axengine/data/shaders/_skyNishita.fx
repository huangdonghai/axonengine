/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"
#include "sky.fxh"

static const float G = -0.990;

float g = G;
float g2 = G * G;
float fExposure = -2.0;

float3 InvWavelength;
float3 WavelengthMie;

float starIntensity = 0.5f;

float getRayleighPhase(float fCos2)
{
	return 0.75 * (1.0 + fCos2);
}

float getMiePhase(float fCos, float fCos2)
{
	float3 v3HG;
	v3HG.x = 1.5f * ((1.0f - g2) / (2.0f + g2));
	v3HG.y = 1.0f + g2;
	v3HG.z = 2.0f * g;
	return v3HG.x * (1.0 + fCos2) / pow(v3HG.y - v3HG.z * fCos, 1.5);
}


struct PS_INPUT
{
	float4 hpos  : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 viewDir : TEXCOORD1;
};

PS_INPUT VP_main(MeshVertex IN)
{
	PS_INPUT result = (PS_INPUT)0;
	result.hpos =  VP_modelToClip(IN, IN.position);
	result.texCoord = IN.streamTc.xy; 
	result.viewDir = -IN.normal.xyz;
	return result;
}

float3 HDR(float3 LDR)
{
	return 1.0f - exp(fExposure * LDR);
}

struct FragmentOut {
	half4 color		: COLOR;
};


FragmentOut FP_main(PS_INPUT input)
{
	FragmentOut result;
	float fCos = dot(g_globalLightPos.xyz, input.viewDir) / length(input.viewDir);
	float fCos2 = fCos * fCos;

	half3 v3RayleighSamples = tex2D(g_diffuseMap, input.texCoord).xyz;
	half3 v3MieSamples = tex2D(g_specularMap, input.texCoord.xy).xyz;

	half3 Color;
	Color.rgb = getRayleighPhase(fCos2) * v3RayleighSamples.rgb + getMiePhase(fCos, fCos2) * v3MieSamples.rgb;
	Color.rgb = HDR(Color.rgb);

	// Hack Sky Night Color
//	Color.rgb += max(float3(0),(1 - Color.rgb)) * float3(0.05, 0.05, 0.1); 

	result.color = half4(Color.rgb, 1);
	result.color = tex2D(g_diffuseMap, input.texCoord);
	return result; // + tex2D(starSampler, input.texCoord) * starIntensity;
}

technique Main {
	pass Pass1 {
		VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();
	}
}
