#include "dx9_private.h"

AX_BEGIN_NAMESPACE

namespace {

	typedef Dict<SamplerDesc, DX9_SamplerState *> DX9_SamplerStateDict;
	DX9_SamplerStateDict s_samplerStateDicts[DX9_SamplerState::MAX_STAGES];

	typedef Dict<DepthStencilDesc, DX9_DepthStencilState *> DX9_DepthStencilStateDict;
	DX9_DepthStencilStateDict s_depthStencilStateDict;

	typedef Dict<RasterizerDesc, DX9_RasterizerState *> DX9_RasterizerStateDict;
	DX9_RasterizerStateDict s_rasterizerStateDict;

	typedef Dict<BlendDesc, DX9_BlendState *> DX9_BlendStateDict;
	DX9_BlendStateDict s_blendStateDict;

	inline D3DCMPFUNC trCompareFunc(DepthStencilDesc::CompareFunc func)
	{
		switch (func) {
		case DepthStencilDesc::CompareFunc_Never: return D3DCMP_NEVER;
		case DepthStencilDesc::CompareFunc_Less: return D3DCMP_LESS;
		case DepthStencilDesc::CompareFunc_Equal: return D3DCMP_EQUAL;
		case DepthStencilDesc::CompareFunc_LessEqual: return D3DCMP_LESSEQUAL;
		case DepthStencilDesc::CompareFunc_Greater: return D3DCMP_GREATER;
		case DepthStencilDesc::CompareFunc_NotEqual: return D3DCMP_NOTEQUAL;
		case DepthStencilDesc::CompareFunc_GreaterEqual: return D3DCMP_GREATEREQUAL;
		case DepthStencilDesc::CompareFunc_Always: return D3DCMP_ALWAYS;
		default: AX_ASSERT(0); return D3DCMP_NEVER;
		}
	}

	inline D3DSTENCILOP trStencilOp(DepthStencilDesc::StencilOp op)
	{
		switch (op) {
		case DepthStencilDesc::StencilOp_Keep: return D3DSTENCILOP_KEEP;
		case DepthStencilDesc::StencilOp_Zero: return D3DSTENCILOP_ZERO;
		case DepthStencilDesc::StencilOp_Replace: return D3DSTENCILOP_REPLACE;
		case DepthStencilDesc::StencilOp_IncrementSat: return D3DSTENCILOP_INCRSAT;
		case DepthStencilDesc::StencilOp_DecrementSat: return D3DSTENCILOP_DECRSAT;
		case DepthStencilDesc::StencilOp_Invert: return D3DSTENCILOP_INVERT;
		case DepthStencilDesc::StencilOp_Increment: return D3DSTENCILOP_INCR;
		case DepthStencilDesc::StencilOp_Decrement: return D3DSTENCILOP_DECR;
		default: AX_ASSERT(0); return D3DSTENCILOP_KEEP;
		}
	}

	inline D3DFILLMODE trFillMode(RasterizerDesc::FillMode mode)
	{

	}

	inline D3DCULL trCullMode(bool frontCounterClockwise, RasterizerDesc::CullMode mode)
	{

	}

	inline D3DBLEND trBlendFactor(BlendDesc::BlendFactor factor) {}

	inline D3DBLENDOP trBlendOp(BlendDesc::BlendOp op) {}

} // namespace

DX9_SamplerState::DX9_SamplerState(int stage, const SamplerDesc &desc)
{
	m_stage = stage;
	m_desc = desc;

	dx9_device->BeginStateBlock();

	switch (desc.clampMode) {
	case SamplerDesc::ClampMode_Clamp:
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
		break;
	case SamplerDesc::ClampMode_Border:
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
		dx9_device->SetSamplerState(stage, D3DSAMP_BORDERCOLOR, 0);
		break;
	case SamplerDesc::ClampMode_Repeat:
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		dx9_device->SetSamplerState(stage, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	}

	switch (desc.filterMode) {
	case SamplerDesc::FilterMode_Nearest:
		dx9_device->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		dx9_device->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		dx9_device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		break;
	case SamplerDesc::FilterMode_Linear:
		dx9_device->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9_device->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9_device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		break;
	case SamplerDesc::FilterMode_LinearMipmap:
		dx9_device->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9_device->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9_device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		break;
	case SamplerDesc::FilterMode_Trilinear:
		dx9_device->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dx9_device->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dx9_device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		break;
	}

	dx9_device->EndStateBlock(&m_stateBlock);

	// add to hash
	s_samplerStateDicts[stage][desc] = this;
}

DX9_SamplerState::~DX9_SamplerState()
{
	s_samplerStateDicts[m_stage].erase(m_desc);
	SAFE_RELEASE(m_stateBlock);
}

DX9_SamplerState * DX9_SamplerState::find(int stage, const SamplerDesc &desc)
{
	AX_ASSERT(stage >= 0 && stage <= MAX_STAGES);

	DX9_SamplerStateDict::const_iterator it = s_samplerStateDicts[stage].find(desc);
	if (it != s_samplerStateDicts[stage].end())
		return it->second;

	return new DX9_SamplerState(stage, desc);
}


DX9_DepthStencilState::DX9_DepthStencilState(const DepthStencilDesc &desc)
{
	m_desc = desc;

	dx9_device->BeginStateBlock();

	dx9_device->SetRenderState(D3DRS_ZENABLE, m_desc.depthEnable);
	dx9_device->SetRenderState(D3DRS_ZWRITEENABLE, m_desc.depthWritable);
	dx9_device->SetRenderState(D3DRS_ZFUNC, trCompareFunc(m_desc.depthFunc));

	dx9_device->SetRenderState(D3DRS_STENCILENABLE, m_desc.stencilEnable);
	dx9_device->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);

	dx9_device->SetRenderState(D3DRS_STENCILFAIL, trStencilOp(m_desc.stencilFailOp));
	dx9_device->SetRenderState(D3DRS_STENCILZFAIL, trStencilOp(m_desc.stencilDepthFailOp));
	dx9_device->SetRenderState(D3DRS_STENCILPASS, trStencilOp(m_desc.stencilPassOp));
	dx9_device->SetRenderState(D3DRS_STENCILFUNC, trCompareFunc(m_desc.stencilFunc));

	dx9_device->SetRenderState(D3DRS_CCW_STENCILFAIL, trStencilOp(m_desc.stencilFailOpBackface));
	dx9_device->SetRenderState(D3DRS_CCW_STENCILZFAIL, trStencilOp(m_desc.stencilDepthFailOpBackface));
	dx9_device->SetRenderState(D3DRS_CCW_STENCILPASS, trStencilOp(m_desc.stencilPassOpBackface));
	dx9_device->SetRenderState(D3DRS_CCW_STENCILFUNC, trCompareFunc(m_desc.stencilFuncBackface));

	dx9_device->EndStateBlock(&m_stateBlock);

	// add to hash
	s_depthStencilStateDict[m_desc] = this;
}

DX9_DepthStencilState::~DX9_DepthStencilState()
{
	// remove from hash
	s_depthStencilStateDict.erase(m_desc);

	// release object
	SAFE_RELEASE(m_stateBlock);
}

DX9_DepthStencilState * DX9_DepthStencilState::find( const DepthStencilDesc &desc )
{
	DX9_DepthStencilStateDict::const_iterator it = s_depthStencilStateDict.find(desc);
	if (it != s_depthStencilStateDict.end())
		return it->second;

	return new DX9_DepthStencilState(desc);
}


DX9_RasterizerState::DX9_RasterizerState( const RasterizerDesc &desc )
{
	m_desc = desc;

	dx9_device->BeginStateBlock();
	dx9_device->SetRenderState(D3DRS_FILLMODE, trFillMode(m_desc.fillMode));
	dx9_device->SetRenderState(D3DRS_CULLMODE, trCullMode(m_desc.frontCounterClockwise, m_desc.cullMode));

	if (m_desc.depthBias) {}

	dx9_device->SetRenderState(D3DRS_SCISSORTESTENABLE, m_desc.scissorEnable);

	dx9_device->EndStateBlock(&m_stateBlock);

	// add to hash
	s_rasterizerStateDict[desc] = this;
}

DX9_RasterizerState::~DX9_RasterizerState()
{
	// remove from hash
	s_rasterizerStateDict.erase(m_desc);

	// release object
	SAFE_RELEASE(m_stateBlock);
}

DX9_RasterizerState * DX9_RasterizerState::find(const RasterizerDesc &desc)
{
	DX9_RasterizerStateDict::const_iterator it = s_rasterizerStateDict.find(desc);
	if (it != s_rasterizerStateDict.end())
		return it->second;

	return new DX9_RasterizerState(desc);
}


DX9_BlendState::DX9_BlendState(const BlendDesc &desc)
{
	m_desc = desc;

	dx9_device->BeginStateBlock();
	dx9_device->EndStateBlock(&m_stateBlock);

	s_blendStateDict[m_desc] = this;
}

DX9_BlendState::~DX9_BlendState()
{
	s_blendStateDict.erase(m_desc);

	SAFE_RELEASE(m_stateBlock);
}

DX9_BlendState * DX9_BlendState::find(const BlendDesc &desc)
{
	DX9_BlendStateDict::const_iterator it = s_blendStateDict.find(desc);
	if (it != s_blendStateDict.end())
		return it->second;

	return new DX9_BlendState(desc);
}

AX_END_NAMESPACE
