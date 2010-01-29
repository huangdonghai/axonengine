/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"
#include "wininput.h"

#include <windows.h>

AX_BEGIN_NAMESPACE


	static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		// WM_INPUT data
		//
		static RAWINPUT buffer[32];
		static RAWINPUT ** pBuffer = new RAWINPUT *[32];
		static unsigned int HEADERSIZE = sizeof(RAWINPUTHEADER);

		switch (msg)
		{
				// input handling: use raw input
				//
		case WM_INPUT:
			{
				unsigned int dataRead = 32;
				GetRawInputBuffer(buffer, &dataRead, HEADERSIZE);
				if (!dataRead) {
					// error
					//
				}

				RAWINPUT * pri = buffer;
				for (unsigned int i = 0; i < dataRead; ++i) {
					pBuffer[i] = pri;
					pri = NEXTRAWINPUTBLOCK(pri);
				}
				DefRawInputProc(pBuffer, dataRead, HEADERSIZE);
			}
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	WinInput::WinInput()
	{
		m_hwnd = 0;
		m_oldWndProc = 0;

		TypeZeroArray(m_mapVkey);

		m_mapVkey[VK_LBUTTON] = InputKey::MouseLeft;
		m_mapVkey[VK_RBUTTON] = InputKey::MouseRight;
		m_mapVkey[VK_CANCEL] = InputKey::Invalid;
		m_mapVkey[VK_MBUTTON] = InputKey::MouseMiddle;

#if (_WIN32_WINNT >= 0x0500)
		m_mapVkey[VK_XBUTTON1] = 0;
		m_mapVkey[VK_XBUTTON2] = 0;
#endif /* _WIN32_WINNT >= 0x0500 */

				/*
				* 0x07 : unassigned
				*/

		m_mapVkey[VK_BACK] = InputKey::Backspace;
		m_mapVkey[VK_TAB] = InputKey::Tab;

				/*
				* 0x0A - 0x0B : reserved
				*/

		m_mapVkey[VK_CLEAR] = InputKey::Clear;
		m_mapVkey[VK_RETURN] = InputKey::Enter;
		m_mapVkey[VK_SHIFT] = InputKey::Shift;
		m_mapVkey[VK_CONTROL] = InputKey::Ctrl;
		m_mapVkey[VK_MENU] = InputKey::Menu;
		m_mapVkey[VK_PAUSE] = InputKey::Pause;
		m_mapVkey[VK_CAPITAL] = InputKey::Capslock;

		m_mapVkey[VK_KANA] = 0;
		m_mapVkey[VK_HANGEUL] = 0;
		m_mapVkey[VK_HANGUL] = 0;
		m_mapVkey[VK_JUNJA] = 0;
		m_mapVkey[VK_FINAL] = 0;
		m_mapVkey[VK_HANJA] = 0;
		m_mapVkey[VK_KANJI] = 0;

		m_mapVkey[VK_ESCAPE] = InputKey::Escape;

		m_mapVkey[VK_CONVERT] = 0;
		m_mapVkey[VK_NONCONVERT] = 0;
		m_mapVkey[VK_ACCEPT] = 0;
		m_mapVkey[VK_MODECHANGE] = 0;

		m_mapVkey[VK_SPACE] = InputKey::Space;
		m_mapVkey[VK_PRIOR] = 0;
		m_mapVkey[VK_NEXT] = 0;
		m_mapVkey[VK_END] = InputKey::End;
		m_mapVkey[VK_HOME] = InputKey::Home;
		m_mapVkey[VK_LEFT] = InputKey::Left;
		m_mapVkey[VK_UP] = InputKey::Up;
		m_mapVkey[VK_RIGHT] = InputKey::Right;
		m_mapVkey[VK_DOWN] = InputKey::Down;
		m_mapVkey[VK_SELECT] = 0;
		m_mapVkey[VK_PRINT] = InputKey::Print;
		m_mapVkey[VK_EXECUTE] = 0;
		m_mapVkey[VK_SNAPSHOT] = 0;
		m_mapVkey[VK_INSERT] = InputKey::Insert;
		m_mapVkey[VK_DELETE] = InputKey::Delete;
		m_mapVkey[VK_HELP] = InputKey::Help;

				/*
				* VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
				* 0x40 : unassigned
				* VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
				*/
		for (int i = '0'; i <= '9'; i++) {
			m_mapVkey[i] = i;
		}

		for (int i = 'A'; i < 'Z'; i++) {
			m_mapVkey[i] = i + 'a' - 'A';
		}

		m_mapVkey[VK_LWIN] = 0;
		m_mapVkey[VK_RWIN] = 0;
		m_mapVkey[VK_APPS] = 0;

				/*
				* 0x5E : reserved
				*/

		m_mapVkey[VK_SLEEP] = 0;

		m_mapVkey[VK_NUMPAD0] = InputKey::NP_Insert;
		m_mapVkey[VK_NUMPAD1] = InputKey::NP_End;
		m_mapVkey[VK_NUMPAD2] = InputKey::NP_Down;
		m_mapVkey[VK_NUMPAD3] = InputKey::NP_Pagedown;
		m_mapVkey[VK_NUMPAD4] = InputKey::NP_Left;
		m_mapVkey[VK_NUMPAD5] = InputKey::NP_5;
		m_mapVkey[VK_NUMPAD6] = InputKey::NP_Right;
		m_mapVkey[VK_NUMPAD7] = InputKey::NP_Home;
		m_mapVkey[VK_NUMPAD8] = InputKey::NP_Up;
		m_mapVkey[VK_NUMPAD9] = InputKey::NP_Pageup;
		m_mapVkey[VK_MULTIPLY] = '*';
		m_mapVkey[VK_ADD] = '+';
		m_mapVkey[VK_SEPARATOR] = InputKey::Invalid;
		m_mapVkey[VK_SUBTRACT] = '-';
		m_mapVkey[VK_DECIMAL] = InputKey::Invalid;
		m_mapVkey[VK_DIVIDE] = '/';
		m_mapVkey[VK_F1] = InputKey::F1;
		m_mapVkey[VK_F2] = InputKey::F2;
		m_mapVkey[VK_F3] = InputKey::F3;
		m_mapVkey[VK_F4] = InputKey::F4;
		m_mapVkey[VK_F5] = InputKey::F5;
		m_mapVkey[VK_F6] = InputKey::F6;
		m_mapVkey[VK_F7] = InputKey::F7;
		m_mapVkey[VK_F8] = InputKey::F8;
		m_mapVkey[VK_F9] = InputKey::F9;
		m_mapVkey[VK_F10] = InputKey::F10;
		m_mapVkey[VK_F11] = InputKey::F11;
		m_mapVkey[VK_F12] = InputKey::F12;
		m_mapVkey[VK_F13] = InputKey::F13;
		m_mapVkey[VK_F14] = InputKey::F14;
		m_mapVkey[VK_F15] = InputKey::F15;
		m_mapVkey[VK_F16] = InputKey::Invalid;
		m_mapVkey[VK_F17] = InputKey::Invalid;
		m_mapVkey[VK_F18] = InputKey::Invalid;
		m_mapVkey[VK_F19] = InputKey::Invalid;
		m_mapVkey[VK_F20] = InputKey::Invalid;
		m_mapVkey[VK_F21] = InputKey::Invalid;
		m_mapVkey[VK_F22] = InputKey::Invalid;
		m_mapVkey[VK_F23] = InputKey::Invalid;
		m_mapVkey[VK_F24] = InputKey::Invalid;

				/*
				* 0x88 - 0x8F : unassigned
				*/

		m_mapVkey[VK_NUMLOCK] = InputKey::NP_Numlock;
		m_mapVkey[VK_SCROLL] = InputKey::ScorllLock;

				/*
				* NEC PC-9800 kbd definitions
				*/
		m_mapVkey[VK_OEM_NEC_EQUAL] = InputKey::Invalid;

				/*
				* Fujitsu/OASYS kbd definitions
				*/
		m_mapVkey[VK_OEM_FJ_JISHO] = InputKey::Invalid;
		m_mapVkey[VK_OEM_FJ_MASSHOU] = InputKey::Invalid;
		m_mapVkey[VK_OEM_FJ_TOUROKU] = InputKey::Invalid;
		m_mapVkey[VK_OEM_FJ_LOYA] = InputKey::Invalid;
		m_mapVkey[VK_OEM_FJ_ROYA] = InputKey::Invalid;

				/*
				* 0x97 - 0x9F : unassigned
				*/

				/*
				* VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
				* Used only as parameters to GetAsyncKeyState() and GetKeyState().
				* No other API or message will distinguish left and right keys in this way.
				*/
		m_mapVkey[VK_LSHIFT] = InputKey::Shift;
		m_mapVkey[VK_RSHIFT] = InputKey::Shift;
		m_mapVkey[VK_LCONTROL] = InputKey::Ctrl;
		m_mapVkey[VK_RCONTROL] = InputKey::Ctrl;
		m_mapVkey[VK_LMENU] = InputKey::Menu;
		m_mapVkey[VK_RMENU] = InputKey::Menu;

		#if (_WIN32_WINNT >= 0x0500)
		m_mapVkey[VK_BROWSER_BACK] = InputKey::Invalid;
		m_mapVkey[VK_BROWSER_FORWARD] = InputKey::Invalid;
		m_mapVkey[VK_BROWSER_REFRESH] = InputKey::Invalid;
		m_mapVkey[VK_BROWSER_STOP] = InputKey::Invalid;
		m_mapVkey[VK_BROWSER_SEARCH] = InputKey::Invalid;
		m_mapVkey[VK_BROWSER_FAVORITES] = InputKey::Invalid;
		m_mapVkey[VK_BROWSER_HOME] = InputKey::Invalid;

		m_mapVkey[VK_VOLUME_MUTE] = InputKey::Invalid;
		m_mapVkey[VK_VOLUME_DOWN] = InputKey::Invalid;
		m_mapVkey[VK_VOLUME_UP] = InputKey::Invalid;
		m_mapVkey[VK_MEDIA_NEXT_TRACK] = InputKey::Invalid;
		m_mapVkey[VK_MEDIA_PREV_TRACK] = InputKey::Invalid;
		m_mapVkey[VK_MEDIA_STOP] = InputKey::Invalid;
		m_mapVkey[VK_MEDIA_PLAY_PAUSE] = InputKey::Invalid;
		m_mapVkey[VK_LAUNCH_MAIL] = InputKey::Invalid;
		m_mapVkey[VK_LAUNCH_MEDIA_SELECT] = InputKey::Invalid;
		m_mapVkey[VK_LAUNCH_APP1] = InputKey::Invalid;
		m_mapVkey[VK_LAUNCH_APP2] = InputKey::Invalid;

		#endif /* _WIN32_WINNT >= 0x0500 */

				/*
				* 0xB8 - 0xB9 : reserved
				*/

		m_mapVkey[VK_OEM_1] = InputKey::Invalid;
		m_mapVkey[VK_OEM_PLUS] = InputKey::Invalid;
		m_mapVkey[VK_OEM_COMMA] = InputKey::Invalid;
		m_mapVkey[VK_OEM_MINUS] = InputKey::Invalid;
		m_mapVkey[VK_OEM_PERIOD] = InputKey::Invalid;
		m_mapVkey[VK_OEM_2] = InputKey::Invalid;
		m_mapVkey[VK_OEM_3] = InputKey::Invalid;

				/*
				* 0xC1 - 0xD7 : reserved
				*/

				/*
				* 0xD8 - 0xDA : unassigned
				*/

		m_mapVkey[VK_OEM_4] = InputKey::Invalid;
		m_mapVkey[VK_OEM_5] = InputKey::Invalid;
		m_mapVkey[VK_OEM_6] = InputKey::Invalid;  //  ']}' for US
		m_mapVkey[VK_OEM_7] = InputKey::Invalid;  //  ''"' for US
		m_mapVkey[VK_OEM_8] = InputKey::Invalid;

				/*
				* 0xE0 : reserved
				*/

				/*
				* Various extended or enhanced keyboards
				*/
		m_mapVkey[VK_OEM_AX] = InputKey::Invalid;  //  'Axon' key on Japanese Axon kbd
		m_mapVkey[VK_OEM_102] = InputKey::Invalid;  //  "<>" or "\|" on RT 102-key kbd.
		m_mapVkey[VK_ICO_HELP] = InputKey::Invalid;  //  Help key on ICO
		m_mapVkey[VK_ICO_00] = InputKey::Invalid;  //  00 key on ICO

		#if (WINVER >= 0x0400)
		m_mapVkey[VK_PROCESSKEY] = InputKey::Invalid;
		#endif /* WINVER >= 0x0400 */

		m_mapVkey[VK_ICO_CLEAR] = InputKey::Invalid;


		#if (_WIN32_WINNT >= 0x0500)
		m_mapVkey[VK_PACKET] = InputKey::Invalid;
		#endif /* _WIN32_WINNT >= 0x0500 */

				/*
				* 0xE8 : unassigned
				*/

				/*
				* Nokia/Ericsson definitions
				*/
		m_mapVkey[VK_OEM_RESET] = InputKey::Invalid;
		m_mapVkey[VK_OEM_JUMP] = InputKey::Invalid;
		m_mapVkey[VK_OEM_PA1] = InputKey::Invalid;
		m_mapVkey[VK_OEM_PA2] = InputKey::Invalid;
		m_mapVkey[VK_OEM_PA3] = InputKey::Invalid;
		m_mapVkey[VK_OEM_WSCTRL] = InputKey::Invalid;
		m_mapVkey[VK_OEM_CUSEL] = InputKey::Invalid;
		m_mapVkey[VK_OEM_ATTN] = InputKey::Invalid;
		m_mapVkey[VK_OEM_FINISH] = InputKey::Invalid;
		m_mapVkey[VK_OEM_COPY] = InputKey::Invalid;
		m_mapVkey[VK_OEM_AUTO] = InputKey::Invalid;
		m_mapVkey[VK_OEM_ENLW] = InputKey::Invalid;
		m_mapVkey[VK_OEM_BACKTAB] = InputKey::Invalid;

		m_mapVkey[VK_ATTN] = InputKey::Invalid;
		m_mapVkey[VK_CRSEL] = InputKey::Invalid;
		m_mapVkey[VK_EXSEL] = InputKey::Invalid;
		m_mapVkey[VK_EREOF] = InputKey::Invalid;
		m_mapVkey[VK_PLAY] = InputKey::Invalid;
		m_mapVkey[VK_ZOOM] = InputKey::Invalid;
		m_mapVkey[VK_NONAME] = InputKey::Invalid;
		m_mapVkey[VK_PA1] = InputKey::Invalid;
		m_mapVkey[VK_OEM_CLEAR] = InputKey::Invalid;
	}

	WinInput::~WinInput()
	{

	}

	void WinInput::startCapture(InputSystem::CaptureMode capturemode)
	{
		if (!m_hwnd) {
			return;
		}

		m_oldWndProc = (WNDPROC)GetWindowLong(m_hwnd, GWL_WNDPROC);
		SetWindowLong(m_hwnd, GWL_WNDPROC, (LONG)&WndProc);

		RAWINPUTDEVICE Rid[4];

		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;            // HID mouse
		Rid[0].dwFlags = RIDEV_NOLEGACY;  // ignore legacy mouse messages
		Rid[0].hwndTarget = NULL;

		Rid[1].usUsagePage = 0x01;
		Rid[1].usUsage = 0x04;            // HID joystick
		Rid[1].dwFlags = 0;
		Rid[1].hwndTarget = m_hwnd;

		Rid[2].usUsagePage = 0x01;
		Rid[2].usUsage = 0x05;            // HID game pad
		Rid[2].dwFlags = 0;
		Rid[2].hwndTarget = m_hwnd;

		Rid[3].usUsagePage = 0x01;
		Rid[3].usUsage = 0x06;            // HID keyboard
		Rid[3].dwFlags = RIDEV_NOLEGACY;  // ignore legacy keyboard messages
		Rid[3].hwndTarget = NULL;

		if (!RegisterRawInputDevices(Rid, 4, sizeof(Rid[0]))) {
			Errorf("can't register rawinput devices");
		}
	}

	void WinInput::process()
	{

	}

	void WinInput::setVibration(float left, float right)
	{

	}

	void WinInput::stopCapture()
	{
		if (!m_hwnd) {
			return;
		}

		SetWindowLong(m_hwnd, GWL_WNDPROC, (LONG)m_oldWndProc);

		RAWINPUTDEVICE Rid[4];

		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;            // HID mouse
		Rid[0].dwFlags = RIDEV_REMOVE;  // ignore legacy mouse messages
		Rid[0].hwndTarget = NULL;

		Rid[1].usUsagePage = 0x01;
		Rid[1].usUsage = 0x04;            // HID joystick
		Rid[1].dwFlags = RIDEV_REMOVE;
		Rid[1].hwndTarget = 0;

		Rid[2].usUsagePage = 0x01;
		Rid[2].usUsage = 0x05;            // HID game pad
		Rid[2].dwFlags = RIDEV_REMOVE;
		Rid[2].hwndTarget = 0;

		Rid[3].usUsagePage = 0x01;
		Rid[3].usUsage = 0x06;            // HID keyboard
		Rid[3].dwFlags = RIDEV_REMOVE;  // ignore legacy keyboard messages
		Rid[3].hwndTarget = NULL;

		if (!RegisterRawInputDevices(Rid, 4, sizeof(Rid[0]))) {
			Errorf("can't register rawinput devices");
		}
	}

	void WinInput::setWindow(RenderTarget *gamewnd)
	{
		if (gamewnd) {
			m_hwnd = (HWND)gamewnd->getWindowHandle();
		} else {
			m_hwnd = 0;
		}
	}

	void WinInput::queWinInput(MSG *msg)
	{
		UINT dwSize;

		GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL) {
			return;
		} 

		if (GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString (TEXT("GetRawInputData doesn't return correct size !\n")); 

		RAWINPUT *raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEKEYBOARD) {
			RAWKEYBOARD &rk = raw->data.keyboard;

			InputEvent e;
			TypeZero(&e);

			e.key = m_mapVkey[rk.VKey];

			if (!e.key) {
				goto quit;
			}

			if (rk.Message == WM_KEYDOWN) {
				e.type = InputEvent::KeyDown;
			} else if (rk.Message == WM_KEYUP) {
				e.type = InputEvent::KeyUp;
			} else {
				Errorf("unknown key message");
			}

			g_inputSystem->queEvent(e);

		} else if (raw->header.dwType == RIM_TYPEMOUSE) {
			RAWMOUSE &rm = raw->data.mouse;

			Printf("%x %x %d %d\n", rm.usFlags, rm.usButtonFlags, rm.lLastX, rm.lLastY);
		} 

quit:
		delete[] lpb; 
	}


	void WinInput::setMouseMode(InputSystem::MouseMode mode)
	{

	}

AX_END_NAMESPACE

