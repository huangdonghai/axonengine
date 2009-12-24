/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

	Articulated::Articulated() {
		m_ragdoll = new PhysicsRagdoll("models/havok/penetration_rig_L4101.mesh");
		m_updateFlags.setAll();
	}

	Articulated::~Articulated() {

	}

	void Articulated::doThink() {
		if (m_updateFlags.isSet(SetPhysics)) {
			if (m_world) {
				m_world->getPhysicsWorld()->addEntity(m_ragdoll);
				m_ragdoll->setMotionType(PhysicsEntity::Motion_Keyframed);
				m_ragdoll->setMatrix(m_matrix_p);
				m_ragdoll->mapInSkeletalPose(m_pose);
				m_ragdoll->setMotionType(PhysicsEntity::Motion_Dynamic);
			}
			m_updateFlags.unset(SetPhysics);
		}

		m_ragdoll->setMatrix(m_matrix_p);
		m_ragdoll->mapOutSkeletalPose(m_pose);

#if 0
		int frametime = m_world->getFrameTime();
		m_animator->step(frametime);
		m_animator->renderToPose(m_renderPose);
		m_ragdoll->mapInSkeletalPose(m_renderPose);

		m_model->getInstance()->setPose(m_renderPose);
#endif
		m_model->setMatrix(m_matrix_p);
		m_model->setPose(m_pose);

//		if (!m_model->isPresented()) {
			m_world->getRenderWorld()->addEntity(m_model);
//		} else {
//			m_world->getRenderWorld()->updateActor(m_model);
//		}
	}

AX_END_NAMESPACE

