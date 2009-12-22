/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_FILE_H
#define AX_PHYSICS_FILE_H

namespace Axon { namespace Physics {

	class Rig;
	class Pose;
	class Animation;
	class Animator;
	class Ragdoll;
	class Package;

	//--------------------------------------------------------------------------
	// class Packable
	//--------------------------------------------------------------------------

	class Packable {
	public:
		Packable() { m_package = nullptr; }
		Packable(Package* package) : m_package(package) {}
		virtual ~Packable() { SafeRelease(m_package); }

	protected:
		Package* m_package;
	};

	//--------------------------------------------------------------------------
	// class Animator
	//--------------------------------------------------------------------------

	class Animator {
	public:
		Animator(Rig* rig);
		~Animator();

		void addAnimation(Animation* anim);
		void removeAnimation(Animation* anim);
		void removeAllAnimation();
		void renderToPose(Pose* pose);
		void step(int frametime);

		inline Rig* getRig() { return m_rig; }

	public:
		Rig* m_rig;
		hkaAnimatedSkeleton* m_animator;
		List<Animation*>		m_animations;
	};

	//--------------------------------------------------------------------------
	// class Animation
	//--------------------------------------------------------------------------

	class Animation : Packable {
	public:
		Animation(const String& name);
		virtual ~Animation();

		bool isValid() const;

		void setMasterWeight(float weight);
		void easeIn(float duration);
		void easeOut(float duration);
		void setLocalTime(float localtime);
		void setPlaybackSpeed(float speed);
		bool isAnimDone(float timeleft);

	public:
		hkaDefaultAnimationControl* m_controler;
		hkaAnimationBinding* m_animBinding;
	};

	//--------------------------------------------------------------------------
	// class Rig
	//--------------------------------------------------------------------------

	class Rig : public Packable {
	public:
		Rig(const String& name);
		Rig(Package* package);
		virtual ~Rig() {}

		int findBoneIndexByName(const char* BoneName);
		const char* findBoneNameByIndex(int BoneIndex);
		int getBoneCount();

		// implement renderRig
		Pose* createPose();

		bool isValid() const;

	public:
		hkaSkeleton* m_havokSkeleton;
	};

	//--------------------------------------------------------------------------
	// class Pose
	//--------------------------------------------------------------------------

	class Pose {
	public:
		Pose(Rig* rig);
		~Pose();

		bool isValid() const;

	public:
		hkaPose* m_havokPose;
	};

	//--------------------------------------------------------------------------
	// class Package
	//--------------------------------------------------------------------------

	class Package : public RefObject {
	public:
		class MeshData;
		typedef List<MeshData*>	MeshDataList;

		struct MaterialMap {
			hkxMaterial* m_hkMat;
			MaterialPtr m_axMat;
			Texture* m_lightMap;
		};
		typedef Sequence<MaterialMap*>	MaterialMaps;

		Package(const String& filename);
		~Package();

		// implement RefObject

		const BoundingBox& getBoundingBox();

		// meshes
		Primitives getPrimitives();
		void issueToQueue(QueuedEntity* qactor, QueuedScene* qscene);

		void initDynamicMeshes(MeshDataList& result);
		void clearDynamicMeshes(MeshDataList& result);
		void applyPose(Pose* pose, Primitives& prims);

		// animation
		hkaSkeleton* getSkeleton();

		int getAnimationCount();
		hkaAnimationBinding* getAnimation(int Index);

		// physics
		RigidBody* getRigidBody();
		hkpRigidBody* getRigidBodyHk() const;
		hkaRagdollInstance* getRagdoll() const;
		hkaSkeletonMapper* getMapper(hkaSkeletonMapper* current) const;

	protected:
		void generateStaticMesh();
		void generateMeshData();
		void findNodeTransform();
		void findNodeTransform_r(hkxNode* node, const hkMatrix4& parentTransform);
		void setMeshTransform(hkxMesh* mesh, const hkMatrix4& localTransform);
		const char* getMeshName(hkxMesh* mesh);
		hkaMeshBinding* findBinding(hkxMesh* mesh);
		const MaterialMap* findMaterialMap(hkxMaterial* hkmat);

	private:
		enum{ MaxLod = 8 };

		String m_name;
		hkLoader* m_loader;
		hkDataWorldDict* m_dataWorld;
		hkRootLevelContainer* m_root;
		hkpPhysicsData* m_physicsData;
		hkaAnimationContainer* m_animationContainer;
		hkxScene* m_sceneData;
		hkaRagdollInstance* m_ragdoll;
		hkaSkeletonMapper* m_mapper1;
		hkaSkeletonMapper* m_mapper2;

		bool m_isMeshDataGenerated;
		bool m_isStaticMeshGenerated;
		MeshDataList m_meshDatas;

		// material map
		MaterialMaps m_materialMaps;

		int m_numLod;

		bool m_isBboxGenerated;
		BoundingBox m_staticBbox;
	};

	//--------------------------------------------------------------------------
	// class SkeletalModel
	//--------------------------------------------------------------------------

	class AX_API SkeletalModel : public RenderEntity {
	public:
		SkeletalModel(const String& name);
		SkeletalModel(Package* package);
		virtual ~SkeletalModel();

		// implement Render::Actor
		virtual BoundingBox getLocalBoundingBox();
		virtual BoundingBox getBoundingBox();
		virtual Primitives getAllPrimitives();
		virtual Primitives getSelectionPrims(){ return getAllPrimitives(); }

		virtual void issueToQueue(QueuedScene* qscene);

		// SkeletalMesh
		virtual Rig* findRig() const;
		virtual void setPose(const Pose* pose, int linkBoneIndex = -1);

	protected:
		void applyPose();

	private:
		Package* m_package;
		Pose* m_pose;
		bool m_poseDirty;
		mutable BoundingBox m_poseBbox;
		bool m_isMeshDataInited;
		Package::MeshDataList m_mestDataList;
	};


	//--------------------------------------------------------------------------
	// class PackageManager
	//--------------------------------------------------------------------------

	class PackageManager {
	public:
		PackageManager();
		~PackageManager();

		Package* findPackage(const String& name);

	private:
		friend class Package;
		void removePackage(const String& name);

	private:
		typedef Dict<String,Package*,hash_pathname,equal_pathname> PackageDict;
		PackageDict m_packageDict;
	};


}} // namespace Axon::Physics


#endif // end guardian

