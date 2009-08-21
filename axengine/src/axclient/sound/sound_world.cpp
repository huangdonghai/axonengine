/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "sound_local.h"

namespace Axon { namespace Sound {

	SoundWorld::SoundWorld()
	{
		TypeZeroArray(m_entities);
		for (int i = 0; i < MAX_ENTITIES; i++) {
			m_freeEntities.push_front(i);
		}

		m_listenerMatrix.setIdentity();
		m_listenerVelocity.clear();
	}

	SoundWorld::~SoundWorld()
	{
	}

	void SoundWorld::addEntity( SoundEntity* entity )
	{
		AX_ASSERT(!entity->m_world);

		if (m_freeEntities.empty()) {
			Errorf("no free entity");
		}

		int id = m_freeEntities.front();
		m_freeEntities.pop_front();

		m_entities[id] = entity;
		entity->m_world = this;
		entity->m_entityId = id;
	}

	void SoundWorld::removeEntity( SoundEntity* entity )
	{
		AX_ASSERT(entity->m_world == this);

		int id = entity->m_entityId;

		AX_ASSERT(entity == m_entities[id]);

		m_entities[id] = 0;
		entity->m_world = 0;
		entity->m_entityId = ENTITYID_NULL;
	}

	void SoundWorld::playSound( int channelId, Sfx* sfx, LoopingMode looping/*= Looping_None*/ )
	{
		g_soundSystem->_playSound(this, 0, channelId, sfx, looping);
	}

	void SoundWorld::stopSound( int channelId )
	{
		g_soundSystem->_stopSound(this, 0, channelId);
	}

	void SoundWorld::setListener( const AffineMat& matrix, const Vector3& velocity )
	{
		m_listenerMatrix = matrix;
		m_listenerVelocity = velocity;
	}

	SoundEntity* SoundWorld::getEntity( int id ) const
	{
		return m_entities[id];
	}

}} // namespace Axon::Sound

