#include "dx11_private.h"

AX_BEGIN_NAMESPACE

ID3D11SamplerState * DX11_StateManager::findSamplerState(const SamplerDesc &desc)
{
	Dict<SamplerDesc, ID3D11SamplerState *>::const_iterator it = m_samplerStateDict.find(desc);
	if (it != m_samplerStateDict.end())
		return it->second;

	CD3D11_SAMPLER_DESC d3ddesc;
	TypeZero(&d3ddesc);

	switch (desc.filterMode) {
	case SamplerDesc::FilterMode_Nearest:
		d3ddesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case SamplerDesc::FilterMode_Linear:
		d3ddesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case SamplerDesc::FilterMode_LinearMipmap:
		d3ddesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case SamplerDesc::FilterMode_Trilinear:
		d3ddesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case SamplerDesc::FilterMode_Anisotropic:
		d3ddesc.Filter = D3D11_FILTER_ANISOTROPIC;
		break;
	default: AX_WRONGPLACE; break;
	}

	switch (desc.clampMode) {
	case SamplerDesc::ClampMode_Clamp:
		d3ddesc.AddressU = d3ddesc.AddressV = d3ddesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case SamplerDesc::ClampMode_Border:
		d3ddesc.AddressU = d3ddesc.AddressV = d3ddesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	case SamplerDesc::ClampMode_Repeat:
		d3ddesc.AddressU = d3ddesc.AddressV = d3ddesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	default: AX_WRONGPLACE; break;
	}

	d3ddesc.MaxAnisotropy = desc.maxAnisotropy;

	if (desc.borderColor == SamplerDesc::BorderColor_One) {
		d3ddesc.BorderColor[0] = d3ddesc.BorderColor[1] = d3ddesc.BorderColor[2] = d3ddesc.BorderColor[3] = 1;
	} else if (desc.borderColor == SamplerDesc::BorderColor_Zero) {
		d3ddesc.BorderColor[0] = d3ddesc.BorderColor[1] = d3ddesc.BorderColor[2] = d3ddesc.BorderColor[3] = 0;
	} else {
		AX_WRONGPLACE;
	}

	ID3D11SamplerState *state = 0;
	V(dx11_device->CreateSamplerState(&d3ddesc, &state));
	m_samplerStateDict[desc] = state;
	return state;
}

static inline D3D11_COMPARISON_FUNC trCompareFunc(uint_t func)
{
	switch (func) {
	case DepthStencilDesc::CompareFunc_Never: return D3D11_COMPARISON_NEVER;
	case DepthStencilDesc::CompareFunc_Less: return D3D11_COMPARISON_LESS;
	case DepthStencilDesc::CompareFunc_Equal: return D3D11_COMPARISON_EQUAL;
	case DepthStencilDesc::CompareFunc_LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
	case DepthStencilDesc::CompareFunc_Greater: return D3D11_COMPARISON_GREATER;
	case DepthStencilDesc::CompareFunc_NotEqual: return D3D11_COMPARISON_NOT_EQUAL;
	case DepthStencilDesc::CompareFunc_GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
	case DepthStencilDesc::CompareFunc_Always: return D3D11_COMPARISON_ALWAYS;
	default: AX_WRONGPLACE; return D3D11_COMPARISON_NEVER;
	}
}

static inline D3D11_STENCIL_OP trStencilOp(uint_t op)
{
	switch (op) {
	case DepthStencilDesc::StencilOp_Keep: return D3D11_STENCIL_OP_KEEP;
	case DepthStencilDesc::StencilOp_Zero: return D3D11_STENCIL_OP_ZERO;
	case DepthStencilDesc::StencilOp_Replace: return D3D11_STENCIL_OP_REPLACE;
	case DepthStencilDesc::StencilOp_IncrementSat: return D3D11_STENCIL_OP_INCR_SAT;
	case DepthStencilDesc::StencilOp_DecrementSat: return D3D11_STENCIL_OP_DECR_SAT;
	case DepthStencilDesc::StencilOp_Invert: return D3D11_STENCIL_OP_INVERT;
	case DepthStencilDesc::StencilOp_Increment: return D3D11_STENCIL_OP_INCR;
	case DepthStencilDesc::StencilOp_Decrement: return D3D11_STENCIL_OP_DECR;
	default: AX_WRONGPLACE; return D3D11_STENCIL_OP_KEEP;
	}
}

ID3D11DepthStencilState * DX11_StateManager::findDepthStencilState(const DepthStencilDesc &desc)
{
	Dict<DepthStencilDesc, ID3D11DepthStencilState *>::const_iterator it = m_depthStencilStateDict.find(desc);
	if (it != m_depthStencilStateDict.end())
		return it->second;

	CD3D11_DEPTH_STENCIL_DESC d3ddesc;

	d3ddesc.DepthEnable = desc.depthEnable;
	if (desc.depthWritable)
		d3ddesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	else
		d3ddesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	d3ddesc.DepthFunc = trCompareFunc(desc.depthFunc);
	d3ddesc.StencilEnable = desc.stencilEnable;

	if (desc.stencilSkyMaskRead)
		d3ddesc.StencilReadMask = 0x80;
	if (desc.stencilSkyMaskWrite)
		d3ddesc.StencilWriteMask = 0x80;

	d3ddesc.FrontFace.StencilFailOp = trStencilOp(desc.stencilFailOp);
	d3ddesc.FrontFace.StencilDepthFailOp = trStencilOp(desc.stencilDepthFailOp);
	d3ddesc.FrontFace.StencilPassOp = trStencilOp(desc.stencilPassOp);
	d3ddesc.FrontFace.StencilFunc = trCompareFunc(desc.stencilFunc);

	d3ddesc.BackFace.StencilFailOp = trStencilOp(desc.stencilFailOpBackface);
	d3ddesc.BackFace.StencilDepthFailOp = trStencilOp(desc.stencilDepthFailOpBackface);
	d3ddesc.BackFace.StencilPassOp = trStencilOp(desc.stencilPassOpBackface);
	d3ddesc.BackFace.StencilFunc = trCompareFunc(desc.stencilFuncBackface);

	ID3D11DepthStencilState *state = 0;
	V(dx11_device->CreateDepthStencilState(&d3ddesc, &state));
	m_depthStencilStateDict[desc] = state;
	return state;
}

ID3D11RasterizerState * DX11_StateManager::findRasterizerState(const RasterizerDesc &desc)
{
	Dict<RasterizerDesc, ID3D11RasterizerState *>::const_iterator it = m_rasterizerStateDict.find(desc);
	if (it != m_rasterizerStateDict.end())
		return it->second;

	CD3D11_RASTERIZER_DESC d3ddesc;

	ID3D11RasterizerState *state = 0;
	V(dx11_device->CreateRasterizerState(&d3ddesc, &state));
	m_rasterizerStateDict[desc] = state;
	return state;
}

ID3D11BlendState * DX11_StateManager::findBlendState(const BlendDesc &desc)
{
	Dict<BlendDesc, ID3D11BlendState *>::const_iterator it = m_blendStateDict.find(desc);
	if (it != m_blendStateDict.end())
		return it->second;

	CD3D11_BLEND_DESC d3ddesc;

	ID3D11BlendState *state = 0;
	V(dx11_device->CreateBlendState(&d3ddesc, &state));
	m_blendStateDict[desc] = state;
	return state;
}

AX_END_NAMESPACE
