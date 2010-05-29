/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_WORLD_H
#define AX_RENDER_WORLD_H


AX_BEGIN_NAMESPACE

struct RenderScene;
struct RenderScene;
class RenderSystem;

class QuadNode {
public:
	float dist[2];
	QuadNode *parent;
	QuadNode *children[4];

	float size;
	BoundingBox bbox;
	IntrusiveList<RenderEntity> linkHead;
	int lastUpdateFrame;

	QuadNode() {
		dist[0] = dist[1] = 0;
		parent = 0;
		memset(children, 0, sizeof(children));
		size = 0;
		// bbox is initialized by world
		bbox = BoundingBox::EmptyBox;
		lastUpdateFrame = 0;
	}

	void expandBbox(const BoundingBox &rhs) {
		if (bbox.contains(rhs))
			return;

		bbox.unite(rhs);

		if (parent)
			parent->expandBbox(rhs);
	}

	void frameUpdated(int frameId) {
		if (lastUpdateFrame < frameId) {
			lastUpdateFrame = frameId;
			if (parent)
				parent->frameUpdated(frameId);
		}
	}
};

class AX_API RenderWorld
{
public:
	friend class RenderSystem;

	enum {
		HISTOGRAM_WIDTH = 32
	};

	RenderWorld();
	~RenderWorld();

	void initialize(int worldSize = 1024);
	void finalize();

	void addEntity(RenderEntity *entity);
	void removeEntity(RenderEntity *entity);

	OutdoorEnv *getOutdoorEnv() { return m_outdoorEnv; }

	int getVisFrameId() const { return m_visFrameId; }
	int getShadowFrameId() const { return m_shadowFrameId; }

protected:
	struct FrameData;

	void updateEntity(RenderEntity *entity);

	// add light, visarea, portal, occluder
	void addLight(RenderLight *light);

	void generateQuadNode();
	void generateChildNode_r(QuadNode *node);
	void linkEntity(RenderEntity *entity);
	void unlinkEntity(RenderEntity *entity);

	// for internal use
	void renderTo(RenderScene *queued);
	void renderTo(RenderScene *queued, QuadNode *node);

	// mark visible
	void markVisible_r(RenderScene *queued, QuadNode *node, Plane::Side parentSide);

	void updateExposure(RenderScene *qscene);

private:
	float m_worldSize;
	QuadNode *m_rootNode;
	RenderTerrain *m_terrain;
	OutdoorEnv *m_outdoorEnv;

	int m_histogram[HISTOGRAM_WIDTH];
	int m_histogramAccumed[HISTOGRAM_WIDTH];
	int m_curHistogramIndex;
	float m_lastExposure;

	bool m_updateShadowVis;

	int m_visFrameId;
	int m_shadowFrameId;
	Vector3 m_shadowDir;
	static int m_frameNum;

public:
	typedef IntrusiveList<RenderLight, &RenderLight::m_shadowLink> ShadowList;
	ShadowList m_shadowLink;
};

AX_END_NAMESPACE

#endif // AX_RENDER_WORLD_H

