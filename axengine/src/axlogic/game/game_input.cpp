/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace {
	static const float MOUSE_SENSITIVITY_ADJUST = 7.5f;
}

AX_BEGIN_NAMESPACE

	AX_BEGIN_COMMAND_MAP(GameInput)
		AX_COMMAND_ENTRY("bind",	bind_f)
		AX_COMMAND_ENTRY("bindlist",	bindlist_f)
	AX_END_COMMAND_MAP()

	GameInput::GameInput()
	{
		g_cmdSystem->registerHandler(this);
	}

	GameInput::~GameInput()
	{
		g_cmdSystem->removeHandler(this);
	}

	void GameInput::initialize()
	{
		// read from system config
		StringPairSeq spv = g_systemConfig->getItems("Binding");
		StringPairSeq::const_iterator it;
		for (it = spv.begin(); it != spv.end(); ++it) {
			Key k = Key::getKey(it->first);
			if (!k) {
				continue;
			}

			m_keybinding[k] = it->second;
		}
	}

	void GameInput::finalize()
	{
		// write to system config

	}

	void GameInput::runFrame(int msec)
	{
		m_frameMsec = msec;

		while (Input::Event* e = g_inputSystem->getEvent()) {
			bool isdown = false;
			switch (e->type) {
			case Input::Event::KeyDown:
				isdown = true;
				break;
			case Input::Event::KeyUp:
				isdown = false;
				break;
			case Input::Event::MouseDown:
				isdown = true;
				break;
			case Input::Event::MouseUp:
				isdown = false;
				break;
			case Input::Event::MouseMove:
				{
					if (!m_mousePosInited) {
						m_mouseDelta[0].set(0,0);
						m_mouseDelta[1].set(0,0);
						m_mouseIndex = 0;
						m_mousePosInited = true;
					} else {
						m_mouseDelta[m_mouseIndex] += e->pos;
					}
					continue;
				}

			case Input::Event::Char:
			case Input::Event::Wheel:
				{
					Key key = Key::MouseWheelUp;
					if (e->delta<0)
						key = Key::MouseWheelDown;

					Dict<Key,String>::const_iterator it = m_keybinding.find(key);
					if (it == m_keybinding.end()) {
						continue;
					}

					this->invokeMethod(it->second.c_str(), (int)e->key, true);
					this->invokeMethod(it->second.c_str(), (int)e->key, false);
					continue;
				}
			case Input::Event::XboxAxis:
				continue;
			}

			Dict<Key,String>::const_iterator it = m_keybinding.find(e->key);
			if (it == m_keybinding.end()) {
				continue;
			}

			this->invokeMethod(it->second.c_str(), (int)e->key, isdown);
		}
	}

	UserInput GameInput::genUserInput()
	{
		UserInput userInput;

		TypeZero(&userInput);

		genKeyMove(userInput);

		genMouseMove(userInput);

		genButtons(userInput);

		return userInput;
	}

	void GameInput::genKeyMove(UserInput& userInput)
	{
		int movespeed;
		int forward, side, up;

		//
		// adjust for speed key / running
		// the walking flag is to keep animations consistent
		// even during acceleration and develeration
		//
		movespeed = 127;

		forward = 0;
		side = 0;
		up = 0;

		side += movespeed * buttonState(MoveRight);
		side -= movespeed * buttonState(MoveLeft);

		up += movespeed * buttonState(MoveUp);
		up -= movespeed * buttonState(MoveDown);

		forward += movespeed * buttonState(MoveForward);
		forward -= movespeed * buttonState(MoveBackward);

		userInput.forwardmove = Math::clampSbyte(forward);
		userInput.rightmove = Math::clampSbyte(side);
		userInput.upmove = Math::clampSbyte(up);
	}

	void GameInput::genMouseMove(UserInput& userInput)
	{
		float mx, my;
		float accelSensitivity;
		float rate;

		// allow mouse smoothing
		if (mouse_smooth->getBool()) {
			mx = (m_mouseDelta[0].x + m_mouseDelta[1].x) * 0.5f;
			my = (m_mouseDelta[0].y + m_mouseDelta[1].y) * 0.5f;
		} else {
			mx = m_mouseDelta[m_mouseIndex].x;
			my = m_mouseDelta[m_mouseIndex].y;
		}
		m_mouseIndex ^= 1;
		m_mouseDelta[m_mouseIndex].set(0, 0);

		rate = sqrt(mx*mx + my*my) / (float)m_frameMsec;
		accelSensitivity = mouse_sensitivity->getFloat() * MOUSE_SENSITIVITY_ADJUST + rate * mouse_accel->getFloat();

		mx *= accelSensitivity;
		my *= accelSensitivity;

		if (!mx && !my) {
			return;
		}

		// add mouse X/Y movement to cmd
		userInput.angles[Angles::Yaw] -= mouse_yaw->getFloat() * mx;
		userInput.angles[Angles::Pitch] += mouse_pitch->getFloat() * my * (mouse_invert->getBool() ? -1 : 1);
	}

	void GameInput::genButtons(UserInput& userInput)
	{
		// buttons
		if (m_buttons[Rush].isActive) {
			userInput.buttons |= UserInput::Rush;
		}

		// triggers
		for (int i = 0; i < UserInput::Trigger_Max; i++) {
			if (m_buttons[TriggerStart+i].wasPressed) {
				userInput.trigger = i;
				m_buttons[TriggerStart+i].wasPressed = false;
				return;
			}
		}
	}


	void GameInput::buttonDown(int bt, Key key, int msec)
	{
		Button& b = m_buttons[bt];

		if (key == b.keys[0] || key == b.keys[1]) {
			return;		// repeating key
		}

		if (!b.keys[0]) {
			b.keys[0] = key;
		} else if (!b.keys[1]) {
			b.keys[1] = key;
		} else {
			Printf ("Three keys down for a button!\n");
			return;
		}

		if (b.isActive) {
			return;		// still down
		}

		// save timestamp for partial frame summing
		b.downtime = msec;

		b.isActive = true;
		b.wasPressed = true;
	}

	void GameInput::buttonUp(int bt, Key key, int msec)
	{
		Button& b = m_buttons[bt];

		if (b.keys[0] == key) {
			b.keys[0] = 0;
		} else if (b.keys[1] == key) {
			b.keys[1] = 0;
		} else {
			return;		// key up without corresponding down (menu pass through)
		}
		if (b.keys[0] || b.keys[1]) {
			return;		// some other key is still holding it down
		}

		// save timestamp for partial frame summing
		b.msec += msec - b.downtime;

		b.isActive = false;
	}

	float GameInput::buttonState(int bt)
	{
		float val;
		int msec;

		Button& b = m_buttons[bt];

		msec = b.msec;
		b.msec = 0;

		if (b.isActive) {
			val = 1.0f;
		} else {
			val = 0.0f;
		}

		return Math::saturate(val);
	}

	void GameInput::bind_f(const CmdArgs& args)
	{
		const StringSeq& tokens = args.tokened;

		if (tokens.size() < 2) {
			return;
		}

		Key k = Key::getKey(tokens[1]);
		if (tokens.size() == 2) {
			Dict<Key,String>::const_iterator it = m_keybinding.find(k);
			if (it == m_keybinding.end()) {
				return;
			}

			Printf("'%s' is bind to command '%s'\n", k.getName().c_str(), it->second.c_str());
			return;
		}

		m_keybinding[k] = tokens[2];
	}

	void GameInput::bindlist_f(const CmdArgs& args)
	{
		Dict<Key,String>::const_iterator it = m_keybinding.begin();

		int count = 0;
		while (it != m_keybinding.end()) {
			Printf("'%s'='%s'\n", it->first.getName().c_str(), it->second.c_str());
			count++;
			++it;
		}

		Printf("total %d bindings\n", count);
	}

	void GameInput::pauseGame(int key, bool isdown)
	{
		g_gameSystem->stopRunning();
	}

	void GameInput::moveForward(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(MoveForward, key, 0);
		} else {
			buttonUp(MoveForward, key, 0);
		}
	}

	void GameInput::moveBackward(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(MoveBackward, key, 0);
		} else {
			buttonUp(MoveBackward, key, 0);
		}
	}

	void GameInput::moveLeft(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(MoveLeft, key, 0);
		} else {
			buttonUp(MoveLeft, key, 0);
		}
	}

	void GameInput::moveRight(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(MoveRight, key, 0);
		} else {
			buttonUp(MoveRight, key, 0);
		}
	}

	void GameInput::moveUp(int key, bool isdown)
	{

	}

	void GameInput::moveDown(int key, bool isdown)
	{

	}

	void GameInput::moveRush(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(Rush, key, 0);
		} else {
			buttonUp(Rush, key, 0);
		}
	}

	void GameInput::startRunning()
	{
		m_mousePosInited = false;
		m_mouseIndex = 0;
		m_mouseDelta[0].set(0,0);
		m_mouseDelta[1].set(0,0);
		TypeZeroArray(m_buttons);
	}

	void GameInput::stopRunning()
	{

	}

	void GameInput::toggleView(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(TriggerStart + UserInput::Trigger_ToggleView, key, 0);
		} else {
			buttonUp(TriggerStart + UserInput::Trigger_ToggleView, key, 0);
		}
	}

	void GameInput::zoomInView(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(TriggerStart + UserInput::Trigger_ZoomIn, key, 0);
		} else {
			buttonUp(TriggerStart + UserInput::Trigger_ZoomIn, key, 0);
		}
	}

	void GameInput::zoomOutView(int key, bool isdown)
	{
		if (isdown) {
			buttonDown(TriggerStart + UserInput::Trigger_ZoomOut, key, 0);
		} else {
			buttonUp(TriggerStart + UserInput::Trigger_ZoomOut, key, 0);
		}
	}

AX_END_NAMESPACE

