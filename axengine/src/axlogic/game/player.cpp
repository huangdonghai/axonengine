/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

Player::Player()
{
	m_body = 0;
	m_physicsPlayer = 0;
}

Player::~Player()
{
	onReset();
}

void Player::doThink()
{
	const UserInput *userinput = g_gameSystem->getUserInput(m_actorNum);
	m_physicsPlayer->runFrame(*userinput, m_world->getFrameTime());

	m_body->setMatrix(m_physicsPlayer->getMatrix());
	HavokPose *pose = m_physicsPlayer->getPose();
	m_body->setPose(pose);
	m_body->refresh();
}

void Player::setMatrix(const Matrix &matrix)
{
	m_physicsPlayer->setMatrix(matrix);
}

Matrix Player::getMatrix() const
{
	return m_physicsPlayer->getMatrix();
}

void Player::onReload()
{
	onReset();

	m_physicsPlayer = new PhysicsPlayer();
	setPhysicsEntity(m_physicsPlayer);

	m_body = new HavokModel("models/characters/_male/fullmale.mesh");

	m_world->getPhysicsWorld()->addEntity(m_physicsPlayer);
	m_world->getRenderWorld()->addEntity(m_body);
}

void Player::onReset()
{
	setRenderEntity(0);
	setPhysicsEntity(0);

	SafeDelete(m_body);
	SafeDelete(m_physicsPlayer);
}

void Player::onMatrixChanged()
{

}

void Player::onPropertyChanged()
{
	m_physicsPlayer->setLinearVelocity(Vector3(0,0,0));
}

Matrix Player::getThirdPersonMatrix() const
{
	return m_physicsPlayer->getThirdPerson();
}

AX_END_NAMESPACE

