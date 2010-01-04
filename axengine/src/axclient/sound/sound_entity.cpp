/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "sound_local.h"

AX_BEGIN_NAMESPACE

SoundEntity::SoundEntity()
{
	m_world = 0;
	m_entityId = ENTITYID_NULL;
	m_matrix.setIdentity();
	m_velocity.clear();
}

SoundEntity::~SoundEntity()
{
}

void SoundEntity::playSound( int channelId, SoundFx *sfx, LoopingMode looping /*= Looping_None*/, float minDist /*= DEFAULT_MIN_DIST*/, float maxDist /*= DEFAULT_MAX_DIST */ )
{
	AX_ASSERT(m_world);

	g_soundSystem->_playSound(m_world, this, channelId, sfx, looping, minDist, maxDist);
}

void SoundEntity::stopSound( int channelId )
{
	g_soundSystem->_stopSound(m_world, this, channelId);
}

AX_END_NAMESPACE

