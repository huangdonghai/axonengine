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
	string TechniqueGeoFill = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

struct PostVertexOut {
	float4	hpos		: POSITION;
	float4	screenTc	: TEXCOORD1;
};


PostVertexOut VP_main(MeshVertex IN) {
	PostVertexOut OUT;

	OUT.hpos = VP_modelToClip(IN, IN.position);
	OUT.screenTc = Clip2Screen(OUT.hpos);

	return OUT;
}

half4 FP_main(PostVertexOut IN) : COLOR {
	float2 tc = IN.screenTc.xy / IN.screenTc.w;

	return tex2D(g_diffuseMap, tc) + tex2D(g_specularMap, tc);
}



technique main {
	pass p0 {
		VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();

		DEPTHTEST = false;
		DEPTHMASK = false;
		CULL_NONE;
		BLEND_NONE;
	}
}

