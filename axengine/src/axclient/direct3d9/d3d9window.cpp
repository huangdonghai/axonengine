/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

static D3DMULTISAMPLE_TYPE d3d9MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
static DWORD d3d9MultiSampleQuality = 0;

AX_BEGIN_NAMESPACE

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		switch (message) {
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}


	D3D9window::D3D9window(const String &name) {
		m_swapChain = 0;
		m_backbuffer = 0;
		m_swapChainWnd = 0;
		m_presentInterval = 0;

		WString ws = u2w(name);
		WNDCLASSEX wcex;	
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = 0;
		wcex.lpfnWndProc = (WNDPROC)WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)WHITE_BRUSH;
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = ws.c_str();
		wcex.hIconSm = 0;

		if (!::RegisterClassEx(&wcex)) {
			Errorf("GLdriver::CreateGLWindow: cann't create OpenGL window");
			return;
		}

		DWORD dwStyle,dwExStyle;
		dwExStyle = 0;		
		dwStyle = WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;

		m_wndId = ::CreateWindowEx(dwExStyle
			, ws.c_str()
			, ws.c_str()
			, dwStyle
			, 0, 0, 640,480
			, NULL, NULL
			, (HINSTANCE)GetModuleHandle(NULL)
			, NULL);

		m_gbuffer = 0;
		m_lightBuffer = 0;
	}

	D3D9window::D3D9window(handle_t wndId, const String &name) : m_name(name) {
		m_swapChain = 0;
		m_backbuffer = 0;
		m_swapChainWnd = 0;

		m_gbuffer = 0;
		m_lightBuffer = 0;

		m_wndId = (HWND)wndId;
	}

	D3D9window::~D3D9window() {
		SAFE_RELEASE(m_backbuffer);
		SAFE_RELEASE(m_swapChain);
	}

	// implement ITarget
	Rect D3D9window::getRect() {
		return Rect(0, 0, m_swapChainSize.x, m_swapChainSize.y);
	}

	void D3D9window::bind() {
		checkSwapChain();

		IDirect3DSurface9 *ds = d3d9TargetManager->getDepthStencil(m_swapChainSize.x, m_swapChainSize.y);

		HRESULT hr;
		V(d3d9Device->SetRenderTarget(0, m_backbuffer));
		V(d3d9StateManager->setDepthStencilSurface(ds, TexFormat::D24S8));

		// depth stencil
//		D3D9target *m_depthStencil = d3d9TargetManager->allocTargetDX(RenderTarget::PermanentAlloc, m_swapChainSize.x, m_swapChainSize.y, TexFormat::D24S8);
	}

	void D3D9window::unbind() {
	}

	void D3D9window::present()
	{
		HRESULT hr = m_swapChain->Present(0, 0, 0, 0, D3DPRESENT_DONOTWAIT);

		if (SUCCEEDED(hr)) {
			return;
		}

		if (hr == D3DERR_WASSTILLDRAWING) {
			return;
		}

		if (hr == D3DERR_DEVICELOST) {
			// TODO
			Errorf("D3D9 device lost. not even handle this yet");
			return;
		}

		if (hr == D3DERR_OUTOFVIDEOMEMORY || hr == E_OUTOFMEMORY) {
			Errorf("D3D9 out of memory.");
			return;
		}

		if (FAILED(hr)) {
			Errorf("%s(%d): %s", __FILE__, __LINE__, D3DErrorString(hr));
		}
	}

	void D3D9window::checkSwapChain()
	{
		RECT r;
		BOOL v = ::GetClientRect(m_wndId, &r);

		if (!v) {
			DWORD error = ::GetLastError();
			Errorf("error wndId");
		}

		DWORD flags = D3DPRESENT_INTERVAL_IMMEDIATE;

		if (r_vsync->getBool())
			flags = D3DPRESENT_INTERVAL_DEFAULT;


		if (m_swapChain && r.right == m_swapChainSize.x && r.bottom == m_swapChainSize.y && m_wndId == m_swapChainWnd && flags == m_presentInterval) {
			return;
		}

		SAFE_RELEASE(m_backbuffer);
		SAFE_RELEASE(m_swapChain);

		D3DPRESENT_PARAMETERS presentparams;
		memset(&presentparams, 0, sizeof(presentparams));
		presentparams.BackBufferWidth = r.right;
		presentparams.BackBufferHeight = r.bottom;
		presentparams.BackBufferFormat = D3DFMT_A8R8G8B8;
		presentparams.BackBufferCount = 1;
		presentparams.EnableAutoDepthStencil= FALSE;
		presentparams.AutoDepthStencilFormat= D3DFMT_D24S8;
		presentparams.Flags = 0;
		presentparams.Windowed = TRUE;
		presentparams.hDeviceWindow = m_wndId;
		presentparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentparams.PresentationInterval = flags;

		HRESULT hr;

		V(d3d9Device->CreateAdditionalSwapChain(&presentparams, &m_swapChain));
		V(m_swapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_backbuffer));

		m_swapChainSize.x = r.right;
		m_swapChainSize.y = r.bottom;
		m_swapChainWnd = m_wndId;
		m_presentInterval = flags;

		if (m_gbuffer) {
			d3d9TargetManager->freeTarget(m_gbuffer);
			m_gbuffer = 0;
		}

		if (m_lightBuffer) {
			d3d9TargetManager->freeTarget(m_lightBuffer);
			m_lightBuffer = 0;
		}

		m_gbuffer = d3d9TargetManager->allocTargetDX(RenderTarget::PermanentAlloc, r.right, r.bottom, TexFormat::RGBA16F);
		m_gbuffer->getTexture()->setFilterMode(Texture::FM_Nearest);
		m_gbuffer->getTexture()->setClampMode(Texture::CM_ClampToEdge);

		m_lightBuffer = d3d9TargetManager->allocTargetDX(RenderTarget::PermanentAlloc, r.right, r.bottom, TexFormat::BGRA8);
		m_lightBuffer->getTexture()->setFilterMode(Texture::FM_Nearest);
		m_lightBuffer->getTexture()->setClampMode(Texture::CM_ClampToEdge);
	}

AX_END_NAMESPACE

