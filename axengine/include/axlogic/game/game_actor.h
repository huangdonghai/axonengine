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
	} t;
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
typedef List<GameActor*> EntityList;
typedef EntityList::iterator EntityIterator;
typedef Dict<String,GameActor*> EntityHash;

class AX_API GameActor : public GameObject, public IObserver
{
	AX_DECLARE_CLASS(GameActor, GameObject)
	AX_END_CLASS()
public:
	friend class GameWorld;

	enum State {
		Hide,
		Inactive,
		Active,
	};

	GameActor();
	virtual ~GameActor();

	virtual void doThink();
	virtual bool isPlayer() const { return false; }

	// implement IObserver
	virtual void doNotify(IObservable* subject, int arg);

	virtual void setState(State state);
	inline State getState() { return m_state; }

	void autoGenerateName();

protected:
	// implement GameObject
	virtual void reload();
	virtual void doSpawn();
	virtual void doRemove();

	// called by subclass
	virtual void onPhysicsActived();
	virtual void onPhysicsDeactived();

	void invoke_onThink();

protected:
	ActorNum m_entityNum;
	GameWorld* m_world;			// world the entity has added to

private:
	State m_state;
};

//--------------------------------------------------------------------------
// class GameActorPtr
//--------------------------------------------------------------------------

template< class type >
class GameActorPtr {
public:
	GameActorPtr();

	GameActorPtr<type>& operator=( type *ent );

	// synchronize entity pointers over the network
	int getSpawnId( void ) const { return spawnId; }
	bool setSpawnId( int id );
	bool updateSpawnId( void );

	bool isValid( void ) const;
	type* getEntity( void ) const;
	int getEntityNum( void ) const;

private:
	GameWorld* m_world;
	int m_spawnId;
};

template< class type >
inline GameActorPtr<type>::GameActorPtr() {
	m_spawnId = 0;
}

template< class type >
inline GameActorPtr<type> &GameActorPtr<type>::operator=( type *ent ) {
	if ( ent == NULL ) {
		m_spawnId = 0;
	} else {
		m_spawnId = ( m_world->m_spawnIds[ent->entityNumber] << ActorNum::MAX_ACTORS_BITS ) | ent->entityNumber;
	}
	return *this;
}

template< class type >
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

template< class type >
inline bool GameActorPtr<type>::isValid( void ) const {
	return ( m_world->m_spawnIds[m_spawnId & ActorNum::MAX_ACTORS_MASK] == ( m_spawnId >> ActorNum::MAX_ACTORS_BITS ) );
}

template< class type >
inline type *GameActorPtr<type>::getEntity( void ) const {
	int entityNum = m_spawnId & ActorNum::MAX_ACTORS_MASK;
	if ( ( m_world->m_spawnIds[entityNum] == ( m_spawnId >> ActorNum::MAX_ACTORS_BITS ) ) ) {
		return static_cast<type *>( m_world->m_spawnIds[ entityNum ] );
	}
	return NULL;
}

template< class type >
inline int GameActorPtr<type>::getEntityNum( void ) const {
	return m_spawnId & ActorNum::MAX_ACTORS_MASK;
}


//--------------------------------------------------------------------------
// class GameRigit
//--------------------------------------------------------------------------

class AX_API GameRigit : public GameActor
{
	AX_DECLARE_CLASS(GameRigit, GameActor)
		AX_METHOD(loadAsset)
	AX_END_CLASS()

public:
	GameRigit();
	virtual ~GameRigit();

	// implement GameActor
	virtual void doThink();

	// properties

protected:
	void loadAsset(const LuaTable& t);
	void clear();

protected:
	HavokModel* m_model;
	PhysicsRigid* m_rigid;
};

AX_END_NAMESPACE


#endif // eng guardian

