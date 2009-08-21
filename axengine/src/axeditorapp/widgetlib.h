/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef WIDGETLIB_H
#define WIDGETLIB_H

#include <QWidget>
#include "private.h"

//------------------------------------------------------------------------------
// class ColorLabel
//------------------------------------------------------------------------------

class ColorLabel : public QLabel {
	Q_OBJECT
	Q_PROPERTY(QColor color READ color WRITE setColor)

public:
	ColorLabel(QWidget* parent);
	~ColorLabel();

public slots:
	QColor color();
	void setColor(const QColor& color);
	void setColor(const Rgb& rgba);

signals:
	void colorChanged(const QColor& color);
	void colorEdited(const QColor& color);

protected:
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseReleaseEvent(QMouseEvent * event);

private:
	QColor m_color;
};

//------------------------------------------------------------------------------
// class ColorEdit
//------------------------------------------------------------------------------

class ColorEdit : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QColor color READ color WRITE setColor)

public:
	ColorEdit(QWidget *parent);
	~ColorEdit();

public slots:
	QColor color();
	void setColor(const QColor& color);

	void textChanged(const QString& text);

signals:
	void colorChanged(const QColor& color);
	void colorEdited(const QColor& color);

private:
	ColorLabel* m_label;
	QLineEdit* m_lineEdit;
	QColor m_color;
};

//------------------------------------------------------------------------------
// class ColorButton
//------------------------------------------------------------------------------
class ColorButton;
class ColorButtonPrivate
{
	ColorButton *q_ptr;
	Q_DECLARE_PUBLIC(ColorButton)
public:
	QColor m_color;
	bool m_backgroundTransparent;

	void slotEditColor();
};


class ColorButton : public QToolButton
{
	Q_OBJECT

	Q_PROPERTY(bool backgroundTransparent READ backgroundTransparent WRITE setBackgroundTransparent)
public:
	ColorButton(QWidget *parent = 0);
	~ColorButton();

	void setBackgroundTransparent(bool transparent);
	bool backgroundTransparent() const;

	void setColor(const QColor &color);
	QColor color() const;

signals:
	void colorChanged(const QColor &color);
protected:
	void paintEvent(QPaintEvent *e);
private:
	class ColorButtonPrivate *d_ptr;
	Q_DECLARE_PRIVATE(ColorButton)
	Q_DISABLE_COPY(ColorButton)
	Q_PRIVATE_SLOT(d_func(), void slotEditColor())
};


#endif // WIDGETLIB_H
