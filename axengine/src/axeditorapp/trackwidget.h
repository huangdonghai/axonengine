#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QGraphicsView>

#include "private.h"

class AnimWrapper;
class TrackWidget;

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

class ValueAxis : public RectLayout
{
public:
	ValueAxis(TrackWidget* widget);
	virtual ~ValueAxis();

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	float getMinValue() const { return m_minValue; }
	void setMinValue(float val) { m_minValue = val; }
	float getMaxValue() const { return m_maxValue; }
	void setMaxValue(float val) { m_maxValue = val; }
	float getValueScale() const { return m_valueScale; }
	void setValueScale(float val) { m_valueScale = val; }
	float getValueOffset() const { return m_valueOffset; }
	void setValueOffset(float val) { m_valueOffset = val; }

private:
	float m_minValue;
	float m_maxValue;
	float m_valueScale;
	float m_valueOffset;
};

class CurveViewItem : public QGraphicsItem
{

};

class CurveViewFrame : public RectLayout
{
public:
	CurveViewFrame();
	virtual ~CurveViewFrame();

	// implement QGraphicsItem
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
};

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

class TrackViewItem : public QGraphicsItem
{

};

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

class TimeAxis : public RectLayout
{
public:
	TimeAxis(TrackWidget* widget);
	virtual ~TimeAxis();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

class CurveRight : public RectLayout
{
public:
	CurveRight(TrackWidget* widget);
	virtual ~CurveRight();

protected:
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

class TrackWidget : public QGraphicsView
{
	Q_OBJECT

public:
	enum LayoutFrameType {
		CurveLeft, CurveRight, HeaderLeft, HeaderRight, TrackLeft, TrackRight
	};

	TrackWidget(QWidget *parent);
	~TrackWidget();

	RectLayout* getLayoutFrame(LayoutFrameType lft) const;

	int getStartTime() const { return m_startTime; }
	void setStartTime(int val) { m_startTime = val; }
	int getEndTime() const { return m_endTime; }
	void setEndTime(int val) { m_endTime = val; }
	int getFrameTime() const { return m_frameTime; }
	void setFrameTime(int val) { m_frameTime = val; }
	float getTimeAxisScale() const { return m_timeAxisScale; }
	void setTimeAxisScale(float val) { m_timeAxisScale = val; }
	float getTimeAxisOffset() const { return m_timeAxisOffset; }
	void setTimeAxisOffset(float val) { m_timeAxisOffset = val; }
	float getValueAxisScale() const { return m_valueAxisScale; }
	void setValueAxisScale(float val) { m_valueAxisScale = val; }
	float getValueAxisOffset() const { return m_valueAxisOffset; }
	void setValueAxisOffset(float val) { m_valueAxisOffset = val; }
	int getTimeAxisFrameStep() const { return m_timeAxisFrameStep; }
	void setTimeAxisFrameStep(int val) { m_timeAxisFrameStep = val; }
	int getTimeAxisLabelStep() const { return m_timeAxisLabelStep; }
	void setTimeAxisLabelStep(int val) { m_timeAxisLabelStep = val; }
	float getTimeAxisPixelsPerTick() const { return m_timeAxisPixelsPerTick; }
	void setTimeAxisPixelsPerTick(float val) { m_timeAxisPixelsPerTick = val; }

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
	float m_minValue;
	float m_maxValue;


	// layout
	RectLayout* m_frameCurveLeft;
	RectLayout* m_frameCurveRight;
	RectLayout* m_frameHeaderLeft;
	TimeAxis* m_timeAxis;
	RectLayout* m_frameTrackLeft;
	RectLayout* m_frameTrackRight;
	ValueAxis* m_valueAxis;
	//
	// runtime layout
	//
	int m_trackFrameHeight;
	int m_y0;
	int m_y1;
	int m_x0;
	int m_x1;
	float m_timeAxisScale;
	float m_timeAxisOffset;
	int m_timeAxisFrameStep;
	int m_timeAxisLabelStep;
	float m_timeAxisPixelsPerTick;

	float m_valueAxisScale;
	float m_valueAxisOffset;
	float m_valueAxisPixelsPerUnit;
	float m_valueStep0;
	float m_valueStep1;
};

#endif // TRACKWIDGET_H
