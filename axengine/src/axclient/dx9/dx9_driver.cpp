/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "dx9_private.h"


AX_BEGIN_NAMESPACE

AX_IMPLEMENT_FACTORY(DX9_Driver)

// console command
AX_BEGIN_COMMAND_MAP(DX9_Driver)
AX_END_COMMAND_MAP()

DX9_Driver::DX9_Driver()
{
	m_initialized = false;
}

DX9_Driver::~DX9_Driver()
{}

void DX9_Driver::initialize()
{
	if (m_initialized)
		return;

	g_renderDriver = this;
	dx9_driver = this;

	dx9AssignRenderApi();

	Printf("..Initializing D3D9Driver...\n");

	dx9_internalWindow = new DX9_Window();
	g_renderDriverInfo.driverType = RenderDriverInfo::D3D;
	g_renderDriverInfo.vendor = "unknown";
	g_renderDriverInfo.renderer = "unknown";
	g_renderDriverInfo.version = "unknown";
	g_renderDriverInfo.extension = "unknown";

	Printf("...Calling Direct3DCreate9(D3D_SDK_VERSION = %d)...", D3D_SDK_VERSION);
	dx9_api = Direct3DCreate9(D3D_SDK_VERSION);

	if (!dx9_api) {
		Errorf("D3D9Driver::initialize: Direct3DCreate9 error\nMaybe you should install DirectX 9 or later version runtime");
		return;
	}

	D3DCAPS9 caps;

	HRESULT hr = dx9_api->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	if (FAILED(hr)) {
		Errorf("D3D9Driver::initialize: GetDeviceCaps failed %s", D3DErrorString(hr));
	}

	g_renderDriverInfo.maxTextureUnits = caps.MaxTextureBlendStages;
	g_renderDriverInfo.maxTextureSize = std::min(caps.MaxTextureWidth, caps.MaxTextureHeight);
	g_renderDriverInfo.max3DTextureSize = caps.MaxVolumeExtent;
	g_renderDriverInfo.maxCubeMapTextureSize = g_renderDriverInfo.maxTextureSize;

	g_renderDriverInfo.transposeMatrix = true;

	// Get the current desktop format
	D3DDISPLAYMODE dispmode;
	hr = dx9_api->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispmode);
	if (FAILED(hr)) {
		Errorf("GetAdapterDisplayMode failed %s", D3DErrorString(hr));
	}

	// create d3d9 device
	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	d3dpp.BackBufferWidth = 0;
	d3dpp.BackBufferHeight = 0;
	d3dpp.BackBufferFormat = dispmode.Format;
	d3dpp.BackBufferCount = 1;
	d3dpp.EnableAutoDepthStencil= FALSE;
	d3dpp.AutoDepthStencilFormat= D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = (HWND)dx9_internalWindow->getHandle();
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	DWORD BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;

	hr = dx9_api->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,(HWND)dx9_internalWindow->getHandle(), BehaviorFlags, &d3dpp, &dx9_device);
	if (FAILED(hr)) {
		Errorf("CreateDevice failed %s", D3DErrorString(hr));
	}

	checkFormats();

#if 0
	d3d9StateManager = new D3D9StateManager();
	d3d9StateManager->DirtyCachedValues();

	V(d3d9StateManager->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL));

	d3d9QueryManager = new D3D9querymanager();
	g_queryManager = d3d9QueryManager;

	d3d9VertexBufferManager = new D3D9VertexBufferManager();

	d3d9PrimitiveManager = new D3D9primitivemanager();
	g_primitiveManager = d3d9PrimitiveManager;
	d3d9TargetManager = new D3D9TargetManager();
	g_targetManager = d3d9TargetManager;
#endif
	g_shaderMacro.setMacro(ShaderMacro::G_D3D);
	dx9_shaderManager = new DX9_ShaderManager;
#if 0
	g_shaderManager = d3d9ShaderManager;

	d3d9TextureManager = new D3D9texturemanager();
	g_textureManager = d3d9TextureManager;
//#else
	D3D9Texture::initManager();
	d3d9Thread = new D3D9Thread();
	if (r_multiThread.getInteger()) {
		d3d9Thread->startThread();
	}

	d3d9Draw = new D3D9Draw();
#endif

	dx9_internalWindow->bind();

	Printf("ok\n");
}

void DX9_Driver::finalize()
{}

#if 0
void D3D9Driver::postInit()
{
	d3d9Postprocess = new D3D9Postprocess();
}
#endif

#if 0
bool D3D9Driver::isHDRRendering()
{ return false; }

RenderTarget *D3D9Driver::createWindowTarget(Handle wndId, const String &name)
{
	DX9_Window *state = new DX9_Window(wndId, name);
	AX_ASSERT(state);
	return state;
}

const RenderDriverInfo *DX9_Driver::getDriverInfo()
{
	return dx9_driverInfo;
}
#endif

#if 0
uint_t D3D9Driver::getBackendCaps() {
	return 0;
}
void D3D9driver::preFrame() {
	d3d9Thread->preFrame();
}

void D3D9driver::beginSelect(const RenderCamera &view) {}

void D3D9driver::loadSelectId(int id) {}

void D3D9driver::testActor(Actor *re) {}

void D3D9driver::testPrimitive(Primitive *prim) {}

SelectRecordSeq D3D9driver::endSelect() {
	return SelectRecordSeq();
}
#endif
void DX9_Driver::reset(int width, int height, bool fullscreen)
{
	onDeviceLost();
	onReset();
}

void DX9_Driver::onReset()
{
#if 0
	d3d9PrimitiveManager->onReset();
	d3d9TargetManager->onReset();
#endif
}

void DX9_Driver::onDeviceLost()
{
#if 0
	d3d9TargetManager->onDeviceLost();
	d3d9PrimitiveManager->onDeviceLost();
#endif
}

#if 0
bool D3D9Driver::isInRenderingThread()
{
	return d3d9Thread->isCurrentThread();
}

#endif

IDirect3DSurface9 * DX9_Driver::getDepthStencil(const Size &size)
{
	return 0;
}

const ShaderInfo * DX9_Driver::findShaderInfo( const FixedString &key )
{
	return dx9_shaderManager->findShaderInfo(key);
}


bool DX9_Driver::checkTextureFormatSupport(TexFormat format, D3DFORMAT d3dformat)
{
	HRESULT hr;
	if (format.isDepth() || format.isStencil()) {
		hr = dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			D3DFMT_X8R8G8B8,
			0,
			D3DRTYPE_TEXTURE,
			d3dformat);
	} else {
		hr = dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			D3DFMT_X8R8G8B8,
			0,
			D3DRTYPE_TEXTURE,
			d3dformat);
	}

	return SUCCEEDED(hr);
}

bool DX9_Driver::checkHardwareMipmapGenerationSupport(TexFormat format, D3DFORMAT d3dformat)
{
	if (dx9_api->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		D3DFMT_X8R8G8B8,
		D3DUSAGE_AUTOGENMIPMAP,
		D3DRTYPE_TEXTURE,
		d3dformat) == S_OK)
	{
		return true;
	} else {
		return false;
	}
}


bool DX9_Driver::checkRenderTargetFormatSupport(TexFormat format, D3DFORMAT d3dformat)
{
	HRESULT hr;
	if (format.isDepth() || format.isStencil()) {
		hr = dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			D3DFMT_X8R8G8B8,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_TEXTURE,
			d3dformat);
	} else {
		hr = dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			D3DFMT_X8R8G8B8,
			D3DUSAGE_RENDERTARGET,
			D3DRTYPE_TEXTURE,
			d3dformat);
	}

	return SUCCEEDED(hr);
}


void DX9_Driver::checkFormats()
{
	D3DFORMAT d3dformat;
	for (int i=0; i<TexFormat::MAX_NUMBER; i++) {
		trTexFormat(i, d3dformat);
		g_renderDriverInfo.textureFormatSupports[i] = checkTextureFormatSupport(i, d3dformat);
		g_renderDriverInfo.renderTargetFormatSupport[i] = checkRenderTargetFormatSupport(i, d3dformat);
		g_renderDriverInfo.autogenMipmapSupports[i] = checkHardwareMipmapGenerationSupport(i, d3dformat);
	}

	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_DepthStencil] = TexFormat::D24S8;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_DepthStencilTexture] = TexFormat::INTZ;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_SceneColor] = TexFormat::BGRA8;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_HdrLightBuffer] = TexFormat::RGBA16F;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_HdrSceneColor] = TexFormat::RGBA16F;
	g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_ShadowMap] = TexFormat::D24S8;
}

AX_END_NAMESPACE

