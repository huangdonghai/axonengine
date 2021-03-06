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

namespace Axon { namespace Input {


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

		m_mapVkey[VK_LBUTTON] = Key::MouseLeft;
		m_mapVkey[VK_RBUTTON] = Key::MouseRight;
		m_mapVkey[VK_CANCEL] = Key::Invalid;
		m_mapVkey[VK_MBUTTON] = Key::MouseMiddle;

#if (_WIN32_WINNT >= 0x0500)
		m_mapVkey[VK_XBUTTON1] = 0;
		m_mapVkey[VK_XBUTTON2] = 0;
#endif /* _WIN32_WINNT >= 0x0500 */

				/*
				* 0x07 : unassigned
				*/

		m_mapVkey[VK_BACK] = Key::Backspace;
		m_mapVkey[VK_TAB] = Key::Tab;

				/*
				* 0x0A - 0x0B : reserved
				*/

		m_mapVkey[VK_CLEAR] = Key::Clear;
		m_mapVkey[VK_RETURN] = Key::Enter;
		m_mapVkey[VK_SHIFT] = Key::Shift;
		m_mapVkey[VK_CONTROL] = Key::Ctrl;
		m_mapVkey[VK_MENU] = Key::Menu;
		m_mapVkey[VK_PAUSE] = Key::Pause;
		m_mapVkey[VK_CAPITAL] = Key::Capslock;

		m_mapVkey[VK_KANA] = 0;
		m_mapVkey[VK_HANGEUL] = 0;
		m_mapVkey[VK_HANGUL] = 0;
		m_mapVkey[VK_JUNJA] = 0;
		m_mapVkey[VK_FINAL] = 0;
		m_mapVkey[VK_HANJA] = 0;
		m_mapVkey[VK_KANJI] = 0;

		m_mapVkey[VK_ESCAPE] = Key::Escape;

		m_mapVkey[VK_CONVERT] = 0;
		m_mapVkey[VK_NONCONVERT] = 0;
		m_mapVkey[VK_ACCEPT] = 0;
		m_mapVkey[VK_MODECHANGE] = 0;

		m_mapVkey[VK_SPACE] = Key::Space;
		m_mapVkey[VK_PRIOR] = 0;
		m_mapVkey[VK_NEXT] = 0;
		m_mapVkey[VK_END] = Key::End;
		m_mapVkey[VK_HOME] = Key::Home;
		m_mapVkey[VK_LEFT] = Key::Left;
		m_mapVkey[VK_UP] = Key::Up;
		m_mapVkey[VK_RIGHT] = Key::Right;
		m_mapVkey[VK_DOWN] = Key::Down;
		m_mapVkey[VK_SELECT] = 0;
		m_mapVkey[VK_PRINT] = Key::Print;
		m_mapVkey[VK_EXECUTE] = 0;
		m_mapVkey[VK_SNAPSHOT] = 0;
		m_mapVkey[VK_INSERT] = Key::Insert;
		m_mapVkey[VK_DELETE] = Key::Delete;
		m_mapVkey[VK_HELP] = Key::Help;

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

		m_mapVkey[VK_NUMPAD0] = Key::NP_Insert;
		m_mapVkey[VK_NUMPAD1] = Key::NP_End;
		m_mapVkey[VK_NUMPAD2] = Key::NP_Down;
		m_mapVkey[VK_NUMPAD3] = Key::NP_Pagedown;
		m_mapVkey[VK_NUMPAD4] = Key::NP_Left;
		m_mapVkey[VK_NUMPAD5] = Key::NP_5;
		m_mapVkey[VK_NUMPAD6] = Key::NP_Right;
		m_mapVkey[VK_NUMPAD7] = Key::NP_Home;
		m_mapVkey[VK_NUMPAD8] = Key::NP_Up;
		m_mapVkey[VK_NUMPAD9] = Key::NP_Pageup;
		m_mapVkey[VK_MULTIPLY] = '*';
		m_mapVkey[VK_ADD] = '+';
		m_mapVkey[VK_SEPARATOR] = Key::Invalid;
		m_mapVkey[VK_SUBTRACT] = '-';
		m_mapVkey[VK_DECIMAL] = Key::Invalid;
		m_mapVkey[VK_DIVIDE] = '/';
		m_mapVkey[VK_F1] = Key::F1;
		m_mapVkey[VK_F2] = Key::F2;
		m_mapVkey[VK_F3] = Key::F3;
		m_mapVkey[VK_F4] = Key::F4;
		m_mapVkey[VK_F5] = Key::F5;
		m_mapVkey[VK_F6] = Key::F6;
		m_mapVkey[VK_F7] = Key::F7;
		m_mapVkey[VK_F8] = Key::F8;
		m_mapVkey[VK_F9] = Key::F9;
		m_mapVkey[VK_F10] = Key::F10;
		m_mapVkey[VK_F11] = Key::F11;
		m_mapVkey[VK_F12] = Key::F12;
		m_mapVkey[VK_F13] = Key::F13;
		m_mapVkey[VK_F14] = Key::F14;
		m_mapVkey[VK_F15] = Key::F15;
		m_mapVkey[VK_F16] = Key::Invalid;
		m_mapVkey[VK_F17] = Key::Invalid;
		m_mapVkey[VK_F18] = Key::Invalid;
		m_mapVkey[VK_F19] = Key::Invalid;
		m_mapVkey[VK_F20] = Key::Invalid;
		m_mapVkey[VK_F21] = Key::Invalid;
		m_mapVkey[VK_F22] = Key::Invalid;
		m_mapVkey[VK_F23] = Key::Invalid;
		m_mapVkey[VK_F24] = Key::Invalid;

				/*
				* 0x88 - 0x8F : unassigned
				*/

		m_mapVkey[VK_NUMLOCK] = Key::NP_Numlock;
		m_mapVkey[VK_SCROLL] = Key::ScorllLock;

				/*
				* NEC PC-9800 kbd definitions
				*/
		m_mapVkey[VK_OEM_NEC_EQUAL] = Key::Invalid;

				/*
				* Fujitsu/OASYS kbd definitions
				*/
		m_mapVkey[VK_OEM_FJ_JISHO] = Key::Invalid;
		m_mapVkey[VK_OEM_FJ_MASSHOU] = Key::Invalid;
		m_mapVkey[VK_OEM_FJ_TOUROKU] = Key::Invalid;
		m_mapVkey[VK_OEM_FJ_LOYA] = Key::Invalid;
		m_mapVkey[VK_OEM_FJ_ROYA] = Key::Invalid;

				/*
				* 0x97 - 0x9F : unassigned
				*/

				/*
				* VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
				* Used only as parameters to GetAsyncKeyState() and GetKeyState().
				* No other API or message will distinguish left and right keys in this way.
				*/
		m_mapVkey[VK_LSHIFT] = Key::Shift;
		m_mapVkey[VK_RSHIFT] = Key::Shift;
		m_mapVkey[VK_LCONTROL] = Key::Ctrl;
		m_mapVkey[VK_RCONTROL] = Key::Ctrl;
		m_mapVkey[VK_LMENU] = Key::Menu;
		m_mapVkey[VK_RMENU] = Key::Menu;

		#if (_WIN32_WINNT >= 0x0500)
		m_mapVkey[VK_BROWSER_BACK] = Key::Invalid;
		m_mapVkey[VK_BROWSER_FORWARD] = Key::Invalid;
		m_mapVkey[VK_BROWSER_REFRESH] = Key::Invalid;
		m_mapVkey[VK_BROWSER_STOP] = Key::Invalid;
		m_mapVkey[VK_BROWSER_SEARCH] = Key::Invalid;
		m_mapVkey[VK_BROWSER_FAVORITES] = Key::Invalid;
		m_mapVkey[VK_BROWSER_HOME] = Key::Invalid;

		m_mapVkey[VK_VOLUME_MUTE] = Key::Invalid;
		m_mapVkey[VK_VOLUME_DOWN] = Key::Invalid;
		m_mapVkey[VK_VOLUME_UP] = Key::Invalid;
		m_mapVkey[VK_MEDIA_NEXT_TRACK] = Key::Invalid;
		m_mapVkey[VK_MEDIA_PREV_TRACK] = Key::Invalid;
		m_mapVkey[VK_MEDIA_STOP] = Key::Invalid;
		m_mapVkey[VK_MEDIA_PLAY_PAUSE] = Key::Invalid;
		m_mapVkey[VK_LAUNCH_MAIL] = Key::Invalid;
		m_mapVkey[VK_LAUNCH_MEDIA_SELECT] = Key::Invalid;
		m_mapVkey[VK_LAUNCH_APP1] = Key::Invalid;
		m_mapVkey[VK_LAUNCH_APP2] = Key::Invalid;

		#endif /* _WIN32_WINNT >= 0x0500 */

				/*
				* 0xB8 - 0xB9 : reserved
				*/

		m_mapVkey[VK_OEM_1] = Key::Invalid;
		m_mapVkey[VK_OEM_PLUS] = Key::Invalid;
		m_mapVkey[VK_OEM_COMMA] = Key::Invalid;
		m_mapVkey[VK_OEM_MINUS] = Key::Invalid;
		m_mapVkey[VK_OEM_PERIOD] = Key::Invalid;
		m_mapVkey[VK_OEM_2] = Key::Invalid;
		m_mapVkey[VK_OEM_3] = Key::Invalid;

				/*
				* 0xC1 - 0xD7 : reserved
				*/

				/*
				* 0xD8 - 0xDA : unassigned
				*/

		m_mapVkey[VK_OEM_4] = Key::Invalid;
		m_mapVkey[VK_OEM_5] = Key::Invalid;
		m_mapVkey[VK_OEM_6] = Key::Invalid;  //  ']}' for US
		m_mapVkey[VK_OEM_7] = Key::Invalid;  //  ''"' for US
		m_mapVkey[VK_OEM_8] = Key::Invalid;

				/*
				* 0xE0 : reserved
				*/

				/*
				* Various extended or enhanced keyboards
				*/
		m_mapVkey[VK_OEM_AX] = Key::Invalid;  //  'Axon' key on Japanese Axon kbd
		m_mapVkey[VK_OEM_102] = Key::Invalid;  //  "<>" or "\|" on RT 102-key kbd.
		m_mapVkey[VK_ICO_HELP] = Key::Invalid;  //  Help key on ICO
		m_mapVkey[VK_ICO_00] = Key::Invalid;  //  00 key on ICO

		#if (WINVER >= 0x0400)
		m_mapVkey[VK_PROCESSKEY] = Key::Invalid;
		#endif /* WINVER >= 0x0400 */

		m_mapVkey[VK_ICO_CLEAR] = Key::Invalid;


		#if (_WIN32_WINNT >= 0x0500)
		m_mapVkey[VK_PACKET] = Key::Invalid;
		#endif /* _WIN32_WINNT >= 0x0500 */

				/*
				* 0xE8 : unassigned
				*/

				/*
				* Nokia/Ericsson definitions
				*/
		m_mapVkey[VK_OEM_RESET] = Key::Invalid;
		m_mapVkey[VK_OEM_JUMP] = Key::Invalid;
		m_mapVkey[VK_OEM_PA1] = Key::Invalid;
		m_mapVkey[VK_OEM_PA2] = Key::Invalid;
		m_mapVkey[VK_OEM_PA3] = Key::Invalid;
		m_mapVkey[VK_OEM_WSCTRL] = Key::Invalid;
		m_mapVkey[VK_OEM_CUSEL] = Key::Invalid;
		m_mapVkey[VK_OEM_ATTN] = Key::Invalid;
		m_mapVkey[VK_OEM_FINISH] = Key::Invalid;
		m_mapVkey[VK_OEM_COPY] = Key::Invalid;
		m_mapVkey[VK_OEM_AUTO] = Key::Invalid;
		m_mapVkey[VK_OEM_ENLW] = Key::Invalid;
		m_mapVkey[VK_OEM_BACKTAB] = Key::Invalid;

		m_mapVkey[VK_ATTN] = Key::Invalid;
		m_mapVkey[VK_CRSEL] = Key::Invalid;
		m_mapVkey[VK_EXSEL] = Key::Invalid;
		m_mapVkey[VK_EREOF] = Key::Invalid;
		m_mapVkey[VK_PLAY] = Key::Invalid;
		m_mapVkey[VK_ZOOM] = Key::Invalid;
		m_mapVkey[VK_NONAME] = Key::Invalid;
		m_mapVkey[VK_PA1] = Key::Invalid;
		m_mapVkey[VK_OEM_CLEAR] = Key::Invalid;
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

	void WinInput::setWindow(Render::Target* gamewnd)
	{
		if (gamewnd) {
			m_hwnd = (HWND)gamewnd->getWindowHandle();
		} else {
			m_hwnd = 0;
		}
	}

	void WinInput::queWinInput(MSG* msg)
	{
		UINT dwSize;

		static int oldtime = 0;

		int curtime = OsUtil::milliseconds();
		int msgtime = curtime - oldtime;
		oldtime = curtime;
		Printf("%d\n", msgtime);

		GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL) {
			return;
		} 

		if (GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString (TEXT("GetRawInputData doesn't return correct size !\n")); 

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEKEYBOARD) {
			RAWKEYBOARD& rk = raw->data.keyboard;

			Event e;
			TypeZero(&e);

			e.key = m_mapVkey[rk.VKey];

			if (!e.key) {
				goto quit;
			}

			if (rk.Message == WM_KEYDOWN) {
				e.type = Event::KeyDown;
			} else if (rk.Message == WM_KEYUP) {
				e.type = Event::KeyUp;
			} else {
				Errorf("unknown key message");
			}

			g_inputSystem->queEvent(e);

		} else if (raw->header.dwType == RIM_TYPEMOUSE) {
			RAWMOUSE& rm = raw->data.mouse;

			Printf("%x %x %d %d\n", rm.usFlags, rm.usButtonFlags, rm.lLastX, rm.lLastY);
		} 

quit:
		delete[] lpb; 
	}


	void WinInput::setMouseMode(InputSystem::MouseMode mode)
	{

	}

}} // namespace Axon::Input

