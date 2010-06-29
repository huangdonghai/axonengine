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


#define SCENE_BUFFER_DEF \
	AX_UNIFORM(float,		float,		g_time,						1, 1)		\
	AX_UNIFORM(float4,		Vector4,	g_cameraPos,				2, 2)		\
	AX_UNIFORM(float4,		Vector4,	g_fogParams,				3, 3)		\
	AX_UNIFORM(float4,		Vector4,	g_waterFogParams,			4, 4)


float4x4 s_testarray;
float4 s_boxrange[4];

VertexOut VP_main(VertexIn IN) {
    VertexOut OUT = (VertexOut)0;

	OUT.hpos = g_gc.waterFogParams;
    return OUT;
}

/********* pixel shaders ********/
float4 FP_main(VertexOut IN) : COLOR0 {

	return g_pic.layerScale.xyxy;
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
