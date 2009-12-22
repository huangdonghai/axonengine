/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_LOGIC_USERINPUT_H
#define AX_LOGIC_USERINPUT_H

namespace Axon { namespace Logic {

	struct UserInput {
	public:
		enum Button {
			Attack = 1,
			Rush = 2,
			Zoom = 4,
		};

		enum Trigger {
			Trigger_None,
			Trigger_ToggleView,
			Trigger_ZoomIn,
			Trigger_ZoomOut,

			Trigger_Max
		};

		int gameFrame;						// frame number
		int gameTime;						// game time
		int duplicateCount;					// duplication count for networking
		byte_t buttons;						// buttons
		sbyte_t forwardmove;					// forward/backward movement
		sbyte_t rightmove;						// left/right movement
		sbyte_t upmove;							// up/down movement
		short angles[3];						// view angles
		short mx;								// mouse delta x
		short my;								// mouse delta y
		sbyte_t trigger;						// trigger command
		byte_t flags;							// additional flags
		int sequence;						// just for debugging

	public:
		void byteSwap();						// on big endian systems, byte swap the shorts and ints
		bool operator==(const UserInput &rhs) const;
	};

}} // namespace Axon::Logic


#endif // end guardian

