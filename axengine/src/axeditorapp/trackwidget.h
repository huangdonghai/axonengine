#ifndef TRACKWIDGET_H
#define TRACKWIDGET_H

#include <QGraphicsView>

class TrackWidget : public QGraphicsView
{
	Q_OBJECT

public:
	TrackWidget(QWidget *parent);
	~TrackWidget();

private:
	IAnimatable* m_anim;
	int m_lineHeight;
	bool m_showBar;
};

#endif // TRACKWIDGET_H
