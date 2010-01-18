/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_ACTOR_H
#define AX_RENDER_ACTOR_H

AX_BEGIN_NAMESPACE

// forward declaration
struct QueuedScene;

struct QueuedEntity {
	enum {
		MAX_TARGETS = 8
	};

	Matrix matrix;
	Vector4 instanceParam;
	int flags;			// Actor::Flag

	float distance;

	// used by interaction
	int m_chainId;
	Interaction *m_headInteraction;

	void interactionChain(Interaction *last, int chainId);
};

struct QueuedLight;

inline void QueuedEntity::interactionChain(Interaction *last, int chainId)
{
	if (m_chainId != chainId) {
		m_headInteraction = last;
		m_chainId = chainId;
		return;
	}

	last->actorNext = m_headInteraction;
	m_headInteraction = last;
}

class QuadNode;

class AX_API RenderEntity
{
	friend class RenderWorld;
	friend class QuadNode;

public:
	enum Flag {
		DepthHack = 1, OutsideOnly = 2
	};

	enum Kind {
		kNone, kModel, kSpeedTree, kAnchor, kEffect, kUserInterface,
		kLight, kFog, kVisArea, kPortal, kOccluder, kTerrain, kOutdoorEnv
	};

	RenderEntity(Kind type );
	virtual ~RenderEntity();

	Kind getKind() const { return m_kind; }

	// read and writable
	const Vector3 &getOrigin() const;
	void setOrigin(const Vector3 &origin);
	const Matrix3 &getAxis() const;
	void setAxis(const Angles &angles);
	void setAxis(const Angles &angles, float scale);
	const Matrix &getMatrix() const;
	void setMatrix(const Matrix &mat);

	void setInstanceColor(const Vector3 &color);
	Vector3 getInstanceColor() const;

	bool isInWorld() const { return m_world != 0; }
	void refresh();

	int getFlags() const;
	void setFlags(int flags);
	void addFlags(int flags);
	bool isFlagSet(Flag flag);

	bool isVisable() const;
	bool isLight() const { return m_kind == kLight; }
	float getVisSize() const { return m_visSize; }

	// queued
	void setQueued(QueuedEntity *queued);
	QueuedEntity *getQueued() const;

	// read only
	Matrix4 getModelMatrix() const;

	RenderWorld *getWorld() const { return m_world; }
	void setWorld(RenderWorld *world) { m_world = world; }

	// helper prims
	void clearHelperPrims();
	void addHelperPrim(Primitive *prim);
	const Primitives &getHelperPrims() const;

	// virtual interface
	virtual BoundingBox getLocalBoundingBox() = 0;
	virtual BoundingBox getBoundingBox() = 0;
	virtual Primitives getHitTestPrims() { return Primitives(); }
	virtual void frameUpdate(QueuedScene *qscene);
	virtual void issueToQueue(QueuedScene *qscene) {}

protected:
	// only called by RenderWorld
	void update(QueuedScene *qscene, Plane::Side side);
	void updateCsm(QueuedScene *qscene, Plane::Side side);
	bool isCsmCulled() const;
	void calculateLod(QueuedScene *qscene);

protected:
	const Kind m_kind;
	Matrix m_affineMat;
	int m_flags;
	Vector4 m_instanceParam;

	QueuedEntity *m_queued;

	int m_visFrameId;
	BoundingBox m_linkedBbox;
	float m_linkedExtends;
	float m_distance;
	float m_lodRatio;
	float m_lod;
	float m_visSize;
	bool m_viewDistCulled;
	bool m_queryCulled;

	// linked info
	IntrusiveLink<RenderEntity> m_link;
	QuadNode *m_linkedNode;
	int m_linkedFrame;

	// helper primitives
	Primitives m_helperPrims;

	// used by world
	RenderWorld *m_world;
	Query *m_visQuery;
	Query *m_shadowQuery;
	Plane::Side m_cullSide;
};

inline void RenderEntity::clearHelperPrims()
{
	m_helperPrims.clear();
}

inline void RenderEntity::addHelperPrim(Primitive *prim)
{
	m_helperPrims.push_back(prim);
}

inline const Primitives &RenderEntity::getHelperPrims() const
{
	return m_helperPrims;
}


//--------------------------------------------------------------------------
// class IEntityManager
//--------------------------------------------------------------------------

class AX_API IEntityManager {
public:
	virtual bool isSupportExt(const String &ext) const = 0;
	virtual RenderEntity *create(const String &name, intptr_t arg = 0) = 0;
	virtual void updateForFrame(QueuedScene *qscene ) {}
	virtual void issueToQueue(QueuedScene *qscene) {}
};

AX_END_NAMESPACE

#endif // AX_RENDER_ACTOR_H
