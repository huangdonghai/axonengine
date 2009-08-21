/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Render {

	Target::Target()
	{
		m_realAllocated = false;
	}

	Target::~Target()
	{

	}

	void Target::allocReal()
	{
		if (m_realAllocated)
			return;

		g_targetManager->allocReal(this);
		m_realAllocated = true;
	}

	void Target::freeReal()
	{
		if (!m_realAllocated)
			return;

		g_targetManager->freeReal(this);
		m_realAllocated = false;
	}

	ReflectionTarget::ReflectionTarget(World* world, Entity* actor, Primitive* prim, int width, int height)
	{
		m_world = world;
		m_actor = actor;
		m_prim = prim;
		m_updateFrame = -1;
		m_target = g_targetManager->allocTarget(Target::PooledAlloc, width, height, TexFormat::BGRA8);
	}

	ReflectionTarget::~ReflectionTarget()
	{
		g_targetManager->freeTarget(m_target);
	}

	void ReflectionTarget::update( QueuedScene* qscene )
	{
		bool needUpdate = false;

		if (!m_target->m_realAllocated) {
			m_target->allocReal();
			needUpdate = true;
		}

		if (m_updateFrame == -1) {
			needUpdate = true;
		}

		if (m_world->getVisFrameId() - m_updateFrame > 4) {
			needUpdate = true;
		}

		if (!needUpdate)
			return;

		QueuedScene* subscene = qscene->addSubScene();
		if (!subscene)
			return;

		subscene->sceneType = QueuedScene::Reflection;
		subscene->camera = qscene->camera.createMirrorCamera(Plane(0, 0, 1, 0));
		subscene->camera.setTarget(m_target);
		subscene->camera.setViewRect(Rect(0, 0, 512, 512));

		m_updateFrame = m_world->getVisFrameId();
	}

	TargetManager::~TargetManager() {}

	TargetManager::TargetManager()
	{
	}

	void TargetManager::allocReal( Target* target )
	{
		m_realAllocTargets.push_back(target);
	}

	void TargetManager::freeReal( Target* target )
	{
		m_freeRealTargets.push_back(target);
	}

	ReflectionTarget* TargetManager::findReflection( World* world, Entity* actor, Primitive* prim, int width, int height )
	{
		List<ReflectionTarget*>::iterator it = m_reflectionTargets.begin();

		ReflectionTarget* reflTarget = 0;
		for (; it != m_reflectionTargets.end(); ++it) {
			ReflectionTarget* curTarget = *it;

			if (curTarget->m_world == world && curTarget->m_actor == actor && curTarget->m_prim == prim) {
				reflTarget = curTarget;
				break;
			}
		}

		if (!reflTarget) {
			reflTarget = new ReflectionTarget(world, actor, prim, width, height);
			m_reflectionTargets.push_back(reflTarget);
		}

		return reflTarget;
	}

}} // namespace Axon::Render

