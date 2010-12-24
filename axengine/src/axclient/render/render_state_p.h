#ifndef AX_RENDER_STATE_P_H
#define AX_RENDER_STATE_P_H

AX_BEGIN_NAMESPACE

class ApiWrap;
extern ApiWrap *g_apiWrap;

class SamplerState : public RefObject
{
public:
	SamplerState(const SamplerStateDesc &desc);

	virtual ~SamplerState();

	const SamplerStateDesc &getDesc() const
	{ return m_desc; }

private:
	SamplerStateDesc m_desc;
	phandle_t m_h;
};

class BlendState : public RefObject
{
public:
	BlendState(const BlendStateDesc &desc);
	virtual ~BlendState();

private:
	BlendStateDesc m_desc;
	phandle_t m_h;
};

class DepthStencilState : public RefObject
{
public:
	DepthStencilState(const DepthStencilStateDesc &desc);
	virtual ~DepthStencilState();

private:
	DepthStencilStateDesc m_desc;
	phandle_t m_h;
};

class RasterizerState : public RefObject
{
public:
	RasterizerState(const RasterizerStateDesc &desc);
	virtual ~RasterizerState();

private:
	RasterizerStateDesc m_desc;
	phandle_t m_h;
};

class RenderState {
public:
	RenderState(RenderStateId id)
	{
		BlendStateDesc blend_desc;
		DepthStencilStateDesc ds_desc;
		RasterizerStateDesc rs_desc;

		if (!id.depthWrite)
			ds_desc.depthWritable = false;

		if (!id.depthTest)
			ds_desc.depthEnable = false;

		if (id.twoSided)
			rs_desc.cullMode = RasterizerStateDesc::CullMode_None;

		if (id.wireframed)
			rs_desc.fillMode = RasterizerStateDesc::FillMode_Wireframe;

		switch (id.stencilMode) {
		case RenderStateId::StencilMode_Disable:
		case RenderStateId::StencilMode_Mark:
		case RenderStateId::StencilMode_MarkVolume:
		case RenderStateId::StencilMode_TestVolume:
			break;
		}

		switch (id.blendMode) {
		case RenderStateId::BlendMode_Disabled:
		case RenderStateId::BlendMode_Add:
		case RenderStateId::BlendMode_Blend:
		case RenderStateId::BlendMode_Modulate:
			break;
		}

		m_blendState = RenderState::findBlendState(&blend_desc);
		m_depthStencilState = RenderState::findDepthStencilState(&ds_desc);
		m_rasterizerState = RenderState::findRasterizerState(&rs_desc);
	}

	~RenderState()
	{
	}

	static SamplerStatePtr findSamplerState(const SamplerStateDesc *desc);
	static BlendStatePtr findBlendState(const BlendStateDesc *desc);
	static RasterizerStatePtr findRasterizerState(const RasterizerStateDesc *desc);
	static DepthStencilStatePtr findDepthStencilState(const DepthStencilStateDesc *desc);

public:
	BlendStatePtr m_blendState;
	DepthStencilStatePtr m_depthStencilState;
	RasterizerStatePtr m_rasterizerState;
};

AX_END_NAMESPACE

#endif // AX_RENDER_STATE_P_H
