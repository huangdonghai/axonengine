/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

	class EntityActivationListener
		: public hkReferencedObject
		, public hkpEntityActivationListener
		, public hkpEntityListener
	{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO);

		EntityActivationListener(PhysicsEntity *e) : m_wrapper(e) {}

		virtual void entityDeactivatedCallback(hkpEntity *entity) {
			m_wrapper->notifyObservers(PhysicsEntity::Deactivated);
		}

		virtual void entityActivatedCallback(hkpEntity *entity) {
			m_wrapper->notifyObservers(PhysicsEntity::Activated);
		}

		/// Called when an entity is deleted. hkpEntityListener subclasses <b>must</b> implement this function.
		virtual void entityDeletedCallback(hkpEntity *entity) {
			delete this;
		}

	private:
		PhysicsEntity *m_wrapper;
	};

	PhysicsEntity::PhysicsEntity() {
		m_world = nullptr;
	}

	PhysicsEntity::~PhysicsEntity() {
	}

	void PhysicsEntity::setGameEntity( GameActor *ent )
	{
		m_gameEntity = ent;
	}

AX_END_NAMESPACE

