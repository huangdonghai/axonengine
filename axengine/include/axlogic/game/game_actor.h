/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_ENTITY_H
#define AX_GAME_ENTITY_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class GameActor
//--------------------------------------------------------------------------

struct ActorNum {
	enum Type {
		MAX_CLIENTS = 32,
		MAX_ACTORS_BITS = 12,
		MAX_ACTORS = 1 << MAX_ACTORS_BITS,
		MAX_ACTORS_MASK = MAX_ACTORS - 1,
		NONE = MAX_ACTORS - 1,
		LANDSCAPE = MAX_ACTORS - 2,
		MAX_NORMAL = LANDSCAPE
	};
	AX_DECLARE_ENUM(ActorNum);
};

enum SndChannelId {
	SndChannelId_Any = ChannelId_Any,
	SndChannelId_Voice = ChannelId_One,
	SndChannelId_Voice2,
	SndChannelId_Body,
	SndChannelId_Body2,
	SndChannelId_Body3,
	SndChannelId_Weapon,
	SndChannelId_Item,
	SndChannelId_Heart,
	SndChannelId_Pda,
	SndChannelId_Demonic,
	SndChannelId_Radio,
	SndChannelId_Ambient,
	SndChannelId_Damage
};

class GameActor;
typedef std::list<GameActor*> ActorList;
typedef ActorList::iterator EntityIterator;
typedef Dict<std::string,GameActor*> ActorHash;

class AX_API GameActor : public GameObject, public IObserver
{
	AX_DECLARE_CLASS(GameActor, GameObject)
	AX_END_CLASS()

	enum LatentId {
		Latent_Continue, // start run next code
		Latent_Sleep, // sleep seconds
		Latent_Wait, // wait latentId is reset to continue
		Latent_Callback, // provide a callback function for frame tick
	};

public:
	friend class GameWorld;

	GameActor();
	virtual ~GameActor();

	virtual void doThink();
	virtual bool isPlayer() const { return false; }

	// implement IObserver
	virtual void beNotified(IObservable *subject, int arg);

	void autoGenerateName();

protected:
	// implement GameObject
	virtual void onReload();
	virtual void doSpawn();
	virtual void doRemove();

	// called by subclass
	virtual void onPhysicsActived();
	virtual void onPhysicsDeactived();

	// scriptable method
	void sleep(float seconds);
	void gotoState(const ScriptValue &state);
	void switchState(const std::string &name);
	// end scriptable method


protected:
	ActorNum m_actorNum;
	GameWorld *m_world;			// world the entity has added to

private:
	ScriptValue m_runningState; // current thread running

	ScriptThread m_thread;
	LatentId m_latentId;
	Variant m_latentParam0;
	Variant m_latentParam1;

	IntrusiveLink<GameActor> m_threadLink;
};

//--------------------------------------------------------------------------
// class GameActorPtr
//--------------------------------------------------------------------------

template <class type>
class GameActorPtr {
public:
	GameActorPtr();

	GameActorPtr<type>& operator=( type *ent );

	// synchronize entity pointers over the network
	int getSpawnId( void ) const { return spawnId; }
	bool setSpawnId( int id );
	bool updateSpawnId( void );

	bool isValid( void ) const;
	type *getEntity( void ) const;
	int getEntityNum( void ) const;

private:
	GameWorld *m_world;
	int m_spawnId;
};

template <class type>
inline GameActorPtr<type>::GameActorPtr() {
	m_spawnId = 0;
}

template <class type>
inline GameActorPtr<type> &GameActorPtr<type>::operator=( type *ent ) {
	if ( ent == NULL ) {
		m_spawnId = 0;
	} else {
		m_spawnId = ( m_world->m_spawnIds[ent->entityNumber] << ActorNum::MAX_ACTORS_BITS ) | ent->entityNumber;
	}
	return *this;
}

template <class type>
inline bool GameActorPtr<type>::setSpawnId( int id ) {
	// the reason for this first check is unclear:
	// the function returning false may mean the spawnId is already set right, or the entity is missing
	if ( id == m_spawnId ) {
		return false;
	}
	if ( ( id >> ActorNum::MAX_ACTORS_BITS ) == m_world->m_spawnIds[id & ActorNum::MAX_ACTORS_MASK] ) {
		m_spawnId = id;
		return true;
	}
	return false;
}

template <class type>
inline bool GameActorPtr<type>::isValid( void ) const {
	return ( m_world->m_spawnIds[m_spawnId & ActorNum::MAX_ACTORS_MASK] == ( m_spawnId >> ActorNum::MAX_ACTORS_BITS ) );
}

template <class type>
inline type *GameActorPtr<type>::getEntity( void ) const {
	int entityNum = m_spawnId & ActorNum::MAX_ACTORS_MASK;
	if ( ( m_world->m_spawnIds[entityNum] == ( m_spawnId >> ActorNum::MAX_ACTORS_BITS ) ) ) {
		return static_cast<type *>( m_world->m_spawnIds[ entityNum ] );
	}
	return NULL;
}

template <class type>
inline int GameActorPtr<type>::getEntityNum( void ) const {
	return m_spawnId & ActorNum::MAX_ACTORS_MASK;
}


AX_END_NAMESPACE


#endif // eng guardian

