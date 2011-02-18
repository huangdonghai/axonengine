#ifndef AX_DX11_WINDOW_H
#define AX_DX11_WINDOW_H

AX_BEGIN_NAMESPACE

class DX11_Window : public DX11_Unknown
{
public:
	DX11_Window(Handle wndId, int width, int height);
	~DX11_Window();

	void update(Handle newId, int width, int height);
	void present();

private:
};

AX_END_NAMESPACE

#endif // AX_DX11_WINDOW_H
