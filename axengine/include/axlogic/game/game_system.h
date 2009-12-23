/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_SYSTEM_H
#define AX_GAME_SYSTEM_H

AX_BEGIN_NAMESPACE

	struct EntityState {
		enum {
			MAX_ENTITY_STATE_SIZE = 512
		};

		int entityNumber;
		netBitMsg state;
		byte_t stateBuf[MAX_ENTITY_STATE_SIZE];
		EntityState* next;
	};

	struct Snapshot {
		enum {
			ENTITY_PVS_SIZE = ((ActorNum::MAX_ENTITIES+31)>>5)
		};

		int sequence;
		EntityState* firstEntityState;
		int pvs[ENTITY_PVS_SIZE];
		Snapshot* next;
	};

	class ClientSlot {
	public:
		enum State {
			Free,		// can be reused for a new connection
			Zombie,		// client has been disconnected, but don't reuse connection for a couple seconds
			Connected,	// has been assigned to a client_t, but no gamestate yet
			Primed,		// gamestate has been sent, but client hasn't sent a usercmd
			Active // client is fully in game
		};

	protected:

	private:
		State m_state;
		netPeer* m_netPeer;
	};

	class GameInput;

	class AX_API GameSystem
		: public Object, public ICmdHandler, public ITickable, public IObservable
	{
		AX_DECLARE_COMMAND_HANDLER(GameSystem);
	public:
		enum ObserveFlag {
			ObserveFlag_StartRunning = 1,
			ObserveFlag_StopRunning = 2,
		};

		enum State {
			Blocking,		// not allow any client connect
			Waiting,		// waiting for client connect
			Loading,		// loading map
			Gaming,			// gaming
			Editing,		// is editing map, network is disabled
		};

		friend class GameWorld;

		GameSystem();
		~GameSystem();

		void reset();

		// implement ITickable
		virtual void tick();

		// public method
		void startRunning();
		void stopRunning();
		bool isRunning() const { return m_running; }

		void setGameWorld(GameWorld* gameworld);

		// player
		const UserInput* getUserInput(ActorNum clientNum);

	protected:


	private:
		GameInput* m_gameInput;
		GameWorld* m_gameWorld;

		// runtime
		State m_state;

		EntityState* m_clientEntityStates[ActorNum::MAX_CLIENTS][ActorNum::MAX_ENTITIES];
		Snapshot* m_clientSnapshots[ActorNum::MAX_CLIENTS];
		UserInput m_userInputs;

		int m_starttime;
		int m_lasttime;
		bool m_running;
	};

AX_END_NAMESPACE

#endif // end AX_GAME_SYSTEM_H

