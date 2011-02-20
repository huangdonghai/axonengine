#include "dx11_private.h"

AX_DX11_BEGIN_NAMESPACE

AX_IMPLEMENT_FACTORY(DX11_Driver)

// console command
AX_BEGIN_COMMAND_MAP(DX11_Driver)
AX_END_COMMAND_MAP()

static HMODULE s_hModDXGI = NULL;
static HMODULE s_hModD3D11 = NULL;
static HMODULE s_hModD3DX11 = NULL;
static LPCREATEDXGIFACTORY dx11_CreateDXGIFactory = NULL;
static LPD3D11CREATEDEVICE dx11_D3D11CreateDevice = NULL;
//static LPD3D11CREATEDEVICEANDSWAPCHAIN dx11_D3D11CreateDeviceAndSwapChain = NULL;
//LPD3DX11COMPILEFROMMEMORY dx11_D3DX11CompileFromMemory = NULL;

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
	}

//	s_hModD3DX11 = LoadLibrary(D3DX11_DLL);
//	if (s_hModD3DX11 != NULL) {
//		dx11_D3DX11CompileFromMemory = (LPD3DX11COMPILEFROMMEMORY)GetProcAddress(s_hModD3DX11, "D3DX11CompileFromMemory");
//	}

	if (!dx11_CreateDXGIFactory) {
		s_hModDXGI = LoadLibrary("dxgi.dll");
		if (s_hModDXGI) {
			dx11_CreateDXGIFactory = (LPCREATEDXGIFACTORY)GetProcAddress(s_hModDXGI, "CreateDXGIFactory1");
		}

		return (s_hModDXGI != NULL) && (s_hModD3D11 != NULL);
	}

	return (s_hModD3D11 != NULL);
#else
	dx11_D3D11CreateDevice = (LPD3D11CREATEDEVICE)D3D11CreateDevice;
	//dx11_D3D11CreateDeviceAndSwapChain = (LPD3D11CREATEDEVICEANDSWAPCHAIN)D3D11CreateDeviceAndSwapChain;
	//sFnPtr_D3DX11CreateEffectFromMemory  = (LPD3DX11CREATEEFFECTFROMMEMORY)D3DX11CreateEffectFromMemory;
//	dx11_D3DX11CompileFromMemory =  (LPD3DX11COMPILEFROMMEMORY)D3DX11CompileFromMemory;
//	dx11_CreateDXGIFactory = (LPCREATEDXGIFACTORY)CreateDXGIFactory;
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
	HRESULT hr = dx11_D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_SINGLETHREADED, 0, 0, D3D11_SDK_VERSION, &g_device, &featureLevel, &g_context);

	IDXGIDevice * pDXGIDevice;
	hr = g_device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&g_dxgiFactory);

	// initialized driver info
	g_renderDriverInfo.driverType = RenderDriverInfo::D3D11;
	g_renderDriverInfo.vendor = "unknown";
	g_renderDriverInfo.renderer = "unknown";
	g_renderDriverInfo.version = "unknown";
	g_renderDriverInfo.extension = "unknown";

	AX_ASSURE(featureLevel >= D3D_FEATURE_LEVEL_9_3);

	if (featureLevel == D3D_FEATURE_LEVEL_9_3) {
		g_renderDriverInfo.maxTextureSize = 4096;
		g_renderDriverInfo.max3DTextureSize = 256;
		g_renderDriverInfo.maxCubeMapTextureSize = 4096;
	} else if (featureLevel == D3D_FEATURE_LEVEL_10_0) {
		g_renderDriverInfo.maxTextureSize = 8192;
		g_renderDriverInfo.max3DTextureSize = 2048;
		g_renderDriverInfo.maxCubeMapTextureSize = 8192;
	} else if (featureLevel == D3D_FEATURE_LEVEL_10_1) {
		g_renderDriverInfo.maxTextureSize = 8192;
		g_renderDriverInfo.max3DTextureSize = 2048;
		g_renderDriverInfo.maxCubeMapTextureSize = 8192;
	} else if (featureLevel == D3D_FEATURE_LEVEL_11_0) {
		g_renderDriverInfo.maxTextureSize = 16384;
		g_renderDriverInfo.max3DTextureSize = 2048;
		g_renderDriverInfo.maxCubeMapTextureSize = 16384;
	} else {
		AX_WRONGPLACE;
	}

	g_renderDriverInfo.transposeMatrix = true;

	checkFormats();
}

void DX11_Driver::finalize()
{

}

const ShaderInfo * DX11_Driver::findShaderInfo(const FixedString &key)
{
	return g_shaderManager->findShaderInfo(key);
}


DXGI_FORMAT DX11_Driver::trTexFormat(TexFormat texformat)
{
	DXGI_FORMAT d3dformat = DXGI_FORMAT_UNKNOWN;

	switch (texformat) {
	case TexFormat::AUTO: break;
	case TexFormat::NULLTARGET: break;
	case TexFormat::R5G6B5: d3dformat = DXGI_FORMAT_B5G6R5_UNORM; break;
	case TexFormat::RGB10A2: d3dformat = DXGI_FORMAT_R10G10B10A2_UNORM; break;
	case TexFormat::RG16: d3dformat = DXGI_FORMAT_R16G16_UNORM; break;
	case TexFormat::L8: d3dformat = DXGI_FORMAT_R8_UNORM; break;
	case TexFormat::LA8: break;
	case TexFormat::A8: d3dformat = DXGI_FORMAT_A8_UNORM; break;
	case TexFormat::BGR8: break;
	case TexFormat::BGRA8: d3dformat = DXGI_FORMAT_B8G8R8A8_UNORM; break;
	case TexFormat::BGRX8: d3dformat = DXGI_FORMAT_B8G8R8X8_UNORM; break;
	case TexFormat::BC1: d3dformat = DXGI_FORMAT_BC1_UNORM; break;
	case TexFormat::BC2: d3dformat = DXGI_FORMAT_BC2_UNORM; break;
	case TexFormat::BC3: d3dformat = DXGI_FORMAT_BC3_UNORM; break;
	case TexFormat::BC4: d3dformat = DXGI_FORMAT_BC4_UNORM; break;
	case TexFormat::BC5: d3dformat = DXGI_FORMAT_BC5_UNORM; break;
	case TexFormat::BC6H: d3dformat = DXGI_FORMAT_BC6H_UF16; break;
	case TexFormat::BC7: d3dformat = DXGI_FORMAT_BC7_UNORM; break;
	case TexFormat::L16: d3dformat = DXGI_FORMAT_R16_UNORM; break;
	case TexFormat::R16F: d3dformat = DXGI_FORMAT_R16_FLOAT; break;
	case TexFormat::RG16F: d3dformat = DXGI_FORMAT_R16G16_FLOAT; break;
	case TexFormat::RGB16F: break;
	case TexFormat::RGBA16F: d3dformat = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
	case TexFormat::R32F: d3dformat = DXGI_FORMAT_R32_FLOAT; break;
	case TexFormat::RG32F: d3dformat = DXGI_FORMAT_R32G32_FLOAT; break;
	case TexFormat::RGB32F: d3dformat = DXGI_FORMAT_R32G32B32_FLOAT; break;
	case TexFormat::RGBA32F: d3dformat = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	case TexFormat::R11G11B10F: d3dformat = DXGI_FORMAT_R11G11B10_FLOAT; break;
	case TexFormat::D16: d3dformat = DXGI_FORMAT_D16_UNORM; break;
	case TexFormat::D24: d3dformat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
	case TexFormat::D32F: d3dformat = DXGI_FORMAT_D32_FLOAT; break;
	case TexFormat::D24S8: d3dformat = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
	case TexFormat::DF16: break;
	case TexFormat::DF24: break;
	case TexFormat::RAWZ: break;
	case TexFormat::INTZ: break;
	default: AX_WRONGPLACE;
	}

	return d3dformat;
}

void DX11_Driver::checkFormats()
{
	for (int i = 0; i < TexFormat::MAX_NUMBER; i++) {
		TexFormat tex_format = (TexFormat::Type)i;
		DXGI_FORMAT dxgi_format = trTexFormat(tex_format);

		if (dxgi_format == DXGI_FORMAT_UNKNOWN) {
			g_renderDriverInfo.textureFormatSupports[i] = false;
			g_renderDriverInfo.renderTargetFormatSupport[i] = false;
			g_renderDriverInfo.autogenMipmapSupports[i] = false;
			continue;
		}

		UINT format_support;
		HRESULT hr = g_device->CheckFormatSupport(dxgi_format, &format_support);
		if (FAILED(hr)) {
			g_renderDriverInfo.textureFormatSupports[i] = false;
			g_renderDriverInfo.renderTargetFormatSupport[i] = false;
			g_renderDriverInfo.autogenMipmapSupports[i] = false;
			continue;
		}

		g_renderDriverInfo.textureFormatSupports[i] = format_support & D3D11_FORMAT_SUPPORT_TEXTURE2D;
		if (tex_format.isDepth()) {
			g_renderDriverInfo.renderTargetFormatSupport[i] = format_support & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL;
		} else {
			g_renderDriverInfo.renderTargetFormatSupport[i] = format_support & D3D11_FORMAT_SUPPORT_RENDER_TARGET;
		}
		g_renderDriverInfo.autogenMipmapSupports[i] = format_support & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN;
	}

	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_DepthStencil] = TexFormat::D24S8;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_DepthStencilTexture] = TexFormat::D24S8;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_SceneColor] = TexFormat::BGRA8;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_HdrLightBuffer] = TexFormat::RGBA16F;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_HdrSceneColor] = TexFormat::RGBA16F;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_NormalBuffer] = TexFormat::RGB10A2;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_ShadowMap] = TexFormat::D24S8;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_CubeShadowMap] = TexFormat::D24S8;
}


AX_DX11_END_NAMESPACE
