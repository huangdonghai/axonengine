#ifndef AX_RENDER_STATE_H
#define AX_RENDER_STATE_H

AX_BEGIN_NAMESPACE

enum ElementType {
	ElementType_PointList,
	ElementType_LineList,
	ElementType_TriList,
	ElementType_TriStrip
};

struct SamplerStateDesc {
	enum ClampMode {
		CM_Repeat,
		CM_Clamp,
		CM_Border // only used in engine internal
	};

	enum FilterMode {
		FM_Nearest,
		FM_Linear,
		FM_LinearMipmap,
		FM_Trilinear,
		FM_Anisotropic
	};

	enum BorderColor {
		BM_Zero, BM_One
	};

	SamplerStateDesc() : clampMode(CM_Repeat), filterMode(FM_Trilinear), borderMode(BM_Zero), maxAnisotropy(0) {}

	size_t hash() const
	{
		return intValue;
	}

	operator size_t() const { return hash(); }

	bool operator==(const SamplerStateDesc &rhs) const
	{
		return intValue == rhs.intValue;
	}

	union {
		struct {
			ClampMode clampMode : 8;
			FilterMode filterMode : 8;
			BorderColor borderMode : 8;
			int maxAnisotropy : 8;
		};
		int intValue;
	};
};

struct DepthStencilStateDesc {
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

	DepthStencilStateDesc()
	{
		depthEnable = true;
		depthWritable = true;
		depthFunc = CompareFunc_LessEqual;

		stencilEnable = false;
		stencilReadMask = 0xff;
		stencilWriteMask = 0xff;

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
		int size = sizeof(*this);
		byte_t const *bytes = reinterpret_cast<byte_t const *>(this);

		size_t result = 0;
		for (int i = 0; i < size; i++) {
			hash_combine(result, bytes[i]);
		}
		return result;
	}

	operator size_t() const { return hash(); }

	bool operator==(const DepthStencilStateDesc &rhs) const
	{
		return ::memcmp(this, &rhs, sizeof(*this)) == 0;
	}

	bool depthEnable;
	bool depthWritable;
	CompareFunc depthFunc;
	bool stencilEnable;
	byte_t stencilReadMask;
	byte_t stencilWriteMask;

	StencilOp stencilFailOp;
	StencilOp stencilDepthFailOp;
	StencilOp stencilPassOp;
	CompareFunc stencilFunc;

	StencilOp stencilFailOpBackface;
	StencilOp stencilDepthFailOpBackface;
	StencilOp stencilPassOpBackface;
	CompareFunc stencilFuncBackface;
};

struct RasterizerStateDesc {
	enum FillMode {
		FillMode_Wireframe,
		FillMode_Solid
	};

	enum CullMode {
		CullMode_Front,
		CullMode_Back,
		CullMode_None
	};

	RasterizerStateDesc()
	{
		fillMode = FillMode_Solid;
		cullMode = CullMode_Back;

		frontCounterClockwise = false;
		depthBias = 0;
		depthBiasClamp = 0;
		slopeScaledDepthBias = 0;
		depthClipEnable = false;
		scissorEnable = true;
		multisampleEnable = false;
		antialiasedLineEnable = false;
	}

	size_t hash() const
	{
		int size = sizeof(*this);
		byte_t const *bytes = reinterpret_cast<byte_t const *>(this);

		size_t result = 0;
		for (int i = 0; i < size; i++) {
			hash_combine(result, bytes[i]);
		}
		return result;
	}

	operator size_t() const { return hash(); }

	bool operator==(const RasterizerStateDesc &rhs) const
	{
		return ::memcmp(this, &rhs, sizeof(*this)) == 0;
	}

	FillMode fillMode;
	CullMode cullMode;
	bool frontCounterClockwise;
	int depthBias;
	float depthBiasClamp;
	float slopeScaledDepthBias;
	bool depthClipEnable;
	bool scissorEnable;
	bool multisampleEnable;
	bool antialiasedLineEnable;
};

struct BlendStateDesc {
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

	BlendStateDesc()
	{
		alphaToCoverageEnable = false;
		independentBlendEnable = false;

		blendEnable = false;
		srcBlend = BlendFactor_One;
		destBlend = BlendFactor_Zero;
		blendOp = BlendOp_Add;

		srcBlendAlpha = srcBlend;
		destBlendAlpha = destBlend;
		blendOpAlpha = blendOp;

		renderTargetWriteMask = 0xff;
	}

	size_t hash() const
	{
		int size = sizeof(*this);
		byte_t const *bytes = reinterpret_cast<byte_t const *>(this);

		size_t result = 0;
		for (int i = 0; i < size; i++) {
			hash_combine(result, bytes[i]);
		}
		return result;
	}

	operator size_t() const { return hash(); }

	bool operator==(const BlendStateDesc &rhs) const
	{
		return ::memcmp(this, &rhs, sizeof(*this)) == 0;
	}

	bool alphaToCoverageEnable;
	bool independentBlendEnable;

	bool blendEnable;
	BlendFactor srcBlend;
	BlendFactor destBlend;
	BlendOp blendOp;
	BlendFactor srcBlendAlpha;
	BlendFactor destBlendAlpha;
	BlendOp blendOpAlpha;
	byte_t renderTargetWriteMask;
};

AX_END_NAMESPACE

#endif // AX_RENDER_STATE_H
