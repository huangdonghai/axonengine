/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_RAGDOLL_H
#define AX_PHYSICS_RAGDOLL_H


AX_BEGIN_NAMESPACE

	class SkeletonMapper {};

	class PhysicsRagdoll : public HavokPackable, public PhysicsEntity {
	public:
		PhysicsRagdoll(const std::string &name);
		virtual ~PhysicsRagdoll();

		virtual void setAutoDeactive(bool val);
		virtual void setActive(bool activate);
		virtual bool isActive() const;
		virtual void setMatrix(const Matrix &matrix);
		virtual Matrix getMatrix() const;
		void setMotionType(MotionType motion);
		MotionType getMotionType() const;
		virtual Type getType() const { return kRagdoll; }
		virtual void bind(PhysicsWorld *world);
		virtual void unbind(PhysicsWorld *world);

		void mapOutSkeletalPose(HavokPose *pose);
		void mapInSkeletalPose(HavokPose *pose);

	protected:
		void setKeyframed();
		void setDynamic();
		void setBodyKeyframed(hkpRigidBody *rb);
		void setBodyDynamic(hkpRigidBody *rb, int boneId, int parentId);

	public:
		hkaRagdollInstance *m_instance;
		hkaSkeletonMapper *m_ragdollToSkeletal;
		hkaSkeletonMapper *m_skeletalToRagdoll;
		hkaPose *m_pose;
		MotionType m_motionType;
		bool m_isValid;
		Matrix m_matrix;
	};

AX_END_NAMESPACE

#endif // end guardian

