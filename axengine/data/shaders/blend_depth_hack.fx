/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"

float Script : STANDARDSGLOBAL <
    string UIWidget = "none";
    string ScriptClass = "object";
    string ScriptOrder = "standard";
    string ScriptOutput = "color";
    string Script = "Technique=Technique?main;";

	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

/*********** Generic Vertex Shader ******/

VertexOut VP_main(VertexIn IN) {
	VertexOut OUT = (VertexOut)0;

#if !G_D3D
	OUT.color = IN.color.bgra;
#else
	OUT.color = IN.color;
#endif

	OUT.worldPos = VP_modelToWorld(IN, IN.position);
	OUT.hpos = VP_worldToClip(OUT.worldPos);

	VP_final(IN, OUT);

    return OUT;
}

struct FragmentOut {
	half4 color		: COLOR;
	float depth		: DEPTH;
};

/********* pixel shaders ********/
FragmentOut FP_main(VertexOut IN) {
	FragmentOut result;
	result.color = IN.color;

#if G_HAVE_DIFFUSE
	result.color *= tex2D(g_diffuseMap, IN.streamTc.xy);
#endif

	result.depth = 0;

	return result;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
#if 0
	    DEPTHTEST = false;
		DEPTHMASK = true;
		CULL_NONE;
		BLEND_BLEND;
#endif
    }
}

/***************************** eof ***/
