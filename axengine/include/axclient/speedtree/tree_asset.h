/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CLIENT_SPEEDTREE_ASSET_H
#define AX_CLIENT_SPEEDTREE_ASSET_H

// forward declaration external library's type
class CSpeedTreeRT;

AX_BEGIN_NAMESPACE

class TreeEntity;
class TreeManager;

// NOTE: TreeAsset not managed by AssetManager, but managed by TreeManager
class TreeAsset : public RefObject {
public:
	enum {
		MAX_LODS = 6
	};

	TreeAsset(TreeManager *forest);
	~TreeAsset();

	bool load(const std::string &filename, int seed);
	std::string getKey() const;

	BoundingBox getBoundingBox() const;
	Primitives getAllPrimitives(float lod) const;

	void addActor(TreeEntity *actor);
	void removeActor(TreeEntity *actor);

	void issueToQueue(TreeEntity *actor, RenderScene *qscene);
#if 0
	void issueToQueueInstancing(QueuedScene *qscene);
#endif
	static std::string genKey(const std::string &filename, int seed);

protected:
	void loadMaterials();
	void buildPrimitives();
	void buildBranch();
	void buildFrond();
	void buildLeafCard();
	void buildLeafMesh();
//		void setMaterialColor(Material *mat, const float *f, float scaler);

private:
	TreeManager *m_manager;
	CSpeedTreeRT *m_treeRt;
	std::string m_filename;
	std::string m_filepath;
	int m_seed;
	std::string m_key;

	// primitives
	int m_numBranchLods;
	Primitive *m_branchPrims[MAX_LODS];

	int m_numFrondLods;
	Primitive *m_frondPrims[MAX_LODS];

	int m_numLeafLods;
	Primitive *m_leafPrims[MAX_LODS];

	// material
	Material *m_branchMat;
	Material *m_frondMat;
	Material *m_leafCardMat;
	Material *m_leafMeshMat;
	
	// actors use this asset
	TreeDict m_treeActors;
};

AX_DECLARE_REFPTR(TreeAsset);

inline std::string TreeAsset::genKey(const std::string &filename, int seed) {
	std::string result;
	StringUtil::sprintf(result, "%s_%d", filename.c_str(), seed);
	return result;
}

inline void TreeAsset::addActor(TreeEntity *actor) {
	m_treeActors.insert(actor);
}

inline void TreeAsset::removeActor(TreeEntity *actor) {
	m_treeActors.erase(actor);
}


AX_END_NAMESPACE

#endif // end guardian

