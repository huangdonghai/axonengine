#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QGraphicsView>

#include "private.h"

class AnimWrapper;
class TrackWidget;

class CurveNameItem : public QGraphicsItem
{

};

class CurveViewItem : public QGraphicsItem
{

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

class RectLayout : public QGraphicsItem
{
public:
	RectLayout();
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
	QRectF m_rect;
};

class TrackRuler : public RectLayout
{
public:
	TrackRuler();
	virtual ~TrackRuler();

	int getStartTime() const { return m_startTime; }
	void setStartTime(int val) { m_startTime = val; }
	int getEndTime() const { return m_endTime; }
	void setEndTime(int val) { m_endTime = val; }
	int getFrameTime() const { return m_frameTime; }
	void setFrameTime(int val) { m_frameTime = val; }

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
	int m_startTime;
	int m_endTime;
	int m_frameTime;
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

	int getTimeRange() const { return 5000; }
	int getFrameTime() const { return 50; }

	RectLayout* getLayoutFrame(LayoutFrameType lft) const;

protected:
	virtual void resizeEvent(QResizeEvent * event);
	virtual void drawBackground(QPainter *painter, const QRectF &rect);

	void drawHeader(QPainter *painter, const QRectF &rect);
	void addAnimWrapper(AnimWrapper* track);
	void relayout();

private:
	IAnimatable* m_anim;
	QGraphicsScene* m_scene;
	QList<AnimWrapper*> m_wrappers;
	int m_lineHeight;
	bool m_showBar;

	// layout
	RectLayout* m_frameCurveLeft;
	RectLayout* m_frameCurveRight;
	RectLayout* m_frameHeaderLeft;
	TrackRuler* m_frameHeaderRight;
	RectLayout* m_frameTrackLeft;
	RectLayout* m_frameTrackRight;

	// runtime layout
	int m_startTime;
	int m_endTime;
	int m_frameTime;
	int m_trackFrameHeight;
};

#endif // TRACKWIDGET_H
