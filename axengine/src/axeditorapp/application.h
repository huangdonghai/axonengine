/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

#include "private.h"

class Application : public QApplication, public ITickable
{
	Q_OBJECT

public:
	Application(int &argc, char **argv);
	~Application();

	virtual void tick();

protected:
	virtual void timerEvent(QTimerEvent * event);
	virtual bool winEventFilter(MSG * msg, long * result);

public slots:
	void myquit();

private:
	int m_timer0;
};

#endif // APPLICATION_H
