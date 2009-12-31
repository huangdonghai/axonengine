/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "workspace.h"
#include "newmapdlg.h"
#include "editorframe.h"
#include "filedialog.h"
#include "workbench.h"


class WindowInput : public IInputSource {
public:
	WindowInput();
	~WindowInput();

	virtual void startCapture() = 0;
	virtual void process() = 0;
	virtual void setVibration(float left, float right) = 0;
	virtual void stopCapture() = 0;

private:
};

Grabber::Grabber(CrossSplitter *parent, Tracking tracking)
	: QWidget(parent)
	, m_parent(parent)
	, m_tracking(tracking)
	, m_isTracking(false)
{
	AX_ASSERT(m_tracking != None);

	setMouseTracking(true);

	if (m_tracking == Horizontal)
		setCursor(Qt::SizeHorCursor);
	else if (m_tracking == Vertical)
		setCursor(Qt::SizeVerCursor);
	else
		setCursor(Qt::SizeAllCursor);
}

void Grabber::mouseMoveEvent(QMouseEvent * event) {
	if (!m_isTracking)
		return;

	QPoint pos = event->pos();
	pos = mapTo(m_parent, pos);

	m_parent->doTracking(pos);
}

void Grabber::mousePressEvent(QMouseEvent * event) {
	Qt::MouseButton button = event->button();

	if (button != Qt::LeftButton) {
		return;
	}
	m_isTracking = true;
	QPoint pos = event->pos();
	pos = mapTo(m_parent, pos);

	m_parent->beginTracking(m_tracking, pos);
}

void Grabber::mouseReleaseEvent(QMouseEvent * event) {
	if (!m_isTracking)
		return;

	Qt::MouseButton button = event->button();

	if (button != Qt::LeftButton) {
		return;
	}

	QPoint pos = event->pos();
	pos = mapTo(m_parent, pos);

	m_parent->endTracking(pos);

	m_isTracking = false;
}

void Grabber::leaveEvent(QEvent * event) {
}


CrossSplitter::CrossSplitter(QWidget *parent, Workspace *ws)
	: QFrame(parent)
	, m_splitPos(0.5, 0.5)
	, m_workspace(ws)
	, m_tracking(Grabber::None)
	, m_frameMaximumed(false)
{
#if 0
	m_rubberBands[0] = NULL;
	m_rubberBands[1] = NULL;
#endif
	for (int i=0; i<4; i++) {
		Frame *frame = ws->getFrame(i);
		frame->setParent(this);
	}

	// create grabber widget
	m_horGrabbers[0] = new Grabber(this, Grabber::Horizontal);
	m_horGrabbers[1] = new Grabber(this, Grabber::Horizontal);
	m_verGrabbers[0] = new Grabber(this, Grabber::Vertical);
	m_verGrabbers[1] = new Grabber(this, Grabber::Vertical);
	m_bothGrabber = new Grabber(this, Grabber::Both);
}

CrossSplitter::~CrossSplitter() {}

void CrossSplitter::setFrameMaximun(int index, bool maximum) {
	m_frameMaximumed = maximum;
	m_maximumIndex = index;
	relayout();
}

void CrossSplitter::beginTracking(Tracking tracking, const QPoint &pos) {
	m_tracking = tracking;
	AX_ASSERT(m_tracking != Grabber::None);

#if 0
	if (!m_rubberBands[0] && m_tracking & Grabber::Horizontal) {
		m_rubberBands[0] = new QRubberBand(QRubberBand::Line, this);

		QRect rect(pos.x()-(TrackingWidth>>1) - 1, 0, TrackingWidth+2, size().height());

		m_rubberBands[0]->setGeometry(rect);
		m_rubberBands[0]->show();
	}

	if (!m_rubberBands[1] && m_tracking & Grabber::Vertical) {
		m_rubberBands[1] = new QRubberBand(QRubberBand::Line, this);

		QRect rect(0, pos.y()-(TrackingWidth>>1) - 1, size().width(), TrackingWidth+2);

		m_rubberBands[1]->setGeometry(rect);
		m_rubberBands[1]->show();
	}
#endif
}

void CrossSplitter::doTracking(const QPoint &pos) {
#if 0
	if (m_tracking & Grabber::Horizontal) {
		QRect rect(pos.x()-(TrackingWidth>>1) - 1, 0, TrackingWidth+2, size().height());
		m_rubberBands[0]->setGeometry(rect);
	}

	if (m_tracking & Grabber::Vertical) {
		QRect rect(0, pos.y()-(TrackingWidth>>1) - 1, size().width(), TrackingWidth+2);
		m_rubberBands[1]->setGeometry(rect);
	}
#endif
	if (m_tracking & Grabber::Horizontal) {
		m_splitPos.setX((float)pos.x() / size().width());
#if 0
		SafeDelete(m_rubberBands[0]);
#endif
	}
	if (m_tracking & Grabber::Vertical) {
		m_splitPos.setY((float)pos.y() / size().height());
#if 0
		SafeDelete(m_rubberBands[1]);
#endif
	}

	relayout();
}

void CrossSplitter::endTracking(const QPoint &pos) {

	if (m_tracking & Grabber::Horizontal) {
		m_splitPos.setX((float)pos.x() / size().width());
#if 0
		SafeDelete(m_rubberBands[0]);
#endif
	}
	if (m_tracking & Grabber::Vertical) {
		m_splitPos.setY((float)pos.y() / size().height());
#if 0
		SafeDelete(m_rubberBands[1]);
#endif
	}

	relayout();

	m_tracking = Grabber::None;
}

void CrossSplitter::relayout() {
	QSize sz = size();

	int x1 = sz.width() * m_splitPos.x() - (TrackingWidth >> 1);
	int y1 = sz.height() * m_splitPos.y() - (TrackingWidth >> 1);
	int x2 = x1 + TrackingWidth;
	int y2 = y1 + TrackingWidth;
	int w2 = sz.width() - x2;
	int h2 = sz.height() - y2;

	m_frameRects[0].setRect(0, 0, x1, y1);
	m_frameRects[1].setRect(x2, 0, w2, y1);
	m_frameRects[2].setRect(0, y2, x1, h2);
	m_frameRects[3].setRect(x2, y2, w2, h2);

	m_bothResizeRect.setRect(x1, y1, TrackingWidth, TrackingWidth);
	m_horResizeRects[0].setRect(x1, 0, TrackingWidth, y1);
	m_horResizeRects[1].setRect(x1, y2, TrackingWidth, h2);
	m_verResizeRects[0].setRect(0, y1, x1, TrackingWidth);
	m_verResizeRects[1].setRect(x2, y1, w2, TrackingWidth);

	if (m_frameMaximumed) {
		m_bothGrabber->hide();
		m_horGrabbers[0]->hide();
		m_horGrabbers[1]->hide();
		m_verGrabbers[0]->hide();
		m_verGrabbers[1]->hide();

		for (int i = 0; i<4; i++) {
			if (i == m_maximumIndex) {
				m_workspace->getFrame(i)->setGeometry(0, 0, size().width(), size().height());
				m_workspace->getFrame(i)->show();
			} else {
				m_workspace->getFrame(i)->hide();
			}
		}
	} else {
		m_bothGrabber->show();
		m_horGrabbers[0]->show();
		m_horGrabbers[1]->show();
		m_verGrabbers[0]->show();
		m_verGrabbers[1]->show();

		m_bothGrabber->setGeometry(m_bothResizeRect);
		m_horGrabbers[0]->setGeometry(m_horResizeRects[0]);
		m_horGrabbers[1]->setGeometry(m_horResizeRects[1]);
		m_verGrabbers[0]->setGeometry(m_verResizeRects[0]);
		m_verGrabbers[1]->setGeometry(m_verResizeRects[1]);
#if 1
		m_workspace->getFrame(0)->show();
		m_workspace->getFrame(1)->show();
		m_workspace->getFrame(2)->show();
		m_workspace->getFrame(3)->show();
#endif
		m_workspace->getFrame(0)->setGeometry(m_frameRects[0]);
		m_workspace->getFrame(0)->repaint();
		m_workspace->getFrame(1)->setGeometry(m_frameRects[1]);
		m_workspace->getFrame(1)->repaint();
		m_workspace->getFrame(2)->setGeometry(m_frameRects[2]);
		m_workspace->getFrame(2)->repaint();
		m_workspace->getFrame(3)->setGeometry(m_frameRects[3]);
		m_workspace->getFrame(3)->repaint();
	}

	update();
}

void CrossSplitter::paintEvent(QPaintEvent *pe) {
	QFrame::paintEvent(pe);
}

void CrossSplitter::resizeEvent(QResizeEvent *event) {
	relayout();
}

void CrossSplitter::mouseMoveEvent(QMouseEvent * event) {
}

void CrossSplitter::mousePressEvent(QMouseEvent * event) {
}

void CrossSplitter::mouseReleaseEvent(QMouseEvent * event) {
}

void CrossSplitter::leaveEvent(QEvent * event) {
}


Workspace::Workspace(QWidget *parent)
    : QWidget(parent)
	, m_frameMaximumed(false)
{
	ui.setupUi(this);

	// create editor view panels
	for (int i = 0; i < g_mapContext->getNumViews(); i++) {
		m_editorFrames[i] = new EditorFrame(nullptr, g_mapContext->getView(i));
	}

	// create panel's frame
	for (int i = 0; i < 4; i++) {
		m_frames[i] = new Frame(NULL, this);
		m_frames[i]->setPanel(m_editorFrames[i]);
	}

	m_frames[3]->setEditorActive(true);
//	ui.frame->setBackgroundRole(QPalette::Shadow);

	m_frameLayouter = new CrossSplitter(ui.frame, this);
	ui.frame->layout()->addWidget(m_frameLayouter);

	toggleMaximun();

	// init editor state
	ui.moveSpeed->setValue(g_mapContext->getMapState()->moveSpeed);
	ui.actionToggleFrameMaximum->setEnabled(true);

	ui.transformX->setSpecialValueText("?");
	ui.transformY->setSpecialValueText("?");
	ui.transformZ->setSpecialValueText("?");
	g_mapContext->getMapState()->attachObserver(this);
}

Workspace::~Workspace()
{
	g_mapContext->getMapState()->detachObserver(this);
}

Frame *Workspace::getActiveFrame() {
	for (int i = 0; i < 4; i++) {
		if (m_frames[i]->isActived())
			return m_frames[i];
	}

	return NULL;
}

int Workspace::getActiveFrameIndex() {
	int i;
	for (i = 0; i < 4; i++) {
		if (m_frames[i]->isActived())
			return i;
	}
	return i;
}


void Workspace::frameActive(Frame *frame) {
	// clear first
	for (int i = 0; i < 4; i++) {
		if (m_frames[i] == frame)
			m_frames[i]->setEditorActive(true);
		else
			m_frames[i]->setEditorActive(false);

		m_frames[i]->update();
	}
}


void Workspace::on_actionToggleFrameMaximum_triggered() {
	toggleMaximun();
}

void Workspace::on_moveSpeed_valueChanged(int v)
{
	g_mapContext->getMapState()->moveSpeed = v;
}

void Workspace::on_transformAbsOrRel_clicked()
{
	if (ui.transformAbsOrRel->isChecked()) {
		g_mapContext->getMapState()->transformRel = true;
		ui.transformAbsOrRel->setIcon(QIcon(QString::fromUtf8(":/images/gizmo_rel.png")));
	} else {
		g_mapContext->getMapState()->transformRel = false;
		ui.transformAbsOrRel->setIcon(QIcon(QString::fromUtf8(":/images/gizmo_abs.png")));
	}

	g_mapContext->getMapState()->notify(State::Transform);
}

void Workspace::doNotify(IObservable *subject, int arg)
{
	if (subject != g_mapContext->getMapState())
		return;

	if (arg != State::TransformToApp)
		return;

	if (g_mapContext->getMapState()->transformState.editable) {
		ui.transformX->setDisabled(false);
		ui.transformY->setDisabled(false);
		ui.transformZ->setDisabled(false);
	} else {
		ui.transformX->setDisabled(true);
		ui.transformY->setDisabled(true);
		ui.transformZ->setDisabled(true);
	}

	if (g_mapContext->getMapState()->transformState.clear) {
		ui.transformX->setValue(ui.transformX->minimum());
		ui.transformY->setValue(ui.transformX->minimum());
		ui.transformZ->setValue(ui.transformX->minimum());
	} else {
		ui.transformX->setValue(g_mapContext->getMapState()->transformState.parameter.x);
		ui.transformY->setValue(g_mapContext->getMapState()->transformState.parameter.y);
		ui.transformZ->setValue(g_mapContext->getMapState()->transformState.parameter.z);
	}
}


void Workspace::on_transformX_editingFinished()
{
	if (!g_mapContext->getMapState()->transformState.editable) {
		return;
	}

	float f = ui.transformX->value();

	if (!g_mapContext->getMapState()->transformState.clear) {
		if (f == g_mapContext->getMapState()->transformState.parameter.x)
			return;
	}

	Tool *tool = g_mapContext->getTool();

	if (!tool)
		return;

	TransformTool *transformtool = dynamic_cast<TransformTool*>(tool);

	if (!transformtool)
		return;

	transformtool->begin();
	transformtool->transform(Vector3(f, 0, 0), 0);
	transformtool->end();
}

void Workspace::on_transformY_editingFinished()
{
	if (!g_mapContext->getMapState()->transformState.editable) {
		return;
	}

	float f = ui.transformY->value();

	if (!g_mapContext->getMapState()->transformState.clear) {
		if (f == g_mapContext->getMapState()->transformState.parameter.y)
			return;
	}

	Tool *tool = g_mapContext->getTool();

	if (!tool)
		return;

	TransformTool *transformtool = dynamic_cast<TransformTool*>(tool);

	if (!transformtool)
		return;

	transformtool->begin();
	transformtool->transform(Vector3(0, f, 0), 1);
	transformtool->end();
}

void Workspace::on_transformZ_editingFinished()
{
	if (!g_mapContext->getMapState()->transformState.editable) {
		return;
	}

	float f = ui.transformZ->value();

	if (!g_mapContext->getMapState()->transformState.clear) {
		if (f == g_mapContext->getMapState()->transformState.parameter.z)
			return;
	}

	Tool *tool = g_mapContext->getTool();

	if (!tool)
		return;

	TransformTool *transformtool = dynamic_cast<TransformTool*>(tool);

	if (!transformtool)
		return;

	transformtool->begin();
	transformtool->transform(Vector3(0, 0, f), 2);
	transformtool->end();
}

void Workspace::toggleMaximun() {
	m_maximunIndex = getActiveFrameIndex();
	m_frameMaximumed = !m_frameMaximumed;

	m_frameLayouter->setFrameMaximun(m_maximunIndex, m_frameMaximumed);

//	grabKeyboard();
//	grabMouse();
}

void Workspace::on_bookmarkList_currentIndexChanged(int index)
{
	
}

void Workspace::on_bookmarkList_editTextChanged(QString text)
{
	int index = ui.bookmarkList->currentIndex();

	if (text == "" || index < 0 
		|| g_mapContext->getBookmark(index)->name == q2u(text))
	{
		return ;
	}

	if (g_mapContext->getBookmark(q2u(text)) != NULL)
	{
		g_mapContext->getBookmark(index)->name = q2u(text) + "_2";
		ui.bookmarkList->setItemText(index, text + "_2");
	}
	else
	{
		g_mapContext->getBookmark(index)->name = q2u(text);
		ui.bookmarkList->setItemText(index, text);
	}
}

void Workspace::on_bookmarkGoto_released()
{
	int n = ui.bookmarkList->currentIndex();
	if (n < 0)
	{
		return ;
	}

	Bookmark *bookmark = g_mapContext->getBookmark(ui.bookmarkList->currentIndex());

	if (bookmark != NULL)
	{
		g_mapContext->getActiveView()->setEyeMatrix(bookmark->viewMatrix);
	}
}

void Workspace::on_bookmarkAdd_released()
{
	const RenderCamera &camera = g_mapContext->getActiveView()->getCamera();
	
	AffineMat viewMatrix;

	viewMatrix.origin = (camera.getOrigin());
	viewMatrix.axis = (camera.getViewAxis());

	/*
	int index = ui.bookmarkList->currentIndex();
	QString text = ui.bookmarkList->currentText();
	
	if (text == "" || gEditorContext->getBookmark(q2u(text)) != NULL)
	{
		gEditorContext->addBookmark(viewMatrix);
	}
	else
	{
		gEditorContext->addBookmark(viewMatrix, q2u(text));
	}
	*/

	g_mapContext->addBookmark(viewMatrix);
	refreshBookmarks();
	ui.bookmarkList->setCurrentIndex(ui.bookmarkList->count() - 1);
}

void Workspace::on_bookmarkDel_released()
{
	int n = ui.bookmarkList->currentIndex();
	if (n < 0)
	{
		return ;
	}
	
	g_mapContext->deleteBookmark(ui.bookmarkList->currentIndex());

	ui.bookmarkList->setCurrentIndex(-1);

	refreshBookmarks();
}

void Workspace::refreshBookmarks()
{
	if (ui.bookmarkList->count() > 0)
	{
		ui.bookmarkList->clear();
	}

	int n = g_mapContext->getNumBookmark();
	Bookmark *bookmark = 0;

	for (int i=0; i<n; ++i)
	{
		bookmark = g_mapContext->getBookmark(i);

		if (bookmark != 0)
		{
			ui.bookmarkList->addItem(u2q(bookmark->name));
		}
	}
}

void Workspace::on_horizontalSlider_valueChanged(int)
{

}