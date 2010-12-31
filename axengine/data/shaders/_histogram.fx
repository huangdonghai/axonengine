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


struct PostVertexOut {
    float4	hpos		: POSITION;
	float4	screenTc	: TEXCOORD1;
};


PostVertexOut VP_main(VertexIn IN) {
	PostVertexOut OUT;

	OUT.hpos = VP_modelToClip(IN, IN.position);
	OUT.screenTc = Clip2Screen(OUT.hpos);

   return OUT;
}

half4 FP_main(PostVertexOut IN) : COLOR {
	half4 color = tex2Dproj(g_diffuseMap, IN.screenTc);

	half lum = Rgb2Lum(color.xyz);

	if (lum < g_instanceParam.x || lum > g_instanceParam.y) {
		discard;
	}

	return color;
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

