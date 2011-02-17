#ifndef AX_DX11_PRIVATE_H
#define AX_DX11_PRIVATE_H

#include "../private.h"

#if defined(_DEBUG) || defined(DEBUG)
#define D3D_DEBUG_INFO
#endif

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11effect.h>

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

extern DX11_Window *dx11_internalWindow;
extern DX11_Driver *dx11_driver;
extern ID3D11Device *dx11_device;
extern ID3D11DeviceContext *dx11_context;

extern DX11_ShaderManager *dx11_shaderManager;
extern DX11_StateManager *dx11_stateManager;
extern ConstBuffers *dx11_constBuffers;

extern ID3D11InputLayout *dx11_inputLayouts[VertexType::kNumber];
extern ID3D11InputLayout *dx11_inputLayoutInstanced[VertexType::kNumber];

AX_END_NAMESPACE

#endif // AX_DX11_PRIVATE_H