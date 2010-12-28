#ifndef AX_RENDER_STATE_H
#define AX_RENDER_STATE_H

AX_BEGIN_NAMESPACE

enum ElementType {
	ElementType_PointList,
	ElementType_LineList,
	ElementType_TriList,
	ElementType_TriStrip
};

struct RHandle
{
public:
	RHandle() : m_phandle(0) {}

	RHandle &operator=(void *p) { *m_phandle = p; }

	template<class Q>
	Q cast() { return m_phandle->to<Q>(); }
	void alloc() { m_phandle = new Handle; }
	void free() { delete m_phandle; }

	Handle *m_phandle;
};

struct SamplerDesc {
	enum ClampMode {
		ClampMode_Repeat,
		ClampMode_Clamp,
		ClampMode_Border // only used in engine internal
	};

	enum FilterMode {
		FilterMode_Nearest,
		FilterMode_Linear,
		FilterMode_LinearMipmap,
		FilterMode_Trilinear,
		FilterMode_Anisotropic
	};

	enum BorderColor {
		BorderColor_Zero, BorderColor_One
	};

	SamplerDesc()
	{
		intValue = 0;
		clampMode = ClampMode_Repeat;
		filterMode = FilterMode_Trilinear;
		borderColor = BorderColor_Zero;
		maxAnisotropy = 0;
	}

	size_t hash() const
	{
		return intValue;
	}

	operator size_t() const { return hash(); }

	bool operator==(const SamplerDesc &rhs) const
	{
		return intValue == rhs.intValue;
	}

	union {
		struct {
			ClampMode clampMode : 8;
			FilterMode filterMode : 8;
			BorderColor borderColor : 8;
			int maxAnisotropy : 8;
		};
		int intValue;
	};
};

AX_STATIC_ASSERT(sizeof(SamplerDesc)<=sizeof(int));


struct DepthStencilDesc {
	enum CompareFunc {
		CompareFunc_Never,
		CompareFunc_Less,
		CompareFunc_Equal,
		CompareFunc_LessEqual,
		CompareFunc_Greater,
		CompareFunc_NotEqual,
		CompareFunc_GreaterEqual,
		CompareFunc_Always
	};

	enum StencilOp {
		StencilOp_Keep,
		StencilOp_Zero,
		StencilOp_Replace,
		StencilOp_IncrementSat,
		StencilOp_DecrementSat,
		StencilOp_Invert,
		StencilOp_Increment,
		StencilOp_Decrement
	};

	DepthStencilDesc()
	{
		intValue = 0;
		depthEnable = true;
		depthWritable = true;
		depthFunc = CompareFunc_LessEqual;

		stencilEnable = false;
		stencilSkyMaskRead = false;
		stencilSkyMaskWrite = false;
#if 0
		stencilReadMask = 0xff;
		stencilWriteMask = 0xff;
#endif
		stencilFailOp = StencilOp_Keep;
		stencilDepthFailOp = StencilOp_Keep;
		stencilPassOp = StencilOp_Keep;
		stencilFunc = CompareFunc_Never;

		stencilFailOpBackface = stencilFailOp;
		stencilDepthFailOpBackface = stencilDepthFailOp;
		stencilPassOpBackface = stencilPassOp;
		stencilFuncBackface = stencilFunc;
	}

	size_t hash() const
	{
		return intValue;
	}

	operator size_t() const { return hash(); }

	bool operator==(const DepthStencilDesc &rhs) const
	{
		return intValue == rhs.intValue;
	}

	union {
		struct {
			int depthEnable : 1;
			int depthWritable : 1;
			int stencilEnable : 1;

			CompareFunc depthFunc : 3;

			int stencilSkyMaskRead : 1;
			int stencilSkyMaskWrite : 1;
#if 0
			int stencilReadMask : 8;
			int stencilWriteMask : 8;
#endif
			StencilOp stencilFailOp : 3;
			StencilOp stencilDepthFailOp : 3;
			StencilOp stencilPassOp : 3;
			CompareFunc stencilFunc : 3;

			StencilOp stencilFailOpBackface : 3;
			StencilOp stencilDepthFailOpBackface : 3;
			StencilOp stencilPassOpBackface : 3;
			CompareFunc stencilFuncBackface : 3;
		};
		int intValue;
	};
};
AX_STATIC_ASSERT(sizeof(DepthStencilDesc)<=sizeof(int));

struct RasterizerDesc {
	enum FillMode {
		FillMode_Point,
		FillMode_Wireframe,
		FillMode_Solid
	};

	enum CullMode {
		CullMode_Front,
		CullMode_Back,
		CullMode_None
	};

	RasterizerDesc()
	{
		intValue = 0;
		fillMode = FillMode_Solid;
		cullMode = CullMode_Back;

		frontCounterClockwise = false;
		depthBias = 0;
		depthClipEnable = false;
		scissorEnable = true;
		multisampleEnable = false;
		antialiasedLineEnable = false;
	}

	size_t hash() const
	{
		return intValue;
	}

	operator size_t() const { return hash(); }

	bool operator==(const RasterizerDesc &rhs) const
	{
		return intValue == rhs.intValue;
	}

	union {
		struct {
			FillMode fillMode : 2;
			CullMode cullMode : 2;
			int frontCounterClockwise : 1;
			int depthBias : 1;
			int depthClipEnable : 1;
			int scissorEnable : 1;
			int multisampleEnable : 1;
			int antialiasedLineEnable : 1;
		};
		int intValue;
	};
};

AX_STATIC_ASSERT(sizeof(RasterizerDesc)<=sizeof(int));


struct BlendDesc {
	enum BlendFactor {
		BlendFactor_Zero,
		BlendFactor_One,
		BlendFactor_SrcColor,
		BlendFactor_OneMinusSrcColor,
		BlendFactor_DstColor,
		BlendFactor_OneMinusDstColor,
		BlendFactor_SrcAlpha,
		BlendFactor_OneMinusSrcAlpha,
		BlendFactor_SrcAlphaSaturate
	};

	enum BlendOp {
		BlendOp_Add, BlendOp_Subtract, BlendOp_RevSubtract, BlendOp_Min, BlendOp_Max
	};

	BlendDesc()
	{
		intValue = 0;
		alphaToCoverageEnable = false;
		independentBlendEnable = false;

		blendEnable = false;
		srcBlend = BlendFactor_One;
		destBlend = BlendFactor_Zero;
		blendOp = BlendOp_Add;

		srcBlendAlpha = srcBlend;
		destBlendAlpha = destBlend;
		blendOpAlpha = blendOp;

		renderTargetWriteMask = 0xf;
	}

	size_t hash() const
	{
		return intValue;
	}

	operator size_t() const { return hash(); }

	bool operator==(const BlendDesc &rhs) const
	{
		return intValue == rhs.intValue;
	}

	union {
		struct {
			int alphaToCoverageEnable : 1;
			int independentBlendEnable : 1;

			int blendEnable : 1;
			BlendFactor srcBlend : 4;
			BlendFactor destBlend : 4;
			BlendOp blendOp : 4;
			BlendFactor srcBlendAlpha : 4;
			BlendFactor destBlendAlpha : 4;
			BlendOp blendOpAlpha : 4;
			int renderTargetWriteMask : 4;
		};

		int intValue;
	};
};

AX_STATIC_ASSERT(sizeof(BlendDesc)<=sizeof(int));


AX_END_NAMESPACE

#endif // AX_RENDER_STATE_H
