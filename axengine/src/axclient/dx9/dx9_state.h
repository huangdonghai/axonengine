#ifndef AX_DX9_STATE_H
#define AX_DX9_STATE_H

AX_BEGIN_NAMESPACE

class DX9_SamplerState : public IUnknown
{
public:
	enum { MAX_STAGES = 8 };

	DX9_SamplerState(const SamplerStateDesc &desc);
	virtual ~DX9_SamplerState();

	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv)
	{
		if (iid == IID_IUnknown) {
			*ppv = this;
		} else {
			*ppv = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return S_OK;
	}

	STDMETHOD_(ULONG, AddRef)(THIS)
	{
		return(ULONG)InterlockedIncrement(&m_ref);
	}

	STDMETHOD_(ULONG, Release)(THIS)
	{
		if (0L == InterlockedDecrement(&m_ref)) {
			delete this;
			return 0L;
		}

		return m_ref;
	}

private:
	LONG m_ref;
	IDirect3DStateBlock9 *m_stateBlocks[MAX_STAGES];
};

AX_END_NAMESPACE

#endif // AX_DX9_STATE_H
