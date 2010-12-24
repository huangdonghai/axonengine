#ifndef AX_DX9_STATE_H
#define AX_DX9_STATE_H

AX_BEGIN_NAMESPACE

class DX9_SamplerState : public Unknown
{
public:
	enum { MAX_STAGES = 8 };

	DX9_SamplerState(const SamplerDesc &desc);
	virtual ~DX9_SamplerState();

private:
	LONG m_ref;
	IDirect3DStateBlock9 *m_stateBlocks[MAX_STAGES];
};

AX_END_NAMESPACE

#endif // AX_DX9_STATE_H
