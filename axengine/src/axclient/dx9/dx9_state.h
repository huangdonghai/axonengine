#ifndef AX_DX9_STATE_H
#define AX_DX9_STATE_H

AX_BEGIN_NAMESPACE

class DX9_SamplerState : public Unknown
{
	friend class DX9_StateManager;

public:
	enum { MAX_STAGES = 16 };
	DX9_SamplerState(int stage, const SamplerDesc &desc);
	virtual ~DX9_SamplerState();

	void apply() const { V(m_stateBlock->Apply()); }
	static DX9_SamplerState *find(int stage, const SamplerDesc &desc);

private:
	int m_stage;
	SamplerDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};

class DX9_DepthStencilState : public Unknown
{
	friend class DX9_StateManager;
public:
	DX9_DepthStencilState(const DepthStencilDesc &desc);
	virtual ~DX9_DepthStencilState();

	void apply() const { V(m_stateBlock->Apply()); }
	static DX9_DepthStencilState *find(const DepthStencilDesc &desc);

private:
	DepthStencilDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};

class DX9_RasterizerState : public Unknown
{
	friend class DX9_StateManager;
public:
	DX9_RasterizerState(const RasterizerDesc &desc);
	virtual ~DX9_RasterizerState();

	void apply() const { V(m_stateBlock->Apply()); }
	static DX9_RasterizerState *find(const RasterizerDesc &desc);

private:
	RasterizerDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};

class DX9_BlendState : public Unknown
{
	friend class DX9_StateManager;
public:
	DX9_BlendState(const BlendDesc &desc);
	virtual ~DX9_BlendState();

	void apply() const { V(m_stateBlock->Apply()); }
	static DX9_BlendState *find(const BlendDesc &desc);

private:
	BlendDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};

extern IDirect3DDevice9 *dx9_device;
class DX9_StateManager
{
public:
	DX9_StateManager()
	{
		TypeZero(this);
	}
	~DX9_StateManager();

	void setTexture(int stage, IDirect3DBaseTexture9 *texture)
	{
		if (m_textures[stage] == texture) return;
		V(dx9_device->SetTexture(stage, texture));
		m_textures[stage] = texture;
	}

	void setSamplerState(int stage, const SamplerDesc &desc)
	{
		if (m_samplerStates[stage]) {
			if (m_samplerStates[stage]->m_desc == desc)
				return;
		}
		DX9_SamplerState *state = DX9_SamplerState::find(stage, desc);
		state->apply();
		m_samplerStates[stage] = state;
	}

	void setDepthStencilState(const DepthStencilDesc &desc)
	{
		if (m_depthStencilState && m_depthStencilState->m_desc == desc)
			return;

		DX9_DepthStencilState *state = DX9_DepthStencilState::find(desc);
		state->apply();
		m_depthStencilState = state;
	}

	void setRasterizerState(const RasterizerDesc &desc)
	{
		if (m_rasterizerState && m_rasterizerState->m_desc == desc)
			return;

		DX9_RasterizerState *state = DX9_RasterizerState::find(desc);
		state->apply();
		m_rasterizerState = state;
	}

	void setBlendState(const BlendDesc &desc)
	{
		if (m_blendState && m_blendState->m_desc == desc)
			return;

		DX9_BlendState *state = DX9_BlendState::find(desc);
		state->apply();
		m_blendState = state;
	}

	void setVertexShader(IDirect3DVertexShader9 *vs)
	{
		if (m_vertexShader == vs)
			return;
		V(dx9_device->SetVertexShader(vs));
		m_vertexShader = vs;
	}

	void setPixelShader(IDirect3DPixelShader9 *ps)
	{
		if (m_pixelShader == ps)
			return;
		V(dx9_device->SetPixelShader(ps));
		m_pixelShader = ps;
	}

	void setVertexDeclaration(IDirect3DVertexDeclaration9 *vd)
	{
		if (m_vertexDeclaration == vd)
			return;

		V(dx9_device->SetVertexDeclaration(vd));
		m_vertexDeclaration = vd;
	}

private:
	IDirect3DBaseTexture9 *m_textures[DX9_SamplerState::MAX_STAGES];
	DX9_SamplerState *m_samplerStates[DX9_SamplerState::MAX_STAGES];
	DX9_DepthStencilState *m_depthStencilState;
	DX9_RasterizerState *m_rasterizerState;
	DX9_BlendState *m_blendState;

	IDirect3DVertexShader9 *m_vertexShader;
	IDirect3DPixelShader9 *m_pixelShader;

	IDirect3DVertexDeclaration9 *m_vertexDeclaration;
};

AX_END_NAMESPACE

#endif // AX_DX9_STATE_H
