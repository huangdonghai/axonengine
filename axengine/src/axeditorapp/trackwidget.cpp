#include "trackwidget.h"

namespace {
	enum {
		HEADER_HEIGHT = 16,
		TRACK_HEIGHT = 20,
		LEFT_WIDTH = 200,

		GRID_LABEL_PAD = 64,
		GRID_PAD = 6,
		SIDE_PAD = 32,

		INIT_TRACK_FRAME_HEIGHT = 100,
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

TrackNameItem::TrackNameItem( AnimWrapper* wrapper )
	: m_wrapper(wrapper)
{}

TrackNameItem::~TrackNameItem()
{}

QRectF TrackNameItem::boundingRect() const
{
	return QRectF(0,0,1,1);
}

void TrackNameItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	AX_ASSERT(widget);

	QRectF left_rect(0, 0, LEFT_WIDTH, TRACK_HEIGHT);

	const QPalette& pal = widget->palette();

	painter->setPen(pal.color(QPalette::Window));
	if (m_wrapper->getTrackIndex() % 2) {
		painter->setBrush(pal.alternateBase());
	} else {
		painter->setBrush(pal.base());
	}
	painter->drawRect(0, 0, LEFT_WIDTH+10, TRACK_HEIGHT);

	painter->drawText(left_rect, Qt::AlignCenter, "track");
}

AnimWrapper::AnimWrapper(TrackWidget* widget) : m_widget(widget), m_parent(0)
{
	init();
}

AnimWrapper::AnimWrapper(AnimWrapper* parent) : m_widget(0), m_parent(parent)
{
	if (m_parent)
		m_widget = m_parent->m_widget;
	init();
}

AnimWrapper::AnimWrapper(AnimWrapper* parent, IAnimatable* anim) : m_widget(0), m_parent(parent)
{
	if (m_parent)
		m_widget = m_parent->m_widget;
	init();
}

AnimWrapper::AnimWrapper(AnimWrapper* parent, AnimWrapper* preceding) : m_widget(0), m_parent(parent)
{
	if (m_parent)
		m_widget = m_parent->m_widget;
	init();
}

AnimWrapper::~AnimWrapper()
{

}

void AnimWrapper::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
}

QRectF AnimWrapper::boundingRect() const
{
	return QRectF(0,0,1,1);
}

void AnimWrapper::setTrackIndex( int index )
{
	m_trackIndex = index;
	relayout();
}

void AnimWrapper::relayout()
{
	m_trackNameItem->setPos(0, m_trackIndex * TRACK_HEIGHT);
}

void AnimWrapper::init()
{
	m_trackIndex = 0;

	m_trackNameItem = new TrackNameItem(this);
//	m_trackViewItem = new TrackViewItem(this);

	RectLayout* track_left = m_widget->getLayoutFrame(TrackWidget::TrackLeft);
	m_trackNameItem->setParentItem(track_left);
}


RectLayout::RectLayout()
{
	setFlag(ItemClipsToShape);
	setFlag(ItemClipsChildrenToShape);
}

RectLayout::~RectLayout()
{ /* do nothing */ }



QRectF RectLayout::rect() const
{
	return m_rect;
}

void RectLayout::setRect(const QRectF &rect)
{
	QPointF pos = rect.topLeft();
	m_rect = rect;
	m_rect.moveTopLeft(QPointF(0,0));
#if 0
	if (m_rect == rect)
		return;
#endif
	prepareGeometryChange();
	setPos(pos);
	update();
}

QRectF RectLayout::boundingRect() const
{
	return m_rect;
}

QPainterPath RectLayout::shape() const
{
	QPainterPath path;
	path.addRect(m_rect);
	return path;
}

void RectLayout::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#if 0 // do nothing
	Q_D(RectPlaceholder);
	Q_UNUSED(widget);
	painter->setPen(m_pen);
	painter->setBrush(m_brush);
	painter->drawRect(m_rect);

	if (option->state & QStyle::State_Selected)
		qt_graphicsItem_highlightSelected(this, painter, option);
#endif
}

TrackRuler::TrackRuler()
{
	m_startTime = 0;
	m_endTime = 5000;
	m_frameTime = 50;
}

TrackRuler::~TrackRuler()
{

}

void TrackRuler::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/ )
{
	if (m_rect.isEmpty())
		return;

	if (!widget)
		return;

	const QPalette& pal = widget->palette();

	QRectF r = m_rect;

#if 0
	qDrawShadePanel(painter, grid_rect, palette(), false, 1, &palette().background());
#else
	painter->setPen(pal.color(QPalette::ButtonText));
	//	painter->setBrush(pal.button());

	painter->drawRect(r.adjusted(0,0,-1,-1));
	qDrawShadePanel(painter, r.adjusted(1,1,-1,-1).toRect(), pal, false, 1, &pal.background());
#endif

	float pixels_per_tick = (float)(r.width() - SIDE_PAD * 2) / (m_endTime - m_startTime);
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
		int x = (ticks - m_startTime) * pixels_per_tick + r.x() + SIDE_PAD;

		painter->drawLine(x, r.bottom() - 4, x, r.bottom()-2);
	}

	// draw label
	int label_time = m_frameTime * label_step;
	int first_label = ceilf((float)m_startTime / label_time);
	int last_label = floorf((float)m_endTime / label_time);

	painter->setPen(pal.color(QPalette::ButtonText));

	for (int i = first_label; i <= last_label; i++) {
		int ticks = i * label_time;
		int x = (ticks - m_startTime) * pixels_per_tick + r.x() + SIDE_PAD;

		painter->drawLine(x, r.top() + 2, x, r.bottom()-2);

		QString msg = QString("%1").arg(i * label_step);
		x += 4;
		painter->drawText(x, r.top() + 1, SIDE_PAD, r.top() + 12, Qt::AlignLeft | Qt::AlignTop, msg);
	}
}

TrackWidget::TrackWidget(QWidget *parent)
	: QGraphicsView(parent)
{
	m_startTime = 0;
	m_endTime = 5000;
	m_frameTime = 50;
	m_trackFrameHeight = INIT_TRACK_FRAME_HEIGHT;

	// layout item
	m_frameCurveLeft = new RectLayout();
	m_frameCurveRight = new RectLayout();
	m_frameHeaderLeft = new RectLayout();
	m_frameHeaderRight = new TrackRuler();
	m_frameTrackLeft = new RectLayout();
	m_frameTrackRight = new RectLayout();

	m_scene = new QGraphicsScene();
	setScene(m_scene);

	m_scene->addItem(m_frameCurveLeft);
	m_scene->addItem(m_frameCurveRight);
	m_scene->addItem(m_frameHeaderLeft);
	m_scene->addItem(m_frameHeaderRight);
	m_scene->addItem(m_frameTrackLeft);
	m_scene->addItem(m_frameTrackRight);

	AnimWrapper* item = new AnimWrapper(this);
	addAnimWrapper(item);
	addAnimWrapper(new AnimWrapper(this));

//	this->setBackgroundBrush(s_background);
//	this->translate(100,100);

	relayout();
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

void TrackWidget::addAnimWrapper(AnimWrapper* track)
{
	m_wrappers.push_back(track);
	track->m_widget = this;

	relayout();
}

void TrackWidget::relayout()
{
	QRectF rect = this->rect();
	setSceneRect(rect);

	int y1 = rect.bottom() - m_trackFrameHeight;
	int y0 = y1 - HEADER_HEIGHT;

//	m_frameHeaderRight->setPos(LEFT_WIDTH, y0);
	m_frameHeaderRight->setRect(LEFT_WIDTH, y0, rect.right() - LEFT_WIDTH, HEADER_HEIGHT);

	m_frameTrackLeft->setRect(0, y1, LEFT_WIDTH, rect.bottom());

	QList<AnimWrapper*>::const_iterator it = m_wrappers.begin();

	int track_index = 0;

	for (; it != m_wrappers.end(); ++it) {
		AnimWrapper* track = *it;
		track->setTrackIndex(track_index);
		track_index++;
	}
}

void TrackWidget::drawBackground( QPainter *painter, const QRectF &rectf )
{
	// draw header
//	drawHeader(painter, rect);

	// draw frame border
	QRect rect = rectf.toRect();
	int y2 = rect.bottom() - m_trackFrameHeight;
	int y1 = y2 - HEADER_HEIGHT;

	painter->drawLine(0, y1, rect.right(), y1);
	painter->drawLine(0, y2, rect.right(), y2);
	painter->drawLine(LEFT_WIDTH, 0, LEFT_WIDTH, rect.bottom());
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

RectLayout* TrackWidget::getLayoutFrame( LayoutFrameType lft ) const
{
	switch (lft) {
	case CurveLeft:
		return m_frameCurveLeft;
	case CurveRight:
		return m_frameCurveRight;
	case HeaderLeft:
		return m_frameHeaderLeft;
	case HeaderRight:
		return m_frameHeaderRight;
	case TrackLeft:
		return m_frameTrackLeft;
	case TrackRight:
		return m_frameTrackRight;
	}

	Errorf("bad arg");
	return 0;
}

