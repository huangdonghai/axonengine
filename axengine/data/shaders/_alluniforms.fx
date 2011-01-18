/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"

// use all global uniform for engine parse uniform struct

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueGeoFill = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;


VertexOut VP_main(VertexIn IN) {
    VertexOut OUT = (VertexOut)0;

	OUT.hpos = g_waterFogParams + g_leafAngles[7] + g_instanceParam;
    return OUT;
}

/********* pixel shaders ********/
float4 FP_main(VertexOut IN) : COLOR0 {

	return g_waterFogParams + g_exposure + g_layerScale.xyxy;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
    }
}

/***************************** eof ***/
