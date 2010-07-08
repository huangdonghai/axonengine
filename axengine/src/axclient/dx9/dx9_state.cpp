#include "dx9_private.h"

AX_BEGIN_NAMESPACE

DX9_SamplerState::DX9_SamplerState( const SamplerStateDesc &desc )
{
	for (int i=0; i<MAX_STAGES; i++) {
		dx9_device->BeginStateBlock();

		switch (desc.clampMode) {
		case SamplerStateDesc::CM_Clamp:
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
			break;
		case SamplerStateDesc::CM_Border:
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
			dx9_device->SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0);
			break;
		case SamplerStateDesc::CM_Repeat:
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			dx9_device->SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		}

		switch (desc.filterMode) {
		case SamplerStateDesc::FM_Nearest:
			dx9_device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			dx9_device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			dx9_device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			break;
		case SamplerStateDesc::FM_Linear:
			dx9_device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			dx9_device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			dx9_device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			break;
		case SamplerStateDesc::FM_LinearMipmap:
			dx9_device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			dx9_device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			dx9_device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			break;
		case SamplerStateDesc::FM_Trilinear:
			dx9_device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			dx9_device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			dx9_device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			break;
		}

		dx9_device->EndStateBlock(&m_stateBlocks[i]);
	}
}

DX9_SamplerState::~DX9_SamplerState()
{
	for (int i=0; i<MAX_STAGES; i++) {
		SAFE_RELEASE(m_stateBlocks[i]);
	}
}

AX_END_NAMESPACE
