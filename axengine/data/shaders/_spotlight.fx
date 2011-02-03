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

ShadowVertexOut VP_main(MeshVertex IN)
{
	ShadowVertexOut OUT;

	float3 worldpos = VP_modelToWorld(IN, IN.position);

	OUT.hpos = VP_worldToClip(worldpos);

	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.viewDir.xyz = worldpos - g_cameraPos.xyz;
	OUT.viewDir.w = OUT.hpos.w;

//	OUT.viewDir.xyz /= OUT.viewDir.w;

	return OUT;
}

half4 FP_main(ShadowVertexOut IN) : COLOR
{
	half4 OUT = 0;

	// get gbuffer
	float depth = tex2Dproj(g_rtDepth, IN.screenTc).r;

	float viewDepth = ZR_GetViewSpace(depth);
	half4 gbuffer = tex2Dproj(g_rt1, IN.screenTc);
	half4 albedo = tex2Dproj(g_rt2, IN.screenTc);

	float3 worldpos = g_cameraPos.xyz + IN.viewDir.xyz / IN.viewDir.w * viewDepth;

	float4 projTc = mul(s_lightMatrix, float4(worldpos,1));
	projTc.xy /= projTc.z;

	half falloff = saturate(1.0f - dot(projTc.xyz,projTc.xyz));

	float3 lightPos = s_lightPos.xyz - worldpos;

	half3 L = normalize(lightPos.xyz);
	half3 N = gbuffer.xyz;
	half3 E = normalize(-IN.viewDir.xyz);

	half NdotL = saturate(dot(N, L));

	half3 R = 2 * NdotL * N - L;
	half RdotE = saturate(dot(E, R));

	OUT.xyz = s_lightColor.xyz * NdotL;

	OUT.w = pow(RdotE, 10) * NdotL * s_lightColor.w;

	return OUT * falloff * getShadow(worldpos, viewDepth);
}



technique main {
    pass p0 {
        VERTEXPROGRAM = compile VP_3_0 VP_main();
		FRAGMENTPROGRAM = compile FP_3_0 FP_main();
    }
}

