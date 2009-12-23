/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXWORKSPACE_H
#define CXWORKSPACE_H

#include "private.h"
#include "ui_workspace.h"
#include "frame.h"
#include "editorframe.h"

class CrossSplitter;
class Workspace;

class IPanelLayout {
public:
	virtual ~IPanelLayout() {}

	virtual int getNumFrames();
	virtual int getActiveFrames();
};

class Grabber : public QWidget {
	Q_OBJECT

public:
	enum Tracking { None, Vertical, Horizontal, Both };

	Grabber(CrossSplitter* parent, Tracking tracking);

protected:
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void leaveEvent(QEvent * event);

private:
	CrossSplitter* m_parent;
	Tracking m_tracking;
	bool m_isTracking;
};

class CrossSplitter : public QFrame {
	Q_OBJECT

public:
	typedef Grabber::Tracking Tracking;

	CrossSplitter(QWidget* parent, Workspace* ws);
	~CrossSplitter();

	// frame maximum
	void setFrameMaximun(int index, bool maximum);

	// tracking
	void beginTracking(Tracking tracking, const QPoint& pos);
	void doTracking(const QPoint& pos);
	void endTracking(const QPoint& pos);

protected:
	void relayout();

	// implement QT event handler
	virtual void paintEvent(QPaintEvent*);
	virtual void resizeEvent(QResizeEvent* event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void leaveEvent(QEvent * event);

private:
	enum { TrackingWidth = 4 };

	QPointF m_splitPos;
	Workspace* m_workspace;
	QPoint m_beginTrackPos;
	Tracking m_tracking;

	// layout info
	QRect m_frameRects[4];
	QRect m_bothResizeRect;
	QRect m_horResizeRects[2];
	QRect m_verResizeRects[2];

	// grabber widget
	Grabber* m_horGrabbers[2];
	Grabber* m_verGrabbers[2];
	Grabber* m_bothGrabber;

	// maximum
	bool m_frameMaximumed;
	int m_maximumIndex;
};

class Workspace : public QWidget, public IObserver
{
    Q_OBJECT

public:
	friend class Workbench;

    Workspace(QWidget *parent = 0);
    ~Workspace();

	Frame* getActiveFrame();
	int getActiveFrameIndex();
	Frame* getFrame(int idx) { return m_frames[idx]; }
	void frameActive(Frame* frame);
	void toggleMaximun();

	// IObserver
	virtual void doNotify(IObservable* subject, int arg);

	void refreshBookmarks();

private slots:
	void on_horizontalSlider_valueChanged(int);
	void on_bookmarkDel_released();
	void on_bookmarkAdd_released();
	void on_bookmarkGoto_released();
	void on_bookmarkList_editTextChanged(QString);
	void on_bookmarkList_currentIndexChanged(int);
	void on_transformZ_editingFinished();
	void on_transformY_editingFinished();
	void on_transformX_editingFinished();
	void on_transformAbsOrRel_clicked();
	void on_moveSpeed_valueChanged(int);
	void on_actionToggleFrameMaximum_triggered();

private:
    Ui::WorkspaceClass ui;

	CrossSplitter* m_frameLayouter;
	Frame* m_frames[4];
	EditorFrame* m_editorFrames[View::MaxView];
	bool m_frameMaximumed;
	int m_maximunIndex;
};


#endif // CXWORKSPACE_H
