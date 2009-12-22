/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_PHYSICS_PUBLIC_H
#define AX_PHYSICS_PUBLIC_H

// forward declaration for havok engine
class hkpShape;
class hkpEntity;
class hkpRigidBody;
class hkpWorld;
class hkaMeshBinding;
class hkLoader;
class hkRootLevelContainer;
class hkVisualDebugger;
class hkpMultithreadingUtil;
class hkaSkeleton;
class hkxScene;
class hkaAnimatedSkeleton;
class hkaAnimationControl;
class hkaSkeletalAnimation;
class hkxNode;
class hkMatrix4;
class hkxMesh;
class hkpPhysicsData;
class hkaAnimationContainer;
class hkaDefaultAnimationControl;
class hkaAnimationBinding;
class hkaPose;
class hkaRagdollInstance;
class hkaSkeletonMapper;
class hkxMaterial;
class hkDataWorldDict;
class hkpCharacterProxy;

namespace Axon { namespace Game {
	class GameEntity;
}} // namespace Axon::Game

AX_BEGIN_NAMESPACE

	using namespace Axon::Logic;

	// forward declaration
	class PhysicsEntity;
	class PhysicsRigid;
	class PhysicsTerrain;
	class PhysicsRagdoll;
	class PhysicsPlayer;

	typedef Game::GameEntity GameEntity;

AX_END_NAMESPACE

#include "shape.h"
#include "package.h"
#include "world.h"
#include "system.h"

namespace Axon {

	extern PhysicsSystem* g_physicsSystem;
	extern HavokPackageManager* g_havokPackageManager;

} // namespace Axon

#endif // end guardian



