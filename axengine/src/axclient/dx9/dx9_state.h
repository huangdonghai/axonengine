#ifndef AX_DX9_STATE_H
#define AX_DX9_STATE_H

AX_BEGIN_NAMESPACE

class DX9_SamplerState : public Unknown
{
public:
	enum {MAX_STAGES = 16 };
	DX9_SamplerState(int stage, const SamplerDesc &desc);
	virtual ~DX9_SamplerState();

	static DX9_SamplerState *find(int stage, const SamplerDesc &desc);

private:
	int m_stage;
	SamplerDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};

class DX9_DepthStencilState : public Unknown
{
public:
	DX9_DepthStencilState(const DepthStencilDesc &desc);
	virtual ~DX9_DepthStencilState();

	static DX9_DepthStencilState *find(const DepthStencilDesc &desc);

private:
	DepthStencilDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};

class DX9_RasterizerState : public Unknown
{
public:
	DX9_RasterizerState(const RasterizerDesc &desc);
	virtual ~DX9_RasterizerState();

	static DX9_RasterizerState *find(const RasterizerDesc &desc);

private:
	RasterizerDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};

class DX9_BlendState : public Unknown
{
public:
	DX9_BlendState(const BlendDesc &desc);
	virtual ~DX9_BlendState();

	static DX9_BlendState *find(const BlendDesc &desc);

private:
	BlendDesc m_desc;
	IDirect3DStateBlock9 *m_stateBlock;
};


AX_END_NAMESPACE

#endif // AX_DX9_STATE_H
