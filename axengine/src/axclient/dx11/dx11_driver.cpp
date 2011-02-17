#include "dx11_private.h"

AX_BEGIN_NAMESPACE

AX_IMPLEMENT_FACTORY(DX11_Driver)

// console command
AX_BEGIN_COMMAND_MAP(DX11_Driver)
AX_END_COMMAND_MAP()

static HMODULE                              s_hModDXGI = NULL;
static HMODULE                              s_hModD3D11 = NULL;
static HMODULE                              s_hModD3DX11 = NULL;
static LPCREATEDXGIFACTORY                  dx11_CreateDXGIFactory = NULL;
static LPD3D11CREATEDEVICE                  dx11_D3D11CreateDevice = NULL;
static LPD3D11CREATEDEVICEANDSWAPCHAIN      dx11_D3D11CreateDeviceAndSwapChain = NULL;
LPD3DX11COMPILEFROMMEMORY            dx11_D3DX11CompileFromMemory = NULL;

// unload the D3D11 DLLs
static bool dynlinkUnloadD3D11API( void )
{
	if (s_hModDXGI) {
		FreeLibrary( s_hModDXGI );
		s_hModDXGI = NULL;
	}
	if (s_hModD3D11) {
		FreeLibrary( s_hModD3D11 );
		s_hModD3D11 = NULL;
	}
	if (s_hModD3DX11) {
		FreeLibrary( s_hModD3DX11 );
		s_hModD3DX11 = NULL;
	}
	return true;
}

// Dynamically load the D3D11 DLLs loaded and map the function pointers
static bool dynlinkLoadD3D11API( void )
{
	// If both modules are non-NULL, this function has already been called.  Note
	// that this doesn't guarantee that all ProcAddresses were found.
	if (s_hModD3D11 != NULL && s_hModD3DX11 != NULL && s_hModDXGI != NULL)
		return true;

#if 1
	// This may fail if Direct3D 11 isn't installed
	s_hModD3D11 = LoadLibrary("d3d11.dll");
	if (s_hModD3D11 != NULL) {
		dx11_D3D11CreateDevice = (LPD3D11CREATEDEVICE)GetProcAddress(s_hModD3D11, "D3D11CreateDevice");
		dx11_D3D11CreateDeviceAndSwapChain = (LPD3D11CREATEDEVICEANDSWAPCHAIN)GetProcAddress(s_hModD3D11, "D3D11CreateDeviceAndSwapChain");
	}

	s_hModD3DX11 = LoadLibrary("D3DX11d_42.dll");
	if (s_hModD3DX11 != NULL) {
		dx11_D3DX11CompileFromMemory = (LPD3DX11COMPILEFROMMEMORY)GetProcAddress(s_hModD3DX11, "D3DX11CompileFromMemory");
	}

	if (!dx11_CreateDXGIFactory) {
		s_hModDXGI = LoadLibrary("dxgi.dll");
		if (s_hModDXGI) {
			dx11_CreateDXGIFactory = (LPCREATEDXGIFACTORY)GetProcAddress(s_hModDXGI, "CreateDXGIFactory1");
		}

		return (s_hModDXGI != NULL) && (s_hModD3D11 != NULL);
	}

	return (s_hModD3D11 != NULL);
#else
	dx11_D3D11CreateDevice = (LPD3D11CREATEDEVICE)D3D11CreateDeviceAndSwapChain;
	dx11_D3D11CreateDeviceAndSwapChain = (LPD3D11CREATEDEVICEANDSWAPCHAIN)D3D11CreateDeviceAndSwapChain;
	//sFnPtr_D3DX11CreateEffectFromMemory  = (LPD3DX11CREATEEFFECTFROMMEMORY)D3DX11CreateEffectFromMemory;
	dx11_D3DX11CompileFromMemory =  (LPD3DX11COMPILEFROMMEMORY)D3DX11CompileFromMemory;
	dx11_CreateDXGIFactory = (LPCREATEDXGIFACTORY)CreateDXGIFactory;
	return true;
#endif
	return true;
}

DX11_Driver::DX11_Driver()
{

}

DX11_Driver::~DX11_Driver()
{

}

void DX11_Driver::initialize(SyncEvent &syncEvent)
{
	dynlinkLoadD3D11API();

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = dx11_D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, 0, &dx11_device, &featureLevel, &dx11_context);
}

void DX11_Driver::finalize()
{

}

const ShaderInfo * DX11_Driver::findShaderInfo( const FixedString &key )
{
	return 0;
}

namespace {
	const D3D11_INPUT_ELEMENT_DESC s_ilSkin[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  2, DXGI_FORMAT_R32G32B32_FLOAT,    0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  3, DXGI_FORMAT_R32G32B32_FLOAT,    0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilMesh[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilDebug[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilBlend[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",     0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilChunk[] =
	{
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const D3D11_INPUT_ELEMENT_DESC s_ilInstance[] =
	{
		{"TEXCOORD",  4, DXGI_FORMAT_R32G32_FLOAT,       1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  6, DXGI_FORMAT_R32G32B32_FLOAT,    1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TEXCOORD",  7, DXGI_FORMAT_R32G32B32_FLOAT,    1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

}

void DX11_Driver::createInputLayouts()
{

}

AX_END_NAMESPACE
