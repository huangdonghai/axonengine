/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



// draw shadow to shadow mask texture

// in engine should set color mask(opengl) or color writeable(d3d) to let
// output only write to correct conponment

#include "common.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

struct v2f {
    float4	hpos		: POSITION;
	float4	screenTc	: TEXCOORD1;
};

struct fragOut {
	float4	color		: COLOR;
	float	depth		: DEPTH;
};

v2f VP_main(VertexIn IN) {
	v2f OUT;

	OUT.hpos = VP_modelToClip(IN, IN.position);
	OUT.screenTc = Clip2Screen(OUT.hpos);

   return OUT;
}

fragOut FP_main(v2f IN) {
	fragOut result;

	result.depth = tex2D(g_diffuseMap, IN.screenTc.xy).r;
	result.color = result.depth;
	return result;

//	float lineardepth = ZR_getLinear(depth);
//	return lineardepth;
}



technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
    }
}

