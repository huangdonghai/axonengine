/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"
#include "rollup.h"

//------------------------------------------------------------------------------
// class RollPage
//------------------------------------------------------------------------------

RollPage::RollPage(QWidget* parent, Rollup* rollup_widget, QWidget* contained, const QString& name)
	: QWidget(parent)
	, m_rollupWidget(rollup_widget)
	, m_containedWidget(contained)
	, m_pageName(name)
	, m_isShrinked(false)
	, m_isHidden(false)
	, m_leftPixmap(":/images/left-12.png")
	, m_downPixmap(":/images/down-12.png")
{
	setContentsMargins(0, 18, 0, 0);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setObjectName(QString::fromUtf8("CxRollupPageLayout"));
	layout->setSizeConstraint(QLayout::SetMinimumSize);
	layout->setSpacing(0);

#if 0
	m_button = new QPushButton(/* this */);
	m_button->setText(name);
	m_button->setFixedHeight(20);

	QObject::connect(m_button, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));

//	contained->setParent(this);
//	contained->show();
	layout->addWidget(m_button);
#endif
	layout->addWidget(contained);

	if (contained->layout()) {
		m_margin = 2;
		contained->layout()->setContentsMargins(2, 2, 2, 2);
	} else {
		m_margin = 4;
	}

	layout->setMargin(m_margin);

	Object* obj = variant_cast<Object*>(g_renderSystem);
	RenderSystem* rs = variant_cast<RenderSystem*>(obj);
}

RollPage::~RollPage() {
}

void RollPage::buttonClicked(bool) {
	m_isShrinked = !m_isShrinked;

	if (m_isShrinked) {
		m_containedWidget->setHidden(true);
		layout()->setMargin(0);
	} else {
		layout()->setMargin(m_margin);
		m_containedWidget->setShown(true);
	}

	//	parentWidget()->adjustSize();
	parentWidget()->setFixedHeight(parentWidget()->minimumHeight());

	m_rollupWidget->relayout();
}

void RollPage::paintEvent(QPaintEvent *) {
	QPainter painter(this);

	QSize size = this->size();

	painter.setPen(QColor(255,151,37));

	painter.drawRect(0, 0, size.width()-1, size.height()-1);

	painter.setBrush(QColor(255,255,238));
	painter.drawRect(0, 0, size.width()-1, 17);

	painter.setPen(QColor(136,0,0));

	QFont f = font();
	f.setBold(true);

	painter.setFont(f);
	painter.drawText(16, 13, m_pageName);

	if (m_isShrinked) {
		painter.drawPixmap(2,3,12,12, m_leftPixmap);
	} else {
		painter.drawPixmap(2,3,12,12, m_downPixmap);
	}
}

void RollPage::mousePressEvent(QMouseEvent * e) {
	QRect r(0,0,size().width(), 18);
	if (!r.contains(e->pos())) {
		return;
	}
	buttonClicked(true);
}

//------------------------------------------------------------------------------
// class RollArea
//------------------------------------------------------------------------------

RollArea::RollArea(Rollup* parent)
	: QWidget(parent)
	, m_parent(parent)
{
	installEventFilter(this);
}

RollArea::~RollArea() {
}

bool RollArea::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::LayoutRequest) {
		QRect r = rect();
		QSize s = minimumSize();
		resize(r.width(), s.height());
		m_parent->relayout();
	}
	return QWidget::eventFilter(watched, event);
}

void
RollArea::mouseMoveEvent (QMouseEvent * e) {
	QWidget::mouseMoveEvent(e);
}


//------------------------------------------------------------------------------
// class Rollup
//------------------------------------------------------------------------------


Rollup::Rollup(QWidget *parent)
	: QFrame(parent)
	, m_scrollBarColor(QColor(54,63,75,255))
	, m_scrollBarSliderColor(QColor(255,199,60,255))
	, m_curScrollPos(0)
	, m_totalPagesHeight(0)
	, m_trackingMode(TrackingNone)
{
	setObjectName(QString::fromUtf8("Rollup"));
	//	setMouseTracking(true);

#if 0
	QFrame::setFrameShape(QFrame::Box);
	QFrame::setFrameShadow(QFrame::Raised);
	QFrame::setLineWidth(1);

	m_central = new QWidget(this);
	m_central->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_central);
	layout->setContentsMargins(1, 1, 1, 1);
#endif

	QSize s = size();
	s.setWidth(s.width() - 6);
	s.setHeight(0);
	m_rollarea = new RollArea(this);
	m_rollarea->resize(s);

	{
		QVBoxLayout* layout = new QVBoxLayout(m_rollarea);
		layout->setSizeConstraint(QLayout::SetMinimumSize);
		layout->setSpacing(4);
		layout->setMargin(2);
	}

//	m_rollScroll = new RollScroll(this);
}

Rollup::~Rollup()
{}


void Rollup::initialize() {
	QLayout* l = layout();
	SafeDelete(l);

	setContentsMargins(1, 1, 1, 1);

#if 0
	QTabWidget* tab = this->findChild<QTabWidget*>();

	if (!tab)
		return;

	for (int i = 0; i < tab->count(); i++) {
		QWidget* widget = tab->widget(i);
		QString label = tab->tabText(i);

		widget = widget->findChild<QWidget*>();
		if (!widget)
			continue;
		addPage(widget, label);
	}

 	delete tab;
#else
	if (!initFromTab()) {
		initFromToolbox();
	}
#endif
}

int Rollup::addPage(QWidget* widget, QString& name) {
	QString objname = widget->objectName();

	RollPage* page = new RollPage(m_rollarea, this, widget, name);
	m_pages.append(page);

	widget->setObjectName(name);

	m_rollarea->layout()->addWidget(page);

	relayout();

	return m_pages.size() - 1;

#if 0
	QPushButton* button;
	button = new QPushButton(mCentralWidget);
	button->setObjectName(QString::fromUtf8("CxRollupWidgetButton"));
	button->setFixedHeight(18);
	button->setText(name);
	QObject::connect(button, SIGNAL(pressed()), this, SLOT(ButtonPressed()));

	//widget->installEventFilter(this);

	/	_RollupPage page;
	page.containedWidget= widget;
	page.buttonWidget = button;
	mPages.append(page);
	return 0;
#endif
}

void Rollup::relayout() {
	m_totalPagesHeight = m_rollarea->height();

	if (m_totalPagesHeight <= 0)
		return;

	QSize widget_size = size();
	QSize cen_size = m_rollarea->size();

	m_rollarea->resize(widget_size.width() - 6, cen_size.height());

	m_scrollRect.setX(widget_size.width() - SCROLL_WIDTH);
	m_scrollRect.setY(0);
	m_scrollRect.setWidth(SCROLL_WIDTH);
	m_scrollRect.setHeight(widget_size.height());

	checkScrollPos();

	m_rollarea->move(0, -m_curScrollPos);

	int start = m_curScrollPos * widget_size.height() / m_totalPagesHeight;
	int height = widget_size.height() * widget_size.height() / m_totalPagesHeight;

	m_scrollSliderRect.setX(widget_size.width() - SCROLL_WIDTH);
	m_scrollSliderRect.setY(start);
	m_scrollSliderRect.setWidth(SCROLL_WIDTH);
	m_scrollSliderRect.setHeight(height);

	update();
}

void Rollup::checkScrollPos() {
	if (m_curScrollPos > (m_totalPagesHeight - m_scrollRect.height()))
		m_curScrollPos = (m_totalPagesHeight - m_scrollRect.height());
	if (m_curScrollPos < 0)
		m_curScrollPos = 0;
}

void Rollup::removeAllPages() {

}

void Rollup::setAllPagesVisible(bool visible) {
	AX_FOREACH(RollPage* page, m_pages) {
		page->setVisible(visible);
	}
}


bool Rollup::event(QEvent *e) {
	return QFrame::event(e);
}

void Rollup::paintEvent(QPaintEvent *pe) {
	QFrame::paintEvent(pe);

	QSize widget_size = size();
	QPainter painter(this);

	painter.fillRect(m_scrollRect, QBrush(m_scrollBarColor));

	// draw slider
	if (m_totalPagesHeight > widget_size.height()) {
		painter.fillRect(m_scrollSliderRect, QBrush(m_scrollBarSliderColor));
	}
}

void Rollup::mouseMoveEvent (QMouseEvent * e) {
	QPoint pos =  e->pos();

	if (m_trackingMode == TrackingScrollbar) {
		int dy = pos.y() - m_trackingPos.y();

		m_trackingPos = pos;
		dy = dy * m_totalPagesHeight / m_scrollRect.height();
		m_curScrollPos += dy;
		checkScrollPos();

		relayout();
		e->accept();

		return;
	}

	if (m_trackingMode == TrackingPage) {
		int dy = pos.y() - m_trackingPos.y();

		m_trackingPos = pos;
		m_curScrollPos -= dy;
		checkScrollPos();

		relayout();
		e->accept();

		return;
	}

	if (!m_scrollRect.contains (pos)) {
		return;
	}
}

void Rollup::mousePressEvent (QMouseEvent * e) {
	QPoint pos =  e->pos();
	e->accept();

	// tracking page
	if (!m_scrollRect.contains (pos)) {
		setCursor(g_globalData->cursorHand);
		m_trackingMode = TrackingPage;
		m_trackingPos = pos;
		return;
	}

	if (!m_scrollSliderRect.contains(pos)) {
		m_curScrollPos = pos.y() * m_totalPagesHeight / m_scrollRect.height();
		relayout();

		return;
	}

	// tracking scroll bar
	setCursor(QCursor(Qt::SizeVerCursor));
	m_trackingPos = pos;
	m_trackingMode = TrackingScrollbar;
}

void Rollup::mouseReleaseEvent (QMouseEvent * e) {
	QPoint pos =  e->pos();
	e->accept();
	unsetCursor();

	m_trackingMode = TrackingNone;

	if (!m_scrollRect.contains (pos)) {
		return;
	}
}

void Rollup::wheelEvent(QWheelEvent * e) {
	e->accept();

	m_curScrollPos -= e->delta() / 4;

	relayout();
}


void Rollup::resizeEvent (QResizeEvent * e) {
	e->accept();

	relayout();
}

void Rollup::hidePage(int index)
{
	if (index < 0 || index >= m_pages.size())
		return;

	m_pages[index]->hide();
}

void Rollup::showPage(int index)
{
	if (index < 0 || index >= m_pages.size())
		return;

	m_pages[index]->show();
}

void Rollup::setPageVisible(int index, bool visible)
{
	if (index < 0 || index >= m_pages.size())
		return;

	m_pages[index]->setVisible(visible);
}

void Rollup::setGroupVisible(const QString& groupname, bool visible) {
	RollGroups::iterator it = m_groups.find(groupname);

	if (it == m_groups.end()) {
		return;
	}

	const QVector<int>& group = it.value();

	Q_FOREACH(int idx, group) {
		setPageVisible(idx, visible);
	}
}

bool Rollup::initFromTab() {
	QTabWidget* tab = this->findChild<QTabWidget*>();

	if (!tab)
		return false;

	for (int i = 0; i < tab->count(); i++) {
		QWidget* widget = tab->widget(i);
		QString label = tab->tabText(i);

		widget = widget->findChild<QWidget*>();
		if (!widget)
			continue;

		if (!initGroup(label, widget)) {
			addPage(widget, label);
		}
	}

	delete tab;

	return true;
}

bool Rollup::initFromToolbox() {
	QToolBox* toolbox = this->findChild<QToolBox*>();

	if (!toolbox)
		return false;

	for (int i = 0; i < toolbox->count(); i++) {
		QWidget* widget = toolbox->widget(i);
		QString label = toolbox->itemText(i);

		widget = widget->findChild<QWidget*>();
		if (!widget)
			continue;
		addPage(widget, label);
	}

	delete toolbox;

	return true;
}

bool Rollup::initGroup(const QString& grpname, QWidget* widget) {
	QToolBox* toolbox = qobject_cast<QToolBox*>(widget);

	if (!toolbox) {
		return false;
	}

	for (int i = 0; i < toolbox->count(); i++) {
		QWidget* widget = toolbox->widget(i);
		QString label = toolbox->itemText(i);

		widget = widget->findChild<QWidget*>();
		if (!widget)
			continue;

		int idx = addPage(widget, label);

		m_groups[grpname].append(idx);
	}

	delete toolbox;

	return true;
}

