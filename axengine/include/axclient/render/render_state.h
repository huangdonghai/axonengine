#ifndef AX_RENDER_STATE_H
#define AX_RENDER_STATE_H

AX_BEGIN_NAMESPACE

struct RenderClearer {
	Rgba color;
	float depth;
	int stencil;
	bool isClearColor : 1;
	bool isClearDepth : 1;
	bool isClearStencil : 1;

	RenderClearer() : color(Rgba::Black), depth(1.0f), stencil(0), isClearColor(false), isClearDepth(false), isClearStencil(false)
	{}

	void clearDepth(bool enable, float ref = 1.0f)
	{
		isClearDepth = enable;
		depth = ref;
	}

	void clearColor(bool enable, Rgba ref = Rgba::Zero)
	{
		isClearColor = enable;
		color = ref;
	}

	void clearStencil(bool enable, int ref)
	{
		isClearStencil = enable;
		stencil = ref;
	}
};

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

	void set(void *p) { *m_phandle = p; }
	bool isValid() const { return *m_phandle != 0; }

	template<class Q>
	Q cast() { return m_phandle->castTo<Q>(); }
	void alloc() { m_phandle = new Handle; }
	void free() { delete m_phandle; }

private:
	Handle * volatile m_phandle;
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
			uint_t clampMode : 8;
			uint_t filterMode : 8;
			uint_t borderColor : 8;
			uint_t maxAnisotropy : 8;
		};
		uint_t intValue;
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
			uint_t depthEnable : 1;
			uint_t depthWritable : 1;
			uint_t stencilEnable : 1;

			uint_t depthFunc : 3;

			uint_t stencilSkyMaskRead : 1;
			uint_t stencilSkyMaskWrite : 1;
#if 0
			uint_t stencilReadMask : 8;
			uint_t stencilWriteMask : 8;
#endif
			uint_t stencilFailOp : 3;
			uint_t stencilDepthFailOp : 3;
			uint_t stencilPassOp : 3;
			uint_t stencilFunc : 3;

			uint_t stencilFailOpBackface : 3;
			uint_t stencilDepthFailOpBackface : 3;
			uint_t stencilPassOpBackface : 3;
			uint_t stencilFuncBackface : 3;
		};
		uint_t intValue;
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
			uint_t fillMode : 2;
			uint_t cullMode : 2;
			uint_t frontCounterClockwise : 1;
			uint_t depthBias : 1;
			uint_t depthClipEnable : 1;
			uint_t scissorEnable : 1;
			uint_t multisampleEnable : 1;
			uint_t antialiasedLineEnable : 1;
		};
		uint_t intValue;
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
			uint_t alphaToCoverageEnable : 1;
			uint_t independentBlendEnable : 1;

			uint_t blendEnable : 1;
			uint_t srcBlend : 4;
			uint_t destBlend : 4;
			uint_t blendOp : 4;
			uint_t srcBlendAlpha : 4;
			uint_t destBlendAlpha : 4;
			uint_t blendOpAlpha : 4;
			uint_t renderTargetWriteMask : 4;
		};

		uint_t intValue;
	};
};

AX_STATIC_ASSERT(sizeof(BlendDesc)<=sizeof(int));


AX_END_NAMESPACE

#endif // AX_RENDER_STATE_H
