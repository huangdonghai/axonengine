/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_CLIENT_WININPUT_H
#define AX_CLIENT_WININPUT_H

AX_BEGIN_NAMESPACE

	class WinInput : public IInputSource {
	public:
		WinInput();
		virtual ~WinInput();

		void setWindow(RenderTarget *gamewnd);
		void queWinInput(MSG *msg);

		// implement IInputSource
		virtual void startCapture(InputSystem::CaptureMode capturemode);
		virtual void process();
		virtual void setVibration(float left, float right);
		virtual void setMouseMode(InputSystem::MouseMode mode);
		virtual void stopCapture();

	protected:

	private:
		HWND m_hwnd;
		WNDPROC m_oldWndProc;

		InputKey m_mapVkey[256];
	};

AX_END_NAMESPACE

#endif // end guardian

