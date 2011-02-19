#ifndef AX_DX11_STATE_H
#define AX_DX11_STATE_H

AX_DX11_BEGIN_NAMESPACE

extern ID3D11DeviceContext *g_context;

class DX11_StateManager
{
	enum ShaderDomain {
		VS, HS, DS, GS, PS, CS, MAX
	};
	enum { MAX_STAGES = 16 };

public:
	DX11_StateManager();

	void setTexture(int stage, ID3D11ShaderResourceView *texture)
	{
		if (m_textures[stage] == texture) return;
		g_context->PSSetShaderResources(stage, 1, &texture);
		m_textures[stage] = texture;
	}

	void setSamplerState(int stage, const SamplerDesc &desc)
	{
		if (m_samplerDescs[stage] == desc) {
			return;
		}
		ID3D11SamplerState *state = findSamplerState(desc);
		g_context->PSSetSamplers(stage, 1, &state);
		m_samplerDescs[stage] = desc;
	}

	void setDepthStencilState(const DepthStencilDesc &desc)
	{
		if (m_depthStencilDesc == desc)
			return;

		ID3D11DepthStencilState *state = findDepthStencilState(desc);
		g_context->OMSetDepthStencilState(state, 0);
		m_depthStencilDesc = desc;
	}

	void setRasterizerState(const RasterizerDesc &desc)
	{
		if (m_rasterizerDesc == desc)
			return;

		ID3D11RasterizerState *state = findRasterizerState(desc);
		g_context->RSSetState(state);
		m_rasterizerDesc = desc;
	}

	void setBlendState(const BlendDesc &desc)
	{
		if (m_blendDesc == desc)
			return;

		ID3D11BlendState *state = findBlendState(desc);
		FLOAT blendFactor[4] = { 0,0,0,0 };
		g_context->OMSetBlendState(state, blendFactor, 0xFFFFFFFF);
		m_blendDesc = desc;
	}

	void setVertexShader(ID3D11VertexShader *vs)
	{
		if (m_vertexShader == vs)
			return;

		g_context->VSSetShader(vs, 0, 0);
		m_vertexShader = vs;
	}

	void setPixelShader(ID3D11PixelShader *ps)
	{
		if (m_pixelShader == ps)
			return;
		g_context->PSSetShader(ps, 0, 0);
		m_pixelShader = ps;
	}

	void setInputLayout(ID3D11InputLayout *il)
	{
		if (m_inputLayout == il)
			return;

		g_context->IASetInputLayout(il);
		m_inputLayout = il;
	}

	ID3D11InputLayout *findInputLayout(VertexType vt, bool isInstanced, const void *bytecode, int bytecodeLength);

protected:
	ID3D11SamplerState *findSamplerState(const SamplerDesc &desc);
	ID3D11DepthStencilState *findDepthStencilState(const DepthStencilDesc &desc);
	ID3D11RasterizerState *findRasterizerState(const RasterizerDesc &desc);
	ID3D11BlendState *findBlendState(const BlendDesc &desc);

private:
	ID3D11ShaderResourceView *m_textures[MAX_STAGES];
	SamplerDesc m_samplerDescs[MAX_STAGES];
	DepthStencilDesc m_depthStencilDesc;
	RasterizerDesc m_rasterizerDesc;
	BlendDesc m_blendDesc;

	ID3D11VertexShader *m_vertexShader;
	ID3D11PixelShader *m_pixelShader;
	ID3D11InputLayout *m_inputLayout;

	Dict<SamplerDesc, ID3D11SamplerState *> m_samplerStateDict;
	Dict<DepthStencilDesc, ID3D11DepthStencilState *> m_depthStencilStateDict;
	Dict<RasterizerDesc, ID3D11RasterizerState *> m_rasterizerStateDict;
	Dict<BlendDesc, ID3D11BlendState *> m_blendStateDict;

	struct InputLayoutKey {
		VertexType vt;
		bool isInstanced;
		std::vector<byte_t> bytecode;

		operator size_t() const
		{
			size_t result = vt;
			hash_combine(result, isInstanced);
			for (std::vector<byte_t>::const_iterator it = bytecode.begin(); it != bytecode.end(); ++it) {
				hash_combine(result, *it);
			}
			return result;
		}
	};
	Dict<InputLayoutKey, ID3D11InputLayout *> m_inputLayoutDict;
};

AX_DX11_END_NAMESPACE

#endif // AX_DX11_STATE_H
