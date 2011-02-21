#ifndef AX_DX11_PRIVATE_H
#define AX_DX11_PRIVATE_H

#include "../private.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11effect.h>
#include <D3Dcompiler.h>

#define AX_DX11_BEGIN_NAMESPACE AX_BEGIN_NAMESPACE namespace DX11 {
#define AX_DX11_END_NAMESPACE } AX_END_NAMESPACE

#if defined(DEBUG) || defined(_DEBUG)
#define V(x) { HRESULT hr = (x); if (FAILED(hr)) { Errorf("%s(%d): %s=%s", __FILE__, __LINE__, DX11_ErrorString(hr), #x); } }
#else
#define V(x) { HRESULT hr = (x); }
#endif

#define SAFE_ADDREF(p) { if (p) p->AddRef(); }
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }

#include "dx11_shader.h"
#include "dx11_window.h"
#include "dx11_driver.h"
#include "dx11_state.h"
#include "dx11_api.h"

AX_DX11_BEGIN_NAMESPACE

inline const char *DX11_ErrorString(HRESULT hr)
{
#define D3DERR(x) case x: return #x;
	switch (hr) {
	D3DERR(D3D11_ERROR_FILE_NOT_FOUND)
	D3DERR(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
	D3DERR(D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS)
	D3DERR(D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD)
	D3DERR(D3DERR_INVALIDCALL)
	D3DERR(D3DERR_WASSTILLDRAWING)
	D3DERR(E_FAIL)
	D3DERR(E_INVALIDARG)
	D3DERR(E_OUTOFMEMORY)
	default: return "unknown error";
	}
#undef D3DERR
}

extern DX11_Driver *g_driver;
extern ID3D11Device *g_device;
extern ID3D11DeviceContext *g_context;
extern IDXGIFactory * g_dxgiFactory;

extern DX11_ShaderManager *g_shaderManager;
extern DX11_StateManager *g_stateManager;
extern ID3D11Buffer *g_d3dConstBuffers[ConstBuffer::MaxType+1]; // +1 for primitive const

AX_DX11_END_NAMESPACE

#endif // AX_DX11_PRIVATE_H