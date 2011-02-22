/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


float3x4 VP_getModelMatrix(MeshVertex input)
{
#if G_GEOMETRY_INSTANCING
	return float3x4(input.matrixX, input.matrixY, input.matrixZ);
#else
	return g_modelMatrix;
#endif
}

float4 VP_getInstanceParam(MeshVertex input)
{
#if G_GEOMETRY_INSTANCING
	return input.userDefined;
#else
	return g_instanceParam;
#endif
}

// transform from model space to world space
float3 VP_modelToWorld(MeshVertex input, float3 ms)
{
	float4 posMS = float4(ms,1);
	return mul(VP_getModelMatrix(input), posMS);
}

float3 VP_modelToWorld(float3 ms)
{
	float4 posMS = float4(ms,1);
	return mul(g_modelMatrix, posMS).xyz;
}

// transform from world space to clip space
float4 VP_worldToClip(float3 ws)
{
	return mul(g_viewProjMatrix, float4(ws,1));
}

float4 VP_modelToClip(MeshVertex input, float3 ms)
{
	return VP_worldToClip(VP_modelToWorld(input, ms));
}

float4 VP_modelToClip(float3 ms)
{
	return VP_worldToClip(VP_modelToWorld(ms));
}

float3 VP_getModelPos(MeshVertex input)
{
	return VP_getModelMatrix(input)._m03_m13_m23;
}

float3 VP_modelRotateScale(MeshVertex input, float3 ms)
{
	return mul((float3x3)VP_getModelMatrix(input), ms);
}

float3 N_modelToWorld(MeshVertex input, float3 ms)
{
	return VP_modelRotateScale(input, ms);
}

VertexOut OutputMeshVertex(MeshVertex IN)
{
    VertexOut OUT = (VertexOut)0;

	// transform tangent space vector to world space
	OUT.normal = N_modelToWorld(IN, IN.normal.xyz);
#if !NO_NORMALMAPPING
	OUT.tangent = N_modelToWorld(IN, IN.tangent.xyz);
	OUT.binormal = cross(OUT.normal, OUT.tangent);
#endif
	float3 posWorld = VP_modelToWorld(IN, IN.position);
	OUT.worldPos = posWorld;

	float4 posClip = VP_worldToClip(posWorld);
	OUT.hpos = posClip;

	OUT.screenTc = Clip2Screen(posClip);

#if G_OPENGL
	OUT.color = IN.color;
#else
	OUT.color = IN.color.bgra;
#endif
	OUT.color.rgb *= VP_getInstanceParam(IN).rgb;

	VP_final(IN, OUT);

    return OUT;
}
