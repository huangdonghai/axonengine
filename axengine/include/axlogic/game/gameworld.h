/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_WORLD_H
#define AX_GAME_WORLD_H

namespace Axon { namespace Game {

	enum Think {
		Think_all = -1,
		Think_render = 1,
		Think_physics = 2,
		Think_animation = 4,
	};

	class AX_API World : public IObserver {
	public:
		friend class GameSystem;

		enum CameraMode {
			FirstPerson,
			ThirdPerson,
			FreeNavigate,
		};

		World();
		~World();

		void reset();

		void setWindow(Render::Target* targetWin);
		Render::Target* getWindow() { return m_targetWindow; }

		void runFrame(int what, int frametime);
		void drawFrame();
		void drawScene(const Render::Camera& camera);

		void addNode(Node* node);
		void removeNode(Node* node);

		Entity* getEntity(int num) const;
		Landscape* getLandscape() const;
		RenderWorld* getRenderWorld() const;
		PhysicsWorld* getPhysicsWorld() const;
		SoundWorld* getSoundWorld() const;
		int getFrameTime() const;

		Map::EnvDef* getEnvironment() const { return m_mapEnvDef; }
		void updateEnvdef();

		// implement IObserver
		virtual void doNotify(IObservable* subject, int arg);

		AffineMat getLastViewMatrix() const;

		// static function
		static Entity* createEntity(const char* clsname);

	protected:
		void addEntity(Entity* entity);
		void removeEntity(Entity* entity);

		void addFixed(Fixed* fixed);
		void removeFixed(Fixed* fixed);

		void restoreEntities();

	private:
		bool m_onlyServer;
		bool m_onlyClient;
		bool m_multiPlayer;

		Entity* m_entities[EntityNum::MAX_ENTITIES];
		int m_spawnIds[EntityNum::MAX_ENTITIES];// for use in EntityPtr
		int m_numEntities;
		int m_firstFreeEntity;
		int m_numClients;

		PhysicsWorld* m_physicsWorld;
		RenderWorld* m_renderWorld;
		SoundWorld* m_soundWorld;
		Render::OutdoorEnv* m_outdoorEnv;
		Map::EnvDef* m_mapEnvDef;

		uint_t m_lasttime;
		int m_frametime;
		Render::Camera m_lastCamera;
		Render::Target* m_targetWindow;
	};

	inline Entity* World::getEntity(int num) const {
		AX_ASSERT(num >= 0 && num < EntityNum::MAX_ENTITIES);
		return m_entities[num];
	}

	inline Landscape* World::getLandscape() const {
		return static_cast<Landscape*>(m_entities[EntityNum::LANDSCAPE]);
	}

	inline RenderWorld* World::getRenderWorld() const {
		return m_renderWorld;
	}

	inline PhysicsWorld* World::getPhysicsWorld() const {
		return m_physicsWorld;
	}

	inline SoundWorld* World::getSoundWorld() const {
		return m_soundWorld;
	}

	inline int World::getFrameTime() const {
		return m_frametime;
	}

}} // namespace Axon::Game

#endif // end AX_GAME_WORLD_H

