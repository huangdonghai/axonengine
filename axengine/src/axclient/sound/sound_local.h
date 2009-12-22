/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"
#include <fmod.hpp>
#include <fmod_errors.h>

namespace Axon {

	inline void ERRCHECK(FMOD_RESULT result)
	{
		if (result != FMOD_OK) {
			Errorf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		}
	}

	struct channel_t {
		int m_startTime;
		int m_finishTime;
		int m_entityNum;
		int m_channelId;

		SoundWorld* m_world;
		FMOD::Channel* m_fmodChannel;
	};

	struct loopChannel_t {
		int m_startTime;
		int m_finishTime;
		int m_entityNum;
		int m_channelId;

		SoundWorld* m_world;
		FMOD::Channel* m_fmodChannel;
	};

}
