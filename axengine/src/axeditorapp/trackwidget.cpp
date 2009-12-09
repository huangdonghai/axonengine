#include "trackwidget.h"

namespace {
	enum {
		HEADER_HEIGHT = 16,
		TRACK_HEIGHT = 20,
		LEFT_WIDTH = 200,

		LEFT_WIDTH_ADD_1 = LEFT_WIDTH + 1,
		LEFT_WIDTH_SUB_1 = LEFT_WIDTH - 1,

		TIME_AXIS_PAD_1 = 64,
		TIME_AXIS_PAD_0 = 6,
		SIDE_BLANK = 55,
		LEFT_BLANK = SIDE_BLANK,
		VALUE_AXIS_WIDTH = 48,
		TIME_AXIS_ALL_BLANK = SIDE_BLANK * 2 + VALUE_AXIS_WIDTH,

		VALUE_AXIS_TOP_BLANK = 32,
		VALUE_AXIS_BOTTOM_BLANK = 32,
		VALUE_AXIS_ALL_BLANK = VALUE_AXIS_TOP_BLANK + VALUE_AXIS_BOTTOM_BLANK,
		VALUE_AXIS_PAD_0 = 6,
		VALUE_AXIS_PAD_1 = 32,

		INIT_TRACK_FRAME_HEIGHT = 100,
	};

	static QColor s_curveBackground(225,225,225);
	static QColor s_verticleTime(192,192,192);

	static int s_pads[] = {
		1, 2, 5, 10, 20, 50, 100, 200, 500,
		1000, 2000, 5000, 10000, 20000, 50000
	};

	static float s_floatPads[] = {
		0.01f, 0.02f, 0.05f, 0.1f, 0.2f, 0.5f,
		1.0f, 2.0f, 5.0f, 10.0f, 20.0f, 50.0f,
		100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f,
		10000, 20000, 50000, 100000, 200000, 500000
	};

	static int nextPad(int x)
	{
		for (int i = 0; i < ArraySize(s_pads); i++) {
			if (x < s_pads[i])
				return s_pads[i];
		}

		Errorf("overflowed");
		return 0;
	}

	static float floatNextPad(float x)
	{
		for (int i = 0; i < ArraySize(s_floatPads); i++) {
			if (x < s_floatPads[i])
				return s_floatPads[i];
		}

		Errorf("overflowed");
		return 0;
	}

} // anonymous namespace

//------------------------------------------------------------------------------
RectLayout::RectLayout(TrackWidget* widget) : m_widget(widget)
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

//------------------------------------------------------------------------------

ValueAxis::ValueAxis(TrackWidget* widget) : RectLayout(widget)
{
	m_minValue = 0.0f;
	m_maxValue = 100.0f;
	m_valueScale = 1.0f;
	m_valueOffset = 0.0f;
}

ValueAxis::~ValueAxis()
{

}

void ValueAxis::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/ )
{

}

//------------------------------------------------------------------------------

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

#if 0
	painter->setPen(pal.color(QPalette::Window));
	if (m_wrapper->getTrackIndex() % 2) {
		painter->setBrush(pal.alternateBase());
	} else {
		painter->setBrush(pal.base());
	}
	painter->drawRect(0, 0, LEFT_WIDTH+10, TRACK_HEIGHT);
#endif
	painter->drawText(left_rect, Qt::AlignCenter, "track");
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------

TimeAxis::TimeAxis(TrackWidget* widget) : RectLayout(widget)
{
}

TimeAxis::~TimeAxis()
{

}

void TimeAxis::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/ )
{
	if (m_rect.isEmpty())
		return;

	if (!widget)
		return;

	const QPalette& pal = widget->palette();

	QRectF r = m_rect;

	int startTime = m_widget->getStartTime();
	int endTime = m_widget->getEndTime();
	int frameTime = m_widget->getFrameTime();

	qDrawShadePanel(painter, r.toRect(), pal, false, 1, &pal.background());

	float pixels_per_tick = m_widget->getTimeAxisPixelsPerTick();

	int frame_step = m_widget->getTimeAxisFrameStep();
	int label_step = m_widget->getTimeAxisLabelStep();

	// draw grid in header
	int grid_time = frameTime * frame_step;
	int first_grid = ceilf((float)startTime / grid_time);
	int last_grid = floorf((float)endTime / grid_time);

	painter->setPen(pal.color(QPalette::Mid));

	for (int i = first_grid; i <= last_grid; i++) {
		int ticks = i * grid_time;
		int x = (ticks - startTime) * pixels_per_tick + r.x() + LEFT_BLANK;

		painter->drawLine(x, r.bottom() - 4, x, r.bottom()-2);
	}

	// draw label
	int label_time = frameTime * label_step;
	int first_label = ceilf((float)startTime / label_time);
	int last_label = floorf((float)endTime / label_time);

	painter->setPen(pal.color(QPalette::ButtonText));

	for (int i = first_label; i <= last_label; i++) {
		int ticks = i * label_time;
		int x = (ticks - startTime) * pixels_per_tick + r.x() + LEFT_BLANK;

		painter->drawLine(x, r.top() + 2, x, r.bottom()-2);

		QString msg = QString("%1").arg(i * label_step);
		x += 4;
		painter->drawText(x, r.top() + 1, LEFT_BLANK, r.top() + 12, Qt::AlignLeft | Qt::AlignTop, msg);
	}
}

//------------------------------------------------------------------------------
TrackWidget::TrackWidget(QWidget *parent)
	: QGraphicsView(parent)
{
	m_startTime = 0;
	m_endTime = 5000;
	m_frameTime = 50;
	m_trackFrameHeight = INIT_TRACK_FRAME_HEIGHT;

	m_timeAxisScale = 1.0f;
	m_timeAxisOffset = 0;
	m_valueAxisScale = 1.0f;
	m_valueAxisOffset = 0;

	m_minValue = -1;
	m_maxValue = 1;

	// layout item
	m_frameCurveLeft = new RectLayout(this);
	m_frameCurveRight = new RectLayout(this);
	m_frameHeaderLeft = new RectLayout(this);
	m_timeAxis = new TimeAxis(this);
	m_frameTrackLeft = new RectLayout(this);
	m_frameTrackRight = new RectLayout(this);
	m_valueAxis = new ValueAxis(this);

	m_scene = new QGraphicsScene();
	setScene(m_scene);

	m_scene->addItem(m_frameCurveLeft);
	m_scene->addItem(m_frameCurveRight);
	m_scene->addItem(m_frameHeaderLeft);
	m_scene->addItem(m_timeAxis);
	m_scene->addItem(m_frameTrackLeft);
	m_scene->addItem(m_frameTrackRight);
	m_scene->addItem(m_valueAxis);

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

	m_x0 = LEFT_WIDTH;
	m_x1 = rect.right() - VALUE_AXIS_WIDTH;

	m_y1 = rect.bottom() - m_trackFrameHeight - 1;
	m_y0 = m_y1 - HEADER_HEIGHT - 1;

	m_timeAxis->setRect(LEFT_WIDTH_ADD_1, m_y0 + 1, rect.right() - LEFT_WIDTH_ADD_1, HEADER_HEIGHT);
	m_valueAxis->setRect(m_x1 + 1, 0, rect.right() - m_x1 - 1, m_y0);

	m_frameTrackLeft->setRect(0, m_y1 + 1, LEFT_WIDTH_SUB_1, m_trackFrameHeight);

	// set index
	QList<AnimWrapper*>::const_iterator it = m_wrappers.begin();
	int track_index = 0;

	for (; it != m_wrappers.end(); ++it) {
		AnimWrapper* track = *it;
		track->setTrackIndex(track_index);
		track_index++;
	}

	// calculate time axis parameter
	float time_axis_width = m_timeAxis->rect().width();

	if (time_axis_width <= 0.0f)
		return;

	m_timeAxisPixelsPerTick = (float)(time_axis_width - TIME_AXIS_ALL_BLANK) / (m_endTime - m_startTime);
	float pixels_per_frame = m_timeAxisPixelsPerTick * m_frameTime;

	int frame_step = 1;
	while (frame_step * pixels_per_frame < TIME_AXIS_PAD_0)
		frame_step = nextPad(frame_step);

	int label_step = nextPad(frame_step);
	label_step = nextPad(frame_step);

	while (label_step * pixels_per_frame < TIME_AXIS_PAD_1)
		label_step = nextPad(label_step);

	m_timeAxisFrameStep = frame_step;
	m_timeAxisLabelStep = label_step;

	// calculate value axis parameter
	float valueAxisHeight = m_y0 - 1;
	m_valueAxisPixelsPerUnit = (valueAxisHeight - VALUE_AXIS_ALL_BLANK) / (m_maxValue - m_minValue);
	m_valueStep0 = s_floatPads[0];
	while (m_valueStep0 * m_valueAxisPixelsPerUnit < VALUE_AXIS_PAD_0)
		m_valueStep0 = floatNextPad(m_valueStep0);

	m_valueStep1 = floatNextPad(m_valueStep0);
	while (m_valueStep1 * m_valueAxisPixelsPerUnit < VALUE_AXIS_PAD_1)
		m_valueStep1 = floatNextPad(m_valueStep1);
}

void TrackWidget::drawBackground( QPainter *painter, const QRectF &rectf )
{
	QRectF rect = this->rect();
	const QPalette& pal = this->palette();

	// draw track background
	QList<AnimWrapper*>::const_iterator it = m_wrappers.begin();
	int track_index = 0;

//	QColor boardcolor = pal.color(QPalette::AlternateBase) * 5 / 6;
	QColor boardcolor(192,192,192);
	painter->setPen(boardcolor);
	for (; it != m_wrappers.end(); ++it) {
		if (track_index % 2) {
			painter->setBrush(pal.alternateBase());
		} else {
			painter->setBrush(pal.base());
		}
		painter->drawRect(0, m_y1 + 1 + TRACK_HEIGHT * track_index, rect.width(), TRACK_HEIGHT);

		track_index++;
	}

	// draw curve view background
	painter->setPen(s_curveBackground);
	painter->setBrush(s_curveBackground);
	painter->drawRect(m_x0 + 1, 0, m_x1 - m_x0 - 2, m_y0 - 1);

	// draw value axis background
	{
		int firstValue = ceilf(m_minValue / m_valueStep0);
		int lastValue = floorf(m_maxValue / m_valueStep0);

		for (int i = firstValue; i <= lastValue; i++) {
			float f = i * m_valueStep0;
			int y = m_y0 - 1 - (f - m_minValue) * m_valueAxisPixelsPerUnit - VALUE_AXIS_BOTTOM_BLANK;
			painter->setPen(s_verticleTime);
			painter->drawLine(m_x1 + 1, y, m_x1 + 3, y);
		}
	}
	{
		int firstValue = ceilf(m_minValue / m_valueStep1);
		int lastValue = floorf(m_maxValue / m_valueStep1);

		for (int i = firstValue; i <= lastValue; i++) {
			float f = i * m_valueStep1;
			int y = m_y0 - 1 - (f - m_minValue) * m_valueAxisPixelsPerUnit - VALUE_AXIS_BOTTOM_BLANK;
			painter->setPen(s_verticleTime);
			painter->drawLine(m_x0 + 1, y, m_x1 - 1, y);
			painter->setPen(Qt::black);
			painter->drawLine(m_x1 + 1, y, m_x1 + 5, y);

			QString msg = QString("%1").arg(f);
			painter->drawText(m_x1 + 9, y - 5, VALUE_AXIS_WIDTH - 9, 11, Qt::AlignVCenter | Qt::AlignLeft, msg);
		}
	}

	{
		// draw verticle time line
		painter->setPen(s_verticleTime);

		int label_time = m_frameTime * m_timeAxisLabelStep;
		int first_label = ceilf((float)m_startTime / label_time);
		int last_label = floorf((float)m_endTime / label_time);

		for (int i = first_label; i <= last_label; i++) {
			int ticks = i * label_time;
			int x = (ticks - m_startTime) * m_timeAxisPixelsPerTick + LEFT_BLANK + LEFT_WIDTH_ADD_1;

			painter->drawLine(x, 0, x, m_y1 + 1 + TRACK_HEIGHT * track_index);
		}
	}

	{
		// draw frame border
		painter->setPen(pal.color(QPalette::Text));
		painter->drawLine(0, m_y0, rect.right(), m_y0);
		painter->drawLine(0, m_y1, rect.right(), m_y1);
		painter->drawLine(LEFT_WIDTH, 0, LEFT_WIDTH, rect.bottom());

		if (m_x1 > m_x0) {
			painter->drawLine(m_x1, 0, m_x1, m_y0);
		}
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
		return m_timeAxis;
	case TrackLeft:
		return m_frameTrackLeft;
	case TrackRight:
		return m_frameTrackRight;
	}

	Errorf("bad arg");
	return 0;
}

