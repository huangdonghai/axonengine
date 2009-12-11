#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QGraphicsView>

#include "private.h"

class AnimWrapper;
class TrackWidget;

//------------------------------------------------------------------------------
class RectLayout : public QGraphicsItem
{
public:
	RectLayout(TrackWidget* widget);
	virtual ~RectLayout();

	QRectF rect() const;
	void setRect(const QRectF &rect);
	inline void setRect(qreal x, qreal y, qreal w, qreal h)
	{ setRect(QRectF(x, y, w, h)); }

	QRectF boundingRect() const;
	QPainterPath shape() const;

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
	Q_DISABLE_COPY(RectLayout);

protected:
	TrackWidget* m_widget;
	QRectF m_rect;
};


//------------------------------------------------------------------------------
class CurveNameItem : public QGraphicsItem
{
public:
	CurveNameItem(AnimWrapper* wrapper);
	virtual ~CurveNameItem();

	// implement QGraphicsItem
	virtual QRectF boundingRect() const;
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

private:
};

//------------------------------------------------------------------------------
class ValueAxis : public RectLayout
{
public:
	ValueAxis(TrackWidget* widget);
	virtual ~ValueAxis();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	qreal getMinValue() const { return m_minValue; }
	void setMinValue(qreal val) { m_minValue = val; }
	qreal getMaxValue() const { return m_maxValue; }
	void setMaxValue(qreal val) { m_maxValue = val; }
	qreal getValueScale() const { return m_valueScale; }
	void setValueScale(qreal val) { m_valueScale = val; }
	qreal getValueOffset() const { return m_valueOffset; }
	void setValueOffset(qreal val) { m_valueOffset = val; }

private:
	qreal m_minValue;
	qreal m_maxValue;
	qreal m_valueScale;
	qreal m_valueOffset;
};

//------------------------------------------------------------------------------
class CurveViewItem : public QGraphicsItem
{

};

//------------------------------------------------------------------------------
class CurveViewFrame : public RectLayout
{
public:
	CurveViewFrame();
	virtual ~CurveViewFrame();

	// implement QGraphicsItem
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
};

//------------------------------------------------------------------------------
class TrackNameItem : public QGraphicsItem
{
public:
	TrackNameItem(AnimWrapper* wrapper);
	virtual ~TrackNameItem();

	// implement QGraphicsItem
	virtual QRectF boundingRect() const;
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

protected:

private:
	AnimWrapper* m_wrapper;
};

//------------------------------------------------------------------------------
class TrackViewItem : public QGraphicsItem
{

};

//------------------------------------------------------------------------------
class AnimWrapper
{
public:
	friend class TrackWidget;

	AnimWrapper(TrackWidget* widget);
	AnimWrapper(AnimWrapper* parent);
	AnimWrapper(AnimWrapper* parent, IAnimatable* anim);
	AnimWrapper(AnimWrapper* parent, AnimWrapper* preceding);
	virtual ~AnimWrapper();

	void setAnimatable(IAnimatable* anim);
	IAnimatable* getAnimatable() const;

	// drawing
	virtual QRectF boundingRect() const;
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	int getTrackIndex() const { return m_trackIndex; }
	void setTrackIndex(int index);

protected:
	void init();
	void emitDataChanged();
	void relayout();

private:
	TrackWidget* m_widget;
	AnimWrapper* m_parent;
	IAnimatable* m_anim;

	// init
	TrackNameItem* m_trackNameItem;
	TrackViewItem* m_trackViewItem;

	// runtime
	int m_trackIndex;
};

//------------------------------------------------------------------------------
class TimeAxis : public RectLayout
{
public:
	TimeAxis(TrackWidget* widget);
	virtual ~TimeAxis();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

//------------------------------------------------------------------------------
class CurveRight : public RectLayout
{
public:
	CurveRight(TrackWidget* widget);
	virtual ~CurveRight();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	virtual void wheelEvent(QGraphicsSceneWheelEvent * event);

private:
	bool m_isPanning;
	int m_panningTicks;
	float m_panningValue;
};

//------------------------------------------------------------------------------
class TrackRight : public RectLayout
{
public:
	TrackRight(TrackWidget* widget);
	virtual ~TrackRight();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	virtual void wheelEvent(QGraphicsSceneWheelEvent * event);

private:
	bool m_isPanning;
	int m_panningTicks;
};

//------------------------------------------------------------------------------
class TrackWidget : public QGraphicsView
{
	Q_OBJECT

public:
	enum LayoutFrameType {
		kCurveLeft, kCurveRight, kHeaderLeft, kHeaderRight, kTrackLeft, kTrackRight
	};

	TrackWidget(QWidget *parent);
	~TrackWidget();

	RectLayout* getLayoutFrame(LayoutFrameType lft) const;

	// prop-get-set
	int getStartTime() const { return m_startTime; }
	void setStartTime(int val) { m_startTime = val; }
	int getEndTime() const { return m_endTime; }
	void setEndTime(int val) { m_endTime = val; }
	int getFrameTime() const { return m_frameTime; }
	void setFrameTime(int val) { m_frameTime = val; }
	qreal getTimeAxisScale() const { return m_timeAxisScale; }
	void setTimeAxisScale(qreal val) { m_timeAxisScale = val; }
	qreal getTimeAxisOffset() const { return m_timeAxisOffset; }
	void setTimeAxisOffset(qreal val) { m_timeAxisOffset = val; }
	qreal getValueAxisScale() const { return m_valueAxisScale; }
	void setValueAxisScale(qreal val) { m_valueAxisScale = val; }
	qreal getValueAxisOffset() const { return m_valueAxisOffset; }
	void setValueAxisOffset(qreal val) { m_valueAxisOffset = val; }
	int getTimeAxisStep0() const { return m_timeAxisStep0; }
	void setTimeAxisStep0(int val) { m_timeAxisStep0 = val; }
	int getTimeAxisStep1() const { return m_timeAxisStep1; }
	void setTimeAxisStep1(int val) { m_timeAxisStep1 = val; }
	qreal getTimeAxisPixelsPerTick() const { return m_timeAxisPixelsPerTick; }
	void setTimeAxisPixelsPerTick(qreal val) { m_timeAxisPixelsPerTick = val; }

	// runtime calculation
	int valueVpos(qreal val) const;
	qreal vposValue(int vpos) const;
	int ticksHpos(int ticks) const;
	int hposTicks(int hpos) const;
	int framesHpos(int frameNum) const;
	int hposFrames(int hpos) const;

	int getStartTime_Show() const { return m_startTime - (m_endTime - m_startTime) / 20; }
	int getEndTime_Show() const { return m_endTime + (m_endTime - m_startTime) / 20; }
	int getDuration_Show() const { return getEndTime_Show() - getStartTime_Show(); }
	float getMinValue_Show() const { return m_minValue - (m_maxValue - m_minValue) * 0.05; }
	float getMaxValue_Show() const { return m_maxValue + (m_maxValue - m_minValue) * 0.05; }
	float getValueRange_Show() const { return getMaxValue_Show() - getMinValue_Show(); }

	// adjust time axis and value axis
	void offsetTimeAxis(int hpos, int ticks); // offset time axis, let hpos's time equal ticks
	void scaleTimeAxis(int hpos, float factor); // scale time axis use hpos as scale center
	void offsetAxis(const QPointF& pos, int ticks, float value);
	void scaleAxis(const QPointF& pos, float factor);

protected:
	virtual void resizeEvent(QResizeEvent * event);
	virtual void drawBackground(QPainter *painter, const QRectF &rect);

	void addAnimWrapper(AnimWrapper* track);
	void relayout();

private:
	IAnimatable* m_anim;
	QGraphicsScene* m_scene;
	QList<AnimWrapper*> m_wrappers;
	int m_lineHeight;
	bool m_showBar;
	int m_startTime;
	int m_endTime;
	int m_frameTime;
	qreal m_minValue;
	qreal m_maxValue;


	// layout
	RectLayout* m_frameCurveLeft;
	RectLayout* m_frameCurveRight;
	RectLayout* m_frameHeaderLeft;
	TimeAxis* m_timeAxis;
	RectLayout* m_frameTrackLeft;
	TrackRight* m_frameTrackRight;
	ValueAxis* m_valueAxis;
	//
	// runtime layout
	//
	int m_trackFrameHeight;
	int m_y0;
	int m_y1;
	int m_x0;
	int m_x1;
	qreal m_timeAxisScale;
	qreal m_timeAxisOffset;
	int m_timeAxisStep0;
	int m_timeAxisStep1;
	qreal m_timeAxisPixelsPerTick;

	qreal m_valueAxisScale;
	qreal m_valueAxisOffset;
	qreal m_valueAxisPixelsPerUnit;
	qreal m_valueStep0;
	qreal m_valueStep1;
};

#endif // TRACKWIDGET_H
