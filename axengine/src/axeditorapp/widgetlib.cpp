/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "widgetlib.h"


//------------------------------------------------------------------------------
// class ColorLabel
//------------------------------------------------------------------------------

ColorLabel::ColorLabel(QWidget* parent) : QLabel(parent), m_color(255,255,255) {
	setColor(m_color);
	setMinimumSize(QSize(24, 24));
	setMaximumSize(QSize(24, 24));
	setFrameShape(QFrame::Box);
	setCursor(QCursor(Qt::PointingHandCursor));
	setScaledContents(true);
}
ColorLabel::~ColorLabel() {}

void ColorLabel::mousePressEvent(QMouseEvent * e) {
	if (e->button() != Qt::LeftButton)
		return;

	e->accept();
}

void ColorLabel::mouseReleaseEvent(QMouseEvent * e) {
	if (e->button() != Qt::LeftButton)
		return;

	e->accept();
	QColor color = QColorDialog::getColor(m_color, this);

	if (!color.isValid())
		return;

	setColor(color);

	emit colorChanged(m_color);
}

QColor ColorLabel::color() {
	return m_color;
}
void ColorLabel::setColor(const QColor& color) {
	m_color = color;

	QPixmap pixmap(24,24);
	pixmap.fill(m_color);
	setPixmap(pixmap);

	QWidget* b = buddy();

	QLineEdit* le = qobject_cast<QLineEdit*>(b);
	if (le) {
		QString tmp;
		tmp.sprintf("%d,%d,%d", m_color.red(), m_color.green(), m_color.blue());

		le->setText(tmp);
	}

	QLabel* label = qobject_cast<QLabel*>(b);
	if (label) {
		QString tmp;
		tmp.sprintf("%d,%d,%d", m_color.red(), m_color.green(), m_color.blue());

		label->setText(tmp);
	}

	emit colorEdited(m_color);
}

void ColorLabel::setColor(const Rgb& rgba)
{
	QColor color(rgba.r, rgba.g, rgba.b);
	setColor(color);
}
//------------------------------------------------------------------------------
// class ColorEdit
//------------------------------------------------------------------------------

ColorEdit::ColorEdit(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(2);
	layout->setContentsMargins(2, 2, 2, 2);
	m_label = new ColorLabel(this);
	m_lineEdit = new QLineEdit(this);
	layout->addWidget(m_label);
	layout->addWidget(m_lineEdit);

	layout->update();

	QObject::connect(m_label, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));
	QObject::connect(this, SIGNAL(colorEdited(QColor)), m_label, SLOT(setColor(QColor)));
	QObject::connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
}

ColorEdit::~ColorEdit()
{}

QColor ColorEdit::color() {
	return m_color;
}

void ColorEdit::setColor(const QColor& color) {
	m_color = color;

	QString tmp;
	tmp.sprintf("%d,%d,%d", m_color.red(), m_color.green(), m_color.blue());

	m_lineEdit->setText(tmp);

	emit colorEdited(m_color);
}

void ColorEdit::textChanged(const QString& text) {
	QByteArray ba = text.toUtf8();
	int r,g,b;
	int v = sscanf(ba.begin(), "%d,%d,%d", &r, &g, &b);

	if (v != 3)
		return;

	m_color = QColor(r,g,b);

	emit colorEdited(m_color);
}

//------------------------------------------------------------------------------
// class ColorButton
//------------------------------------------------------------------------------

void ColorButtonPrivate::slotEditColor()
{
	bool ok;
	QRgb rgba = QColorDialog::getRgba(m_color.rgba(), &ok, q_ptr);
	if (ok == false)
		return;
	QColor c;
	c.setRgba(rgba);
	q_ptr->setColor(c);
	emit q_ptr->colorChanged(m_color);
}

///////////////

ColorButton::ColorButton(QWidget *parent)
: QToolButton(parent)
{
	d_ptr = new ColorButtonPrivate;
	d_ptr->q_ptr = this;
	d_ptr->m_backgroundTransparent = true;

	connect(this, SIGNAL(clicked()), this, SLOT(slotEditColor()));
	setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
}

ColorButton::~ColorButton()
{
	delete d_ptr;
}

void ColorButton::setColor(const QColor &color)
{
	if (d_ptr->m_color == color)
		return;
	d_ptr->m_color = color;
	update();
}

QColor ColorButton::color() const
{
	return d_ptr->m_color;
}

void ColorButton::setBackgroundTransparent(bool transparent)
{
	if (d_ptr->m_backgroundTransparent == transparent)
		return;
	d_ptr->m_backgroundTransparent = transparent;
	update();
}

bool ColorButton::backgroundTransparent() const
{
	return d_ptr->m_backgroundTransparent;
}

void ColorButton::paintEvent(QPaintEvent *e)
{
	QToolButton::paintEvent(e);
	if (!isEnabled())
		return;

	int pixSize = 20;
	QBrush br(d_ptr->m_color);
	if (!d_ptr->m_backgroundTransparent) {
		QPixmap pm(2 * pixSize, 2 * pixSize);
		QPainter pmp(&pm);
		pmp.fillRect(0, 0, pixSize, pixSize, Qt::lightGray);
		pmp.fillRect(pixSize, pixSize, pixSize, pixSize, Qt::lightGray);
		pmp.fillRect(0, pixSize, pixSize, pixSize, Qt::darkGray);
		pmp.fillRect(pixSize, 0, pixSize, pixSize, Qt::darkGray);
		pmp.fillRect(0, 0, 2 * pixSize, 2 * pixSize, d_ptr->m_color);
		br = QBrush(pm);
	}

	QPainter p(this);
	int corr = 2;
	QRect r = rect().adjusted(corr, corr, -corr, -corr);
	p.setBrushOrigin((r.width() % pixSize + pixSize) / 2 + corr, (r.height() % pixSize + pixSize) / 2 + corr);
	p.fillRect(r, br);
}
