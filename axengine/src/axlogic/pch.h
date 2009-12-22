/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include <axcore/stdinc.h>

#define HAVOK_PHYSICS_ENABLED
#define HAVOK_ANIMATION_ENABLED

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/Memory/hkMemory.h>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/System/Io/Writer/Stdio/hkStdioStreamWriter.h>
#include <Common/Base/System/Io/Reader/Memory/hkMemoryStreamReader.h>

#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.h>
#include <Common/Serialize/Util/hkChainedClassNameRegistry.h>
#include <Common/Serialize/Tagfile/Binary/hkBinaryTagfileReader.h>
#include <Common/Serialize/Data/Dict/hkDataObjectDict.h>
#include <Common/Serialize/Data/hkDataObject.h>
#include <Common/Serialize/Data/Util/hkDataObjectUtil.h>

#include <Common/Visualize/hkVisualDebugger.h>

#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/Memory/Debug/hkDebugMemory.h>
#include <Common/Base/Memory/hkDebugMemorySnapshot.h>
#include <Common/Base/Memory/Memory/Pool/hkPoolMemory.h>
#include <Common/Base/Memory/MemoryClasses/hkMemoryClassDefinitions.h>

#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/System/Io/FileSystem/hkNativeFileSystem.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/System/Io/Writer/hkStreamWriter.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>
#include <Common/Base/System/Stopwatch/hkStopwatch.h>

#include <Common/Base/Memory/Memory/FreeList/hkFreeListMemory.h>
#include <Common/Base/Memory/Memory/FreeList/SystemMemoryBlockServer/hkSystemMemoryBlockServer.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Common/Base/Config/hkConfigSimd.h>

#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>
#include <Physics/Collide/Filter/Group/hkpGroupFilterSetup.h>

#include <Physics/Collide/Shape/Convex/hkpConvexShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Convex/Triangle/hkpTriangleShape.h>
#include <Physics/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldBaseCinfo.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Collide/Query/Collector/PointCollector/hkpAllCdPointCollector.h>

#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Utilities/Actions/MouseSpring/hkpMouseSpringAction.h>
#include <Physics/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics/Utilities/Serialize/hkpPhysicsData.h>

#include <Physics/ConstraintSolver/Simplex/hkpSimplexSolver.h>

#include <Physics/Dynamics/Entity/hkpEntityActivationListener.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Dynamics/Phantom/hkpSimpleShapePhantom.h>

#include <Animation/Animation/hkaAnimationContainer.h>
// Vertex Deformation
#include <Animation/Animation/Deform/Skinning/Fpu/hkaFPUSkinningDeformer.h>
#include <Animation/Animation/Deform/Skinning/Simd/hkaSimdSkinningDeformer.h>

// RenderMesh info
#include <Common/SceneData/Material/hkxMaterial.h>
#include <Common/SceneData/Material/hkxTextureFile.h>
#include <Common/SceneData/Mesh/hkxMesh.h>
#include <Common/SceneData/Mesh/hkxMeshSection.h>
#include <Common/SceneData/Mesh/hkxVertexBuffer.h>
#include <Common/SceneData/Mesh/hkxIndexBuffer.h>
#include <Common/SceneData/Mesh/Formats/hkxVertexP4N4T4B4C1T.h>
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/SceneData/Graph/hkxNode.h>

// Skeletal Animation
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Animation/hkaAnimationBinding.h>
#include <Animation/Animation/Animation/hkaAnnotationTrack.h>
#include <Animation/Animation/Animation/hkaAnimation.h>
#include <Animation/Animation/Motion/hkaAnimatedReferenceFrame.h>
#include <Animation/Animation/Motion/Default/hkaDefaultAnimatedReferenceFrame.h>
#include <Animation/Animation/Rig/hkaBone.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Animation/Rig/hkaBoneAttachment.h>
#include <Animation/Animation/Rig/hkaSkeletonUtils.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>

// Deformation
#include <Animation/Animation/Deform/Skinning/hkaMeshBinding.h>

// visual debuger
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>

// Ragdoll
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <Animation/Ragdoll/Penetration/hkaDetectRagdollPenetration.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif