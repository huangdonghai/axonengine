/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class GameActor
//--------------------------------------------------------------------------

GameActor::GameActor()
{
	m_actorNum = -1;
	m_world = nullptr;

	setSoundEntity(new SoundEntity());
}

GameActor::~GameActor()
{
	onReset();
	delete getSoundEntity();
	setSoundEntity(0);
}

void GameActor::doThink()
{
}

void GameActor::doNotify(IObservable *subject, int arg)
{
}

void GameActor::onPhysicsActived()
{
	m_updateFlags.set(ReadPhysics);
}

void GameActor::onPhysicsDeactived()
{
	m_updateFlags.unset(ReadPhysics);
}

void GameActor::onReload()
{
}

void GameActor::autoGenerateName()
{
	String objname = "entity";

	if (getScriptClass())
		objname = getScriptClass()->getName();

	StringUtil::strlwr(&objname[0]);
	objname = g_scriptSystem->generateObjectName(objname);

	set_objectName(objname);
}

void GameActor::doSpawn()
{
	if (m_spawned) {
		Errorf("already spawned");
	}

	m_spawned = true;

	m_world->getSoundWorld()->addEntity(getSoundEntity());
	onReload();
}

void GameActor::doRemove()
{
	SoundEntity *soundEntity = getSoundEntity();
	m_world->getSoundWorld()->removeEntity(soundEntity);

	if (!m_spawned) {
		Errorf("not even spawned");
	}

	onReset();
	m_spawned = false;
}

void GameActor::switchState(const String &name)
{
#if 0
	if (!isScriptInstanced())
		return;

	m_currentState = m_scriptInstance.getSqObject().getValue(name.c_str());

	if (!m_currentState.isTable()) {
		m_currentState.getSqObject().reset();
		Errorf("Invalid state name");
	}

	sqVM::ms_objThreadList.push_back(this);
#endif
}

void GameActor::sleep(float seconds)
{
	m_latentId = Latent_Sleep;
	m_latentParam0 = seconds;
}

AX_END_NAMESPACE

