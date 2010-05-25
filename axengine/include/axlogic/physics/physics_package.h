/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PHYSICS_FILE_H
#define AX_PHYSICS_FILE_H

AX_BEGIN_NAMESPACE

class HavokRig;
class HavokPose;
class HavokAnimation;
class HavokAnimator;

class HavokPackage;
AX_DECLARE_REFPTR(HavokPackage);

class PhysicsRagdoll;

//--------------------------------------------------------------------------
// class HavokPackable
//--------------------------------------------------------------------------

class HavokPackable {
public:
	HavokPackable() {}
	HavokPackable(const HavokPackagePtr &package) : m_package(package) {}
	virtual ~HavokPackable() {}

protected:
	HavokPackagePtr m_package;
};

//--------------------------------------------------------------------------
// class HavokAnimator
//--------------------------------------------------------------------------

class HavokAnimator {
public:
	HavokAnimator(HavokRig *rig);
	~HavokAnimator();

	void addAnimation(HavokAnimation *anim);
	void removeAnimation(HavokAnimation *anim);
	void removeAllAnimation();
	void renderToPose(HavokPose *pose);
	void step(int frametime);

	inline HavokRig *getRig() { return m_rig; }

public:
	HavokRig *m_rig;
	hkaAnimatedSkeleton *m_animator;
	List<HavokAnimation*>		m_animations;
};

//--------------------------------------------------------------------------
// class HavokAnimation
//--------------------------------------------------------------------------

class HavokAnimation : HavokPackable {
public:
	HavokAnimation(const String &name);
	virtual ~HavokAnimation();

	bool isValid() const;

	void setMasterWeight(float weight);
	void easeIn(float duration);
	void easeOut(float duration);
	void setLocalTime(float localtime);
	void setPlaybackSpeed(float speed);
	bool isAnimDone(float timeleft);

public:
	hkaDefaultAnimationControl *m_controler;
	hkaAnimationBinding *m_animBinding;
};

//--------------------------------------------------------------------------
// class HavokRig
//--------------------------------------------------------------------------

class HavokRig : public HavokPackable {
public:
	HavokRig(const String &name);
	HavokRig(const HavokPackagePtr &package);
	virtual ~HavokRig() {}

	int findBoneIndexByName(const char *BoneName);
	const char *findBoneNameByIndex(int BoneIndex);
	int getBoneCount();

	// implement renderRig
	HavokPose *createPose();

	bool isValid() const;

public:
	hkaSkeleton *m_havokSkeleton;
};

//--------------------------------------------------------------------------
// class HavokPose
//--------------------------------------------------------------------------

class HavokPose {
public:
	HavokPose(HavokRig *rig);
	~HavokPose();

	bool isValid() const;

public:
	hkaPose *m_havokPose;
};

//--------------------------------------------------------------------------
// class HavokPackage
//--------------------------------------------------------------------------

class HavokPackage : public RefObject {
public:
	class MeshData;
	typedef List<MeshData*>	MeshDataList;

	struct MaterialMap {
		hkxMaterial *m_hkMat;
		Material *m_axMat;
		Texture *m_lightMap;
	};
	typedef Sequence<MaterialMap*>	MaterialMaps;

	HavokPackage(const String &filename);
	~HavokPackage();

	// implement RefObject

	const BoundingBox &getBoundingBox();

	// meshes
	Primitives getPrimitives();
	void issueToQueue(RenderEntity *qactor, QueuedScene *qscene);

	void initDynamicMeshes(MeshDataList &result);
	void clearDynamicMeshes(MeshDataList &result);
	void applyPose(HavokPose *pose, Primitives &prims);

	// animation
	hkaSkeleton *getSkeleton();

	int getAnimationCount();
	hkaAnimationBinding *getAnimation(int Index);

	// physics
	PhysicsRigid *getRigidBody();
	hkpRigidBody *getRigidBodyHk() const;
	hkaRagdollInstance *getRagdoll() const;
	hkaSkeletonMapper *getMapper(hkaSkeletonMapper *current) const;

protected:
	void generateStaticMesh();
	void generateMeshData();
	void findNodeTransform();
	void findNodeTransform_r(hkxNode *node, const hkMatrix4 &parentTransform);
	void setMeshTransform(hkxMesh *mesh, const hkMatrix4 &localTransform);
	const char *getMeshName(hkxMesh *mesh);
	hkaMeshBinding *findBinding(hkxMesh *mesh);
	const MaterialMap *findMaterialMap(hkxMaterial *hkmat);

private:
	enum{ MaxLod = 8 };

	String m_name;
	hkLoader *m_loader;
	hkDataWorldDict *m_dataWorld;
	hkRootLevelContainer *m_root;
	hkpPhysicsData *m_physicsData;
	hkaAnimationContainer *m_animationContainer;
	hkxScene *m_sceneData;
	hkaRagdollInstance *m_ragdoll;
	hkaSkeletonMapper *m_mapper1;
	hkaSkeletonMapper *m_mapper2;

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
// class HavokModel
//--------------------------------------------------------------------------

class AX_API HavokModel : public RenderEntity {
public:
	HavokModel(const String &name);
	HavokModel(HavokPackage *package);
	virtual ~HavokModel();

	// implement Render::Actor
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual Primitives getHitTestPrims();

	virtual void issueToQueue(QueuedScene *qscene);

	// SkeletalMesh
	virtual HavokRig *findRig() const;
	virtual void setPose(const HavokPose *pose, int linkBoneIndex = -1);

protected:
	void applyPose();

private:
	HavokPackagePtr m_package;
	HavokPose *m_pose;
	bool m_poseDirty;
	mutable BoundingBox m_poseBbox;
	bool m_isMeshDataInited;
	HavokPackage::MeshDataList m_mestDataList;
};


//--------------------------------------------------------------------------
// class HavokPackageManager
//--------------------------------------------------------------------------

class HavokPackageManager {
public:
	HavokPackageManager();
	~HavokPackageManager();

	HavokPackagePtr findPackage(const String &name);

private:
	friend class HavokPackage;
	void removePackage(const String &name);

private:
	typedef Dict<String,HavokPackage*,hash_pathname,equal_pathname> PackageDict;
	PackageDict m_packageDict;
};


AX_END_NAMESPACE


#endif // end guardian

