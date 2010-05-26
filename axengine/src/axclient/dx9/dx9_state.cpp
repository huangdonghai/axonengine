#include "dx9_private.h"

AX_BEGIN_NAMESPACE

DX9_SamplerState::DX9_SamplerState( const SamplerStateDesc &desc )
{
	for (int i=0; i<MAX_STAGES; i++) {
		d3d9Device->BeginStateBlock();

		switch (desc.clampMode) {
		case SamplerStateDesc::CM_Clamp:
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
			break;
		case SamplerStateDesc::CM_Border:
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
			d3d9Device->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0);
			break;
		case SamplerStateDesc::CM_Repeat:
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			d3d9Device->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		}

		switch (desc.filterMode) {
		case SamplerStateDesc::FM_Nearest:
			d3d9Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			d3d9Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			d3d9Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			break;
		case SamplerStateDesc::FM_Linear:
			d3d9Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			d3d9Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			d3d9Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			break;
		case SamplerStateDesc::FM_LinearMipmap:
			d3d9Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			d3d9Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			d3d9Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			break;
		case SamplerStateDesc::FM_Trilinear:
			d3d9Device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			d3d9Device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			d3d9Device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			break;
		}

		d3d9Device->EndStateBlock(&m_stateBlocks[i]);
	}
}

DX9_SamplerState::~DX9_SamplerState()
{
	for (int i=0; i<MAX_STAGES; i++) {
		SAFE_RELEASE(m_stateBlocks[i]);
	}
}

AX_END_NAMESPACE
