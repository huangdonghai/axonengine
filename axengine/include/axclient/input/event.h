/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CLIENT_EVENT_H
#define AX_CLIENT_EVENT_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// struct InputKey
//--------------------------------------------------------------------------
#define AX_KEYITEMS \
	KEYITEM(Tab) \
	KEYITEM(Backtab) \
	KEYITEM(Enter) \
	KEYITEM(Escape) \
	KEYITEM(Space) \
	KEYITEM(Backspace) \
	KEYITEM(Command) \
	KEYITEM(Capslock) \
	KEYITEM(Power) \
	KEYITEM(Pause) \
	KEYITEM(Up) \
	KEYITEM(Down) \
	KEYITEM(Left) \
	KEYITEM(Right) \
	KEYITEM(Print) \
	KEYITEM(SysReq) \
	KEYITEM(Clear) \
	KEYITEM(ScorllLock) \
	KEYITEM(Alt) \
	KEYITEM(Ctrl) \
	KEYITEM(Shift) \
	KEYITEM(Meta) \
	KEYITEM(Menu) \
	KEYITEM(Help) \
	KEYITEM(Insert) \
	KEYITEM(Delete) \
	KEYITEM(PageDown) \
	KEYITEM(PageUp) \
	KEYITEM(Home) \
	KEYITEM(End) \
	KEYITEM(F1) \
	KEYITEM(F2) \
	KEYITEM(F3) \
	KEYITEM(F4) \
	KEYITEM(F5) \
	KEYITEM(F6) \
	KEYITEM(F7) \
	KEYITEM(F8) \
	KEYITEM(F9) \
	KEYITEM(F10) \
	KEYITEM(F11) \
	KEYITEM(F12) \
	KEYITEM(F13) \
	KEYITEM(F14) \
	KEYITEM(F15) \
	KEYITEM(MouseLeft) \
	KEYITEM(MouseRight) \
	KEYITEM(MouseMiddle) \
	KEYITEM(MouseBtn4) \
	KEYITEM(MouseBtn5) \
	KEYITEM(MouseBtn6) \
	KEYITEM(MouseBtn7) \
	KEYITEM(MouseBtn8) \
	KEYITEM(MouseWheelDown) \
	KEYITEM(MouseWheelUp) \
	KEYITEM(NP_Home) \
	KEYITEM(NP_Up) \
	KEYITEM(NP_Pageup) \
	KEYITEM(NP_Left) \
	KEYITEM(NP_5) \
	KEYITEM(NP_Right) \
	KEYITEM(NP_End) \
	KEYITEM(NP_Down) \
	KEYITEM(NP_Pagedown) \
	KEYITEM(NP_Enter) \
	KEYITEM(NP_Insert) \
	KEYITEM(NP_Delete) \
	KEYITEM(NP_Slash) \
	KEYITEM(NP_Minus) \
	KEYITEM(NP_Plus) \
	KEYITEM(NP_Numlock) \
	KEYITEM(NP_Star) \
	KEYITEM(NP_Equals) \
	KEYITEM(XBOX_A) \
	KEYITEM(XBOX_B) \
	KEYITEM(XBOX_X) \
	KEYITEM(XBOX_Y) \
	KEYITEM(XBOX_rshoulder) \
	KEYITEM(XBOX_lshoulder) \
	KEYITEM(XBOX_ltrigger) \
	KEYITEM(XBOX_rtrigger) \
	KEYITEM(XBOX_lthumb) \
	KEYITEM(XBOX_rthumb) \
	KEYITEM(XBOX_dpad_up) \
	KEYITEM(XBOX_dpad_down) \
	KEYITEM(XBOX_dpad_left) \
	KEYITEM(XBOX_dpad_right) \
	KEYITEM(XBOX_start) \
	KEYITEM(XBOX_back) \
	KEYITEM(XBOX_lthumb_press) \
	KEYITEM(XBOX_rthumb_press) \
	KEYITEM(XBOX_lthumb_up) \
	KEYITEM(XBOX_lthumb_down) \
	KEYITEM(XBOX_lthumb_right) \
	KEYITEM(XBOX_lthumb_left) \
	KEYITEM(XBOX_lthumb_upleft) \
	KEYITEM(XBOX_lthumb_upright) \
	KEYITEM(XBOX_lthumb_downright) \
	KEYITEM(XBOX_lthumb_downleft) \
	KEYITEM(XBOX_rthumb_up) \
	KEYITEM(XBOX_rthumb_down) \
	KEYITEM(XBOX_rthumb_right) \
	KEYITEM(XBOX_rthumb_left) \
	KEYITEM(XBOX_rthumb_upleft) \
	KEYITEM(XBOX_rthumb_upright) \
	KEYITEM(XBOX_rthumb_downright) \
	KEYITEM(XBOX_rthumb_downleft)

struct AX_API InputKey
{
	enum Type {
		// lower than 32 is invalid
		Invalid = 0,

		// lower than 128 is ascii key code

		// keyboard controls
		SPECIAL_START = 128,

#define KEYITEM(key) key,
		AX_KEYITEMS
#undef KEYITEM

		MAX
	};
	AX_DECLARE_ENUM(InputKey)

	std::string getName() const;
	static int getKey(const std::string &keyname);
};

//--------------------------------------------------------------------------
// struct InputEvent
//--------------------------------------------------------------------------

struct AX_API InputEvent
{
	enum Type {
		KeyDown, KeyUp, MouseDown, MouseUp, MouseMove, Char, Wheel, XboxAxis,
		User
	};

	enum Flag {
		ShiftModifier = 1,
		ControlModifier = 2,
		AltModifier = 4
	};

	bool isMouseEvent();
	bool isKeyEvent();
	bool isCharEvent();
	bool isWheelEvent();

	// data
	InputEvent::Type type : 7;
	bool accepted : 1;
	int flags : 24;
	double time;
	InputKey key;
	Point pos;
	int charCode;
	int delta;
};

inline bool InputEvent::isMouseEvent()
{
	return type >= MouseDown && type <= MouseMove;
}

inline bool InputEvent::isKeyEvent()
{
	return type == KeyDown || type == KeyUp;
}

inline bool InputEvent::isCharEvent()
{
	return type == Char;
}

inline bool InputEvent::isWheelEvent()
{
	return type == Wheel;
}

//--------------------------------------------------------------------------
// class IInputHandler
//--------------------------------------------------------------------------

class AX_API IInputHandler
{
public:
	virtual void preHandleEvent(InputEvent *e) {}
	virtual void handleEvent(InputEvent *e);
	virtual void postHandleEvent(InputEvent *e) {}

protected:
	virtual void onEvent(InputEvent *e);
	virtual void onKeyDown(InputEvent *e);
	virtual void onKeyUp(InputEvent *e);
	virtual void onMouseDown(InputEvent *e);
	virtual void onMouseUp(InputEvent *e);
	virtual void onMouseMove(InputEvent *e);
	virtual void onMouseWheel(InputEvent *e);
};

//--------------------------------------------------------------------------
// class InputSystem
//--------------------------------------------------------------------------

class IInputSource;
class WinInput;

class AX_API InputSystem : public ITickable
{
public:
	InputSystem();
	~InputSystem();

	enum CaptureMode {
		Cooperative,
		Exclusive
	};

	enum MouseMode {
		Normal_Mode,
		FPS_Mode,		// move mouse to windows center, return delta move
	};

	void initialize();
	void finalize();

	// implement ITickable
	virtual void tick();

	// input event
	void setGameWindow(RenderTarget *gamewindow);
	void queEvent(const InputEvent &e);
	InputEvent *getEvent();				// return null if no more event
	void clearEvents();

	void startCapture(CaptureMode mode);
	void processEvents();
	void setVibration(float left, float right);
	void setMouseMode(MouseMode mode);
	void stopCapture();

	void registerEventSource(IInputSource *eventsource);
	void removeEventSource(IInputSource *eventsource);

	// qt message must que to input system
	void queWinInput(void *msg);

private:
	// events
	enum { EVENT_POOL_SIZE = 64 };
	InputEvent m_events[EVENT_POOL_SIZE];
	int m_eventReadPos;
	int m_eventWritePos;
	std::list<IInputSource*>	m_eventSources;
	RenderTarget *m_gameWnd;
	WinInput *m_winInput;

	bool m_isCapturing;
	CaptureMode m_captureMode;
};

//--------------------------------------------------------------------------
// class IInputSource
//--------------------------------------------------------------------------

class IInputSource
{
public:
	virtual void startCapture(InputSystem::CaptureMode capturemode) = 0;
	virtual void process() = 0;
	virtual void setVibration(float left, float right) = 0;
	virtual void setMouseMode(InputSystem::MouseMode mode) = 0;
	virtual void stopCapture() = 0;
};

AX_END_NAMESPACE

#endif // AX_CLIENT_EVENT_H

