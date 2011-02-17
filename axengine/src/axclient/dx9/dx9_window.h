/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_DX9_WINDOW_H
#define AX_DX9_WINDOW_H

AX_BEGIN_NAMESPACE

class DX9_Window : public DX9_Unknown
{
public:
	DX9_Window();
	DX9_Window(Handle wndId, int width, int height);
	~DX9_Window();

	// implement RenderTarget
	Size getSize();

	void update(Handle newId, int width, int height);
	Handle getWindowHandle() { return Handle(m_wndId); }

	HWND getHandle() const { return m_wndId; }
	void present();

	IDirect3DSurface9 *getSurface();

protected:
	void checkSwapChain();

private:
	LONG m_ref;
	HWND m_wndId;
	Size m_swapChainSize;
	Size m_updatedSize;
	IDirect3DSwapChain9 *m_swapChain;
	IDirect3DSurface9 *m_backbuffer;
	HWND m_swapChainWnd;
	DWORD m_presentInterval;
};

AX_END_NAMESPACE

#endif // end guardian

