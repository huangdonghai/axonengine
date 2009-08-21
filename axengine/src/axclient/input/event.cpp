/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

#include "wininput.h"

namespace Axon { namespace Input {

	String Key::getName() const {
		if (t < 32) {
			Errorf("Error Key");
		}

		if (t >= 32 && t < 128) {
			return String(1,char(t));
		}

#define KEYITEM(key) case key: return #key; break;

		switch (t) {
			AX_KEYITEMS
		}
#undef KEYITEM

		return String();
	}

	int Key::getKey(const String& keyname)
	{
		if (keyname.empty()) {
			Errorf("error keyname");
		}

		if (keyname.size() == 1) {
			return keyname[0];
		}

		static Dict<String,int> nameDict;
		if (nameDict.empty()) {
#define KEYITEM(key) nameDict[#key] = key;
			AX_KEYITEMS
#undef KEYITEM
		}

		Dict<String,int>::const_iterator it = nameDict.find(keyname);
		if (it == nameDict.end()) {
			return 0;
		}

		return it->second;
	}

	//--------------------------------------------------------------------------
	// class IEventHandler
	//--------------------------------------------------------------------------

	void IEventHandler::handleEvent(Event* e) {
		preHandleEvent(e);

		onEvent(e);

		if (e->accepted)
			goto exit;

		switch (e->type) {
		case Event::KeyDown:
			{
				Event* ke = static_cast<Event*>(e);
				onKeyDown(ke);

				break;
			}

		case Event::KeyUp:
			{
				Event* ke = static_cast<Event*>(e);
				onKeyUp(ke);

				break;
			}
		case Event::MouseDown:
			{
				Event* me = static_cast<Event*>(e);
				onMouseDown(me);

				break;
			}

		case Event::MouseUp:
			{
				Event* me = static_cast<Event*>(e);
				onMouseUp(me);
	
				break;
			}

		case Event::MouseMove:
			{
				Event* me = static_cast<Event*>(e);
				onMouseMove(me);

				break;
			}

		case Event::Wheel:
			{
				Event* we = static_cast<Event*>(e);
				onMouseWheel(we);

				break;
			}
		}

exit:
		postHandleEvent(e);
	}

	void IEventHandler::onEvent(Event* e) {}
	void IEventHandler::onKeyDown(Event* e) {}
	void IEventHandler::onKeyUp(Event* e) {}
	void IEventHandler::onMouseDown(Event* e) {}
	void IEventHandler::onMouseUp(Event* e) {}
	void IEventHandler::onMouseMove(Event* e) {}
	void IEventHandler::onMouseWheel(Event* e) {}


	//--------------------------------------------------------------------------
	// class InputSystem
	//--------------------------------------------------------------------------

	InputSystem::InputSystem() {
		m_eventReadPos = 0;
		m_eventWritePos = 0;
		m_gameWnd = 0;
		m_isCapturing = false;

		g_system->registerTickable(System::TickEvent, this);
	}

	InputSystem::~InputSystem() {
		g_system->removeTickable(System::TickEvent, this);
	}

	void InputSystem::initialize() {
		m_winInput = new Input::WinInput;
	}

	void InputSystem::finalize() {
		SafeDelete(m_winInput);
	}

	void InputSystem::tick()
	{
		if (!m_isCapturing) {
			return;
		}

		processEvents();
	}

	void InputSystem::queEvent(const Event& e) {
		if (!m_isCapturing) {
			return;
		}

		if (m_eventWritePos - m_eventReadPos > EVENT_POOL_SIZE) {
			Debugf("%s: event overflowed.\n", __func__);
			return;
		}

		m_events[m_eventWritePos&(EVENT_POOL_SIZE-1)] = e;
		m_eventWritePos++;
	}

	Event* InputSystem::getEvent() {
		if (m_eventReadPos == m_eventWritePos) {
			return nullptr;
		}

		return &m_events[m_eventReadPos++ & (EVENT_POOL_SIZE-1)];
	}

	void InputSystem::clearEvents() {
		m_eventReadPos = m_eventWritePos;
	}

	void InputSystem::startCapture(CaptureMode mode) {
		m_isCapturing = true;
		m_captureMode = mode;

		List<IEventSource*>::iterator it = m_eventSources.begin();

		m_winInput->setWindow(m_gameWnd);
		m_winInput->startCapture(mode);

		for (; it != m_eventSources.end(); ++it) {
			(*it)->startCapture(mode);
		}
	}

	void InputSystem::processEvents() {
		List<IEventSource*>::iterator it = m_eventSources.begin();

		m_winInput->process();

		for (; it != m_eventSources.end(); ++it) {
			(*it)->process();
		}
	}

	void InputSystem::setVibration(float left, float right) {
		List<IEventSource*>::iterator it = m_eventSources.begin();

		for (; it != m_eventSources.end(); ++it) {
			(*it)->setVibration(left, right);
		}
	}

	void InputSystem::setMouseMode(MouseMode mode)
	{
		List<IEventSource*>::iterator it = m_eventSources.begin();

		for (; it != m_eventSources.end(); ++it) {
			(*it)->setMouseMode(mode);
		}
	}

	void InputSystem::stopCapture() {
		List<IEventSource*>::iterator it = m_eventSources.begin();

		for (; it != m_eventSources.end(); ++it) {
			(*it)->stopCapture();
		}

		m_winInput->stopCapture();

		m_isCapturing = false;
	}

	void InputSystem::registerEventSource(IEventSource* eventsource) {
		m_eventSources.push_back(eventsource);
	}

	void InputSystem::removeEventSource(IEventSource* eventsource) {
		m_eventSources.remove(eventsource);
	}

	void InputSystem::setGameWindow(Render::Target* gamewindow)
	{
		m_gameWnd = gamewindow;
	}

	void InputSystem::queWinInput(void* msg)
	{
		m_winInput->queWinInput((MSG*)msg);
	}

}} // namespace Axon::Input

