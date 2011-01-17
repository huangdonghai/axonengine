/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



// draw shadow to shadow mask texture

// in engine should set color mask(opengl) or color writeable(d3d) to let
// output only write to correct conponment

#include "common.fxh"
#include "shadow.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// features typedef
#define F_MASKFRONT G_FEATURE0
#define F_CSMATLAS	G_FEATURE1

float2		s_shadowRange;		// blur out if out of range

struct ShadowVertexOut {
    float4	hpos	: POSITION;
	float4	screenTc: TEXCOORD0;
	float4	viewDir	: TEXCOORD1;
};


ShadowVertexOut VP_main(VertexIn IN) {
	ShadowVertexOut OUT;

	float3 worldpos = VP_modelToWorld(IN, IN.position);

	OUT.hpos = VP_worldToClip(worldpos);

	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.viewDir.xyz = worldpos - g_cameraPos.xyz;
	OUT.viewDir.w = OUT.hpos.w;

	OUT.viewDir.xyz /= OUT.viewDir.w;

	return OUT;
}

static const float bias = 2.0 / 65536.0;
static const float delta = 0.000001;

half4 FP_main(ShadowVertexOut IN) : COLOR {
	// get scene depth
	float depth = tex2Dproj(g_rt1, IN.screenTc).a;

	float4 worldpos = float4(g_cameraPos.xyz + IN.viewDir.xyz * depth, 1);

#if F_CSMATLAS
	half shadow = SampleShadowCsmAtlas(worldpos.xyz, depth);
#else
	half shadow = SampleShadow(worldpos.xyz, depth);
#endif

	return HardStep(128, 256, depth) + shadow;
}



technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();

#if F_MASKFRONT
	    DEPTHTEST = true;
		CULL_BACK;
#else
		DEPTHTEST = false;
		CULL_FRONT;
#endif
		DEPTHMASK = false;
		BLEND_NONE;
    }
}

