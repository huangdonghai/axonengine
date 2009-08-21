/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "application.h"
#include "workbench.h"

Application::Application(int &argc, char **argv)
	: QApplication(argc, argv)
{
	m_timer0 = startTimer(0);
}

Application::~Application()
{

}

void Application::tick() {
	QApplication::processEvents();
}

void Application::myquit() {
//	QApplication::quit();
	g_system->setExitable();
}

void Application::timerEvent(QTimerEvent * event)
{
	if (event->timerId() != m_timer0) {
		return;
	}

	if (g_system) {
		if (!g_workbench->isActiveWindow()) {
			g_system->forceTick(30);
		} else {
			g_system->forceTick(0);
		}
	}
}

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

bool Application::winEventFilter(MSG * msg, long * result)
{
	if (msg->message != WM_INPUT) {
		return false;
	}

	g_inputSystem->queWinInput(msg);
	return true;
}
