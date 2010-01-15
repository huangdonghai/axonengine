/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_RIGIDBODY_H
#define AX_PHYSICS_RIGIDBODY_H

AX_BEGIN_NAMESPACE

	// forward decl
	class HavokPackage;

	//--------------------------------------------------------------------------
	// class PhysicsRigid
	//--------------------------------------------------------------------------

	class PhysicsRigid : public PhysicsEntity {
	public:
		PhysicsRigid();
		PhysicsRigid(const String &name);
		PhysicsRigid(HavokPackage *package, hkpRigidBody *rigid);
		virtual ~PhysicsRigid();

		bool isValid() const { return m_havokRigid!=0; }

		// implement PhysicsEntity
		virtual void setActive(bool activate);
		virtual bool isActive() const;
		virtual void setAutoDeactive(bool val);
		virtual void setMatrix(const Matrix3x4 &matrix);
		virtual Matrix3x4 getMatrix() const;
		virtual void setMotionType(MotionType motion);
		virtual MotionType getMotionType() const;
		virtual Type getType() const { return kRigidBody; }
		virtual void bind(PhysicsWorld *world);
		virtual void unbind(PhysicsWorld *world);

	protected:
		HavokPackage *m_package;
		hkpRigidBody *m_havokRigid;
		Matrix3x4 m_matrix;
		MotionType m_motionType;
	};

	//--------------------------------------------------------------------------
	// class PhysicsTerrain
	//--------------------------------------------------------------------------

	class PhysicsTerrain : public PhysicsRigid {
	public:
		PhysicsTerrain(const ushort_t *data, int size, float tilemeters);
		virtual ~PhysicsTerrain();

	protected:
	};

AX_END_NAMESPACE

#endif // end guardian

