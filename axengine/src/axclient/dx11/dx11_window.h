#ifndef AX_DX11_WINDOW_H
#define AX_DX11_WINDOW_H

AX_DX11_BEGIN_NAMESPACE

class DX11_Window : public DX11_Unknown
{
public:
	DX11_Window(Handle wndId, int width, int height);
	~DX11_Window();

	void update(Handle newId, int width, int height);
	void present();
	ID3D11RenderTargetView *getRenderTargetView() const { return m_renderTargetView; }

protected:
	void checkSwapChain();

private:
	HWND m_wndId;
	Size m_swapChainSize;
	Size m_updatedSize;
	IDXGISwapChain *m_swapChain;
	ID3D11Texture2D *m_backbuffer;
	ID3D11RenderTargetView *m_renderTargetView;
	HWND m_swapChainWnd;
};

AX_DX11_END_NAMESPACE

#endif // AX_DX11_WINDOW_H
