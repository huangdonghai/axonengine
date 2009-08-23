/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



/***************************** eof ***/


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

    if (IN.tangent.x == 1.0f)
    {
        float4x4 rotate = fromRotate(IN.xyz,IN.binormal,IN.st2.x,IN.st2.y);
        IN.xyz = transformPoint3FromMatrix4(rotate,IN.xyz);
    }
    
    if (IN.tangent.x == 2.0f)
    {
        float4 q = getQuaternionFromAxis(IN.binormal,IN.st2.x);
        float3 trans = float3(IN.st2.y,IN.tangent.y,IN.tangent.y);
        float3 localPos = IN.xyz - trans;
        localPos = transformPointFromQuaternion(q,localPos);
        IN.xyz = localPos + trans;
    }
    
	OUT.color = IN.color;

	OUT.worldPos = VP_modelToWorld(IN, IN.xyz);
	OUT.hpos = VP_worldToClip(OUT.worldPos);

	VP_final(IN, OUT);

	return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	half4 c = IN.color;

#if G_HAVE_DIFFUSE
	c *= tex2D(g_diffuseMap, IN.streamTc.xy);
#endif

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
		DEPTHMASK = false;
		CULL_ENABLED;
		BLEND_NONE;
    }
}

/***************************** eof ***/
