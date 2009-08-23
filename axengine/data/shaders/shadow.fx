

// draw shadow to shadow mask texture

// in engine should set color mask(opengl) or color writeable(d3d) to let
// output only write to correct conponment

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

sampler2D s_depthMap <
	int RenderType = RenderType_zbuffer;
> = sampler_state {
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;
	AddressU = ClampToEdge;
	AddressV = ClampToEdge;   
};

struct ShadowVertexIn {
    float3 xyz		: POSITION;
};

struct ShadowVertexOut {
    float4	hpos	: POSITION;
	float4	clipPos	: TEXCOORD1;
};


ShadowVertexOut main_vp( ShadowVertexIn IN) {
	ShadowVertexOut OUT;

 	OUT.hpos = mul( g_wvp, float4(IN.xyz, 1) );
	OUT.clipPos = OUT.hpos;
   return OUT;
}

half4 main_fp( ShadowVertexOut IN ) : COLOR {
	// get scene depth
	float4 shadowtc = IN.clipPos / IN.clipPos.w;
	float depth = tex2D( s_depthMap, shadowtc.xy * 0.5 + 0.5 );

	// set clip pos
	shadowtc.z = depth;

	float4 shadowPos = mul( g_shadowMatrix, shadowtc );
	half shadow = tex2Dproj( g_shadowMap, shadowPos.xyzw ).r;

	return shadow;
}



technique main {
    pass p0 {
        VertexShader = compile arbvp1 main_vp();
	    DepthTestEnable = true;
		DepthMask = false;
		CullFaceEnable = true;
		DepthFunc = LEqual;
//		BlendEnable = true;
//		BlendFunc = { SrcAlpha, OneMinusSrcAlpha };
		FragmentProgram = compile arbfp1 main_fp();
    }
}

