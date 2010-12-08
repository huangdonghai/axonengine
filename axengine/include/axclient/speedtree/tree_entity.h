/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CLIENT_SPEEDTREE_ACTOR_H
#define AX_CLIENT_SPEEDTREE_ACTOR_H

AX_BEGIN_NAMESPACE

class TreeAsset;

class AX_API TreeEntity : public RenderEntity {
public:
	friend class TreeAsset;

	enum {
		LeafAngleScalarX = 0,
		LeafAngleScalarY = 1,
		WindMatrixOffset = 2,
		InstanceScale = 3
	};

	TreeEntity(const std::string &filename, int seed = 1);
	~TreeEntity();

	void setWindMatrixOffset(float fOffset) { m_instanceParam[WindMatrixOffset] = float(int(10.0f * fOffset)); }

	// generate render primitive
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual Kind getType() const { return kSpeedTree; }
	virtual Primitives getHitTestPrims();

	// new interface
	virtual void frameUpdate( RenderScene *qscene );
	virtual void issueToQueue(RenderScene *qscene);


private:
	RefPtr<TreeAsset> m_treeAsset;
	Vector4 m_instanceParam;
};

typedef DictSet<TreeEntity*> TreeDict;

AX_END_NAMESPACE

#endif // end guardian

