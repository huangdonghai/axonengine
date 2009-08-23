/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


float3x4 VP_getModelMatrix(VertexIn input) {
#if G_GEOMETRY_INSTANCING
	return float3x4(input.matrixX, input.matrixY, input.matrixZ);
#else
	return g_modelMatrix;
#endif
}

float4 VP_getInstanceParam(VertexIn input) {
#if G_GEOMETRY_INSTANCING
	return input.userDefined;
#else
	return g_instanceParam;
#endif
}

// transform from model space to world space
float3 VP_modelToWorld(VertexIn input, float3 ms) {
#if G_MODELMATRIX_IDENTITY
	return ms;
#else
	float4 posMS = float4(ms,1);

	return mul(VP_getModelMatrix(input), posMS);
#endif
}

// transform from world space to clip space
float4 VP_worldToClip(float3 ws) {
	return mul(g_viewProjMatrix, float4(ws,1));
}

float4 VP_modelToClip(VertexIn input, float3 ms) {
	return VP_worldToClip(VP_modelToWorld(input, ms));
}

float3 VP_getModelPos(VertexIn input) {
#if G_MODELMATRIX_IDENTITY
	return 0;
#else
	return VP_getModelMatrix(input)._m03_m13_m23;
#endif
}

float3 VP_modelRotateScale(VertexIn input, float3 ms) {
#if G_MODELMATRIX_IDENTITY
	return ms;
#else
	return mul((float3x3)VP_getModelMatrix(input), ms);
#endif
}

float3 N_modelToWorld(VertexIn input, float3 ms) {
#if G_MODELMATRIX_IDENTITY
	return ms;
#else
	return VP_modelRotateScale(input, ms);
#endif
}

