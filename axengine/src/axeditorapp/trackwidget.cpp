#include "trackwidget.h"

namespace {

	enum {
		TRACK_HEIGHT = 32
	};

	static const QColor s_background(64,64,64);
}

TrackWidgetItem::TrackWidgetItem()
{

}

TrackWidgetItem::TrackWidgetItem(TrackWidget* widget)
{

}

TrackWidgetItem::TrackWidgetItem(TrackWidgetItem* parent)
{

}

TrackWidgetItem::TrackWidgetItem(TrackWidgetItem* parent, Gfx::IAnimatable* anim)
{

}

TrackWidgetItem::TrackWidgetItem(TrackWidgetItem* parent, TrackWidgetItem* preceding)
{

}

TrackWidgetItem::~TrackWidgetItem()
{

}

void TrackWidgetItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	painter->drawText(0, 0, "track");
	painter->drawRect(0, 0, 50, 50);
	painter->drawText(0, 80, "track");
}

QRectF TrackWidgetItem::boundingRect() const
{
	return QRectF(0,0,1,1);
}

TrackWidget::TrackWidget(QWidget *parent)
	: QGraphicsView(parent)
{
	m_scene = new QGraphicsScene();
	setScene(m_scene);

	TrackWidgetItem* item = new TrackWidgetItem();
	m_scene->addItem(item);

//	this->setBackgroundBrush(s_background);
//	this->translate(100,100);
}

TrackWidget::~TrackWidget()
{

}

void TrackWidget::resizeEvent(QResizeEvent * event)
{
	// make sure the top-left is in (0,0) pos
	QSize size = event->size();
	setSceneRect(0,0,size.width(),size.height());

//	centerOn(size.width()/2, size.height()/2);
//	centerOn(0,0);
}
