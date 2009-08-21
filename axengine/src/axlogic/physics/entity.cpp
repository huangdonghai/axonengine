/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

namespace Axon { namespace Physics {

	class EntityActivationListener
		: public hkReferencedObject
		, public hkpEntityActivationListener
		, public hkpEntityListener
	{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO);

		EntityActivationListener(Entity* e) : m_wrapper(e) {}

		virtual void entityDeactivatedCallback(hkpEntity* entity) {
			m_wrapper->notify(Entity::Deactivated);
		}

		virtual void entityActivatedCallback(hkpEntity* entity) {
			m_wrapper->notify(Entity::Activated);
		}

		/// Called when an entity is deleted. hkpEntityListener subclasses <b>must</b> implement this function.
		virtual void entityDeletedCallback(hkpEntity* entity) {
			delete this;
		}

	private:
		Entity* m_wrapper;
	};

	Entity::Entity() {
		m_world = nullptr;
	}

	Entity::~Entity() {
	}

	void Entity::setGameEntity( gameEntity* ent )
	{
		m_gameEntity = ent;
	}

}} // namespace Axon::Physics

