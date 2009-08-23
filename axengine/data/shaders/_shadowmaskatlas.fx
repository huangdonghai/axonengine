

// draw shadow to shadow mask texture

// in engine should set color mask(opengl) or color writeable(d3d) to let
// output only write to correct conponment

#include "common.fxh"

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

float4x4	s_shadowMatrix;
float2		s_shadowRange;		// blur out if out of range

float4		s_csmMinRange;
float4		s_csmMaxRange;
float4x4	s_csmOffsetScales;


struct ShadowVertexOut {
    float4	hpos	: POSITION;
	float4	screenTc: TEXCOORD0;
	float4	viewDir	: TEXCOORD1;
};


ShadowVertexOut VP_main( VertexIn IN) {
	ShadowVertexOut OUT;

	float3 worldpos = VP_modelToWorld( IN, IN.xyz );

	OUT.hpos = VP_worldToClip( worldpos );

	OUT.screenTc = Clip2Screen( OUT.hpos );

	OUT.viewDir.xyz = worldpos - g_cameraPos;
	OUT.viewDir.w = OUT.hpos.w;

	OUT.viewDir.xyz /= OUT.viewDir.w;


	return OUT;
}

static const float delta = 0.000001;
static const float scale = 1.0 / 1024.0;

half4 FP_main( ShadowVertexOut IN ) : COLOR {
	// get scene depth
	float depth = tex2Dproj( g_sceneDepth, IN.screenTc ).a;

	float4 worldpos = float4( g_cameraPos + IN.viewDir.xyz * depth, 1 );

	float4 shadowPos = mul( s_shadowMatrix, worldpos );

	shadowPos /= shadowPos.w;

	float4 mask = depth >= s_csmMinRange && depth < s_csmMaxRange;
	float4 offsetScale = mul( s_csmOffsetScales, mask );

	shadowPos.xy = shadowPos.xy * offsetScale.xy + offsetScale.zw;

	half shadow;
#if 0
	shadow = tex2D( g_diffuseMap, shadowPos.xyz ).r;
	return shadow;
#else
	shadow = tex2D( g_diffuseMap, shadowPos.xyz + float3(-0.5,-0.5,0)*scale ).r;
	shadow += tex2D( g_diffuseMap, shadowPos.xyz + float3(-0.5,0.5,0)*scale ).r;
	shadow += tex2D( g_diffuseMap, shadowPos.xyz + float3(0.5,-0.5,0)*scale ).r;
	shadow += tex2D( g_diffuseMap, shadowPos.xyz + float3(0.5,0.5,0)*scale ).r;
//	shadow = step( 2, shadow );

	shadow *= 0.25;
#endif
	return HardStep( s_shadowRange.x, s_shadowRange.y, depth ) + shadow;
}



technique main {
    pass p0 {
        VertexShader = compile VP_2_0 VP_main();
		FRAGMENTPROGRAM = compile FP_2_0 FP_main();
#if F_MASKFRONT
	    DEPTHTESTENABLE = true;
#else
		FrontFace = CW;
#endif
		DEPTHMASK = false;
		CULL_ENABLED;
//		//DepthFunc = GREATER ;
//		BLENDENABLE = true;
//		BlendFunc = { SrcAlpha, OneMinusSrcAlpha };
    }
}

