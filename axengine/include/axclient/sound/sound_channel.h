/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_SOUND_CHANNEL_H
#define AX_SOUND_CHANNEL_H

AX_BEGIN_NAMESPACE

class SoundWorld;
class SoundEntity;

	struct SoundKey
	{
		SoundKey() : world(0), entity(0), channelId(ChannelId_Any) {}
		SoundKey(SoundWorld *_world, SoundEntity *_entity, int _channelId) : world(_world), entity(_entity), channelId(_channelId) {}
		size_t hash() const {
			size_t h = 0;
			hash_combine(h, size_t(world));
			hash_combine(h, size_t(entity));
			hash_combine(h, channelId);
			return h;
		}

		bool operator==(const SoundKey &rhs) const {
			return world == rhs.world && entity == rhs.entity && channelId == rhs.channelId;
		}

		operator size_t() const {
			return hash();
		}

		SoundWorld *world;
		SoundEntity *entity;
		int channelId;
	};

	class SoundChannel {
	public:
		SoundKey m_key;
		int m_startTime;
		int m_finishTime;
		FMOD::Channel *m_fmodChannel;
	};

AX_END_NAMESPACE

#endif

