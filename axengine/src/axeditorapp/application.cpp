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
	m_engTimer = new QTimer();

//	connect(m_engTimer, SIGNAL(timeout()), this, SLOT(engineTick()));

	setUpdate(0);
//	m_engTimer->start();
}

Application::~Application()
{
}

void Application::tick()
{
	QApplication::processEvents();
}

void Application::myQuit()
{
//	QApplication::quit();
	g_system->setExitable();
}

void Application::timerEvent(QTimerEvent * event)
{
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

void Application::setUpdate(int ms)
{
	if (m_engTimer->interval() == ms)
		return;

	m_engTimer->setInterval(ms);
}

void Application::engineTick()
{
	if (g_system) {
		if (!g_workbench->isActiveWindow()) {
			setUpdate(30);
		} else {
			setUpdate(0);
		}
		g_system->forceTick(0);
	}
}
