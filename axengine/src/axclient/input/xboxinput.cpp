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

AX_BEGIN_NAMESPACE

	struct ControlerState {
		XINPUT_STATE    state;       
		bool            bConnected;
	};

	ControlerState gControllers[MAX_CONTROLLERS];

	Xinput::Xinput() {
		m_isInCapture = false;
		TypeZeroArray(gControllers);

		m_keymap[VK_PAD_A]			= InputKey::XBOX_A;
		m_keymap[VK_PAD_B]			= InputKey::XBOX_B;
		m_keymap[VK_PAD_X]			= InputKey::XBOX_X;
		m_keymap[VK_PAD_Y]			= InputKey::XBOX_Y;
		m_keymap[VK_PAD_RSHOULDER]	= InputKey::XBOX_rshoulder;
		m_keymap[VK_PAD_LSHOULDER]	= InputKey::XBOX_lshoulder;
		m_keymap[VK_PAD_LTRIGGER]	= InputKey::XBOX_ltrigger;
		m_keymap[VK_PAD_RTRIGGER]	= InputKey::XBOX_rtrigger;

		m_keymap[VK_PAD_DPAD_UP]		= InputKey::XBOX_dpad_up;
		m_keymap[VK_PAD_DPAD_DOWN]	= InputKey::XBOX_dpad_down;
		m_keymap[VK_PAD_DPAD_LEFT]	= InputKey::XBOX_dpad_left;
		m_keymap[VK_PAD_DPAD_RIGHT]	= InputKey::XBOX_dpad_right;
		m_keymap[VK_PAD_START]		= InputKey::XBOX_start;
		m_keymap[VK_PAD_BACK]		= InputKey::XBOX_back;
		m_keymap[VK_PAD_LTHUMB_PRESS] = InputKey::XBOX_lthumb_press;
		m_keymap[VK_PAD_RTHUMB_PRESS] = InputKey::XBOX_rthumb_press;

		m_keymap[VK_PAD_LTHUMB_UP]	= InputKey::XBOX_lthumb_up;
		m_keymap[VK_PAD_LTHUMB_DOWN] = InputKey::XBOX_lthumb_down;
		m_keymap[VK_PAD_LTHUMB_RIGHT] = InputKey::XBOX_lthumb_right;
		m_keymap[VK_PAD_LTHUMB_LEFT] = InputKey::XBOX_lthumb_left;
		m_keymap[VK_PAD_LTHUMB_UPLEFT]	= InputKey::XBOX_lthumb_upleft;
		m_keymap[VK_PAD_LTHUMB_UPRIGHT]	= InputKey::XBOX_lthumb_upright;
		m_keymap[VK_PAD_LTHUMB_DOWNRIGHT]	= InputKey::XBOX_lthumb_downright;
		m_keymap[VK_PAD_LTHUMB_DOWNLEFT] = InputKey::XBOX_lthumb_downleft;

		m_keymap[VK_PAD_RTHUMB_UP]		= InputKey::XBOX_rthumb_up;
		m_keymap[VK_PAD_RTHUMB_DOWN]		= InputKey::XBOX_rthumb_down;
		m_keymap[VK_PAD_RTHUMB_RIGHT]	= InputKey::XBOX_rthumb_right;
		m_keymap[VK_PAD_RTHUMB_LEFT]		= InputKey::XBOX_rthumb_left;
		m_keymap[VK_PAD_RTHUMB_UPLEFT]	= InputKey::XBOX_rthumb_upleft;
		m_keymap[VK_PAD_RTHUMB_UPRIGHT]	= InputKey::XBOX_rthumb_upright;
		m_keymap[VK_PAD_RTHUMB_DOWNRIGHT] = InputKey::XBOX_rthumb_downright;
		m_keymap[VK_PAD_RTHUMB_DOWNLEFT] = InputKey::XBOX_rthumb_downleft;

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

		InputEvent e;
		for (int i = 0; i < MAX_CONTROLLERS; i++) {
			XINPUT_STATE xstate;
			DWORD dwResult = XInputGetState(i, &xstate);

			if (dwResult != ERROR_SUCCESS) {
				gControllers[i].bConnected = false;
				continue;
			}

			// lthumb
			TypeZero(&e);

			e.type = InputEvent::XboxAxis;
			e.pos.set(xstate.Gamepad.sThumbLX, xstate.Gamepad.sThumbLY);
			e.key = InputKey::XBOX_lthumb;
//			gSystem->queEvent(e);

			e.type = InputEvent::XboxAxis;
			e.pos.set(xstate.Gamepad.sThumbRX, xstate.Gamepad.sThumbRY);
			e.key = InputKey::XBOX_rthumb;
//			gSystem->queEvent(e);

			e.type = InputEvent::XboxAxis;
			e.pos.set(xstate.Gamepad.bLeftTrigger, 0);
			e.key = InputKey::XBOX_ltrigger;
//			gSystem->queEvent(e);

			e.type = InputEvent::XboxAxis;
			e.pos.set(xstate.Gamepad.bRightTrigger, 0);
			e.key = InputKey::XBOX_rtrigger;
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
				e.type = InputEvent::KeyDown;
			} else if (keystroke.Flags & XINPUT_KEYSTROKE_KEYUP) {
				e.type = InputEvent::KeyUp;
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

AX_END_NAMESPACE

