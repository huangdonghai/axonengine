/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "customlabel.h"

CustomLabel::CustomLabel(QWidget *parent)
	: QLabel(parent)
{

}

CustomLabel::~CustomLabel()
{

}

void CustomLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	//if (ev->type() == QMouseEvent::MouseButtonDblClick)
	{
		emit mouseDoubleClicked(QPoint(ev->pos()));
	}
}