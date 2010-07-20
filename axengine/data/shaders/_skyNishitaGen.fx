/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "common.fxh"
#include "sky.fxh"

float Script : STANDARDSGLOBAL <
	// technique
	string TechniqueZpass = "";
	string TechniqueShadowGen = "";
	string TechniqueMain = "Update";
	string TechniqueGlow = "";
	string TechniqueLayer = "";
> = 0.8;

static const float PI = 3.1415159;

static const float RADIUS = 6356.7523142;
static const float OUTER_RADIUS_SCALE = 1.0157313;
static const float ESUN = 20.0f;
static const float KR = 0.0025f;
static const float KM = 0.0015f;
static const float G = -0.990;
static const float3 WAVE_LENGTH = {0.65f, 0.57f, 0.475f};

static const float OUT_RADIUS = RADIUS * OUTER_RADIUS_SCALE;

static const float InnerRadius = RADIUS;
static const float OuterRadius = OUT_RADIUS;
static const float NumSamples = 25;
static const float fScale = 1.0 / (OUT_RADIUS - RADIUS);
static const float2 v2dRayleighMieScaleHeight = {0.25, 0.1};

//float ESun = 20.0f;
//float Kr = 0.0025f;
//float Km = 0.0010f;
static const float KrESun = KR * ESUN;
static const float KmESun = KM * ESUN;
static const float Kr4PI = KR * 4.0f * PI;
static const float Km4PI = KM * 4.0f * PI;

static const float g = G;
static const float g2 = G * G;

static const float3 InvWavelength = { 5.60204554,9.473285,19.6438026 };
static const float3 WavelengthMie = { 1.43599451,1.60348582,1.86886263 };

struct VertOut {
    float4 hpos		: POSITION;
    float4 screenTc	: TEXCOORD0;
};

struct FragOut {
	float4 RayLeigh : COLOR0;
	float4 Mie		: COLOR1;
};


VertOut VP_main(VertexIn input) {
    VertOut result = (VertOut)0;
    result.hpos = VP_worldToClip(input.xyz);
	result.screenTc = Clip2Screen(result.hpos);
	return result;
}

float HitOuterSphere(float3 O, float3 Dir) {
	float3 L = -O;

	float B = dot(L, Dir);
	float C = dot(L, L);
	float D = C - B * B; 
	float q = sqrt(OuterRadius * OuterRadius - D);
	float t = B;
	t += q;

	return t;
}

float2 GetDensityRatio(float fHeight) {
	const float fAltitude = (fHeight - InnerRadius) * fScale;
	return exp(-fAltitude / v2dRayleighMieScaleHeight.xy);
}

float2 t(float3 P, float3 Px) {
	float2 OpticalDepth = 0;

	float3 v3Vector =  Px - P;
	float fFar = length(v3Vector);
	float3 v3Dir = v3Vector / fFar;
			
	float fSampleLength = fFar / NumSamples;
	float fScaledLength = fSampleLength * fScale;
	float3 v3SampleRay = v3Dir * fSampleLength;
	P += v3SampleRay * 0.5f;
			
	LOOP
	for(int i = 0; i < NumSamples; i++) {
		float fHeight = length(P);
		OpticalDepth += GetDensityRatio(fHeight);
		P += v3SampleRay;
	}		

	OpticalDepth *= fScaledLength;
	return OpticalDepth;
}

FragOut FP_main(VertOut input) {
	FragOut result = (FragOut)0;
	
	float2 screenTC = input.screenTc.xy / input.screenTc.w;
	 
	const float3 v3PointPv = float3(0, 0, InnerRadius + 1e-3);

#if 0
	const float AngleXY = 2 * PI * screenTC.x;
	const float AngleZ = PI * 0.5 * screenTC.y;
#else
	const float AngleXY = 2 * PI * -screenTC.x;
	const float AngleZ = PI * 0.5 * ((1.0-screenTC.y) * 1.1);
#endif
	float3 v3Dir;
	v3Dir.x = sin(AngleZ) * cos(AngleXY );
	v3Dir.y = sin(AngleZ) * sin(AngleXY );
	v3Dir.z = cos(AngleZ);
	v3Dir = normalize(v3Dir);

	float3 v3SunDir = g_globalLightPos.xyz;
	float fFarPvPa = HitOuterSphere(v3PointPv , v3Dir);
	float3 v3Ray = v3Dir;

	float3 v3PointP = v3PointPv;
	float fSampleLength = fFarPvPa / NumSamples;
	float fScaledLength = fSampleLength * fScale;
	float3 v3SampleRay = v3Ray * fSampleLength;
	v3PointP += v3SampleRay * 0.5f;
				
	float3 v3RayleighSum = 0;
	float3 v3MieSum = 0;

	LOOP
	for(int k = 0; k < NumSamples; k++) {
		float PointPHeight = length(v3PointP);

		float2 DensityRatio = GetDensityRatio(PointPHeight);
		DensityRatio *= fScaledLength;

		float2 ViewerOpticalDepth = t(v3PointP, v3PointPv);
						
		float dFarPPc = HitOuterSphere(v3PointP, v3SunDir.xyz);
		float2 SunOpticalDepth = t(v3PointP, v3PointP + v3SunDir.xyz * dFarPPc);

		float2 OpticalDepthP = SunOpticalDepth.xy + ViewerOpticalDepth.xy;
		float3 v3Attenuation = exp(- Kr4PI * InvWavelength * OpticalDepthP.x - Km4PI * OpticalDepthP.y);

		v3RayleighSum += DensityRatio.x * v3Attenuation;
		v3MieSum += DensityRatio.y * v3Attenuation;

		v3PointP += v3SampleRay;
	}

	float3 RayLeigh = v3RayleighSum * KrESun;
	float3 Mie = v3MieSum * KmESun;
	RayLeigh *= InvWavelength;
	Mie *= WavelengthMie;
	
	result.RayLeigh = float4(RayLeigh, 1);
	result.Mie = float4(Mie, 1);
	return result;
}


technique Update {
    pass Pass1 {
		VertexShader = compile VP_3_0 VP_main();
		PixelShader = compile FP_3_0 FP_main();

		DEPTHTEST = false;
		DEPTHMASK = false;
		CULL_ENABLED;
		BLEND_NONE;
    }
}
