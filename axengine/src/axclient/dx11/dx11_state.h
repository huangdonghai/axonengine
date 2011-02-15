#ifndef AX_DX11_STATE_H
#define AX_DX11_STATE_H

AX_BEGIN_NAMESPACE

class DX11_StateManager
{
public:
private:
	DepthStencilDesc m_depthStencilDesc;
	RasterizerDesc m_rasterizerDesc;
	BlendDesc m_blendDesc;

	Dict<DepthStencilDesc, ID3D11DepthStencilState *> m_depthStencilStateDict;
	Dict<RasterizerDesc, ID3D11RasterizerState *> m_rasterizerStateDict;
	Dict<BlendDesc, ID3D11BlendState *> m_blendStateDict;
};

AX_END_NAMESPACE

#endif // AX_DX11_STATE_H
