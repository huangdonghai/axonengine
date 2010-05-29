#ifndef AX_SAMPLERDATA_H
#define AX_SAMPLERDATA_H

AX_BEGIN_NAMESPACE

class ApiWrap;
extern ApiWrap *g_apiWrap;

class SamplerState : public RefObject
{
public:
	SamplerState(const SamplerStateDesc &desc)
	{ g_apiWrap->createSamplerState(&m_h, desc); }

	virtual ~SamplerState()
	{ g_apiWrap->deleteSamplerState(&m_h); }

	const SamplerStateDesc &getDesc() const
	{ return m_desc; }


private:
	SamplerStateDesc m_desc;
	Handle m_h;
};

class BlendState : public RefObject
{
public:
	BlendState(const BlendStateDesc &desc);
	virtual ~BlendState();

private:
	Handle m_h;
};

class DepthStencilState : public RefObject
{
public:
	DepthStencilState(const DepthStencilStateDesc &desc);
	virtual ~DepthStencilState();

private:
	Handle m_h;
};

class RasterizerState : public RefObject
{
public:
	RasterizerState(const RasterizerStateDesc &desc);
	virtual ~RasterizerState();

private:
	Handle m_h;
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

		m_blendState = g_renderSystem->findBlendState(&blend_desc);
		m_depthStencilState = g_renderSystem->findDepthStencilState(&ds_desc);
		m_rasterizerState = g_renderSystem->findRasterizerState(&rs_desc);
	}

	~RenderState()
	{
	}

public:
	BlendStatePtr m_blendState;
	DepthStencilStatePtr m_depthStencilState;
	RasterizerStatePtr m_rasterizerState;
};

AX_END_NAMESPACE

#endif // AX_SAMPLERDATA_H
