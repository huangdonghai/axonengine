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

#define TEST0 1

#define G_DECL \
	A_ITEM(float4, v_T, TEST0)

#define A_ITEM(t, n, i) t n = i;
G_DECL

VertexOut VP_main(MeshVertex IN) {
    VertexOut OUT = (VertexOut)0;

	OUT.hpos = v_T;
    return OUT;
}


/********* pixel shaders ********/
float4 FP_main(VertexOut IN) : COLOR0 {
	while (1) for (int i = 0; i < 4; i++) {

	}
	while (1) for (int i = 0; i < 4; i++) {

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
    pass p0 <
		string state = "blend=blend;depthwrites=true"
#if M_FEATURE0
		"cull=none;"
#else
		"cull=enable;"
#endif
		;
	>
	{
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
#if 0
	    DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_NONE;
		BLEND_BLEND;
#endif
    }
}

/***************************** eof ***/
