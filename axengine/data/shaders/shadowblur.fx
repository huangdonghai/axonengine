/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



// blur shadow mask texture

#include "common.fxh"

float2 		g_sampleOffsets[32];
float 		g_sampleWeights[32];

static const float s_depthDeltaMax = 1e-4;

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

#if 0
sampler2D s_depthMap <
	int RenderType = RenderType_zbuffer;
> = sampler_state {
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
	AddressU = ClampToEdge;
	AddressV = ClampToEdge;   
};
#endif

struct PostVertexOut {
    float4	hpos		: POSITION;
	float4	screenTc	: TEXCOORD1;
};


PostVertexOut VP_main(VertexIn IN) {
	PostVertexOut OUT;

	float4 p = VP_modelToClip(IN, IN.position);
 	OUT.hpos = p;
	OUT.screenTc = float4(float2(p.x + p.w, p.w + p.y) * 0.5, p.zw);
	return OUT;
}

half4 FP_main(PostVertexOut IN) : COLOR {
//	return ZR_getLinear(IN.screenTc.x);

	// get scene depth
	float4 screenTc = IN.screenTc / IN.screenTc.w;

	half4	result = 0;
	float	weight = 0;

	// get center depth
	float center_depth = tex2D(g_rt1, screenTc.xy).r;

	for(int i = 0; i < 11; i++) {
		float2 offset_tc = screenTc.xy + g_sampleOffsets[i];
#if 0
		float cur_depth = tex2D(g_rt1, offset_tc);
		if (abs(cur_depth - center_depth) > s_depthDeltaMax) {
			continue;
		}
#else
		float cur_depth = tex2D(g_rt1, offset_tc).r;
		float c = step(abs(cur_depth - center_depth), s_depthDeltaMax);
		float D = c * g_sampleWeights[i];
#endif
		result += tex2D(g_diffuseMap, offset_tc) * D;
		weight += D;
	}

	result /= weight;

//	return tex2D(g_diffuseMap, screenTc.xy);

	return result;
}



technique main {
    pass p0 {
        VertexShader = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();

	    DEPTHTEST = false;
		DEPTHMASK = false;
		CULL_NONE;
		BLEND_NONE;
    }
}

