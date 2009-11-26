#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QGraphicsView>

#include "private.h"

class TrackWidget;

class TrackWidgetItem : public QGraphicsItem
{
public:
	friend class TrackWidget;

	TrackWidgetItem();
	TrackWidgetItem(TrackWidget* widget);
	TrackWidgetItem(TrackWidgetItem* parent);
	TrackWidgetItem(TrackWidgetItem* parent, IAnimatable* anim);
	TrackWidgetItem(TrackWidgetItem* parent, TrackWidgetItem* preceding);
	virtual ~TrackWidgetItem();

	void setAnimatable(IAnimatable* anim);
	IAnimatable* getAnimatable() const;

	virtual QRectF boundingRect() const;

	// drawing
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

protected:
	void emitDataChanged();

	void setTrackIndex(int index);
	void relayout();

private:
	TrackWidget* m_widget;
	TrackWidgetItem* m_parent;
	IAnimatable* m_anim;
	QGraphicsItem* m_graphItem;
	int m_trackIndex;
};



class TrackWidget : public QGraphicsView
{
	Q_OBJECT

public:
	TrackWidget(QWidget *parent);
	~TrackWidget();

	int getTimeRange() const { return 5000; }
	int getFrameTime() const { return 50; }

protected:
	virtual void resizeEvent(QResizeEvent * event);
	virtual void drawBackground(QPainter *painter, const QRectF &rect);

	void drawHeader(QPainter *painter, const QRectF &rect);
	void addTrackItem(TrackWidgetItem* track);
	void relayout();

private:
	IAnimatable* m_anim;
	QGraphicsScene* m_scene;
	QList<TrackWidgetItem*> m_tracks;
	int m_lineHeight;
	bool m_showBar;

	// runtime layout
	int m_startTime;
	int m_endTime;
	int m_frameTime;
};

#endif // TRACKWIDGET_H
