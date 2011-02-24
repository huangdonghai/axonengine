// draw point light to light buffer

#include "common.fxh"
#include "shadow.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueGeoFill = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "main";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

// features typedef
#define F_SHADOWED		M_FEATURE0

AX_BEGIN_PC
	float4x4 s_lightMatrix : PREG0;
	float4 s_lightPos : PREG4;		// (xyz)*invR, invR
	float4 s_lightColor : PREG5 = float4(1,1,1,1);
AX_END_PC

struct ShadowVertexOut {
    float4	hpos	: POSITION;
	float4	screenTc: TEXCOORD0;
	float4	viewDir	: TEXCOORD1;
};

half getShadow(float3 worldpos, float depth)
{
#if F_SHADOWED
	return SampleShadow(worldpos,depth);
#else
	return 1;
#endif
}

ShadowVertexOut VP_main(ChunkVertex IN)
{
	ShadowVertexOut OUT;

	float3 worldpos = VP_modelToWorld(IN.position);

	OUT.hpos = VP_worldToClip(worldpos);

	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.viewDir.xyz = worldpos - g_cameraPos.xyz;
	OUT.viewDir.w = OUT.hpos.w;

//	OUT.viewDir.xyz /= OUT.viewDir.w;

	return OUT;
}

half4 FP_main(ShadowVertexOut IN) : COLOR
{
	half4 OUT = 1;

	// get gbuffer
	DeferredData data = GB_Input(IN.viewDir, IN.screenTc);

	float4 projTc = mul(s_lightMatrix, float4(data.worldPos,1));
	projTc.xy /= projTc.z;

	half falloff = saturate(1.0f - dot(projTc.xyz,projTc.xyz));

	float3 lightPos = s_lightPos.xyz - data.worldPos;

	half3 L = normalize(lightPos.xyz);
	half3 N = data.normal;
	half3 E = normalize(-IN.viewDir.xyz);

	half NdotL = saturate(dot(N, L));

	half3 R = 2 * NdotL * N - L;
	half RdotE = saturate(dot(E, R));
	RdotE = pow(RdotE, data.shiness);

	OUT.xyz = ((NdotL * data.diffuse) + (RdotE * data.specular)) * s_lightColor * falloff * getShadow(data.worldPos, data.viewDepth);
	return OUT;
}



technique main {
    pass p0 {
        VertexShader = compile VS_3_0 VP_main();
		PixelShader = compile PS_3_0 FP_main();
    }
}

