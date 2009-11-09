#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QGraphicsView>

class CurveWidget : public QGraphicsView
{
	Q_OBJECT

public:
	CurveWidget(QWidget *parent);
	~CurveWidget();

private:
};

#endif // CURVEWIDGET_H
