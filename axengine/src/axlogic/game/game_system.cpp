/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

AX_BEGIN_COMMAND_MAP(GameSystem)
AX_END_COMMAND_MAP()

GameSystem::GameSystem()
{
	m_state = Blocking;

	m_running = false;
	m_gameWorld = nullptr;

	m_gameInput = new GameInput();
	m_gameInput->initialize();

	g_system->registerTickable(System::TickGame, this);
}

GameSystem::~GameSystem()
{
	g_system->removeTickable(System::TickGame, this);

	m_gameInput->finalize();
	SafeDelete(m_gameInput);
}

void GameSystem::reset()
{

}


void GameSystem::setGameWorld(GameWorld *gameworld)
{
	if (m_gameWorld && gameworld) {
		Errorf("already has a game world");
	}

	m_gameWorld = gameworld;

	if (!gameworld) {
		return;
	}

	// spawn player
	if (m_gameWorld->m_numClients) {
		return;
	}

	GameActor *ent = GameWorld::createActor("Game.Player");
	m_gameWorld->addActor(ent);
}

void GameSystem::tick()
{
	if (!m_running) {
		return;
	}

	if (!m_gameWorld) {
		return;
	}

	double lasttime = m_gameWorld->m_lasttime;
	double curtime = OsUtil::getTime();

	float frametime = 1.0f / g_fps->getInteger();

	while (curtime - lasttime < frametime) {
		float sleeptime = frametime - (curtime - lasttime);
		OsUtil::sleep(sleeptime);
		double c = OsUtil::getTime();
//			Printf("sleep time: %d\n", c-curtime);
		curtime = c;
	}

	int alltime = curtime - lasttime;

	// fix long time
	if (alltime > 50)
		alltime = 50;

	while (alltime >= frametime) {
		m_gameInput->runFrame(frametime);
		m_userInputs = m_gameInput->genUserInput();

		m_gameWorld->runFrame(-1, frametime);
		alltime -= frametime;
	}

	m_gameWorld->drawFrame();
}

void GameSystem::startRunning()
{
	notify(ObserveFlag_StartRunning);

	m_running = true;
	m_gameInput->startRunning();
//		gInputSystem->setGameWindow(m_gameWorld->getWindow());
	g_inputSystem->startCapture(InputSystem::Exclusive);
	g_inputSystem->setMouseMode(InputSystem::FPS_Mode);

	m_gameWorld->m_lasttime = OsUtil::getTime();
}

void GameSystem::stopRunning()
{
	g_inputSystem->setMouseMode(InputSystem::Normal_Mode);
	g_inputSystem->stopCapture();
	m_gameInput->stopRunning();
	m_running = false;

	m_gameWorld->restoreActors();

	notify(ObserveFlag_StopRunning);
}

const UserInput *GameSystem::getUserInput(ActorNum clientNum)
{
	return &m_userInputs;
}

AX_END_NAMESPACE
