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

#define S_GLOW G_FEATURE0

/*********** Generic Vertex Shader ******/

VertexOut VP_main(VertexIn IN) {
	VertexOut OUT = (VertexOut)0;

#if G_D3D
	OUT.color = IN.color.bgra;
#else
	OUT.color = IN.color;
#endif

	OUT.color.rgb *= VP_getInstanceParam(IN).rgb;

	OUT.worldPos = VP_modelToWorld(IN, IN.xyz);
	OUT.hpos = VP_worldToClip(OUT.worldPos);

#if S_GLOW // glow will adjust color by view/normal
	float3 viewdir = normalize(OUT.worldPos - g_cameraPos.xyz);
	
	OUT.normal = N_modelToWorld(IN, IN.normal);
	OUT.normal = normalize(OUT.normal);
	float f = abs(dot(viewdir, OUT.normal));
	f = smoothstep(0,1,f);
	OUT.color.a *= f;
#endif

	VP_final(IN, OUT);

	return OUT;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	half4 c = IN.color;
	c.rgb *= c.a;

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
		CULL_NONE;
		BLEND_ADD;
    }
}

/***************************** eof ***/
