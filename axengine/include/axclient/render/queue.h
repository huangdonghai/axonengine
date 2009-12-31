/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_QUEUE_H
#define AX_RENDER_QUEUE_H

AX_BEGIN_NAMESPACE

	struct RenderScene {
		RenderCamera camera;
		RenderWorld *world;

		ActorSeq actors;
		Primitives primitives;

		// overlay
		Primitives overlays;
	};

	struct BBoxQuery {
		int queryId;
		BoundingBox bbox;
	};

	typedef shared_ptr<RenderScene>		ScenePtr;
	typedef Sequence<ScenePtr>		SceneSeq;

	struct AX_API QueuedScene {
		enum {
			MAX_DEBUG_INTERACTIONS = 1024,
			MAX_SUB_SCENES = 32,
			MAX_LIGHTS = 512,
			MAX_ACTORS = 2048*4,
			MAX_INTERACTIONS = 2048*16,
		};

		enum SceneType {
			Default, WorldMain, ShadowGen, Reflection, RenderToTexture
		};

		SceneType sceneType;

		int worldFrameId;

		QueuedLight *sourceLight;			// if is shadowGen scene, this is light cast this shadow
		int splitIndex;				// if is csm split, this is order index
		bool rendered;

		// some global shader uniform parameters
		Matrix4 windMatrices[RenderWind::NUM_WIND_MATRIXES];
		Vector4 leafAngles[RenderWind::NUM_LEAF_ANGLES];

		int m_histogramIndex;		// which histogram need to query
		int m_histogramQueryId;

		float exposure;

		int numSubScenes;
		QueuedScene *subScenes[MAX_SUB_SCENES];

		RenderScene *source;
		RenderCamera camera;
		RenderTarget *target;		// main scene maybe have a render target

		int numLights;
		QueuedLight *lights[MAX_LIGHTS];
		QueuedLight *globalLight;

		QueuedFog *globalFog;
		QueuedFog *waterFog;
		Vector4 clearColor;				// clear color
		bool isEyeInWater;

		int numActors;
		QueuedEntity *queuedActors[MAX_ACTORS];

		int numPrimitives;
		int *primIds;

		int numInteractions;
		Interaction *interactions[MAX_INTERACTIONS];

		int numDebugInteractions;
		Interaction *debugInteractions[MAX_DEBUG_INTERACTIONS];

		int numOverlayPrimitives;
		int *overlayPrimIds;

		QueuedScene *addSubScene();
		QueuedLight *addLight(RenderLight *light);
		QueuedEntity *addActor(RenderEntity *actor);
		Interaction *addInteraction(QueuedEntity *actor, Primitive *prim, bool chain = true);
		void addHelperInteraction(QueuedEntity *qactor, Primitive *prim);

		bool addLight(QueuedLight *light);
		bool addActor(QueuedEntity *actor);

		void finalProcess();

		bool isLastCsmSplits() const;

	protected:
		void checkLights();
		bool addInteraction(Interaction *ia);
		void addHelperPrims(RenderEntity *actor);

		void findInstance();
		void sortInteractions();
	};


	inline bool QueuedScene::addLight(QueuedLight *light) {
		if (numLights >= MAX_LIGHTS) {
			return false;
		}

		lights[numLights++] = light;
		return true;
	}

	inline bool QueuedScene::addActor(QueuedEntity *actor) {
		if (numActors >= MAX_ACTORS) {
			return false;
		}

		queuedActors[numActors++] = actor;
		return true;
	}

	inline bool QueuedScene::isLastCsmSplits() const
	{
		if (sceneType != ShadowGen)
			return false;

		if (!sourceLight)
			return false;

		if (sourceLight->type != RenderLight::kGlobal)
			return false;

		if (splitIndex != sourceLight->shadowInfo->numSplitCamera-1)
			return false;

		return true;
	}



	typedef Sequence<QueuedScene*>	QueuedSceneSeq;

	class AX_API RenderQueue {
	public:
		enum {
			QUERY_FRAME_STACK_DEPTH = 2,
			MAX_QUERIES = 4096,
			MAX_VIEW = 16
		};
		RenderQueue();
		~RenderQueue();

		void initialize();
		void finalize();
		RenderTarget *getTarget();

		// for providing thread
		void beginProviding();
		MemoryStack *getMemoryStack();
		void setTarget(RenderTarget *target);
		QueuedScene *allocQueuedScene();
		void addScene(QueuedScene *scene);
		Interaction *allocInteraction();
		Interaction** allocInteractionPointer(int num);
		QueuedLight *allocQueuedLight();
		QueuedEntity *allocQueuedActor(int num = 1);
		int *allocPrimitives(int num);
		void endProviding();

		template< class T >
		T *allocType(int num = 1);

		// for consuming thread
		void beginConsuming();
		void setCacheEnd();
		int getSceneCount() { return m_sceneCount; }
		QueuedScene *getScene(int index);
		void clear();
		void endConsuming();

	private:
		MemoryStack *m_stack;
		RenderTarget *m_target;
		int m_sceneCount;
		QueuedScene *m_queuedScenes[MAX_VIEW];
		SyncEvent *m_providingEvent;
		SyncEvent *m_consumingEvent;
		SyncEvent *m_cacheEndEvent;
	};

	template< class T >
	T *RenderQueue::allocType(int num) {
		if (num == 1) {
			return new(m_stack) T;
		} else {
			return new(m_stack) T[num];
		}
	}

	inline RenderTarget *RenderQueue::getTarget() {
		return m_target;
	}


AX_END_NAMESPACE

#endif // AX_RENDER_QUEUE_H
