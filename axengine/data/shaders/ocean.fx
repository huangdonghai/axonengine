/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"
#include "light.fxh"

float Script : STANDARDSGLOBAL <
	// features
	string	Features =	"F_GLOBAL_SPECULAR:Global Specular:Specular for global light(sun or moon)|"
	"F_TEST2:Test2:just for test";

	// macro parameters
	string	MacroParameters = "P_TEST|P_TEST2";

	// technique
	string	TechniqueGeoFill = "";
	string	TechniqueShadowGen = "";
	string	TechniqueMain = "main";
	string	TechniqueGlow = "";
	string	TechniqueLayer = "";
> = 0.8;

float FresnelBias <
	string UIHelp = "Make specular and reflection more visible  \nMin value = 0 (only visible at sharp angles) \nMax value = 1 (always visible) \nCorrect name - FresnelBias";
	string UIName = "Specular/Reflection visibility";  

	string UIWidget = "slider";
	float UIMin = 0;
	float UIMax = 1.0;
	float UIStep = 0.05;
> = 0;

float FresnelPow <
	string UIHelp = "Changes reflection strength/sharpness \nMin value= 0, Max value = 30 \nCorrect name - FresnelPow";                   
	string UIName = "Reflection intensity";    

	string UIWidget = "slider";
	float UIMin = 1.0;
	float UIMax = 30.0;
	float UIStep = 0.05;
> = 5;

#define S_ENABLE_FOME		M_FEATURE0
#define S_ENABLE_SPECULAR	M_FEATURE1

//------------------------------------------------------------------------------
// untweakable parameter
//------------------------------------------------------------------------------

texture s_reflectionMap_tex <
	string file = "$Reflection";
>;


sampler2D s_reflectionMap = sampler_state
{
	texture = <s_reflectionMap_tex>;

	FILTER_LINEAR;
	CLAMP_EDGE;
};

texture s_refractionMap_tex <
	string file = "$SceneColor";
>;

sampler2D s_refractionMap = sampler_state
{
	texture = <s_refractionMap_tex>;

	FILTER_LINEAR;
	CLAMP_EDGE;
};

texture s_foamMap_tex <
	string file = "textures/water_foam";
>;

sampler2D s_foamMap = sampler_state {
	texture = <s_foamMap_tex>;

	FILTER_TRILINEAR;
	CLAMP_REPEAT;
};

// wavelength, freq, amplitude, phase
float4	s_waveparams = { 12, 0.5, 0.2, 0.0 };

// wave direction
float3	c_wavedir = { 1.0, 1.0, 0.0 };

float4 s_waterColor = { 0.12, 0.22, 0.29, 1.0 };

half Fresnel(half NdotE, half bias, half power) {
	half facing = abs(1.0 - NdotE);
	return saturate(bias + pow(facing, power)) * 0.6;  // opt: removed (1-bias) mul
}

/*********** Generic Vertex Shader ******/

VertexOut VP_main(MeshVertex IN) {
	VertexOut OUT = (VertexOut)0;

	OUT.color = IN.color;

	float4 pos = float4(IN.position,1);

	pos.z = g_cameraPos.z * IN.streamTc.y;

	float3 worldpos;
	worldpos.xy = pos.xy + g_cameraPos.xy;
	worldpos.z = pos.z;

	OUT.streamTc.xy = (worldpos.xy) * 0.01f;
	OUT.streamTc.z = 1;
	OUT.streamTc.w = (1.0 - IN.streamTc.y) + 1e-4;

	OUT.streamTc.xy += float2(0.01, 0.01) * g_time;

	OUT.hpos = VP_worldToClip(worldpos.xyz);

#if G_FOG
	OUT.fog = FOG_compute(OUT.hpos);
#endif

	OUT.screenTc = Clip2Screen(OUT.hpos);

	OUT.worldPos = worldpos;

	return OUT;
}

float ComputeRefrFog(float sceneDepth, float sceneZ, float viewZ) {
	float minh = min(sceneZ, viewZ);
	float maxh = max(sceneZ, viewZ);

	float d = sceneDepth * saturate((0 - minh) / (maxh - minh));
	float fog = exp(- g_waterFogParams.w * d) * 0.7;

	return fog;
}

/********* pixel shaders ********/
half4 FP_main(VertexOut IN) : COLOR {
	half3 viewdir = IN.worldPos - g_cameraPos.xyz;

	half3 virtualWorldPos = g_cameraPos.xyz + viewdir * (g_cameraPos.z / (g_cameraPos.z - IN.worldPos.z));

	float2 worldtc = (virtualWorldPos.xy + g_time) * 0.01;

	float2 bumptc = worldtc + float2(0.02, 0.02) * g_time;

	half3 N = Expand(tex2D(g_normalMap, bumptc).xyz);

	half4 foam = tex2D(s_foamMap, bumptc.xy);

	bumptc.xy = worldtc + float2(0.04, 0.04) * g_time;
	N += Expand(tex2D(g_normalMap, bumptc).xyz);
	foam += tex2D(s_foamMap, bumptc.xy);

#if 0
	bumptc.xy = worldtc + float2(0.03, 0.05) * g_time;
	bumptc.xy *= 0.25;
	N += Expand(tex2D(g_normalMap, bumptc).xyz);
	foam += tex2D(s_foamMap, bumptc.xy);
#endif

	N = normalize(N);
	half3 E = normalize(viewdir);
	half3 L = g_globalLightPos.xyz;

	half3 sunNormal = N.xyz;
	sunNormal.xy *= 0.2;
	sunNormal = normalize(sunNormal);

	half3 mirrorEye = 2*dot(E,sunNormal)*sunNormal - E;
	half dotSpec = saturate(dot(mirrorEye.xyz, L)*1.0015);	// sun is disc

	half3 spec = pow(dotSpec, g_matShiness*4) * g_globalLightColor.xyz;// * shadow;
	half3 diff = saturate(dot(N, L)) * g_globalLightColor.xyz * 0.2;

#if 1
	half NdotE = abs(dot(E, sunNormal));
#else
	half NdotE = E.z;
#endif
	half fresnel = Fresnel(NdotE, FresnelBias, FresnelPow);

	half3 offset = N * 0.02;

	IN.screenTc.xyz /= IN.screenTc.w;

	float zscene = tex2D(g_rt1, IN.screenTc.xy).w;

	float3 deferredWorldPos = g_cameraPos.xyz + viewdir / IN.screenTc.w * zscene;

	float dz = IN.screenTc.w - zscene;
	float factor1 = HardStep(-45, -25, dz);
	float factor2 = HardStep(0, 25, -dz);
	float factor = min(factor1, factor2);

	float refrfog = ComputeRefrFog(zscene, deferredWorldPos.z, g_cameraPos.z);

//	fresnel *= factor2;

#if G_FOG
//	IN.fog = saturate(IN.fog + 1 - factor2);
//	refrfog = saturate(refrfog + 1 - factor2);
#endif

//	offset.xy *= factor2;

	float2 offsetTc = IN.screenTc.xy + offset.xy * factor2;

	float offsetW = tex2D(g_rt1, offsetTc).w;

	half refrSoft = saturate((zscene - IN.screenTc.w) * 0.1 + 0.1);
	float2 refrOffset = offset.xy * refrSoft;
	float2 reflOffset = refrOffset;
	float refrDepth = tex2D(g_rt1, IN.screenTc.xy + refrOffset).w;

	refrOffset *= refrDepth > IN.screenTc.w;

	half3 refr = tex2D(s_refractionMap, IN.screenTc.xy + refrOffset).xyz;

	refr = lerp(g_waterFogParams.xyz, refr, refrfog);
	half3 refl = tex2D(s_reflectionMap, IN.screenTc.xy + reflOffset.yx).xyz;// * g_exposure.y;

#if 0 && G_HDR
	refl.xyz *= MAX_EXPOSURE;
#endif


	half4 c = 1;
	c.xyz = lerp(refr, refl, fresnel);
	
#if S_ENABLE_SPECULAR
	c.xyz += spec;// * factor2;
#endif

#if S_ENABLE_FOME
	c.xyz += factor * N.z * saturate(foam.x) * 0.5;
#endif

#if G_FOG
	c.xyz = lerp(g_fogParams.xyz, c.xyz, IN.fog);
#endif

//	c.xyz += diff;

	c.xyz = FinalOutput(c.xyz) ;

	return c;
}

//////////////////////////////////////////////////////////////////////
// TECHNIQUES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


technique main {
	pass p0 {
		VERTEXPROGRAM = compile VP_3_0 VP_main();
		FRAGMENTPROGRAM = compile FP_3_0 FP_main();

#if 0
		DEPTHTEST = true;
		DEPTHMASK = false;
		CULL_NONE;
		BLEND_NONE;
#endif
	}
}

/***************************** eof ***/
