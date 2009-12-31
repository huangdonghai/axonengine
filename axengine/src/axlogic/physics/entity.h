/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_ENTITY_H
#define AX_PHYSICS_ENTITY_H

AX_BEGIN_NAMESPACE

	class PhysicsEntity : public IObservable {
	public:
		friend class PhysicsWorld;

		enum Type {
			kInvalid,		// invalid type, just for error check
			kRigidBody,
			kRagdoll,
			kPlayer
		};

		enum MotionType {
			Motion_Invalid,
			Motion_Fixed,
			Motion_Keyframed,
			Motion_Dynamic,
		};

		enum ObserveFlag {
			Activated = 1,
			Deactivated = 2,
			Synchronize = 4
		};

		PhysicsEntity();
		virtual ~PhysicsEntity();

		virtual void setActive(bool activate) = 0;
		virtual bool isActive() const = 0;
		virtual void setAutoDeactive(bool val) = 0;
		virtual void setMatrix(const AffineMat &matrix) = 0;
		virtual AffineMat getMatrix() const = 0;
		virtual Type getType() const { return kInvalid; }
		virtual void bind(PhysicsWorld *world) = 0;
		virtual void unbind(PhysicsWorld *world) = 0;

		void setGameEntity(GameActor *ent);

	protected:
		PhysicsWorld *m_world;
		GameActor *m_gameEntity;
	};

AX_END_NAMESPACE

#endif // end guardian

