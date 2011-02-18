#include "dx11_private.h"

AX_BEGIN_NAMESPACE


DX11_StateManager::DX11_StateManager()
{
	TypeZeroArray(m_textures);
	TypeZeroArray(m_samplerDescs);
	m_depthStencilDesc.intValue = 0;
	m_rasterizerDesc.intValue = 0;
	m_blendDesc.intValue = 0;;

	m_vertexShader = 0;
	m_pixelShader = 0;
	m_inputLayout = 0;
}


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

static inline D3D11_FILL_MODE trFillMode(uint_t fillMode)
{
	switch (fillMode) {
	case RasterizerDesc::FillMode_Point: return D3D11_FILL_WIREFRAME;
	case RasterizerDesc::FillMode_Wireframe: return D3D11_FILL_WIREFRAME;
	case RasterizerDesc::FillMode_Solid: return D3D11_FILL_SOLID;
	default: AX_WRONGPLACE; return D3D11_FILL_SOLID;
	}
}

static inline D3D11_CULL_MODE trCullMode(bool frontCounterClockwise, uint_t mode)
{
	if (frontCounterClockwise) {
		switch (mode) {
			case RasterizerDesc::CullMode_Front: return D3D11_CULL_BACK;
			case RasterizerDesc::CullMode_Back: return D3D11_CULL_FRONT;
			case RasterizerDesc::CullMode_None: return D3D11_CULL_NONE;
			default: AX_WRONGPLACE; return D3D11_CULL_NONE;
		}
	} else {
		switch (mode) {
			case RasterizerDesc::CullMode_Front: return D3D11_CULL_FRONT;
			case RasterizerDesc::CullMode_Back: return D3D11_CULL_BACK;
			case RasterizerDesc::CullMode_None: return D3D11_CULL_NONE;
			default: AX_WRONGPLACE; return D3D11_CULL_NONE;
		}
	}
}

ID3D11RasterizerState * DX11_StateManager::findRasterizerState(const RasterizerDesc &desc)
{
	Dict<RasterizerDesc, ID3D11RasterizerState *>::const_iterator it = m_rasterizerStateDict.find(desc);
	if (it != m_rasterizerStateDict.end())
		return it->second;

	CD3D11_RASTERIZER_DESC d3ddesc;

	d3ddesc.FillMode = trFillMode(desc.fillMode);
	d3ddesc.CullMode = trCullMode(desc.frontCounterClockwise, desc.cullMode);
	d3ddesc.FrontCounterClockwise = false;
	
	if (desc.depthBias) {
		float factor = gl_shadowOffsetFactor.getFloat();
		float units = gl_shadowOffsetUnits.getFloat() / 0x10000;

		d3ddesc.DepthBias = factor;
		d3ddesc.SlopeScaledDepthBias = units;
	}

	d3ddesc.ScissorEnable = desc.scissorEnable;

	ID3D11RasterizerState *state = 0;
	V(dx11_device->CreateRasterizerState(&d3ddesc, &state));
	m_rasterizerStateDict[desc] = state;
	return state;
}

static inline D3D11_BLEND trBlendFactor(uint_t factor)
{
	switch (factor) {
	case BlendDesc::BlendFactor_Zero: return D3D11_BLEND_ZERO;
	case BlendDesc::BlendFactor_One: return D3D11_BLEND_ONE;
	case BlendDesc::BlendFactor_SrcColor: return D3D11_BLEND_SRC_COLOR;
	case BlendDesc::BlendFactor_OneMinusSrcColor: return D3D11_BLEND_INV_SRC_COLOR;
	case BlendDesc::BlendFactor_DstColor: return D3D11_BLEND_DEST_COLOR;
	case BlendDesc::BlendFactor_OneMinusDstColor: return D3D11_BLEND_INV_DEST_COLOR;
	case BlendDesc::BlendFactor_SrcAlpha: return D3D11_BLEND_SRC_ALPHA;
	case BlendDesc::BlendFactor_OneMinusSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
	case BlendDesc::BlendFactor_SrcAlphaSaturate: return D3D11_BLEND_SRC_ALPHA_SAT;
	default: AX_WRONGPLACE; return D3D11_BLEND_ZERO;
	}
}

static inline D3D11_BLEND_OP trBlendOp(uint_t op)
{
	switch (op) {
	case BlendDesc::BlendOp_Add: return D3D11_BLEND_OP_ADD;
	case BlendDesc::BlendOp_Subtract: return D3D11_BLEND_OP_SUBTRACT;
	case BlendDesc::BlendOp_RevSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
	case BlendDesc::BlendOp_Min: return D3D11_BLEND_OP_MIN;
	case BlendDesc::BlendOp_Max: return D3D11_BLEND_OP_MAX;
	default: AX_WRONGPLACE; return D3D11_BLEND_OP_ADD;
	}
}

ID3D11BlendState * DX11_StateManager::findBlendState(const BlendDesc &desc)
{
	Dict<BlendDesc, ID3D11BlendState *>::const_iterator it = m_blendStateDict.find(desc);
	if (it != m_blendStateDict.end())
		return it->second;

	CD3D11_BLEND_DESC d3ddesc;

	d3ddesc.AlphaToCoverageEnable = false;
	d3ddesc.IndependentBlendEnable = false;

	d3ddesc.RenderTarget[0].BlendEnable = desc.blendEnable;
	d3ddesc.RenderTarget[0].SrcBlend = trBlendFactor(desc.srcBlend);
	d3ddesc.RenderTarget[0].DestBlend = trBlendFactor(desc.destBlend);
	d3ddesc.RenderTarget[0].BlendOp = trBlendOp(desc.blendOp);
	if (desc.separateAlphaBlendEnable) {
		d3ddesc.RenderTarget[0].SrcBlendAlpha = trBlendFactor(desc.srcBlendAlpha);
		d3ddesc.RenderTarget[0].DestBlendAlpha = trBlendFactor(desc.destBlendAlpha);
		d3ddesc.RenderTarget[0].BlendOpAlpha = trBlendOp(desc.blendOpAlpha);
	} else {
		d3ddesc.RenderTarget[0].SrcBlendAlpha = d3ddesc.RenderTarget[0].SrcBlend;
		d3ddesc.RenderTarget[0].DestBlendAlpha = d3ddesc.RenderTarget[0].DestBlend;
		d3ddesc.RenderTarget[0].BlendOpAlpha = d3ddesc.RenderTarget[0].BlendOp;
	}
	d3ddesc.RenderTarget[0].RenderTargetWriteMask = desc.renderTargetWriteMask;

	for (UINT i = 1; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		d3ddesc.RenderTarget[i] = d3ddesc.RenderTarget[0];

	ID3D11BlendState *state = 0;
	V(dx11_device->CreateBlendState(&d3ddesc, &state));
	m_blendStateDict[desc] = state;
	return state;
}

namespace {
	const D3D11_INPUT_ELEMENT_DESC s_ilSkin[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  2, DXGI_FORMAT_R32G32B32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  3, DXGI_FORMAT_R32G32B32_FLOAT,    0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilMesh[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilDebug[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilBlend[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilChunk[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilSkinInstanced[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  2, DXGI_FORMAT_R32G32B32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  3, DXGI_FORMAT_R32G32B32_FLOAT,    0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD",  4, DXGI_FORMAT_R32G32_FLOAT,       1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  6, DXGI_FORMAT_R32G32B32_FLOAT,    1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  7, DXGI_FORMAT_R32G32B32_FLOAT,    1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilMeshInstanced[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD",  4, DXGI_FORMAT_R32G32_FLOAT,       1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  6, DXGI_FORMAT_R32G32B32_FLOAT,    1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  7, DXGI_FORMAT_R32G32B32_FLOAT,    1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilDebugInstanced[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD",  4, DXGI_FORMAT_R32G32_FLOAT,       1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  6, DXGI_FORMAT_R32G32B32_FLOAT,    1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  7, DXGI_FORMAT_R32G32B32_FLOAT,    1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilBlendInstanced[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD",  4, DXGI_FORMAT_R32G32_FLOAT,       1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  6, DXGI_FORMAT_R32G32B32_FLOAT,    1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  7, DXGI_FORMAT_R32G32B32_FLOAT,    1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilChunkInstanced[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},

		{"TEXCOORD",  4, DXGI_FORMAT_R32G32_FLOAT,       1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  6, DXGI_FORMAT_R32G32B32_FLOAT,    1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  7, DXGI_FORMAT_R32G32B32_FLOAT,    1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilInstance[] =
	{
		{"TEXCOORD",  4, DXGI_FORMAT_R32G32_FLOAT,       1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  6, DXGI_FORMAT_R32G32B32_FLOAT,    1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  7, DXGI_FORMAT_R32G32B32_FLOAT,    1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	const D3D11_INPUT_ELEMENT_DESC *s_ilDesc[] = {
		s_ilSkin, s_ilMesh, s_ilDebug, s_ilBlend, s_ilChunk
	};

	const int s_ilDescNumElements[] = {
		ArraySize(s_ilSkin), ArraySize(s_ilMesh), ArraySize(s_ilDebug), ArraySize(s_ilBlend), ArraySize(s_ilChunk)
	};

	const D3D11_INPUT_ELEMENT_DESC *s_ilDescInstanced[] = {
		s_ilSkinInstanced, s_ilMeshInstanced, s_ilDebugInstanced, s_ilBlendInstanced, s_ilChunkInstanced
	};

	const int s_ilDescInstancedNumElements[] = {
		ArraySize(s_ilSkinInstanced), ArraySize(s_ilMeshInstanced), ArraySize(s_ilDebugInstanced), ArraySize(s_ilBlendInstanced), ArraySize(s_ilChunkInstanced)
	};

}

ID3D11InputLayout *DX11_StateManager::findInputLayout(VertexType vt, bool isInstanced, const void *bytecode, int bytecodeLength)
{
	InputLayoutKey key;
	key.vt = vt;
	key.isInstanced = isInstanced;
	key.bytecode.resize(bytecodeLength);
	memcpy(&key.bytecode[0], bytecode, bytecodeLength);
	Dict<InputLayoutKey, ID3D11InputLayout *>::const_iterator it = m_inputLayoutDict.find(key);

	if (it != m_inputLayoutDict.end())
		return it->second;

	ID3D11InputLayout *il = 0;
	if (!isInstanced) {
		dx11_device->CreateInputLayout(s_ilDesc[vt], s_ilDescNumElements[vt], bytecode, bytecodeLength, &il);
	} else {
		dx11_device->CreateInputLayout(s_ilDescInstanced[vt], s_ilDescInstancedNumElements[vt], bytecode, bytecodeLength, &il);
	}

	m_inputLayoutDict[key] = il;
	return il;
}


AX_END_NAMESPACE
