/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "frame.h"
#include "workspace.h"

Frame::Frame(QWidget *parent, Workspace *ws)
	: QFrame(parent)
	, m_workspace(ws)
	, m_editorActive(false)
	, m_panel(nullptr)
{
	ui.setupUi(this);
}

Frame::~Frame()
{
}

void Frame::focusInEvent(QFocusEvent *event) {
	m_workspace->frameActive(this);
}

void Frame::setPanel(IPanel *panel) {
	if (m_panel) {
		m_panel->getWidget()->setParent(0);
		m_panel->getWidget()->hide();
	}
	m_panel = panel;
	ui.panelContainer->layout()->addWidget(m_panel->getWidget());
	ui.label->setText(m_panel->getTitle());
}

void Frame::paintEvent(QPaintEvent *event) {
	QFrame::paintEvent(event);

	QPen pen;

	if (m_editorActive)
		pen.setColor(QColor(230,139,44,255));
	else
		pen.setColor(QColor(54,63,75,255));

	pen.setWidth(4);

	QSize s = size();
	QPainter painter(this);

	painter.setPen(pen);
	QRect rect(0, 0, s.width(), s.height());

	painter.drawRect(rect);
}

void Frame::resizeEvent (QResizeEvent * event) {
	if (!m_panel) {
		return;
	}

	QSize size = ui.panelContainer->size();

	QString str = QString("[%1x%2]").arg(size.width()).arg(size.height());
	ui.viewSize->setText(str);
}


void Frame::on_toggleMax_clicked()
{
	m_workspace->frameActive(this);
	m_workspace->toggleMaximun();
}
