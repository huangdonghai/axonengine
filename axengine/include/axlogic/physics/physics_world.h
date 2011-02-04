/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_WORLD_H
#define AX_PHYSICS_WORLD_H

AX_BEGIN_NAMESPACE

class PhysicsWorld : public IObservable
{
public:
	enum ObserveFlag {
		Synchronize = 1
	};
	typedef Flags_<ObserveFlag> ObserveFlags;

	enum CollisionLayer {
		LAYER_LANDSCAPE = 1,
		LAYER_RAGDOLL_DYNAMIC = 2,
		LAYER_RAGDOLL_KEYFRAMED = 3,
		LAYER_RAGDOLL_PENETRATING = 4,
		LAYER_RAYCAST = 5,
		LAYER_PROXY = 6
	};

	PhysicsWorld(float worldsize = 2048, bool enableVisualDebug = true, bool enableMultithread = true);
	~PhysicsWorld();

	void step(float frametime);

	void addEntity(PhysicsEntity *entity);
	void removeEntity(PhysicsEntity *entity);

protected:
	void stepMt(float frametime);

public:
	hkpWorld *m_havokWorld;

private:
	hkVisualDebugger *m_vdb;
	hkpMultithreadingUtil *m_multithreadingUtil;
	float m_physicsDeltaTime;
};

AX_END_NAMESPACE

#endif // end guardian

