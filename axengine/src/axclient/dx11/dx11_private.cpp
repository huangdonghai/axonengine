#include "dx11_private.h"

AX_DX11_BEGIN_NAMESPACE

AX_BEGIN_CLASS_MAP(AxDX11)
	AX_CLASS_ENTRY("Driver", DX11_Driver)
AX_END_CLASS_MAP()

DX11_Window *dx11_internalWindow;
DX11_Driver *dx11_driver;
ID3D11Device *dx11_device;
ID3D11DeviceContext *dx11_context;
IDXGIFactory *dxgi_factory;

DX11_ShaderManager *dx11_shaderManager;
DX11_StateManager *dx11_stateManager;
ID3D11Buffer *dx11_constBuffers[ConstBuffer::MaxType];
ID3D11Buffer *dx11_primConstBuffers[PRIMITIVECONST_COUNT]; // for different size primitive const buffers

AX_DX11_END_NAMESPACE
