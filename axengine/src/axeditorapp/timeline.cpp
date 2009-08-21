/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "timeline.h"

enum {
	MIN_DELTA = 8,
	PAD_LEFT = 8,
	PAD_RIGHT = 32,
	PAD = PAD_LEFT + PAD_RIGHT
};

TimeLine::TimeLine(QWidget *parent)
	: QGraphicsView(parent)
{
	m_totalTime = 4000;
	m_frameTime = 40;
	m_masterTime = 200;
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

//	painter->setPen(Qt::darkGray);
//	painter->drawLine(rect.x(), rect.y(), rect.right(), rect.y());

//	painter->setPen(Qt::white);
//	painter->drawLine(rect.x(), rect.bottom(), rect.right(), rect.bottom());

	drawLine(painter, rect, m_frameTime, 11, false);
	drawLine(painter, rect, m_masterTime, 24, true);

#if 0
	// Shadow
	QRectF sceneRect = rect;
	QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
	QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
	if (rightShadow.intersects(rect) || rightShadow.contains(rect))
		painter->fillRect(rightShadow, Qt::darkGray);
	if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
		painter->fillRect(bottomShadow, Qt::darkGray);

	// Fill
	QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
	gradient.setColorAt(0, Qt::white);
	gradient.setColorAt(1, Qt::lightGray);
	painter->fillRect(rect.intersect(sceneRect), gradient);
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(sceneRect);

	// Text
	QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
		sceneRect.width() - 4, sceneRect.height() - 4);
	QString message(tr("Click and drag the nodes around, and zoom with the mouse "
		"wheel or the '+' and '-' keys"));

	QFont font = painter->font();
	font.setBold(true);
	font.setPointSize(14);
	painter->setFont(font);
	painter->setPen(Qt::lightGray);
	painter->drawText(textRect.translated(2, 2), message);
	painter->setPen(Qt::black);
	painter->drawText(textRect, message);
#endif
}

void TimeLine::drawLine( QPainter* painter, const QRect& rect, int stepf, int len, bool draw_text )
{
	const QFont& font = painter->font();

	int totalSteps = m_totalTime / stepf;

	int showSteps = (rect.width() - PAD) / MIN_DELTA;
	int step = totalSteps / showSteps;
	if (step < 1) step = 1;

	// draw step line
	painter->setPen(Qt::gray);
	for (int i = 0; i <= totalSteps; i += step) {

		painter->setPen(Qt::gray);
		if (!draw_text) {
			int x = PAD_LEFT + rect.x() + i * (rect.width() - PAD) / totalSteps;
			painter->drawLine(x, rect.y(), x, rect.y() + len);
			continue;
		}

		int x = PAD_LEFT + rect.x() + i * (rect.width() - PAD) / totalSteps;
		painter->drawLine(x, rect.y(), x, rect.y() + len - 8);

		painter->setPen(Qt::black);
		QRect textRect(x - 23, rect.y()+18, 49, 8);
		QString msg = QString("%1").arg(i * stepf / m_frameTime);
		painter->drawText(textRect, Qt::AlignCenter, msg);
	}
}
