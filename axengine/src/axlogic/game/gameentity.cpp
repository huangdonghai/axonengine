/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

namespace Axon { namespace Game {

	//--------------------------------------------------------------------------
	// class Entity
	//--------------------------------------------------------------------------

	Entity::Entity() {
		m_entityNum = -1;
		m_world = nullptr;
		m_state = Active;

		setSoundEntity(new SoundEntity());
	}

	Entity::~Entity() {
		clear();
		delete getSoundEntity();
		setSoundEntity(0);
	}

	void Entity::doThink() {
		invoke_onThink();
	}

	void Entity::setState(State state)
	{
		m_state = state;
	}

	void Entity::doNotify(IObservable* subject, int arg) {
	}

	void Entity::onPhysicsActived() {
		m_updateFlags.set(ReadPhysics);
	}

	void Entity::onPhysicsDeactived() {
		m_updateFlags.unset(ReadPhysics);
	}

	void Entity::invoke_onThink() {
		invokeCallback("onThink", m_world->getFrameTime());
	}

	void Entity::reload()
	{
		invokeCallback("onReset");
	}

	void Entity::autoGenerateName()
	{
		String objname = "entity";

		if (getClassInfo())
			objname = getClassInfo()->m_className;

		StringUtil::strlwr(&objname[0]);
		objname = g_scriptSystem->generateObjectName(objname);

		set_objectName(objname);
	}

	void Entity::doSpawn()
	{
		if (m_spawned) {
			Errorf("already spawned");
		}

		m_spawned = true;

		m_world->getSoundWorld()->addEntity(getSoundEntity());
		reload();
	}

	void Entity::doRemove()
	{
		SoundEntity* soundEntity = getSoundEntity();
		m_world->getSoundWorld()->removeEntity(soundEntity);

		if (!m_spawned) {
			Errorf("not even spawned");
		}

		clear();
		m_spawned = false;
	}

	//--------------------------------------------------------------------------
	// class RigidBody
	//--------------------------------------------------------------------------

	RigidBody::RigidBody() {
		m_model = nullptr;
		m_rigid = nullptr;
	}

	RigidBody::~RigidBody() {
		clear();
	}

	void RigidBody::doThink() {
		m_model->setMatrix(m_rigid->getMatrix());
		m_world->getRenderWorld()->addActor(m_model);
	}

	void RigidBody::loadAsset(const LuaTable& t)
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

		m_model = new PhysicsModel(modelName);
		m_model->setMatrix(m_matrix_p);
		m_model->setInstanceColor(m_instanceColor_p);
		m_world->getRenderWorld()->addActor(m_model);

		if (rigidName.empty()) {
			rigidName = modelName;
		}

		if (!rigidName.empty()) {
			m_rigid = new physicsRigid(rigidName);
//			setPhysics(m_rigid);
			m_rigid->setMatrix(m_matrix_p);
			m_rigid->setMotionType(PhysicsEntity::Motion_Dynamic);
			m_world->getPhysicsWorld()->addEntity(m_rigid);
		}

		setRenderEntity(m_model);
		setPhysicsEntity(m_rigid);
	}

	void RigidBody::clear()
	{
		setRenderEntity(0);
		setPhysicsEntity(0);

		SafeDelete(m_model);
		SafeDelete(m_rigid);
	}

}} // namespace Axon::Game

