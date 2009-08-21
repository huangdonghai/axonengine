/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>

class TimeLine : public QGraphicsView
{
	Q_OBJECT

public:
	TimeLine(QWidget *parent);
	~TimeLine();

	int getTotalTime() const { return m_totalTime; }
	void setTotalTime(int val) { m_totalTime = val; }
	int getFrameTime() const { return m_frameTime; }
	void setFrameTime(int val) { m_frameTime = val; }
	int getMasterTime() const { return m_masterTime; }
	void setMasterTime(int val) { m_masterTime = val; }
	int getCurTime() const { return m_curTime; }
	void setCurTime(int val) { m_curTime = val; }

protected:
	void drawLine(QPainter* painter, const QRect& rect, int stepf, int len, bool draw_text);
	virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
	int m_totalTime;
	int m_frameTime;
	int m_masterTime;
	int m_curTime;
};

#endif // TIMELINE_H
