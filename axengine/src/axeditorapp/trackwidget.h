#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QGraphicsView>

#include "private.h"

class TrackWidget;

class TrackWidgetItem : public QGraphicsItem
{
public:
	TrackWidgetItem();
	TrackWidgetItem(TrackWidget* widget);
	TrackWidgetItem(TrackWidgetItem* parent);
	TrackWidgetItem(TrackWidgetItem* parent, Gfx::IAnimatable* anim);
	TrackWidgetItem(TrackWidgetItem* parent, TrackWidgetItem* preceding);
	virtual ~TrackWidgetItem();

	void setAnimatable(Gfx::IAnimatable* anim);
	Gfx::IAnimatable* getAnimatable() const;

	virtual QRectF boundingRect() const;

	// drawing
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

protected:
	void emitDataChanged();

private:
	TrackWidget* m_widget;
	TrackWidgetItem* m_parent;
	Gfx::IAnimatable* m_anim;
	QGraphicsItem* m_graphItem;
};



class TrackWidget : public QGraphicsView
{
	Q_OBJECT

public:
	TrackWidget(QWidget *parent);
	~TrackWidget();

protected:
	virtual void resizeEvent(QResizeEvent * event);

private:
	Gfx::IAnimatable* m_anim;
	QGraphicsScene* m_scene;
	int m_lineHeight;
	bool m_showBar;
};

#endif // TRACKWIDGET_H
