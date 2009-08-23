/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "common.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

static const int g_kernelSize = 13;

float2 s_invTextureSize;

float2 g_sampleOffsets[g_kernelSize] = {
	{ -6, 0 },
	{ -5, 0 },
	{ -4, 0 },
	{ -3, 0 },
	{ -2, 0 },
	{ -1, 0 },
	{  0, 0 },
	{  1, 0 },
	{  2, 0 },
	{  3, 0 },
	{  4, 0 },
	{  5, 0 },
	{  6, 0 },
};

float2 TexelKernel[g_kernelSize]
<
	string ConvertPixelsToTexels = "PixelKernel";
>;

static const float g_sampleWeights[g_kernelSize] =  {
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};


struct PostVertexOut {
	float4	hpos		: POSITION;
	float4	screenTc	: TEXCOORD1;
};


PostVertexOut VP_main(VertexIn IN) {
	PostVertexOut OUT;

	OUT.hpos = VP_modelToClip(IN, IN.xyz);
	OUT.screenTc = Clip2Screen(OUT.hpos);

	return OUT;
}

half4 FP_main(PostVertexOut IN) : COLOR {
	float2 tc = IN.screenTc.xy / IN.screenTc.w;

	//	return tex2D(g_diffuseMap, tc);


	half4 result = 0;

	for(int i = 0; i < g_kernelSize; i++) {
		result += tex2D(g_diffuseMap, tc +  g_sampleOffsets[i] * s_invTextureSize) * g_sampleWeights[i];
	}

	return result;
}



technique main {
	pass p0 {
		VertexShader = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();

		DEPTHTEST = false;
		DEPTHMASK = false;
		CULL_NONE;
		BLEND_NONE;
	}
}

