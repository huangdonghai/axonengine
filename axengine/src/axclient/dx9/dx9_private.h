/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_DX9_PRIVATE_H
#define AX_DX9_PRIVATE_H

#include "../private.h"

#if defined(_DEBUG) || defined(DEBUG)
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#if defined(DEBUG) || defined(_DEBUG)
#define V(x)           { HRESULT hr = (x); if (FAILED(hr)) { Errorf("%s(%d): %s", __FILE__, __LINE__, #x); } }
#define V_RETURN(x)    { hr = (x); if (FAILED(hr)) { Errorf("%s(%d): %s in %s", __FILE__, __LINE__, DX9_ErrorString(hr), #x); return hr; } }
#else
#define V(x)           { HRESULT hr = (x); }
#define V_RETURN(x)    { hr = (x); if (FAILED(hr)) { return hr; } }
#endif

#define SAFE_ADDREF(p) { if (p) p->AddRef(); }
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p)=NULL; } }

#include "dx9_shader.h"
#include "dx9_window.h"
#include "dx9_driver.h"
#include "dx9_state.h"
#include "dx9_api.h"

AX_BEGIN_NAMESPACE

inline const char *DX9_ErrorString(HRESULT hr)
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

extern DX9_Window *dx9_internalWindow;
extern DX9_Driver *dx9_driver;
extern IDirect3D9 *dx9_api;
extern IDirect3DDevice9 *dx9_device;

extern DX9_ShaderManager *dx9_shaderManager;
extern DX9_StateManager *dx9_stateManager;
extern ConstBuffers *dx9_constBuffers;

extern IDirect3DVertexDeclaration9 *dx9_vertexDeclarations[VertexType::MaxType];
extern IDirect3DVertexDeclaration9 *dx9_vertexDeclarationsInstanced[VertexType::MaxType];

AX_END_NAMESPACE

#endif // end guardian

