#include "../private.h"

AX_BEGIN_NAMESPACE

namespace {
	// sampler states
	typedef Dict<SamplerStateDesc, SamplerState*> SamplerStateDict;
	SamplerStateDict s_samplerStateDict;
	SamplerStateDesc s_defaultSamplerStateDesc;

	// depth stencil states
	typedef Dict<DepthStencilStateDesc, DepthStencilState *> DepthStencilStateDict;
	DepthStencilStateDict s_depthStencilStateDict;
	DepthStencilStateDesc s_defaultDepthStencilStateDesc;

	// rasterizer state
	typedef Dict<RasterizerStateDesc, RasterizerState *> RasterizerStateDict;
	RasterizerStateDict s_rasterizerStateDict;
	RasterizerStateDesc s_defaultRasterizerStateDesc;

	// blend state
	typedef Dict<BlendStateDesc, BlendState *> BlendStateDict;
	BlendStateDict s_blendStateDict;
	BlendStateDesc s_defaultBlendStateDesc;
} // namespace


SamplerState::SamplerState(const SamplerStateDesc &desc) : m_desc(desc)
{
	g_apiWrap->createSamplerState(m_h, m_desc);
	s_samplerStateDict[m_desc] = this;
}

SamplerState::~SamplerState()
{
	s_samplerStateDict.erase(m_desc);
	g_apiWrap->deleteSamplerState(m_h);
}


BlendState::BlendState(const BlendStateDesc &desc) : m_desc(desc)
{
	g_apiWrap->createBlendState(m_h, m_desc);
	s_blendStateDict[m_desc] = this;
}

BlendState::~BlendState()
{
	s_blendStateDict.erase(m_desc);
	g_apiWrap->deleteBlendState(m_h);
}


DepthStencilState::DepthStencilState(const DepthStencilStateDesc &desc) : m_desc(desc)
{
	g_apiWrap->createDepthStencilState(m_h, m_desc);
	s_depthStencilStateDict[m_desc] = this;
}

DepthStencilState::~DepthStencilState()
{
	s_depthStencilStateDict.erase(m_desc);
	g_apiWrap->deleteDepthStencilState(m_h);
}


RasterizerState::RasterizerState(const RasterizerStateDesc &desc)
{
	g_apiWrap->createRasterizerState(m_h, m_desc);
	s_rasterizerStateDict[m_desc] = this;
}

RasterizerState::~RasterizerState()
{
	s_rasterizerStateDict.erase(m_desc);
	g_apiWrap->deleteRasterizerState(m_h);
}

SamplerStatePtr RenderState::findSamplerState(const SamplerStateDesc *desc)
{
	if (!desc)
		desc = &s_defaultSamplerStateDesc;

	SamplerStateDict::const_iterator it = s_samplerStateDict.find(*desc);
	if (it != s_samplerStateDict.end())
		return it->second;

	return new SamplerState(*desc);
}

BlendStatePtr RenderState::findBlendState(const BlendStateDesc *desc)
{
	if (!desc)
		desc = &s_defaultBlendStateDesc;

	BlendStateDict::const_iterator it = s_blendStateDict.find(*desc);
	if (it != s_blendStateDict.end())
		return it->second;

	return new BlendState(*desc);
}

RasterizerStatePtr RenderState::findRasterizerState(const RasterizerStateDesc *desc)
{
	if (!desc)
		desc = &s_defaultRasterizerStateDesc;

	RasterizerStateDict::const_iterator it = s_rasterizerStateDict.find(*desc);
	if (it != s_rasterizerStateDict.end())
		return it->second;

	return new RasterizerState(*desc);
}

DepthStencilStatePtr RenderState::findDepthStencilState(const DepthStencilStateDesc *desc)
{
	if (!desc)
		desc = &s_defaultDepthStencilStateDesc;

	DepthStencilStateDict::const_iterator it = s_depthStencilStateDict.find(*desc);
	if (it != s_depthStencilStateDict.end())
		return it->second;

	return new DepthStencilState(*desc);
}


AX_END_NAMESPACE
