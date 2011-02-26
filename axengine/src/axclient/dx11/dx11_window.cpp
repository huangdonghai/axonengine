#include "dx11_private.h"

AX_DX11_BEGIN_NAMESPACE

DX11_Window::DX11_Window(Handle wndId, int width, int height)
{
	m_swapChain = 0;
	m_backbuffer = 0;
	m_renderTargetView = 0;
	m_swapChainWnd = 0;
	m_swapChainSize.set(-1,-1);

	m_wndId = (HWND)wndId.toVoidStar();
	m_updatedSize.set(width,height);
}

DX11_Window::~DX11_Window()
{
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_backbuffer);
	SAFE_RELEASE(m_swapChain);
}

void DX11_Window::update(Handle newId, int width, int height)
{
	m_wndId = handle_cast<HWND>(newId);
	m_updatedSize = Size(width, height);
	checkSwapChain();
}

void DX11_Window::present()
{
	HRESULT hr = m_swapChain->Present(0, 0);

	if (SUCCEEDED(hr))
		return;

	AX_WRONGPLACE;
}

void DX11_Window::checkSwapChain()
{
	if (m_swapChain && m_swapChainSize == m_updatedSize && m_wndId == m_swapChainWnd)
		return;

	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_backbuffer);
	SAFE_RELEASE(m_swapChain);

	DXGI_SWAP_CHAIN_DESC desc;
	TypeZero(&desc);
	desc.BufferCount = 1;
	desc.BufferDesc.Width = m_updatedSize.width;
	desc.BufferDesc.Height = m_updatedSize.height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = m_wndId;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0;

	dxgi_factory->CreateSwapChain(dx11_device, &desc, &m_swapChain);
	m_swapChain->GetBuffer(0, __uuidof(m_backbuffer), reinterpret_cast<void**>(&m_backbuffer));
	dx11_device->CreateRenderTargetView(m_backbuffer, NULL, &m_renderTargetView);

	m_swapChainSize = m_updatedSize;
	m_swapChainWnd = m_wndId;
}

AX_DX11_END_NAMESPACE
