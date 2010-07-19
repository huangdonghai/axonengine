AX_BEGIN_CBUFFER(cbInteractionConst, 0)
	/* vs use */
	AX_UNIFORM(float3x4,	Matrix,		g_modelMatrix,				41, 0)
	AX_UNIFORM(float4,		Vector4,	g_instanceParam,			44, 0)
	AX_UNIFORM(float4x4,	Matrix4,	g_baseTcMatrix,				45, 0)
	/* ps use */
	AX_UNIFORM(float3,		Vector3,	g_matDiffuse,				0, 12)
	AX_UNIFORM(float3,		Vector3,	g_matSpecular,				0, 13)
	AX_UNIFORM(float,		float,		g_matShiness,				0, 14)
	AX_UNIFORM(float2,		Vector2,	g_layerScale,				0, 15)
	/*shadow parameter*/
	AX_UNIFORM(float4x4,	Matrix4,	g_shadowMatrix,				0, 16)
	AX_UNIFORM(float4x4,	Matrix4,	g_csmOffsetScales,			0, 20)
AX_END_CBUFFER()