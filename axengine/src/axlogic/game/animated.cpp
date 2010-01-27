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
// class GameRigit
//--------------------------------------------------------------------------

GameRigit::GameRigit() {
	m_model = nullptr;
	m_rigid = nullptr;
}

GameRigit::~GameRigit() {
	onReset();
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

void GameRigit::onReload()
{
	onReset();


}

void GameRigit::onReset()
{
	setRenderEntity(0);
	setPhysicsEntity(0);

	SafeDelete(m_model);
	SafeDelete(m_rigid);
}


Animated::Animated()
{
	m_model = 0;
	m_rig = 0;
	m_animator = 0;
	m_pose = 0;
}

Animated::~Animated()
{
	onReset();
}

void Animated::doThink()
{
	GameActor::doThink();

	if (m_animator) {
		int frametime = m_world->getFrameTime();
		m_animator->step(frametime);
		m_animator->renderToPose(m_pose);
		m_model->setPose(m_pose);
		m_model->setMatrix(m_matrix_p);
	}

}

#if 0
void Animated::loadAsset(const LuaTable &t)
{
	clear();

	if (!m_spawned)
		return;

	t.beginRead();
	String mdl = t.get("model");
	String animname = t.get("anim");
	t.endRead();

	if (mdl.empty()) {
		mdl = "models/box.mesh";
	}

	m_model = new HavokModel(mdl);
	m_model->setMatrix(m_matrix_p);
	m_world->getRenderWorld()->addEntity(m_model);

	if (!animname.empty()) {
		m_animation = new HavokAnimation(animname);
		m_rig = m_model->findRig();
		m_pose = m_rig->createPose();
		m_animator = new HavokAnimator(m_rig);
		m_animator->addAnimation(m_animation);
	}
	setRenderEntity(m_model);
}
#endif

void Animated::onReset()
{
	setRenderEntity(0);
	SafeDelete(m_animator);
	SafeDelete(m_pose);
	SafeDelete(m_rig);
	SafeDelete(m_model);
}

AX_END_NAMESPACE
