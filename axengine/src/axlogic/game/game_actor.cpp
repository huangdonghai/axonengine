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

GameActor::GameActor() {
	m_actorNum = -1;
	m_world = nullptr;
	m_state = Active;

	setSoundEntity(new SoundEntity());
}

GameActor::~GameActor() {
	clear();
	delete getSoundEntity();
	setSoundEntity(0);
}

void GameActor::doThink() {
	invoke_onThink();
}

void GameActor::setState(State state)
{
	m_state = state;
}

void GameActor::doNotify(IObservable *subject, int arg) {
}

void GameActor::onPhysicsActived() {
	m_updateFlags.set(ReadPhysics);
}

void GameActor::onPhysicsDeactived() {
	m_updateFlags.unset(ReadPhysics);
}

void GameActor::invoke_onThink() {
	invokeCallback("onThink", m_world->getFrameTime());
}

void GameActor::reload()
{
	invokeCallback("onReset");
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
	reload();
}

void GameActor::doRemove()
{
	SoundEntity *soundEntity = getSoundEntity();
	m_world->getSoundWorld()->removeEntity(soundEntity);

	if (!m_spawned) {
		Errorf("not even spawned");
	}

	clear();
	m_spawned = false;
}

//--------------------------------------------------------------------------
// class GameRigit
//--------------------------------------------------------------------------

GameRigit::GameRigit() {
	m_model = nullptr;
	m_rigid = nullptr;
}

GameRigit::~GameRigit() {
	clear();
}

void GameRigit::doThink() {
	m_model->setMatrix(m_rigid->getMatrix());
	m_world->getRenderWorld()->addEntity(m_model);
}

#if 0
void GameRigit::loadAsset(const LuaTable &t)
{
	clear();

	if (!m_spawned)
		return;

	t.beginRead();
	String modelName = t.get("model");
	String rigidName = t.get("rigid");
	t.endRead();

	if (modelName.empty()) {
		modelName = "models/box.mesh";
	}

	m_model = new HavokModel(modelName);
	m_model->setMatrix(m_matrix_p);
	m_model->setInstanceColor(m_instanceColor_p);
	m_world->getRenderWorld()->addEntity(m_model);

	if (rigidName.empty()) {
		rigidName = modelName;
	}

	if (!rigidName.empty()) {
		m_rigid = new PhysicsRigid(rigidName);
//			setPhysics(m_rigid);
		m_rigid->setMatrix(m_matrix_p);
		m_rigid->setMotionType(PhysicsEntity::Motion_Dynamic);
		m_world->getPhysicsWorld()->addEntity(m_rigid);
	}

	setRenderEntity(m_model);
	setPhysicsEntity(m_rigid);
}
#endif

void GameRigit::clear()
{
	setRenderEntity(0);
	setPhysicsEntity(0);

	SafeDelete(m_model);
	SafeDelete(m_rigid);
}

AX_END_NAMESPACE

