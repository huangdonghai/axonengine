/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"

float Script : STANDARDSGLOBAL <
	// sort hint
	int		SortHint = SortHint_aboveWater;

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

	OUT.worldPos = VP_modelToWorld(IN, IN.xyz);
	OUT.hpos = VP_worldToClip(OUT.worldPos);

//	VP_final(IN, OUT);

    return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	half4 c = IN.color;


	c *= FP_GetDiffuse(IN);
	c.rgb *= g_matDiffuse.xyz;

	return c;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();

	    DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_NONE;
		BLEND_BLEND;
    }
}

/***************************** eof ***/
