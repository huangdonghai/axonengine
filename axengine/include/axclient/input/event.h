/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EVENT_H
#define AX_EVENT_H

namespace Axon { namespace Input {

	//--------------------------------------------------------------------------
	// struct Key
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

	struct AX_API Key {
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
		} t;
		AX_DECLARE_ENUM(Key)

		String getName() const;
		static int getKey(const String& keyname);
	};

	//--------------------------------------------------------------------------
	// struct Event
	//--------------------------------------------------------------------------

	struct AX_API Event {
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
		Event::Type type : 7;
		bool accepted : 1;
		int flags : 24;
		unsigned int time;
		Key key;
		Point pos;
		int charCode;
		int delta;
	};

	inline bool Event::isMouseEvent() {
		return type >= MouseDown && type <= MouseMove;
	}

	inline bool Event::isKeyEvent() {
		return type == KeyDown || type == KeyUp;
	}

	inline bool Event::isCharEvent() {
		return type == Char;
	}

	inline bool Event::isWheelEvent() {
		return type == Wheel;
	}

#if 0
	struct Event : public Event {
	};

	struct Event : public Event {
	};

	struct Event : public Event {
	};

	struct Event : public Event {
	};
#endif

	//--------------------------------------------------------------------------
	// class IEventHandler
	//--------------------------------------------------------------------------

	class AX_API IEventHandler {
	public:
		virtual void preHandleEvent(Event* e) {}
		virtual void handleEvent(Event* e);
		virtual void postHandleEvent(Event* e) {}

	protected:
		virtual void onEvent(Event* e);
		virtual void onKeyDown(Event* e);
		virtual void onKeyUp(Event* e);
		virtual void onMouseDown(Event* e);
		virtual void onMouseUp(Event* e);
		virtual void onMouseMove(Event* e);
		virtual void onMouseWheel(Event* e);
	};

	//--------------------------------------------------------------------------
	// class InputSystem
	//--------------------------------------------------------------------------

	class IEventSource;
	class WinInput;

	class AX_API InputSystem : public ITickable {
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
		void setGameWindow(RenderTarget* gamewindow);
		void queEvent(const Event& e);
		Event* getEvent();				// return null if no more event
		void clearEvents();

		void startCapture(CaptureMode mode);
		void processEvents();
		void setVibration(float left, float right);
		void setMouseMode(MouseMode mode);
		void stopCapture();

		void registerEventSource(IEventSource* eventsource);
		void removeEventSource(IEventSource* eventsource);

		// qt message must que to input system
		void queWinInput(void* msg);

	private:
		// events
		enum { EVENT_POOL_SIZE = 64 };
		Event m_events[EVENT_POOL_SIZE];
		int m_eventReadPos;
		int m_eventWritePos;
		List<IEventSource*>	m_eventSources;
		RenderTarget* m_gameWnd;
		WinInput* m_winInput;

		bool m_isCapturing;
		CaptureMode m_captureMode;
	};

	//--------------------------------------------------------------------------
	// class IEventSource
	//--------------------------------------------------------------------------

	class IEventSource {
	public:
		virtual void startCapture(InputSystem::CaptureMode capturemode) = 0;
		virtual void process() = 0;
		virtual void setVibration(float left, float right) = 0;
		virtual void setMouseMode(InputSystem::MouseMode mode) = 0;
		virtual void stopCapture() = 0;
	};

}} // namespace Axon::Input

#endif // end guardian

