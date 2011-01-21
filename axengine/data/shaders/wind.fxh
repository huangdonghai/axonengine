/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


///////////////////////////////////////////////////////////////////////  
//  WindEffect
//
//  New with 4.0 is a two-weight wind system that allows the tree model
//  to bend at more than one branch level.
//
//  In order to keep the vertex size small, the wind parameters have been
//  compressed as detailed here:
//
//      vWindInfo.x = (wind_matrix_index1 * 10.0) / NUM_WIND_MATRICES  + wind_weight1
//      vWindInfo.y = (wind_matrix_index2 * 10.0) / NUM_WIND_MATRICES  + wind_weight2
//
//  * Note: NUM_WIND_MATRICES cannot be larger than 10 in this case
//
//	* Caution: Negative wind weights will not work with this scheme.  We rely on the
//		       fact that the SpeedTreeRT library clamps wind weights to [0.0, 1.0]

#define TWO_WEIGHT_WIND		1
#define NUM_WIND_MATRICES	3


float3 WindEffect(MeshVertex IN, float3 vPosition, float2 vWindInfo) {
	// decode both wind weights and matrix indices at the same time in order to save
	// vertex instructions

	//float2 vWeights = frac(vWindInfo.xy);
	//float2 vIndices = float2(2.0f, 2.0f);//vWindInfo;//(vWindInfo - vWeights) - 1.0f;
	vWindInfo.xy += VP_getInstanceParam(IN).zz;
	float2 vWeights = frac(vWindInfo.xy);
	float2 vIndices = (vWindInfo - vWeights) * 0.05f * NUM_WIND_MATRICES;

	// first-level wind effect - interpolate between static position and fully-blown
	// wind position by the wind weight value
#if 1 //G_OPENGL
	float3 vWindEffect = lerp(vPosition.xyz, mul(vPosition, (float3x3)(g_windMatrices[vIndices.x])), vWeights.x);
#else
	float3 vWindEffect = lerp(vPosition.xyz, mul(vPosition, g_windMatrices[vIndices.x]), vWeights.x);
#endif

	// second-level wind effect - interpolate between first-level wind position and 
	// the fully-blown wind position by the second wind weight valuey
#if TWO_WEIGHT_WIND
#if 1 //G_OPENGL
	return lerp(vWindEffect, mul(vWindEffect, (float3x3)(g_windMatrices[vIndices.y])), vWeights.y);
#else
	return lerp(vWindEffect, mul(vWindEffect, g_windMatrices[vIndices.y]), vWeights.y);
#endif
#else
	return vWindEffect;
#endif
}

