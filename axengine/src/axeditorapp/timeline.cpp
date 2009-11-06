/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "timeline.h"
#include "workbench.h"

enum {
	MIN_DELTA = 8,
	TEXT_DELTA = 45,
	PAD_LEFT = 8,
	PAD_RIGHT = 32,
	PAD = PAD_LEFT + PAD_RIGHT
};

static int s_pads[] = { 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000 };

static int nextPad(int x)
{
	for (int i = 0; i < ArraySize(s_pads); i++) {
		if (x < s_pads[i])
			return s_pads[i];
	}

	Errorf("over flowed");
	return 0;
}

TimeLine::TimeLine(QWidget *parent)
	: QGraphicsView(parent)
{
	m_totalTime = 5000;
	m_frameTime = 50;
	m_curTime = 0;

	QGraphicsScene *scene = new QGraphicsScene(this);
	setScene(scene);
}

TimeLine::~TimeLine()
{

}

void TimeLine::drawBackground(QPainter *painter, const QRectF &rectf)
{
	QRect rect = rectf.toRect();

	m_frameCount = m_totalTime / m_frameTime;
	int totalPixels = rect.width() - PAD;
	m_framePad = (float)totalPixels / m_frameCount;

	int baseStep = 1;

	while (baseStep * m_framePad < MIN_DELTA)
		baseStep = nextPad(baseStep);

	drawLine(painter, rect, baseStep, 11, false);

	// draw text
	int textStep = nextPad(baseStep);
	textStep = nextPad(textStep);

	while (textStep * m_framePad < TEXT_DELTA) {
		textStep = nextPad(textStep);
	}
	drawLine(painter, rect, textStep, 22, true);

#if 0
	int step = drawLine(painter, rect, m_frameTime, 11, false);
	int masterStep = m_masterTime * step;

	drawLine(painter, rect, masterStep, 24, true);
#endif
}

void TimeLine::drawLine(QPainter* painter, const QRect& rect, int stepf, int len, bool draw_text)
{
	// draw step line
	painter->setPen(Qt::gray);

	for (int i = 0; i <= m_frameCount; i += stepf) {
		painter->setPen(Qt::gray);
		if (!draw_text) {
			int x = PAD_LEFT + rect.x() + i * m_framePad;
			painter->drawLine(x, rect.y(), x, rect.y() + len);
			continue;
		}

		int x = PAD_LEFT + rect.x() + i * m_framePad;
		painter->drawLine(x, rect.y(), x, rect.y() + len - 8);

		painter->setPen(Qt::black);
		QRect textRect(x - 23, rect.y()+18, 49, 8);
		QString msg = QString("%1").arg(i);
		painter->drawText(textRect, Qt::AlignCenter, msg);
	}
}

