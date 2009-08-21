/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef ROLLUP_H
#define ROLLUP_H

#include <QFrame>

class Rollup;

//------------------------------------------------------------------------------
// class RollPage
//------------------------------------------------------------------------------
class RollPage : public QWidget {
	Q_OBJECT
public:
	RollPage(QWidget* parent, Rollup* rollup_widget, QWidget* contained, const QString& name);
	~RollPage();

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent * e);

private slots:
	void buttonClicked(bool);

private:
	QString m_pageName;
	QWidget* m_containedWidget;
	Rollup* m_rollupWidget;
#if 0
	QPushButton* m_button;
	QLabel* m_expandWidget;
	QLabel* m_shrinkWidget;
#endif
	bool m_isHidden;
	bool m_isShrinked;
	QPixmap m_leftPixmap;
	QPixmap m_downPixmap;
	int m_margin;
};

//------------------------------------------------------------------------------
// class RollArea
//------------------------------------------------------------------------------
class RollArea : public QWidget {
	Q_OBJECT

public:
	RollArea(Rollup* parent);
	~RollArea();

private:
	// overload
	virtual bool eventFilter(QObject * watched, QEvent * event);
	virtual void mouseMoveEvent (QMouseEvent * e);

private:
	Rollup* m_parent;
};

//------------------------------------------------------------------------------
// class Rollup
//------------------------------------------------------------------------------
class Rollup : public QFrame {
	Q_OBJECT

public:
	Rollup(QWidget *parent);
	~Rollup();

	void initialize();
	int addPage(QWidget* page, QString& name);	// return index
	void removeAllPages();
	void setAllPagesVisible(bool visible);
	void hidePage(int index);
	void showPage(int index);
	void setPageVisible(int index, bool visible);
	void setGroupVisible(const QString& groupname, bool visible);

public:
	void relayout();

private:
	void checkScrollPos();
	bool initFromTab();
	bool initFromToolbox();
	bool initGroup(const QString& grpname, QWidget* widget);

protected:
	// overload
	virtual bool event(QEvent *e);
	virtual void paintEvent(QPaintEvent *);
	virtual void mouseMoveEvent(QMouseEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseReleaseEvent(QMouseEvent * e);
	virtual void wheelEvent(QWheelEvent * e);
	virtual void resizeEvent(QResizeEvent * e);

	private slots:

private:
	enum {
		SCROLL_WIDTH = 4
	};

	enum ScrollTracking {
		TrackingNone, TrackingScrollbar, TrackingPage
	};

	typedef QVector<RollPage*> RollPages;
	typedef QHash<QString,QVector<int>> RollGroups;

	QVector<RollPage*> m_pages;
	RollGroups m_groups;
	RollArea* m_rollarea;
	QWidget* m_central;
	ScrollTracking m_trackingMode;
	int m_curScrollPos;
	int m_totalPagesHeight;
	QColor m_scrollBarColor, m_scrollBarSliderColor;
	QRect m_scrollRect, m_scrollSliderRect;
	bool m_scrollMouseDown;
	QPoint m_trackingPos;
};

#endif // ROLLUP_H
