#ifndef AX_GL_WINDOW_H
#define AX_GL_WINDOW_H

AX_BEGIN_NAMESPACE

class GL_Window
{
public:
	GL_Window();
	GL_Window(Handle wndId, int width, int height);
	~GL_Window();

	// implement RenderTarget
	Size getSize();

	void update(Handle newId, int width, int height);
	Handle getWindowHandle() { return Handle(m_wndId); }

	HWND getHandle() const { return m_wndId.castTo<HWND>(); }
	void present();

private:
	Handle m_wndId;
};

AX_END_NAMESPACE

#endif // AX_GL_WINDOW_H