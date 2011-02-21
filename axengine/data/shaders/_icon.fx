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

float4 s_iconparam;		// xyz is pos, w is scale

/*********** Generic Vertex Shader ******/

VertexOut VP_main(MeshVertex IN) {
    VertexOut OUT = (VertexOut)0;

	OUT.color = IN.color;

	IN.position *= s_iconparam.w;

	float3 worldpos = IN.position.x * g_cameraAxis._m01_m11_m21 - IN.position.y * g_cameraAxis._m02_m12_m22 + s_iconparam.xyz;

	OUT.worldPos = VP_modelToWorld(IN, IN.position + s_iconparam.xyz);
	OUT.worldPos = worldpos;
	OUT.hpos = VP_worldToClip(OUT.worldPos);

	VP_final(IN, OUT);

    return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	half4 c = IN.color;

	c *= FP_GetDiffuse(IN);
	c.a = 1 - c.x * 2;

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
		BLEND_NONE;
#endif
    }
}

/***************************** eof ***/
