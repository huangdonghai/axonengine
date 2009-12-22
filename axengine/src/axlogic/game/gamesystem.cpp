/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Game {

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


	void GameSystem::setGameWorld(GameWorld* gameworld)
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

		GameActor* ent = GameWorld::createEntity("Game.Player");
		m_gameWorld->addEntity(ent);
	}

	void GameSystem::tick()
	{
		if (!m_running) {
			return;
		}

		if (!m_gameWorld) {
			return;
		}

		int lasttime = m_gameWorld->m_lasttime;
		int curtime = OsUtil::milliseconds();

		int frametime = 1000 / g_fps->getInteger();

		while (curtime - lasttime < frametime) {
			int sleeptime = frametime - (curtime - lasttime);
			OsUtil::sleep(sleeptime);
			int c = OsUtil::milliseconds();
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

		m_gameWorld->m_lasttime = OsUtil::milliseconds();
	}

	void GameSystem::stopRunning()
	{
		g_inputSystem->setMouseMode(InputSystem::Normal_Mode);
		g_inputSystem->stopCapture();
		m_gameInput->stopRunning();
		m_running = false;

		m_gameWorld->restoreEntities();

		notify(ObserveFlag_StopRunning);
	}

	const UserInput* GameSystem::getUserInput(ActorNum clientNum)
	{
		return &m_userInputs;
	}

}} // namespace Axon::Game
