#ifndef AX_DX11_PRIVATE_H
#define AX_DX11_PRIVATE_H

#include "../private.h"

#if defined(_DEBUG) || defined(DEBUG)
#define D3D_DEBUG_INFO
#endif

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#if defined(DEBUG) || defined(_DEBUG)
#define V(x)           { HRESULT hr = (x); if (FAILED(hr)) { Errorf("%s(%d): %s", __FILE__, __LINE__, #x); } }
#define V_RETURN(x)    { hr = (x); if (FAILED(hr)) { Errorf("%s(%d): %s in %s", __FILE__, __LINE__, D3DErrorString(hr), #x); return hr; } }
#else
#define V(x)           { hr = (x); }
#define V_RETURN(x)    { hr = (x); if (FAILED(hr)) { return hr; } }
#endif

#define SAFE_ADDREF(p) { if (p) p->AddRef(); }
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }

#include "dx11_shader.h"
#include "dx11_window.h"
#include "dx11_driver.h"
#include "dx11_state.h"
#include "dx11_api.h"

AX_BEGIN_NAMESPACE

inline const char *DX11_ErrorString(HRESULT hr)
{
#define D3DERR(x) case x: return #x;
	switch (hr) {
		D3DERR(D3D_OK)
			D3DERR(D3DERR_WRONGTEXTUREFORMAT)
			D3DERR(D3DERR_UNSUPPORTEDCOLOROPERATION)
			D3DERR(D3DERR_UNSUPPORTEDCOLORARG)
			D3DERR(D3DERR_UNSUPPORTEDALPHAOPERATION)
			D3DERR(D3DERR_UNSUPPORTEDALPHAARG)
			D3DERR(D3DERR_TOOMANYOPERATIONS)
			D3DERR(D3DERR_CONFLICTINGTEXTUREFILTER)
			D3DERR(D3DERR_UNSUPPORTEDFACTORVALUE)
			D3DERR(D3DERR_CONFLICTINGRENDERSTATE)
			D3DERR(D3DERR_UNSUPPORTEDTEXTUREFILTER)
			D3DERR(D3DERR_CONFLICTINGTEXTUREPALETTE)
			D3DERR(D3DERR_DRIVERINTERNALERROR)
			D3DERR(D3DERR_NOTFOUND)
			D3DERR(D3DERR_MOREDATA)
			D3DERR(D3DERR_DEVICELOST)
			D3DERR(D3DERR_DEVICENOTRESET)
			D3DERR(D3DERR_NOTAVAILABLE)
			D3DERR(D3DERR_OUTOFVIDEOMEMORY)
			D3DERR(D3DERR_INVALIDDEVICE)
			D3DERR(D3DERR_INVALIDCALL)
			D3DERR(D3DERR_DRIVERINVALIDCALL)
			D3DERR(D3DERR_WASSTILLDRAWING)
	default: return "unknown error";
	}
#undef D3DERR
}

extern DX11_Window *dx11_internalWindow;
extern DX11_Driver *dx11_driver;
extern ID3D11Device *dx11_device;
extern ID3D11DeviceContext *dx11_context;

extern DX11_ShaderManager *dx11_shaderManager;
extern DX11_StateManager *dx11_stateManager;
extern ConstBuffers *dx11_constBuffers;

extern ID3D11InputLayout *dx11_inputLayouts[VertexType::kNumber];
extern ID3D11InputLayout *dx11_inputLayoutInstanced[VertexType::kNumber];

#endif // AX_DX11_PRIVATE_H