/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_WORLD_H
#define AX_RENDER_WORLD_H


namespace Axon { namespace Render {

	struct Scene;
	struct QueuedScene;
	class System;

	class QuadNode {
	public:
		float dist[2];
		QuadNode* parent;
		QuadNode* children[4];

		float size;
		BoundingBox bbox;
		Link<Entity>		linkHead;
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

		void expandBbox(const BoundingBox& rhs) {
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

	class AX_API World {
	public:
		friend class ::Axon::Render::System;

		enum {
			HISTOGRAM_WIDTH = 32
		};

		World();
		~World();

		void initialize(int worldSize = 1024);
		void finalize();

		void addActor(Entity* actor);
		void removeActor(Entity* actor);
		void addTempActor(Entity* actor);

		OutdoorEnv* getOutdoorEnv() { return m_outdoorEnv; }

		int getVisFrameId() const { return m_visFrameId; }
		int getShadowFrameId() const { return m_shadowFrameId; }

	protected:
		struct FrameData;

		void updateActor(Entity* actor);

		// add light, visarea, portal, occluder
		void addLight(Light* light);

		void generateQuadNode();
		void generateChildNode_r(QuadNode* node);
		void linkActor(Entity* actor);
		void unlinkActor(Entity* actor);

		// for internal use
		void renderTo(QueuedScene* queued);
		void renderTo(QueuedScene* queued, QuadNode* node);

		// mark visible
		void markVisible_r(QueuedScene* queued, QuadNode* node, Plane::Side parentSide);

		void updateExposure(QueuedScene* qscene);

	private:
		float m_worldSize;
		QuadNode* m_rootNode;
		Terrain* m_terrain;
		OutdoorEnv* m_outdoorEnv;

		int m_histogram[HISTOGRAM_WIDTH];
		int m_histogramAccumed[HISTOGRAM_WIDTH];
		int m_curHistogramIndex;
		Query* m_histogramQuery;
		float m_lastExposure;

		bool m_updateShadowVis;

		int m_visFrameId;
		int m_shadowFrameId;
		Vector3 m_shadowDir;
		static int m_frameNum;

	public:
		Link<Light>		m_shadowLink;
	};

}} // namespace Axon::Render

#endif // AX_RENDER_WORLD_H

