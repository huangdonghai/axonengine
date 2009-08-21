/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_SOUND_ALL_H
#define AX_SOUND_ALL_H

// forward declare FMOD type
namespace FMOD {
	class Sound;
	class Channel;
	class ChannelGroup;
	class System;
}

// forward declaration
namespace Axon { namespace Sound {

	class Sfx;
	class Channel;
	class SoundEntity;
	class SoundWorld;
	class SoundSystem;

	enum LoopingMode {
		Looping_Forever = -1,
		Looping_None = 0,
		Looping_Once = 1,
	};

	enum PlayMode {
		PlayMode_2D,
		PlayMode_3D,
	};

	enum ChannelId {
		ChannelId_Any,
		ChannelId_One
	};

	enum {
		MAX_CHANNELS = 1024,
		MAX_ENTITIES = 4096,
		ENTITYID_NULL = -1,
		DEFAULT_MIN_DIST = 0,
		DEFAULT_MAX_DIST = 64
	};

}} // namespace Axon::Sound

#include "sound_sfx.h"
#include "sound_channel.h"
#include "sound_entity.h"
#include "sound_world.h"
#include "sound_system.h"

namespace Axon {

	typedef Sound::Sfx SoundSfx;
	typedef Sound::SfxPtr SoundSfxPtr;
	typedef Sound::Channel SoundChannel;
	typedef Sound::SoundEntity SoundEntity;
	typedef Sound::SoundWorld SoundWorld;
	typedef Sound::SoundSystem SoundSystem;

}

#endif
