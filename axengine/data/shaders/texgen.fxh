/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


float FOG_compute(float4 hpos) {
	bool fogenable = G_FOG;

	if (!fogenable) {
		return 1;
	}
	return exp(- g_fogParams.w * hpos.w);
}

float4 VP_computeStreamTc(VertexIn a2v) {
	float4 result;
#if G_BASETC_ANIM
	result.xy = mul(g_baseTcMatrix, float4(a2v.st, 0, 1)).xy;
	result.zw = a2v.st2;
#else
	result.xy = a2v.st;
	result.zw = a2v.st2;
#endif
	return result;
}

void VP_final(VertexIn a2v, inout VertexOut v2f) {
#if G_BASETC_ANIM
	v2f.streamTc.xy = mul(g_baseTcMatrix, float4(a2v.st, 0, 1)).xy;
	v2f.streamTc.zw = a2v.st2;
#else
	v2f.streamTc.xy = a2v.st;
	v2f.streamTc.zw = a2v.st2;
#endif

#if G_FOG
	v2f.fog = FOG_compute(v2f.hpos);
#endif
}
