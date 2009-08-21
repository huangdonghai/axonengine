/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>

class CustomLabel : public QLabel
{
	Q_OBJECT

public:
	CustomLabel(QWidget *parent);
	~CustomLabel();

	void mouseReleaseEvent(QMouseEvent *ev);

Q_SIGNALS:
	void mouseDoubleClicked(QPoint pos);

private:
	
};

#endif // CUSTOMLABEL_H
