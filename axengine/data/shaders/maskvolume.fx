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
	string TechniqueGeoFill = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

#define F_TWOSIDES	G_FEATURE0

float4 VP_main(float3 IN : POSITION) : POSITION {
#if 0
    return mul(g_wvp, float4(IN,1));
#else
	return VP_worldToClip(IN);
#endif
}

half4 FP_main(float4 IN : POSITION) : COLOR {
	return 1;
}


//------------------------------------------------------------------------------
// TECHNIQUES
//------------------------------------------------------------------------------

technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
	    DEPTHTEST = true;
		//DepthFunc = LEqual;
		DEPTHMASK = false;
#if F_TWOSIDES
		CULL_NONE;
#else
		CULL_FRONT;
#endif
		BLEND_NONE;
    }
}

