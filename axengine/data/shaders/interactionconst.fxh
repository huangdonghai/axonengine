AX_BEGIN_CBUFFER(cbInteractionConst, 0)
	/* vs use */
	AX_ITEM(float3x4,	Matrix,		g_modelMatrix,				41, 0)
	AX_ITEM(float4,		Vector4,	g_instanceParam,			44, 0)
	AX_ITEM(float4x4,	Matrix4,	g_baseTcMatrix,				45, 0)
	/* ps use */
	AX_ITEM(float3,		Vector3,	g_matDiffuse,				0, 12)
	AX_ITEM(float3,		Vector3,	g_matSpecular,				0, 13)
	AX_ITEM(float,		float,		g_matShiness,				0, 14)
	AX_ITEM(float2,		Vector2,	g_layerScale,				0, 15)
	/*shadow parameter*/
	AX_ITEM(float4x4,	Matrix4,	g_shadowMatrix,				0, 16)
	AX_ITEM(float4x4,	Matrix4,	g_csmOffsetScales,			0, 20)
AX_END_CBUFFER()