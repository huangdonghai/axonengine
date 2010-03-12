/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class PhysicsWorld
//--------------------------------------------------------------------------

PhysicsWorld::PhysicsWorld(float worldsize, bool vdb, bool mt) {
	mt = false;

	m_physicsDeltaTime = 1.0f / 30.0f;

	hkpWorldCinfo info;
//		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_MEDIUM); 
	info.m_enableDeactivation = true;
	info.m_deactivationReferenceDistance = 0.01f;
	info.setBroadPhaseWorldSize(worldsize * 2);
	info.m_gravity.set(0, 0, -9.8f);
	info.m_collisionTolerance = 0.05f;
//		info.m_broadPhaseBorderBehaviour = info.BROADPHASE_BORDER_DO_NOTHING;	// we do our own stuff

	if (mt) {
		info.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;
	} else {
		info.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_DISCRETE;
	}
	m_havokWorld = new hkpWorld(info);
	m_havokWorld->lock();

	// setup layer collision
	{
		// Replace filter
		hkpGroupFilter *groupFilter = new hkpGroupFilter();

		// We disable collisions between different layers to determine 
		// what behavior we want
		groupFilter->disableCollisionsBetween(LAYER_RAGDOLL_KEYFRAMED, LAYER_LANDSCAPE);
		groupFilter->disableCollisionsBetween(LAYER_RAGDOLL_PENETRATING, LAYER_LANDSCAPE);
//			groupFilter->disableCollisionsBetween(LAYER_RAGDOLL_DYNAMIC, LAYER_RAGDOLL_DYNAMIC);
		groupFilter->disableCollisionsBetween(LAYER_RAYCAST, LAYER_RAGDOLL_DYNAMIC);
		groupFilter->disableCollisionsBetween(LAYER_RAYCAST, LAYER_RAGDOLL_KEYFRAMED);
		groupFilter->disableCollisionsBetween(LAYER_RAYCAST, LAYER_RAGDOLL_PENETRATING);

		m_havokWorld->setCollisionFilter(groupFilter, true);
		groupFilter->removeReference();
	}

	// As we want collision detection, we must register the agents for 
	// the shapes we are going to use. Here we will just use the 
	// hkpAgentRegisterUtil to register the common ones and that includes 
	// the heightfields.
	hkpAgentRegisterUtil::registerAllAgents(m_havokWorld->getCollisionDispatcher());

	if (vdb) {
		hkpPhysicsContext *physicsContext = new hkpPhysicsContext;
		physicsContext->addWorld(m_havokWorld); 

		hkpPhysicsContext::registerAllPhysicsProcesses();

		hkArray<hkProcessContext*> contexts;
		contexts.pushBack(physicsContext);  
		m_vdb = new hkVisualDebugger(contexts);
		m_vdb->serve(/* optional port number */);
	} else {
		m_vdb = nullptr;
	}

#if 0
	if (mt) {
		hkpMultithreadingUtilCinfo ci;
		ci.m_world = m_havokWorld;
		ci.m_numThreads = 1;
		m_multithreadingUtil = new hkpMultithreadingUtil(ci);
	} else {
#endif
		m_multithreadingUtil = nullptr;
#if 0
	}
#endif
	m_havokWorld->unlock();
}

PhysicsWorld::~PhysicsWorld() {
	m_havokWorld->removeReference();
}

void PhysicsWorld::step(float frametime) {
	if (m_multithreadingUtil) {
		stepMt(frametime);
	}

	double start = OsUtil::seconds();

	m_havokWorld->stepDeltaTime(frametime);
	notify(Synchronize);

	if (m_vdb) {
		m_vdb->step(frametime);
	}

	float physicstime = OsUtil::seconds() - start;
}

void PhysicsWorld::stepMt(float frametime) {
#if 0
	uint_t start = Milliseconds();
	m_multithreadingUtil->waitForStepWorldFinished();

	notify(Synchronize);

	if (m_vdb) {
		m_vdb->step(frametime);
	}

	m_havokWorld->resetThreadTokens();
//		m_multithreadingUtil->setFrameTimeMarker(frametime * 0.001f);
	m_multithreadingUtil->startStepWorld(frametime * 0.001f);

	int physicstime = Milliseconds() - start;
	Printf("Physics Time: %d", physicstime);
#endif
}

void PhysicsWorld::addEntity(PhysicsEntity *entity) {
	if (!entity) {
		return;
	}

	if (entity->m_world) {
		if (entity->m_world == this) {
			return;
		} else {
			Errorf("bind entity to different world");
		}
	}

	entity->m_world = this;
	entity->bind(this);
}

void PhysicsWorld::removeEntity(PhysicsEntity *entity) {
	if (!entity) {
		return;
	}

	if (!entity->m_world || entity->m_world != this) {
		Errorf("error entity to remove");
	}
	entity->unbind(this);
	entity->m_world = 0;
}


AX_END_NAMESPACE

