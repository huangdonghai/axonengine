/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_ENTITY_H
#define AX_GAME_ENTITY_H

namespace Axon { namespace Game {

	//--------------------------------------------------------------------------
	// class Entity
	//--------------------------------------------------------------------------

	struct EntityNum {
		enum Type {
			MAX_CLIENTS = 32,
			MAX_ENTITIES_BITS = 12,
			MAX_ENTITIES = 1 << MAX_ENTITIES_BITS,
			MAX_ENTITIES_MASK = MAX_ENTITIES - 1,
			NONE = MAX_ENTITIES - 1,
			LANDSCAPE = MAX_ENTITIES - 2,
			MAX_NORMAL = LANDSCAPE
		} t;
		AX_DECLARE_ENUM(EntityNum);
	};

	enum SndChannelId {
		SndChannelId_Any = Sound::ChannelId_Any,
		SndChannelId_Voice = Sound::ChannelId_One,
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

	class Entity;
	typedef List<Entity*>			EntityList;
	typedef EntityList::iterator EntityIterator;
	typedef Dict<String,Entity*>	EntityHash;

	class AX_API Entity : public Node, public IObserver {
	public:
		AX_DECLARE_CLASS(Entity, Node, "Game.Entity")
		AX_END_CLASS()

		friend class World;

		enum State {
			Hide,
			Inactive,
			Active,
		};

		Entity();
		virtual ~Entity();

		virtual void doThink();
		virtual bool isPlayer() const { return false; }

		// implement IObserver
		virtual void doNotify(IObservable* subject, int arg);

		virtual void setState(State state);
		inline State getState() { return m_state; }

		void autoGenerateName();

	protected:
		// implement Node
		virtual void reload();
		virtual void doSpawn();
		virtual void doRemove();

		// called by subclass
		virtual void onPhysicsActived();
		virtual void onPhysicsDeactived();

		void invoke_onThink();

	protected:
		EntityNum m_entityNum;
		World* m_world;			// world the entity has added to

	private:
		State m_state;
	};

	//--------------------------------------------------------------------------
	// class EntityPtr
	//--------------------------------------------------------------------------

	template< class type >
	class EntityPtr {
	public:
		EntityPtr();

		EntityPtr<type>& operator=( type *ent );

		// synchronize entity pointers over the network
		int getSpawnId( void ) const { return spawnId; }
		bool setSpawnId( int id );
		bool updateSpawnId( void );

		bool isValid( void ) const;
		type* getEntity( void ) const;
		int getEntityNum( void ) const;

	private:
		World* m_world;
		int m_spawnId;
	};

	template< class type >
	inline EntityPtr<type>::EntityPtr() {
		m_spawnId = 0;
	}

	template< class type >
	inline EntityPtr<type> &EntityPtr<type>::operator=( type *ent ) {
		if ( ent == NULL ) {
			m_spawnId = 0;
		} else {
			m_spawnId = ( m_world->m_spawnIds[ent->entityNumber] << EntityNum::MAX_ENTITIES_BITS ) | ent->entityNumber;
		}
		return *this;
	}

	template< class type >
	inline bool EntityPtr<type>::setSpawnId( int id ) {
		// the reason for this first check is unclear:
		// the function returning false may mean the spawnId is already set right, or the entity is missing
		if ( id == m_spawnId ) {
			return false;
		}
		if ( ( id >> EntityNum::MAX_ENTITIES_BITS ) == m_world->m_spawnIds[id & EntityNum::MAX_ENTITIES_MASK] ) {
			m_spawnId = id;
			return true;
		}
		return false;
	}

	template< class type >
	inline bool EntityPtr<type>::isValid( void ) const {
		return ( m_world->m_spawnIds[m_spawnId & EntityNum::MAX_ENTITIES_MASK] == ( m_spawnId >> EntityNum::MAX_ENTITIES_BITS ) );
	}

	template< class type >
	inline type *EntityPtr<type>::getEntity( void ) const {
		int entityNum = m_spawnId & EntityNum::MAX_ENTITIES_MASK;
		if ( ( m_world->m_spawnIds[entityNum] == ( m_spawnId >> EntityNum::MAX_ENTITIES_BITS ) ) ) {
			return static_cast<type *>( m_world->m_spawnIds[ entityNum ] );
		}
		return NULL;
	}

	template< class type >
	inline int EntityPtr<type>::getEntityNum( void ) const {
		return m_spawnId & EntityNum::MAX_ENTITIES_MASK;
	}


	//--------------------------------------------------------------------------
	// class RigidBody
	//--------------------------------------------------------------------------

	class AX_API RigidBody : public Entity {
	public:
		AX_DECLARE_CLASS(RigidBody, Entity, "Game.Physics.RigidBody")
			AX_METHOD(loadAsset)
		AX_END_CLASS()

		RigidBody();
		virtual ~RigidBody();

		// implement Entity
		virtual void doThink();

		// properties

	protected:
		void loadAsset(const LuaTable& t);
		void clear();

	protected:
		PhysicsModel* m_model;
		physicsRigid* m_rigid;
	};

}} // namespace Axon::Game


#endif // eng guardian

