/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include "ui_frame.h"

class Workspace;

class IPanel {
public:
	virtual QString getTitle() = 0;
	virtual QWidget *getWidget() = 0;
};

class Frame : public QFrame
{
	Q_OBJECT

public:
	Frame(QWidget *parent, Workspace *ws);
	~Frame();

	void setEditorActive(bool a) { m_editorActive = a; }
	bool isActived() { return m_editorActive; }
	void setPanel(IPanel *panel);

protected:
	// Qt event
	virtual void focusInEvent(QFocusEvent * event);
	virtual void paintEvent(QPaintEvent *);
	virtual void resizeEvent (QResizeEvent * event);

private:
	Ui::FrameClass ui;

	Workspace *m_workspace;
	bool m_editorActive;
	IPanel *m_panel;

private slots:
	void on_toggleMax_clicked();
};

#endif // FRAME_H
