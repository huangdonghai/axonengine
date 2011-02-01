/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "editorframe.h"
#include "workbench.h"

EditorFrame::EditorFrame(QWidget *parent, View *view)
	: QWidget(parent, Qt::MSWindowsOwnDC)
	, m_editorView(view)
	, m_renderTarget(nullptr)
	, m_autoUpdateTimer(0)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_PaintUnclipped);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true); // for compatibility

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::ClickFocus);

	m_editorView->bindFrame(this);

	m_keymap[Qt::Key_Escape]		= InputKey::Escape;                // misc keys
	m_keymap[Qt::Key_Tab]		= InputKey::Tab;
	m_keymap[Qt::Key_Backtab]	= InputKey::Backtab;
	m_keymap[Qt::Key_Backspace]	= InputKey::Backspace;
	m_keymap[Qt::Key_Return]		= InputKey::Enter;
	m_keymap[Qt::Key_Enter]		= InputKey::Enter;
	m_keymap[Qt::Key_Insert]		= InputKey::Insert;
	m_keymap[Qt::Key_Delete]		= InputKey::Delete;
	m_keymap[Qt::Key_Pause]		= InputKey::Pause;
	m_keymap[Qt::Key_Print]		= InputKey::Print;
	m_keymap[Qt::Key_SysReq]		= InputKey::SysReq;
	m_keymap[Qt::Key_Clear]		= InputKey::Clear;
	m_keymap[Qt::Key_Home]		= InputKey::Home;
	m_keymap[Qt::Key_End]		= InputKey::End;
	m_keymap[Qt::Key_Left]		= InputKey::Left;
	m_keymap[Qt::Key_Up]			= InputKey::Up;
	m_keymap[Qt::Key_Right]		= InputKey::Right;
	m_keymap[Qt::Key_Down]		= InputKey::Down;
	m_keymap[Qt::Key_PageUp]		= InputKey::PageUp;
	m_keymap[Qt::Key_PageDown]	= InputKey::PageDown;
	m_keymap[Qt::Key_Shift]		= InputKey::Shift;
	m_keymap[Qt::Key_Control]	= InputKey::Ctrl;
	m_keymap[Qt::Key_Meta]		= InputKey::Meta;
	m_keymap[Qt::Key_Alt]		= InputKey::Alt;
	m_keymap[Qt::Key_CapsLock]	= InputKey::Capslock;
	m_keymap[Qt::Key_NumLock]	= InputKey::NP_Numlock;
	m_keymap[Qt::Key_ScrollLock]	= InputKey::ScorllLock;
	m_keymap[Qt::Key_F1]			= InputKey::F1;
	m_keymap[Qt::Key_F2]			= InputKey::F2;
	m_keymap[Qt::Key_F3]			= InputKey::F3;
	m_keymap[Qt::Key_F4]			= InputKey::F4;
	m_keymap[Qt::Key_F5]			= InputKey::F5;
	m_keymap[Qt::Key_F6]			= InputKey::F6;
	m_keymap[Qt::Key_F7]			= InputKey::F7;
	m_keymap[Qt::Key_F8]			= InputKey::F8;
	m_keymap[Qt::Key_F9]			= InputKey::F9;
	m_keymap[Qt::Key_F10]		= InputKey::F10;
	m_keymap[Qt::Key_F11]		= InputKey::F11;
	m_keymap[Qt::Key_F12]		= InputKey::F12;
	m_keymap[Qt::Key_F13]		= InputKey::F13;
	m_keymap[Qt::Key_F14]		= InputKey::F14;
	m_keymap[Qt::Key_F15]		= InputKey::F15;
#if 0
	m_keymap[Qt::Key_F16]
	m_keymap[Qt::Key_F17]
	m_keymap[Qt::Key_F18]
	m_keymap[Qt::Key_F19]
	m_keymap[Qt::Key_F20]
	m_keymap[Qt::Key_F21]
	m_keymap[Qt::Key_F22]
	m_keymap[Qt::Key_F23]
	m_keymap[Qt::Key_F24]
	m_keymap[Qt::Key_F25]
	m_keymap[Qt::Key_F26]
	m_keymap[Qt::Key_F27]
	m_keymap[Qt::Key_F28]
	m_keymap[Qt::Key_F29]
	m_keymap[Qt::Key_F30]
	m_keymap[Qt::Key_F31]
	m_keymap[Qt::Key_F32]
	m_keymap[Qt::Key_F33]
	m_keymap[Qt::Key_F34]
	m_keymap[Qt::Key_F35]
	m_keymap[Qt::Key_Super_L]
	m_keymap[Qt::Key_Super_R]
#endif
	m_keymap[Qt::Key_Menu]		= InputKey::Menu;
#if 0
	m_keymap[Qt::Key_Hyper_L]
	m_keymap[Qt::Key_Hyper_R]
#endif
	m_keymap[Qt::Key_Help]		= InputKey::Help;
#if 0
	m_keymap[Qt::Key_Direction_L]
	m_keymap[Qt::Key_Direction_R]
#endif
	m_keymap[Qt::Key_Space]		= InputKey::Space;
#if  0
	m_keymap[Qt::Key_Any]
#endif
	m_keymap[Qt::Key_Exclam]		= '!';
	m_keymap[Qt::Key_QuoteDbl]	= '\"';
	m_keymap[Qt::Key_NumberSign]	= '#';
	m_keymap[Qt::Key_Dollar]		= '$';
	m_keymap[Qt::Key_Percent]	= '%';
	m_keymap[Qt::Key_Ampersand]	= '&';
	m_keymap[Qt::Key_Apostrophe]	= '\'';
	m_keymap[Qt::Key_ParenLeft]	= '(';
	m_keymap[Qt::Key_ParenRight]	= ')';
	m_keymap[Qt::Key_Asterisk]	= '*';
	m_keymap[Qt::Key_Plus]		= '+';
	m_keymap[Qt::Key_Comma]		= ',';
	m_keymap[Qt::Key_Minus]		= '-';
	m_keymap[Qt::Key_Period]		= '.';
	m_keymap[Qt::Key_Slash]		= '/';
	m_keymap[Qt::Key_0]			= '0';
	m_keymap[Qt::Key_1]			= '1';
	m_keymap[Qt::Key_2]			= '2';
	m_keymap[Qt::Key_3]			= '3';
	m_keymap[Qt::Key_4]			= '4';
	m_keymap[Qt::Key_5]			= '5';
	m_keymap[Qt::Key_6]			= '6';
	m_keymap[Qt::Key_7]			= '7';
	m_keymap[Qt::Key_8]			= '8';
	m_keymap[Qt::Key_9]			= '9';
	m_keymap[Qt::Key_Colon]		= ':';
	m_keymap[Qt::Key_Semicolon]	= ';';
	m_keymap[Qt::Key_Less]		= '<';
	m_keymap[Qt::Key_Equal]		= '=';
	m_keymap[Qt::Key_Greater]	= '>';
	m_keymap[Qt::Key_Question]	= '?';
	m_keymap[Qt::Key_At]			= '@';
	m_keymap[Qt::Key_A]			= 'a';
	m_keymap[Qt::Key_B]			= 'b';
	m_keymap[Qt::Key_C]			= 'c';
	m_keymap[Qt::Key_D]			= 'd';
	m_keymap[Qt::Key_E]			= 'e';
	m_keymap[Qt::Key_F]			= 'f';
	m_keymap[Qt::Key_G]			= 'g';
	m_keymap[Qt::Key_H]			= 'h';
	m_keymap[Qt::Key_I]			= 'i';
	m_keymap[Qt::Key_J]			= 'j';
	m_keymap[Qt::Key_K]			= 'k';
	m_keymap[Qt::Key_L]			= 'l';
	m_keymap[Qt::Key_M]			= 'm';
	m_keymap[Qt::Key_N]			= 'n';
	m_keymap[Qt::Key_O]			= 'o';
	m_keymap[Qt::Key_P]			= 'p';
	m_keymap[Qt::Key_Q]			= 'q';
	m_keymap[Qt::Key_R]			= 'r';
	m_keymap[Qt::Key_S]			= 's';
	m_keymap[Qt::Key_T]			= 't';
	m_keymap[Qt::Key_U]			= 'u';
	m_keymap[Qt::Key_V]			= 'v';
	m_keymap[Qt::Key_W]			= 'w';
	m_keymap[Qt::Key_X]			= 'x';
	m_keymap[Qt::Key_Y]			= 'y';
	m_keymap[Qt::Key_Z]			= 'z';
	m_keymap[Qt::Key_BracketLeft]= '[';
	m_keymap[Qt::Key_Backslash]	= '\\';
	m_keymap[Qt::Key_BracketRight]= ']';
#if 0
	m_keymap[Qt::Key_AsciiCircum]	
#endif
	m_keymap[Qt::Key_Underscore]	= '_';
	m_keymap[Qt::Key_QuoteLeft]	= '\'';
	m_keymap[Qt::Key_BraceLeft]	= '{';
	m_keymap[Qt::Key_Bar]		= '|';
	m_keymap[Qt::Key_BraceRight] = '}';
	m_keymap[Qt::Key_AsciiTilde]	= '~';

	m_tabletPressed = false;
	m_isCapturing = false;
	m_mouseMode = InputSystem::Normal_Mode;
}

EditorFrame::~EditorFrame()
{
	m_editorView->bindFrame(0);
}

RenderTarget *EditorFrame::renderTarget()
{
	if (m_renderTarget == nullptr) {
		m_renderTarget = new RenderTarget(Handle(winId()), "EditorFrame", size());
	} else {
		m_renderTarget->updateWindowInfo(Handle(winId()), size());
	}

	return m_renderTarget;
}

Size EditorFrame::size()
{
	QSize s = QWidget::size();

	return Size(s.width(), s.height());
}

void EditorFrame::setCursor(CursorType cursor_type)
{
	switch (cursor_type) {
	case CursorType::Default:
		unsetCursor();
		break;
	case CursorType::Arrow:
		QWidget::setCursor(Qt::ArrowCursor);
		break;
	case CursorType::UpArrow:		
		QWidget::setCursor(Qt::UpArrowCursor);
		break;
	case CursorType::Cross:		
		QWidget::setCursor(Qt::CrossCursor);
		break;
	case CursorType::Wait:		
		QWidget::setCursor(Qt::WaitCursor);
		break;
	case CursorType::IBeam:		
		QWidget::setCursor(Qt::IBeamCursor);
		break;
	case CursorType::SizeVer:		
		QWidget::setCursor(Qt::SizeVerCursor);
		break;
	case CursorType::SizeHor:		
		QWidget::setCursor(Qt::SizeHorCursor);
		break;
	case CursorType::SizeFDiag:	
		QWidget::setCursor(Qt::SizeFDiagCursor);
		break;
	case CursorType::SizeBDiag:	
		QWidget::setCursor(Qt::SizeBDiagCursor);
		break;
	case CursorType::SizeAll:		
		QWidget::setCursor(Qt::SizeAllCursor);
		break;
	case CursorType::Blank:		
		QWidget::setCursor(Qt::BlankCursor);
		break;
	case CursorType::SplitV:		
		QWidget::setCursor(Qt::SplitVCursor);
		break;
	case CursorType::SplitH:		
		QWidget::setCursor(Qt::SplitHCursor);
		break;
	case CursorType::PointingHand:
		QWidget::setCursor(Qt::PointingHandCursor);
		break;
	case CursorType::Forbidden:	
		QWidget::setCursor(Qt::ForbiddenCursor);
		break;
	case CursorType::WhatsThis:	
		QWidget::setCursor(Qt::WhatsThisCursor);
		break;
	case CursorType::Busy:		
		QWidget::setCursor(Qt::BusyCursor);
		break;

	case CursorType::ViewRotate:
		QWidget::setCursor(g_globalData->cursorViewRotate);
		break;
	case CursorType::ViewZoom:
		QWidget::setCursor(g_globalData->cursorViewZoom);
		break;
	case CursorType::ViewPan:
		QWidget::setCursor(g_globalData->cursorViewPan);
		break;
	}
}

void EditorFrame::setCursorPos(const Point &pos) {
	QPoint p(pos.x, pos.y);
	p = mapToGlobal(p);
	cursor().setPos(p);
}

void EditorFrame::setNeedUpdate() {
	update();
}

void EditorFrame::setAutoUpdate(bool b) {
	if (b) {
		if (m_autoUpdateTimer)
			return;

		m_autoUpdateTimer = startTimer(50);
		return;
	}

	if (m_autoUpdateTimer) {
		killTimer(m_autoUpdateTimer);
		m_autoUpdateTimer = 0;
	}
}


bool EditorFrame::eventFilter(QObject * watched, QEvent * event) {
	if (event->type() == QEvent::Enter) {
		setMouseTracking(true);
	}

	if (event->type() == QEvent::Leave) {
		setMouseTracking(false);
	}
	return QWidget::eventFilter(watched, event);
}

void EditorFrame::paintEvent(QPaintEvent *pe) {
	if (!updatesEnabled())
		return;

#if 1
	m_editorView->doUpdate();
#endif
}

void EditorFrame::translateMouseEvent(QMouseEvent *e, InputEvent *xe) {
	Qt::MouseButtons btns = e->buttons();
	Qt::MouseButton btn = e->button();

	if (e->type() == QEvent::MouseMove) {
		if (btns & Qt::LeftButton)
			xe->key = InputKey::MouseLeft;
		else if ( btns & Qt::RightButton)
			xe->key = InputKey::MouseRight;
		else if (btns & Qt::MidButton)
			xe->key = InputKey::MouseMiddle;
	} else {
		if (btn == Qt::LeftButton) {
			xe->key = InputKey::MouseLeft;
		} else if (btn == Qt::RightButton) {
			xe->key = InputKey::MouseRight;
		} else if (btn == Qt::MidButton) {
			xe->key = InputKey::MouseMiddle;
		}
	}

	if (xe->type == InputEvent::MouseMove && m_mouseMode == InputSystem::FPS_Mode) {
		xe->pos.x = e->x();
		xe->pos.y = e->y();
		xe->pos = xe->pos - m_mouseCenter;
		setCursorPos(m_mouseCenter);
	} else {
		xe->pos.x = e->x();
		xe->pos.y = e->y();
	}

	Qt::KeyboardModifiers mod = e->modifiers();
	if (mod & Qt::ShiftModifier)
		xe->flags |= InputEvent::ShiftModifier;
	if (mod & Qt::AltModifier)
		xe->flags |= InputEvent::AltModifier;
	if (mod & Qt::ControlModifier)
		xe->flags |= InputEvent::ControlModifier;

	xe->time = OsUtil::seconds();

	issueEvent(e, *xe);
}


void EditorFrame::mousePressEvent(QMouseEvent *e) {
	InputEvent xe;

	TypeZero(&xe);

	xe.type = InputEvent::MouseDown;

	translateMouseEvent(e, &xe);
}

void EditorFrame::mouseReleaseEvent(QMouseEvent *e) {
	InputEvent xe;

	TypeZero(&xe);

	xe.type = InputEvent::MouseUp;

	translateMouseEvent(e, &xe);
}

void EditorFrame::mouseMoveEvent(QMouseEvent *e) {
	int x = e->x();
	int y = e->y();
	static int last_x = x,last_y = y;
	int dx = x - last_x;
	int dy = y - last_y;
	last_x = x;
	last_y = y;

	InputEvent xe;

	TypeZero(&xe);

	xe.type = InputEvent::MouseMove;

	translateMouseEvent(e, &xe);
}

void EditorFrame::wheelEvent(QWheelEvent *e) {
	InputEvent xe;

	TypeZero(&xe);

	xe.type = InputEvent::Wheel;
	xe.delta = e->delta();

	if (e->delta() > 0)
		xe.key = InputKey::MouseWheelUp;
	else
		xe.key = InputKey::MouseWheelDown;

	xe.time = OsUtil::seconds();

	Qt::KeyboardModifiers mod = e->modifiers();
	if (mod & Qt::ShiftModifier)
		xe.flags |= InputEvent::ShiftModifier;
	if (mod & Qt::AltModifier)
		xe.flags |= InputEvent::AltModifier;
	if (mod & Qt::ControlModifier)
		xe.flags |= InputEvent::ControlModifier;

	issueEvent(e, xe);
}

void EditorFrame::enterEvent(QEvent *e) {
//	grabMouse();
	setMouseTracking(true);
}

void EditorFrame::leaveEvent(QEvent *e) {
//	releaseMouse();
	setMouseTracking(false);
}

#if 0
void EditorFrame::tabletEvent(QTabletEvent *e) {
	static int seq = 0;

	Event xe;
	TypeZero(&xe);
	static Point lastpos;

	QPoint curpos = QCursor::pos();
	curpos = this->mapFromGlobal(curpos);

#if 0
	xe.pos.x = e->x();
	xe.pos.y = e->y();
#else
	xe.pos.x = curpos.x();
	xe.pos.y = curpos.y();
#endif

	int pressure = e->pressure() * 256;

	switch (e->type()) {
		 case QEvent::TabletPress:
//			m_tabletPressed = true;
			xe.key = InputKey::MouseLeft;
			xe.type = InputEvent::MouseDown;
			goto localexit;
			break;
		 case QEvent::TabletRelease:
//			m_tabletPressed = false;
			xe.key = InputKey::MouseLeft;
			xe.type = InputEvent::MouseUp;			
			goto localexit;
			break;
		 case QEvent::TabletMove:
			 {
				 if (!m_tabletPressed && pressure > 127) {
					 m_tabletPressed = true;
					 xe.key = InputKey::MouseLeft;
					 xe.type = InputEvent::MouseDown;
					 break;
				 }

				 if (m_tabletPressed && pressure <= 127) {
					 m_tabletPressed = false;
					 xe.key = InputKey::MouseLeft;
					 xe.type = InputEvent::MouseUp;
					 break;
				 }
				 seq++;

				 if (lastpos == xe.pos) {
					 Printf("discard Move %d", seq);
					 goto localexit;
				 }

				 if (e->pressure() == 0) {
					 goto localexit;
				 }

#if 0
				 if (xe.pos.x != curpos.x() || xe.pos.y != curpos.y()) {
					 goto localexit;
				 }
#endif
				 if (seq % 8) {
					 goto localexit;
				 }
				 Printf("used Move %d", seq);
				 lastpos = xe.pos;
				 xe.type = InputEvent::MouseMove;

				 if (m_tabletPressed) {
					 xe.key = InputKey::MouseLeft;
				 } else {
					 xe.key = 0;
				 }
				 if (e->pointerType() == QTabletEvent::Pen){
					 xe.delta = e->pressure() * 256;					 
				 }else if (e->pointerType() == QTabletEvent::Eraser){
					 xe.delta = e->pressure() * -256;
				 }
				 //char tmp[128];
				 //sprintf(tmp,"x=%d y=%d gx=%d gy=%d ugx=%f ugy=%f pressure = %f\n"
				 //	,e->x(),e->y(),e->globalX(),e->globalY(),e->hiResGlobalX(),e->hiResGlobalY(),e->pressure());
				 //Debugf(tmp);
			 }
		 break;
		 default:
			 return;
			 break;
	}
	xe.time = milliseconds();

	if (m_isCapturing) {
		g_inputSystem->queEvent(xe);
		return;
	}

	m_editorView->handleEvent(&xe);

localexit:
#if 0
	if (xe.accepted)
#endif
		e->accept();
}
#endif

void EditorFrame::keyPressEvent(QKeyEvent *e) {
	if (e->isAutoRepeat())
		return;

	int k = e->key();

	QHash<int,int>::const_iterator it = m_keymap.find(k);
	if (it == m_keymap.end())
		return;

	InputEvent xe;
	
	TypeZero(&xe);

	xe.type = InputEvent::KeyDown;
	xe.key = (InputKey::Type)it.value();

	Qt::KeyboardModifiers mod = e->modifiers();
	if (mod & Qt::ShiftModifier)
		xe.flags |= InputEvent::ShiftModifier;
	if (mod & Qt::AltModifier)
		xe.flags |= InputEvent::AltModifier;
	if (mod & Qt::ControlModifier)
		xe.flags |= InputEvent::ControlModifier;

	xe.time = OsUtil::seconds();

	issueEvent(e, xe);
}

void EditorFrame::keyReleaseEvent(QKeyEvent *e) {
	if (e->isAutoRepeat())
		return;
	int k = e->key();

	QHash<int,int>::const_iterator it = m_keymap.find(k);
	if (it == m_keymap.end())
		return;

	InputEvent xe;

	TypeZero(&xe);

	xe.type = InputEvent::KeyUp;
	xe.key = (InputKey::Type)it.value();

	Qt::KeyboardModifiers mod = e->modifiers();
	if (mod & Qt::ShiftModifier)
		xe.flags |= InputEvent::ShiftModifier;
	if (mod & Qt::AltModifier)
		xe.flags |= InputEvent::AltModifier;
	if (mod & Qt::ControlModifier)
		xe.flags |= InputEvent::ControlModifier;

	xe.time = OsUtil::seconds();

	issueEvent(e, xe);
}

void EditorFrame::timerEvent(QTimerEvent *e) {
	if (e->timerId() == m_autoUpdateTimer) {
		update();
		return;
	}
}

void EditorFrame::startCapture(InputSystem::CaptureMode capturemode) {
	m_isCapturing = true;
	m_captureMode = capturemode; 

	if (m_captureMode == InputSystem::Exclusive) {
#ifndef _DEBUG
		grabMouse(Qt::BlankCursor);
		grabKeyboard();
#else
		qApp->setOverrideCursor(Qt::BlankCursor);
#endif
	}
}

void EditorFrame::process() {
	AX_ASSERT(m_isCapturing);
	QApplication::processEvents();
}

void EditorFrame::setVibration(float left, float right) {
	// do nothing
}

void EditorFrame::stopCapture() {
	m_isCapturing = false;

	if (m_captureMode == InputSystem::Exclusive) {
#ifndef _DEBUG
		releaseMouse();
		releaseKeyboard();
#else
		qApp->restoreOverrideCursor();
#endif
	}
}

#ifndef WM_INPUT
#define WM_INPUT                        0x00FF
#endif

#if 0
bool EditorFrame::winEvent(MSG *msg, long * result) {
	if (!m_isInEventCapture) {
		return false;
	}

	if (msg->message != WM_INPUT) {
		return false;
	}

	UINT dwSize = 40;
	static BYTE lpb[40];

	GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT *raw = (RAWINPUT*)lpb;

	Event e;
	TypeZero(&e);

	if (raw->header.dwType == RIM_TYPEMOUSE) {
		e.pos.x = raw->data.mouse.lLastX;
		e.pos.y = raw->data.mouse.lLastY;
	}

	*result = 0;

	return true;
}
#endif

void EditorFrame::registerEventSource()
{
	g_inputSystem->registerEventSource(this);
}

void EditorFrame::removeEventSource()
{
	g_inputSystem->removeEventSource(this);
}

void EditorFrame::resetCursor() {
	this->unsetCursor();
}

void EditorFrame::issueEvent(QEvent *qe, InputEvent &xe)
{
	if (m_isCapturing) {
		g_inputSystem->queEvent(xe);
		return;
	}

	m_editorView->handleEvent(&xe);

	if (xe.accepted)
		qe->accept();
}

#if 0
void EditorFrame::resetMousePos()
{
	QSize s = size();
	setCursorPos(Point(s.width()/2, s.height()/2));
}

void EditorFrame::hideHardwareCursor()
{
	setCursor(CursorType::Blank);
}

void EditorFrame::showHardwareCursor()
{
	setCursor(CursorType::Default);
}
#endif

void EditorFrame::setMouseMode(InputSystem::MouseMode mode)
{
	m_mouseMode = mode;
	
	if (m_mouseMode == InputSystem::Normal_Mode) {
		setCursor(CursorType::Default);
	} else {
		QSize s = QWidget::size();
		m_mouseCenter.set(s.width()/2, s.height()/2);
		setCursorPos(m_mouseCenter);
	}
}
