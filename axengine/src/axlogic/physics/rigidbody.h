/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_RIGIDBODY_H
#define AX_PHYSICS_RIGIDBODY_H

namespace Axon { namespace Physics {

	// forward decl
	class Package;

	//--------------------------------------------------------------------------
	// class RigidBody
	//--------------------------------------------------------------------------

	class RigidBody : public Entity {
	public:
		RigidBody();
		RigidBody(const String& name);
		RigidBody(Package* package, hkpRigidBody* rigid);
		virtual ~RigidBody();

		bool isValid() const { return m_havokRigid!=0; }

		// implement Entity
		virtual void setActive(bool activate);
		virtual bool isActive() const;
		virtual void setAutoDeactive(bool val);
		virtual void setMatrix(const AffineMat& matrix);
		virtual AffineMat getMatrix() const;
		virtual void setMotionType(MotionType motion);
		virtual MotionType getMotionType() const;
		virtual Type getType() const { return kRigidBody; }
		virtual void bind(World* world);
		virtual void unbind(World* world);

	protected:
		Package* m_package;
		hkpRigidBody* m_havokRigid;
		AffineMat m_matrix;
		MotionType m_motionType;
	};

	//--------------------------------------------------------------------------
	// class Terrain
	//--------------------------------------------------------------------------

	class Terrain : public RigidBody {
	public:
		Terrain(const ushort_t* data, int size, float tilemeters);
		virtual ~Terrain();

	protected:
	};

}} // namespace Axon::Physics

#endif // end guardian

