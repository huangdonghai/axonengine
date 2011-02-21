#include "dx11_private.h"

AX_DX11_BEGIN_NAMESPACE

AX_BEGIN_CLASS_MAP(AxDX11)
	AX_CLASS_ENTRY("Driver", DX11_Driver)
AX_END_CLASS_MAP()

DX11_Window *dx11_internalWindow;
DX11_Driver *g_driver;
ID3D11Device *g_device;
ID3D11DeviceContext *g_context;
IDXGIFactory *g_dxgiFactory;

DX11_ShaderManager *g_shaderManager;
DX11_StateManager *g_stateManager;
ID3D11Buffer *g_d3dConstBuffers[ConstBuffer::MaxType+1]; // +1 for primitive const

AX_DX11_END_NAMESPACE
