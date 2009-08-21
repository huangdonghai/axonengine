/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"
#include "xboxinput.h"

#include <windows.h>
#include <XInput.h>

#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 

namespace Axon { namespace Input {

	struct ControlerState {
		XINPUT_STATE    state;       
		bool            bConnected;
	};

	ControlerState gControllers[MAX_CONTROLLERS];

	Xinput::Xinput() {
		m_isInCapture = false;
		TypeZeroArray(gControllers);

		m_keymap[VK_PAD_A]			= Key::XBOX_A;
		m_keymap[VK_PAD_B]			= Key::XBOX_B;
		m_keymap[VK_PAD_X]			= Key::XBOX_X;
		m_keymap[VK_PAD_Y]			= Key::XBOX_Y;
		m_keymap[VK_PAD_RSHOULDER]	= Key::XBOX_rshoulder;
		m_keymap[VK_PAD_LSHOULDER]	= Key::XBOX_lshoulder;
		m_keymap[VK_PAD_LTRIGGER]	= Key::XBOX_ltrigger;
		m_keymap[VK_PAD_RTRIGGER]	= Key::XBOX_rtrigger;

		m_keymap[VK_PAD_DPAD_UP]		= Key::XBOX_dpad_up;
		m_keymap[VK_PAD_DPAD_DOWN]	= Key::XBOX_dpad_down;
		m_keymap[VK_PAD_DPAD_LEFT]	= Key::XBOX_dpad_left;
		m_keymap[VK_PAD_DPAD_RIGHT]	= Key::XBOX_dpad_right;
		m_keymap[VK_PAD_START]		= Key::XBOX_start;
		m_keymap[VK_PAD_BACK]		= Key::XBOX_back;
		m_keymap[VK_PAD_LTHUMB_PRESS] = Key::XBOX_lthumb_press;
		m_keymap[VK_PAD_RTHUMB_PRESS] = Key::XBOX_rthumb_press;

		m_keymap[VK_PAD_LTHUMB_UP]	= Key::XBOX_lthumb_up;
		m_keymap[VK_PAD_LTHUMB_DOWN] = Key::XBOX_lthumb_down;
		m_keymap[VK_PAD_LTHUMB_RIGHT] = Key::XBOX_lthumb_right;
		m_keymap[VK_PAD_LTHUMB_LEFT] = Key::XBOX_lthumb_left;
		m_keymap[VK_PAD_LTHUMB_UPLEFT]	= Key::XBOX_lthumb_upleft;
		m_keymap[VK_PAD_LTHUMB_UPRIGHT]	= Key::XBOX_lthumb_upright;
		m_keymap[VK_PAD_LTHUMB_DOWNRIGHT]	= Key::XBOX_lthumb_downright;
		m_keymap[VK_PAD_LTHUMB_DOWNLEFT] = Key::XBOX_lthumb_downleft;

		m_keymap[VK_PAD_RTHUMB_UP]		= Key::XBOX_rthumb_up;
		m_keymap[VK_PAD_RTHUMB_DOWN]		= Key::XBOX_rthumb_down;
		m_keymap[VK_PAD_RTHUMB_RIGHT]	= Key::XBOX_rthumb_right;
		m_keymap[VK_PAD_RTHUMB_LEFT]		= Key::XBOX_rthumb_left;
		m_keymap[VK_PAD_RTHUMB_UPLEFT]	= Key::XBOX_rthumb_upleft;
		m_keymap[VK_PAD_RTHUMB_UPRIGHT]	= Key::XBOX_rthumb_upright;
		m_keymap[VK_PAD_RTHUMB_DOWNRIGHT] = Key::XBOX_rthumb_downright;
		m_keymap[VK_PAD_RTHUMB_DOWNLEFT] = Key::XBOX_rthumb_downleft;

//		gSystem->registerEventSource(this);
	}

	Xinput::~Xinput() {
//		gSystem->removeEventSource(this);
	}

	void Xinput::startCapture(InputSystem::CaptureMode capturemode) {
		m_isInCapture = true;
		TypeZeroArray(gControllers);
	}

	void Xinput::process() {
		AX_ASSERT(m_isInCapture);

		Event e;
		for (int i = 0; i < MAX_CONTROLLERS; i++) {
			XINPUT_STATE xstate;
			DWORD dwResult = XInputGetState(i, &xstate);

			if (dwResult != ERROR_SUCCESS) {
				gControllers[i].bConnected = false;
				continue;
			}

			// lthumb
			TypeZero(&e);

			e.type = Event::XboxAxis;
			e.pos.set(xstate.Gamepad.sThumbLX, xstate.Gamepad.sThumbLY);
			e.key = Key::XBOX_lthumb;
//			gSystem->queEvent(e);

			e.type = Event::XboxAxis;
			e.pos.set(xstate.Gamepad.sThumbRX, xstate.Gamepad.sThumbRY);
			e.key = Key::XBOX_rthumb;
//			gSystem->queEvent(e);

			e.type = Event::XboxAxis;
			e.pos.set(xstate.Gamepad.bLeftTrigger, 0);
			e.key = Key::XBOX_ltrigger;
//			gSystem->queEvent(e);

			e.type = Event::XboxAxis;
			e.pos.set(xstate.Gamepad.bRightTrigger, 0);
			e.key = Key::XBOX_rtrigger;
//			gSystem->queEvent(e);
		}

		while (true) {
			XINPUT_KEYSTROKE keystroke;
			DWORD dwResult = XInputGetKeystroke(XUSER_INDEX_ANY, XINPUT_FLAG_GAMEPAD, &keystroke);

			if (dwResult != ERROR_SUCCESS) {
				break;
			}

			TypeZero(&e);

			if (keystroke.Flags & XINPUT_KEYSTROKE_KEYDOWN) {
				e.type = Event::KeyDown;
			} else if (keystroke.Flags & XINPUT_KEYSTROKE_KEYUP) {
				e.type = Event::KeyUp;
			} else {
				continue;
			}
			
			e.key = m_keymap[keystroke.VirtualKey];

//			gSystem->queEvent(e);
		}
	}

	void Xinput::setVibration(float left, float right) {
	}

	void Xinput::stopCapture() {
		m_isInCapture = false;
	}

}} // namespace Axon::Input

