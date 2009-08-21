/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_XINPUT_H
#define AX_XINPUT_H

namespace Axon { namespace Input {

	class Xinput : public IEventSource {
	public:
		Xinput();
		~Xinput();

		// IEventSource
		virtual void startCapture(InputSystem::CaptureMode capturemode);
		virtual void process();
		virtual void setVibration(float left, float right);
		virtual void stopCapture();

	private:
		bool m_isInCapture;
		Dict<int,int>	m_keymap;
	};

}} // namespace Axon::Input

#endif // end guardian

