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

/*********** Generic Vertex Shader ******/

VertexOut VP_main(MeshVertex IN)
{
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

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR
{
	half4 c = IN.color;

	c *= FP_GetDiffuse(IN);

	return c;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
    pass p0 {
        VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();
#if 0
	    DEPTHTEST = true;
		DEPTHMASK = false;
		CULL_NONE;
		BLEND_BLEND;
#endif
    }
}

/***************************** eof ***/
