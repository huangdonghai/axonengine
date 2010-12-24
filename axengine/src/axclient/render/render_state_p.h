#ifndef AX_RENDER_STATE_P_H
#define AX_RENDER_STATE_P_H

AX_BEGIN_NAMESPACE

class ApiWrap;
extern ApiWrap *g_apiWrap;

#if 0
class SamplerState : public RefObject
{
public:
	SamplerState(const SamplerDesc &desc);

	virtual ~SamplerState();

	const SamplerDesc &getDesc() const
	{ return m_desc; }

private:
	SamplerDesc m_desc;
	phandle_t m_h;
};

class BlendState : public RefObject
{
public:
	BlendState(const BlendDesc &desc);
	virtual ~BlendState();

private:
	BlendDesc m_desc;
	phandle_t m_h;
};

class DepthStencilState : public RefObject
{
public:
	DepthStencilState(const DepthStencilDesc &desc);
	virtual ~DepthStencilState();

private:
	DepthStencilDesc m_desc;
	phandle_t m_h;
};

class RasterizerState : public RefObject
{
public:
	RasterizerState(const RasterizerDesc &desc);
	virtual ~RasterizerState();

private:
	RasterizerDesc m_desc;
	phandle_t m_h;
};

class RenderState {
public:
	RenderState(RenderStateId id)
	{
		BlendDesc blend_desc;
		DepthStencilDesc ds_desc;
		RasterizerDesc rs_desc;

		if (!id.depthWrite)
			ds_desc.depthWritable = false;

		if (!id.depthTest)
			ds_desc.depthEnable = false;

		if (id.twoSided)
			rs_desc.cullMode = RasterizerDesc::CullMode_None;

		if (id.wireframed)
			rs_desc.fillMode = RasterizerDesc::FillMode_Wireframe;

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

	static SamplerStatePtr findSamplerState(const SamplerDesc *desc);
	static BlendStatePtr findBlendState(const BlendDesc *desc);
	static RasterizerStatePtr findRasterizerState(const RasterizerDesc *desc);
	static DepthStencilStatePtr findDepthStencilState(const DepthStencilDesc *desc);

public:
	BlendStatePtr m_blendState;
	DepthStencilStatePtr m_depthStencilState;
	RasterizerStatePtr m_rasterizerState;
};

#endif

AX_END_NAMESPACE

#endif // AX_RENDER_STATE_P_H
