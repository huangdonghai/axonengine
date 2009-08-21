/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_SOUND_WORLD_H
#define AX_SOUND_WORLD_H

namespace Axon { namespace Sound {

	class AX_API SoundWorld {
	public:
		SoundWorld();
		~SoundWorld();

		void addEntity(SoundEntity* entity);
		void removeEntity(SoundEntity* entity);

		SoundEntity* getEntity(int id) const;

		void playSound(int channelId, Sfx* sfx, LoopingMode = Looping_None);
		void stopSound(int channelId);

		void setListener(const AffineMat& matrix, const Vector3& velocity);

	private:
		friend class SoundSystem;

		SoundEntity* m_entities[MAX_ENTITIES];
		List<int> m_freeEntities;
		FMOD::ChannelGroup* m_channelGroup;
		AffineMat m_listenerMatrix;
		Vector3 m_listenerVelocity;
	};

}} // namespace Axon::Sound

#endif
