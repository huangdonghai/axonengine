/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_INPUT_EVENTSOURCE_H
#define AX_INPUT_EVENTSOURCE_H

namespace Axon { namespace Input {

	class WinEventSource : public IEventSource {
	public:
		WinEventSource();
		~WinEventSource();

		// implement IEventSource
		virtual void startCapture(InputSystem::CaptureMode capturemode);
		virtual void process();
		virtual void setVibration(float left, float right);
		virtual void stopCapture();

	private:
		handle_t m_hwnd;
	};

	class XenonEventSource : public IEventSource {};

	class XWinEventSource : public IEventSource {};

}} // namespace Axon::Input

#endif // end guardian

