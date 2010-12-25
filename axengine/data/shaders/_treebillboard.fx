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

	OUT.color = IN.color;

	OUT.hpos = VP_modelToClip(IN, IN.xyz);
	OUT.screenTc = Clip2Screen(OUT.hpos);

	VP_final(IN, OUT);

    return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	half4 c = IN.color;

	c *= tex2Dproj(g_diffuseMap, IN.screenTc);
//	c.xyz = saturate(HdrDecode(c.xyz) - 3);
//	c.xyz /= (1 + c.xyz);

	return c;
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
