/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_SOUND_ENTITY_H
#define AX_SOUND_ENTITY_H

namespace Axon { namespace Sound {

	class SoundWorld;

	class AX_API SoundEntity {
	public:
		SoundEntity();
		~SoundEntity();

		AffineMat getMatrix() const { return m_matrix; }
		void setMatrix(AffineMat val) { m_matrix = val; }
		Vector3 getVelocity() const { return m_velocity; }
		void setVelocity(Vector3 val) { m_velocity = val; }

		void playSound(int channelId, Sfx* sfx, LoopingMode looping = Looping_None, float minDist = DEFAULT_MIN_DIST, float maxDist = DEFAULT_MAX_DIST );
		void stopSound(int channelId);

	private:
		friend class SoundWorld;
		friend class SoundSystem;

		SoundWorld* m_world;
		int m_entityId;
		AffineMat m_matrix;
		Vector3 m_velocity;
	};

}} // namespace Axon::Sound

#endif
