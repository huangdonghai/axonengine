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

float4x4 s_testarray;
float4 s_boxrange[4];

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
	return all(c);
	float4 sb;
#if 1
	c = c > s_boxrange[0] && c < s_boxrange[1] && c > s_boxrange[2] && c < s_boxrange[3];

	if (c.x!=0)
		sb = s_testarray[0];
	else if (c.y!=0)
		sb = s_testarray[1];
	else if (c.z!=0)
		sb = s_testarray[2];
	else
		sb = s_testarray[3];
#else
	c = c > s_boxrange[0] && c < s_boxrange[1];
	c = mul(s_testarray, c);
	sb = c;
#endif
	return sb;

	c *= FP_GetDiffuse(IN);
	c.rgb *= g_matDiffuse.xyz;

	return c;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
    pass p0 <
		string state = "blend=blend;depthwrites=true"
#if G_FEATURE0
		"cull=none;"
#else
		"cull=enable;"
#endif
		;
	>
	{
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();

	    DEPTHTEST = true;
		DEPTHMASK = true;
		CULL_NONE;
		BLEND_BLEND;
    }
}

/***************************** eof ***/
