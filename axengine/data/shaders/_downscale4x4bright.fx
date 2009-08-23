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

float2 s_invTextureSize;

float2 g_sampleOffsets[4] = {
	{ 1.5,  -1.5 },
//	{ 1.5,  -0.5 },
//	{ 1.5,   0.5 },
	{ 1.5,   1.5 },

//	{ 0.5,  -1.5 },
//	{ 0.5,  -0.5 },
//	{ 0.5,   0.5 },
//	{ 0.5,   1.5 },

//	{-0.5,  -1.5 },
//	{-0.5,  -0.5 },
//	{-0.5,   0.5 },
//	{-0.5,   1.5 },

	{-1.5,  -1.5 },
//	{-1.5,  -0.5 },
//	{-1.5,   0.5 },
	{-1.5,   1.5 },
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

	for(int i = 0; i < 4; i++) {
		half4 cur = tex2D(g_diffuseMap, tc +  g_sampleOffsets[i] * s_invTextureSize);
//		cur.xyz = saturate(HdrDecode(cur.xyz) - 4);

#if 0
		half lum = Rgb2Lum(cur.xyz);
		lum *= lum;
		lum *= lum;
		lum *= lum;
#endif
		result += cur;
	}

	result /= 4;
	half lum = Rgb2Lum(result.xyz);
	lum = HardStep(0.95, 1, lum);

	return result * lum;
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

