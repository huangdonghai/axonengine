/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

	enum { LAYER_TEST = 1 };

	PhysicsRagdoll::PhysicsRagdoll(const String &name) {
		m_instance = nullptr;
		m_ragdollToSkeletal = nullptr;
		m_skeletalToRagdoll = nullptr;
		m_pose = nullptr;

		MotionType m_motionType = PhysicsEntity::Motion_Invalid;

		m_package = g_havokPackageManager->findPackage(name);

		if (!m_package) {
			return;
		}

		hkaRagdollInstance *origin_instance_ = m_package->getRagdoll();
//		m_instance = origin_instance_->clone(hkpConstraintInstance::CLONE_DATAS_WITH_MOTORS);
//		m_instance = origin_instance_;
		m_instance = Util::clone(origin_instance_);
//		const hkArray<hkpConstraintInstance*>& origin_constraints = origin_instance_->getConstraintArray();
//		const hkArray<hkpConstraintInstance*>& constraints = m_instance->getConstraintArray();

//		m_instance->getRigidBodyOfBone(0)->setMotionType(hkpMotion::MOTION_KEYFRAMED);

		m_pose = new hkaPose(m_instance->getSkeleton());

		hkaSkeletonMapper *mapper = m_package->getMapper(nullptr);
		while (mapper) {
			// Use the skeleton to determine which mapper is which
			if (mapper->m_mapping.m_skeletonA == m_instance->m_skeleton) {
				m_ragdollToSkeletal = mapper;
			} else {
				HK_ASSERT(0,mapper->m_mapping.m_skeletonB == m_instance->m_skeleton);
				m_skeletalToRagdoll = mapper;
			}

			mapper = m_package->getMapper(mapper);
		}

		// This routine iterates through the bodies pointed to by the constraints and stabilizes their inertias.
		// This makes both ragdoll controllers lees sensitive to angular effects and hence more effective
//		const hkArray<hkpConstraintInstance*>& constraints = m_instance->getConstraintArray();
//		hkpInertiaTensorComputer::optimizeInertiasOfConstraintTree(constraints.begin(), constraints.getSize(), m_instance->getRigidBodyOfBone(0));
	}

	PhysicsRagdoll::~PhysicsRagdoll() {
		if (m_instance) m_instance->removeReference();
		if (m_ragdollToSkeletal) m_ragdollToSkeletal->removeReference();
		if (m_skeletalToRagdoll) m_skeletalToRagdoll->removeReference();
	}

	void PhysicsRagdoll::setMotionType(MotionType motion) {
		if (m_motionType == motion) {
			return;
		}

		m_motionType = motion;

		switch (motion) {
		case Motion_Invalid:
		case Motion_Fixed:
		case Motion_Keyframed:
			setKeyframed();
			return;
		case Motion_Dynamic:
			setDynamic();
			return;
		}
	}

	PhysicsEntity::MotionType PhysicsRagdoll::getMotionType() const {
		return m_motionType;
	}

	void PhysicsRagdoll::mapOutSkeletalPose(HavokPose *pose) {
		HavokPose *physicspose = (HavokPose*)pose;

		if (!physicspose || !physicspose->m_havokPose) {
			return;
		}

		hkQsTransform qst;
		qst.setFromTransformNoScale(x2h(m_matrix));
//		qst.setIdentity();

		// get pose from ragdoll to model space
		m_instance->getPoseModelSpace(m_pose->accessUnsyncedPoseModelSpace().begin(), qst);

		// map to highres
		hkaPose *poseHighRes = physicspose->m_havokPose;
		poseHighRes->setToReferencePose();

//		m_ragdollToSkeletal->mapPose(m_pose->getPoseModelSpace().begin(), poseHighRes->getPoseLocalSpace().begin(), poseHighRes->accessPoseModelSpace().begin(), hkaSkeletonMapper::CURRENT_POSE);
		m_ragdollToSkeletal->mapPose(m_pose->getSyncedPoseModelSpace().begin(), poseHighRes->getSkeleton()->m_referencePose, poseHighRes->accessUnsyncedPoseModelSpace().begin(), hkaSkeletonMapper::CURRENT_POSE);
	}

	void PhysicsRagdoll::mapInSkeletalPose(HavokPose *pose) {
		if (!m_pose) {
			return;
		}

		HavokPose *physicspose = (HavokPose*)pose;

		hkArray<hkQsTransform> ragdollArrayModelSpace(m_instance->getNumBones());

		// Map the pose from the animation (highres) to ragdoll (lowres)
		m_skeletalToRagdoll->mapPose(physicspose->m_havokPose->getSyncedPoseModelSpace().begin(), m_instance->getSkeleton()->m_referencePose, ragdollArrayModelSpace.begin(), hkaSkeletonMapper::CURRENT_POSE);

		m_pose->setPoseModelSpace(ragdollArrayModelSpace);

		// set pose to ragdoll instance
		hkQsTransform qst;
		qst.setFromTransformNoScale(x2h(m_matrix));
//		qst.setIdentity();

		m_instance->setPoseModelSpace(m_pose->getSyncedPoseModelSpace().begin(), qst);
	}

	void PhysicsRagdoll::setAutoDeactive(bool val) {
		// set all bodies to keyframed motion
		for (int i = 0; i < m_instance->getNumBones(); i++) {
			hkpRigidBody *rb = m_instance->getRigidBodyOfBone(i);
			// Initialize with quality type and collision filter
			if (!rb) {
				continue;
			}
			if (val) {
				rb->setDeactivator(hkpRigidBodyDeactivator::DEACTIVATOR_SPATIAL);
			} else {
				rb->setDeactivator(hkpRigidBodyDeactivator::DEACTIVATOR_NEVER);
			}
		}
	}

	void PhysicsRagdoll::setActive(bool activate)
	{

	}

	bool PhysicsRagdoll::isActive() const
	{
		return true;
	}


	void PhysicsRagdoll::setMatrix(const Matrix &matrix) {
		m_matrix = matrix;
	}

	Axon::Matrix PhysicsRagdoll::getMatrix() const {
		return m_matrix;
	}

	void PhysicsRagdoll::bind(PhysicsWorld *world) {
		m_instance->addToWorld(world->m_havokWorld, true);
	}

	void PhysicsRagdoll::unbind(PhysicsWorld *world) {
		m_instance->removeFromWorld();
	}

	void PhysicsRagdoll::setKeyframed() {
		// set all bodies to keyframed motion
		for (int i = 0; i < m_instance->getNumBones(); i++) {
			hkpRigidBody *rb = m_instance->getRigidBodyOfBone(i);
			// Initialize with quality type and collision filter
			if (rb != HK_NULL) {
				setBodyKeyframed(rb);
			}
		}
	}

	void PhysicsRagdoll::setDynamic() {
		for (int b = 0; b < m_instance->getNumBones(); b++) {
			hkpRigidBody *rb = m_instance->getRigidBodyOfBone(b);

			if (rb != HK_NULL) {
				const int parentId = m_instance->getParentOfBone(b);

				setBodyDynamic(rb, b, parentId);
			}
		}

		setAutoDeactive(true);
	}

	void PhysicsRagdoll::setBodyKeyframed(hkpRigidBody *rb)
	{
		const hkUint32 fi = hkpGroupFilter::calcFilterInfo(PhysicsWorld::LAYER_RAGDOLL_KEYFRAMED, 1, 0, 0);

		if ((rb->getMotionType() != hkpMotion::MOTION_KEYFRAMED) ||
			(rb->getQualityType() != HK_COLLIDABLE_QUALITY_KEYFRAMED) ||
			(rb->getCollisionFilterInfo() != fi))
		{
			rb->setMotionType(hkpMotion::MOTION_KEYFRAMED);
			rb->setQualityType(HK_COLLIDABLE_QUALITY_KEYFRAMED);
			rb->setCollisionFilterInfo(fi);

			if (rb->getWorld())
			{
				rb->getWorld()->updateCollisionFilterOnEntity(rb, HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS);
			}
		}
	}

	void PhysicsRagdoll::setBodyDynamic(hkpRigidBody *rb, int boneId, int parentId)
	{
		const hkUint32 newFi = hkpGroupFilter::calcFilterInfo(PhysicsWorld::LAYER_RAGDOLL_DYNAMIC, 1, boneId+1, parentId+1);
//		const hkUint32 newFi = hkpGroupFilter::calcFilterInfo(PhysicsWorld::LAYER_RAGDOLL_DYNAMIC, 1, 0, 0);

		if ((rb->getMotionType() == hkpMotion::MOTION_KEYFRAMED) ||
			(rb->getQualityType() != HK_COLLIDABLE_QUALITY_MOVING) ||
			(rb->getCollisionFilterInfo() != newFi))
		{
			rb->setMotionType(hkpMotion::MOTION_DYNAMIC);
			rb->setQualityType(HK_COLLIDABLE_QUALITY_MOVING);
			rb->setCollisionFilterInfo(newFi);

			if (rb->getWorld())
			{
				rb->getWorld()->updateCollisionFilterOnEntity(rb, HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_PROCESS_SHAPE_COLLECTIONS);
				const hkpCollisionFilter *filter = rb->getWorld()->getCollisionFilter();

				AX_ASSERT(filter);
			}
		} else {
			Printf("Debug..\n");
		}
	}

AX_END_NAMESPACE
