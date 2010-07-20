AX_BEGIN_CBUFFER(cbSceneConst, 0)
	/* both vs and ps use */
	AX_ITEM(float,		float,		g_time,						SREG0)
	AX_ITEM(float4,		Vector4,	g_cameraPos,				SREG1)
	AX_ITEM(float4,		Vector4,	g_fogParams,				SREG2)
	AX_ITEM(float4,		Vector4,	g_waterFogParams,			SREG3)
	/* vs use */
	AX_ITEM(float4x4,	Matrix4,	g_viewProjMatrix,			SREG4)
	AX_ITEM(float4x4,	Matrix4,	g_viewProjNoTranslate,		SREG8)
	AX_ITEM(float3x3,	Matrix3,	g_cameraAxis,				SREG12)
	AX_ITEM(float3,		Vector3,	g_cameraAngles,				SREG15)
	AX_ITEM(float4,		Vector4,	g_sceneSize,				SREG16)
	AX_ARRAY(float4x4,	Matrix4,	g_windMatrices,		3,		SREG17)
	AX_ARRAY(float4,	Vector4,	g_leafAngles,		8,		SREG29)
	/* ps use */
	AX_ITEM(float4,		Vector4,	g_globalLightPos,			SREG37)
	AX_ITEM(float4,		Vector4,	g_globalLightColor,			SREG38)
	AX_ITEM(float4x4,	Matrix4,	g_csmOffsetScales,			SREG39)
	AX_ITEM(float4,		Vector4,	g_skyColor,					SREG43)
	AX_ITEM(float4,		Vector4,	g_exposure,					SREG44)
AX_END_CBUFFER