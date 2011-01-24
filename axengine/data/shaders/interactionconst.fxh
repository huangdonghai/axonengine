AX_BEGIN_CBUFFER(cbInteractionConst, 1)
	/* vs use */
	AX_ITEM(float3x4,	Matrix,		g_modelMatrix,				IREG0)
	AX_ITEM(float4,		Vector4,	g_instanceParam,			IREG3)
	AX_ITEM(float4x4,	Matrix4,	g_texMatrix,				IREG4)
	/* ps use */
	AX_ITEM(float3,		Vector3,	g_matDiffuse,				IREG8)
	AX_ITEM(float3,		Vector3,	g_matSpecular,				IREG9)
	AX_ITEM(float,		float,		g_matShiness,				IREG10)
	AX_ITEM(float2,		Vector2,	g_detailScale,				IREG11)
	AX_ITEM(float4,		Vector4,	g_textureSize,				IREG12) // xy is texture size, zw is inverse texture size
AX_END_CBUFFER