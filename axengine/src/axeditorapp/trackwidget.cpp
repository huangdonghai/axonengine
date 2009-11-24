#include "trackwidget.h"

namespace {

	enum {
		TRACK_HEIGHT = 32,
		LEFT_WIDTH = 200
	};

	static const QColor s_background(64,64,64);
}

TrackWidgetItem::TrackWidgetItem() : m_widget(0), m_parent(0)
{

}

TrackWidgetItem::TrackWidgetItem(TrackWidget* widget) : m_widget(widget), m_parent(0)
{

}

TrackWidgetItem::TrackWidgetItem(TrackWidgetItem* parent) : m_widget(0), m_parent(parent)
{
	if (m_parent)
		m_widget = m_parent->m_widget;
}

TrackWidgetItem::TrackWidgetItem(TrackWidgetItem* parent, Gfx::IAnimatable* anim) : m_widget(0), m_parent(parent)
{
	if (m_parent)
		m_widget = m_parent->m_widget;
}

TrackWidgetItem::TrackWidgetItem(TrackWidgetItem* parent, TrackWidgetItem* preceding) : m_widget(0), m_parent(parent)
{
	if (m_parent)
		m_widget = m_parent->m_widget;
}

TrackWidgetItem::~TrackWidgetItem()
{

}

void TrackWidgetItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	QRectF rect = m_widget->sceneRect();
	QRectF left_rect(0, 0, LEFT_WIDTH, TRACK_HEIGHT);

	const QPalette& pal = m_widget->palette();

	painter->setPen(pal.color(QPalette::Window));
	if (m_trackIndex % 2) {
		painter->setBrush(pal.alternateBase());
	} else {
		painter->setBrush(pal.base());
	}
	painter->drawRect(0, 0, LEFT_WIDTH, TRACK_HEIGHT);
	painter->drawRect(LEFT_WIDTH, 0, rect.width() - LEFT_WIDTH, TRACK_HEIGHT);

	painter->drawText(left_rect, Qt::AlignCenter, "track");
}

QRectF TrackWidgetItem::boundingRect() const
{
	return QRectF(0,0,1,1);
}

void TrackWidgetItem::setTrackIndex( int index )
{
	m_trackIndex = index;
	relayout();
}

void TrackWidgetItem::relayout()
{
	setPos(0, TRACK_HEIGHT * m_trackIndex);
}

TrackWidget::TrackWidget(QWidget *parent)
	: QGraphicsView(parent)
{
	m_scene = new QGraphicsScene();
	setScene(m_scene);

	TrackWidgetItem* item = new TrackWidgetItem();
	addTrackItem(item);
	addTrackItem(new TrackWidgetItem());

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

	relayout();
}

void TrackWidget::addTrackItem(TrackWidgetItem* track)
{
	m_tracks.push_back(track);
	m_scene->addItem(track);
	track->m_widget = this;

	relayout();
}

void TrackWidget::relayout()
{
	QList<TrackWidgetItem*>::const_iterator it = m_tracks.begin();

	int track_index = 0;

	for (; it != m_tracks.end(); ++it) {
		TrackWidgetItem* track = *it;
		track->setTrackIndex(track_index);
		track_index++;
	}
}

