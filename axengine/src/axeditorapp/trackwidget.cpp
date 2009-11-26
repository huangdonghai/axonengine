#include "trackwidget.h"

namespace {
	enum {
		HEADER_HEIGHT = 16,
		TRACK_HEIGHT = 20,
		LEFT_WIDTH = 200,

		GRID_LABEL_PAD = 64,
		GRID_PAD = 6,
		SIDE_PAD = 32,
	};

	static int s_pads[] = { 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000 };

	static int nextPad(int x)
	{
		for (int i = 0; i < ArraySize(s_pads); i++) {
			if (x < s_pads[i])
				return s_pads[i];
		}

		Errorf("overflowed");
		return 0;
	}
} // anonymous namespace

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

TrackWidgetItem::TrackWidgetItem(TrackWidgetItem* parent, IAnimatable* anim) : m_widget(0), m_parent(parent)
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
	AX_ASSERT(m_widget);

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
	setPos(0, HEADER_HEIGHT + TRACK_HEIGHT * m_trackIndex);
}


TrackWidget::TrackWidget(QWidget *parent)
	: QGraphicsView(parent)
{
	m_startTime = 0;
	m_endTime = 5000;
	m_frameTime = 50;

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

void TrackWidget::drawBackground( QPainter *painter, const QRectF &rect )
{
	// draw header
	drawHeader(painter, rect);
}

void TrackWidget::drawHeader(QPainter *painter, const QRectF &rect)
{
	const QPalette& pal = palette();

	QRectF grid_rectf(LEFT_WIDTH, 0, rect.width() - LEFT_WIDTH, HEADER_HEIGHT);
	QRect grid_rect = grid_rectf.toRect();

#if 0
	qDrawShadePanel(painter, grid_rect, palette(), false, 1, &palette().background());
#else
	painter->setPen(pal.color(QPalette::ButtonText));
//	painter->setBrush(pal.button());

	painter->drawRect(grid_rect.adjusted(0,0,-1,-1));
	qDrawShadePanel(painter, grid_rect.adjusted(1,1,-1,-1), palette(), false, 1, &palette().background());
#endif

	float pixels_per_tick = (grid_rectf.width() - SIDE_PAD * 2) / (m_endTime - m_startTime);
	float pixels_per_frame = pixels_per_tick * m_frameTime;

	int frame_step = 1;
	while (frame_step * pixels_per_frame < GRID_PAD)
		frame_step = nextPad(frame_step);

	int label_step = nextPad(frame_step);
	label_step = nextPad(frame_step);

	while (label_step * pixels_per_frame < GRID_LABEL_PAD)
		label_step = nextPad(label_step);

	// draw grid in header
	int grid_time = m_frameTime * frame_step;
	int first_grid = ceilf((float)m_startTime / grid_time);
	int last_grid = floorf((float)m_endTime / grid_time);

	painter->setPen(pal.color(QPalette::Mid));

	for (int i = first_grid; i <= last_grid; i++) {
		int ticks = i * grid_time;
		int x = (ticks - m_startTime) * pixels_per_tick + grid_rect.x() + SIDE_PAD;

		painter->drawLine(x, grid_rect.bottom() - 4, x, grid_rect.bottom()-2);
	}

	// draw label
	int label_time = m_frameTime * label_step;
	int first_label = ceilf((float)m_startTime / label_time);
	int last_label = floorf((float)m_endTime / label_time);

	painter->setPen(pal.color(QPalette::ButtonText));

	for (int i = first_label; i <= last_label; i++) {
		int ticks = i * label_time;
		int x = (ticks - m_startTime) * pixels_per_tick + grid_rect.x() + SIDE_PAD;

		painter->drawLine(x, 2, x, grid_rect.bottom()-2);

		QString msg = QString("%1").arg(i * label_step);
		x += 4;
		painter->drawText(x, 1, SIDE_PAD, 12, Qt::AlignLeft | Qt::AlignTop, msg);
	}
}
