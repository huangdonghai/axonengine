/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_RAGDOLL_H
#define AX_PHYSICS_RAGDOLL_H


namespace Axon { namespace Physics {

	class SkeletonMapper {};

	class Ragdoll : public Packable, public Entity {
	public:
		Ragdoll(const String& name);
		virtual ~Ragdoll();

		virtual void setAutoDeactive(bool val);
		virtual void setActive(bool activate);
		virtual bool isActive() const;
		virtual void setMatrix(const AffineMat& matrix);
		virtual AffineMat getMatrix() const;
		void setMotionType(MotionType motion);
		MotionType getMotionType() const;
		virtual Type getType() const { return kRagdoll; }
		virtual void bind(World* world);
		virtual void unbind(World* world);

		void mapOutSkeletalPose(Pose* pose);
		void mapInSkeletalPose(Pose* pose);

	protected:
		void setKeyframed();
		void setDynamic();
		void setBodyKeyframed(hkpRigidBody* rb);
		void setBodyDynamic(hkpRigidBody* rb, int boneId, int parentId);

	public:
		hkaRagdollInstance* m_instance;
		hkaSkeletonMapper* m_ragdollToSkeletal;
		hkaSkeletonMapper* m_skeletalToRagdoll;
		hkaPose* m_pose;
		MotionType m_motionType;
		bool m_isValid;
		AffineMat m_matrix;
	};

}} // namespace Axon::Physics

#endif // end guardian

