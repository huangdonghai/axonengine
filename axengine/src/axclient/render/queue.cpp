/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon { namespace Render {

	QueuedLight* QueuedScene::addLight(Light* light) {
		if (numLights >= MAX_LIGHTS) {
			Errorf("MAX_LIGHTS exceeded");
			light->fillQueued(nullptr);
			return 0;
		}

		if (light->getLightType() != Light::kGlobal && !r_lightBuf->getBool()) {
			light->fillQueued(nullptr);
//			return 0;
		}

		QueuedLight* ql = g_renderQueue->allocQueuedLight();

		ql->queuedActor = light->getQueued();

		light->fillQueued(ql);

		lights[numLights++] = ql;

		if (light->getLightType() == Light::kGlobal) {
			globalLight = ql;
		}

		return ql;
	}

	QueuedEntity* QueuedScene::addActor(Entity* actor) {
		if (numActors >= MAX_ACTORS) {
			Errorf("MAX_ACTORS exceeded");
			return 0;
		}

		QueuedEntity* qa = g_renderQueue->allocQueuedActor();
		actor->setQueued(qa);

		queuedActors[numActors++] = qa;

		addHelperPrims(actor);

		if (actor->getKind() == Entity::kLight) {
			Light* light = dynamic_cast<Light*>(actor);
			AX_ASSERT(light);
			addLight(light);
		}

		if (actor->getKind() == Entity::kFog) {
			Fog* fog = dynamic_cast<Fog*>(actor);
			AX_ASSERT(fog);
			QueuedFog* qf = g_renderQueue->allocType<QueuedFog>();
			fog->fillQueuedFog(qf);
		}

		return qa;
	}

	Interaction* QueuedScene::addInteraction(QueuedEntity* actor, Primitive* prim, bool chain) {
		if (numInteractions == MAX_INTERACTIONS) {
			Errorf("MAX_INTERACTIONS exceeded");
			return 0;
		}

		Interaction* ia = g_renderQueue->allocInteraction();
		ia->primitive = prim;
		ia->qactor = actor;

		if (addInteraction(ia)) {
			if (chain) {
				if (ia->primitive)
					prim->interactionChain(ia, worldFrameId);

				if (ia->qactor)
					ia->qactor->interactionChain(ia, worldFrameId);
			}

			return ia;
		}

		return 0;
	}

	bool QueuedScene::addInteraction( Interaction* ia )
	{
		interactions[numInteractions++] = ia;
#if 0
		if (globalLight) {
			ia->addLight(globalLight);
		}
#endif
		Material* mat = ia->primitive->getMaterial();
		if (!mat) {
			return true;
		}

		if (!r_showPhysics->getBool() && mat->isPhysicsHelper()) {
			numInteractions--;
			return false;
		}

		Shader* shader = mat->getShaderTemplate();
		if (!shader) {
			return true;
		}

		switch (sceneType) {
		case ShadowGen:
			if (!shader->haveTechnique(Technique::ShadowGen)) {
				numInteractions--;
				return false;
			}
		}

		if (shader->haveTextureTarget() && sceneType != WorldMain) {
			numInteractions--;
			return false;
		}

#if 0
		ia->calcSort(isEyeInWater);

		ia->setupShader();
#endif
		return true;
	}

	void QueuedScene::addHelperPrims(Entity* actor)
	{
		if (!r_helper->getBool())
			return;

		const Primitives& prims = actor->getHelperPrims();

		AX_FOREACH(Primitive* prim, prims) {
			addHelperInteraction(actor->getQueued(), prim);
		}
	}

	void QueuedScene::addHelperInteraction(QueuedEntity* qactor, Primitive* prim)
	{
		if (numDebugInteractions >= MAX_DEBUG_INTERACTIONS) {
			Errorf("MAX_DEBUG_INTERACTIONS exceeded");
			return;
		}

		Interaction* ia = g_renderQueue->allocInteraction();
		ia->primitive = prim;
		ia->qactor = qactor;
		debugInteractions[numDebugInteractions++] = ia;
	}

	inline static bool Lesser(const Interaction* a, const Interaction* b) {
		return a->sortkey < b->sortkey;
	}

	inline static bool LesserLight(const QueuedLight* a, const QueuedLight* b) {
		return a->preQueued->getVisSize() < b->preQueued->getVisSize();
	}

	void QueuedScene::finalProcess()
	{
		findInstance();

		for (int i = 0; i < numInteractions; i++) {
			interactions[i]->calcSort(isEyeInWater);
		}

		sortInteractions();

		checkLights();
	}

	void QueuedScene::findInstance()
	{
		if (r_geoInstancing->getInteger() < 2)
			return;

		int oldNumInteractions = numInteractions;
		Interaction** oldInteractions = g_renderQueue->allocType<Interaction*>(numInteractions);
		memcpy(oldInteractions, interactions, numInteractions*sizeof(oldInteractions[0]));

		numInteractions = 0;
		int numInstancedIA = 0;

		for (int i = 0; i < oldNumInteractions; i++) {
			Interaction* ia = oldInteractions[i];

			if (ia->instanced) {
				continue;
			}

			Primitive* prim = ia->primitive;
			Interaction* head = prim->getHeadInteraction();

			if (!head->primNext) {
				interactions[numInteractions++] = ia;
				continue;
			}

			numInstancedIA++;

			GeoInstance* geoins = new GeoInstance(Primitive::OneFrame);
			geoins->setInstanced(head->primitive);

			for ( ia = head; ia; ia = ia->primNext) {
				if (!ia->qactor) {
					interactions[numInteractions++] = ia;
					continue;
				}

				geoins->addInstance(ia->qactor->m_matrix, ia->qactor->instanceParam);
				ia->instanced = true;
			}

			addInteraction(0, geoins, false);
		}

		g_statistic->addValue(stat_instancedIA, numInstancedIA);
	}

	void QueuedScene::sortInteractions()
	{
		std::sort(&interactions[0], &interactions[numInteractions], Lesser);
	}

	void QueuedScene::checkLights()
	{
		// check lights shadow map gen
		if (sceneType != WorldMain)
			return;

		if (!r_shadow->getInteger())
			return;

		if (!numLights)
			return;

		// copy shadowed light
		QueuedLight** shadowed = g_renderQueue->allocType<QueuedLight*>(numLights);

		int numShadowed = 0;
		for (int i = 0; i < numLights; i++) {}

		for (int i = 0; i < numLights; i++) {
			bool v = lights[i]->preQueued->checkShadow(this);
			if (!v)
				continue;

			shadowed[numShadowed++] = lights[i];
		}

		if (!numShadowed)
			return;

		// sort by visSize
		std::sort(&shadowed[0], &shadowed[numShadowed], LesserLight);
		World* world = shadowed[0]->preQueued->getWorld();

		// add to world's shadow link
		for (int i = 0; i < numShadowed; i++) {
			shadowed[i]->preQueued->linkShadow();
		}

		// check shadow map memory used
		int totalUsed = 0;
		int frameUsed = 0;
		int frameFreed = 0;

		int desired = r_shadowPoolSize->getInteger();
		desired = Math::clamp(desired, 8, 128) * 1024 * 1024;

		Light* light = world->m_shadowLink.getNext();
		while (light) {
			if (totalUsed > desired) {
				frameFreed += light->getShadowMemoryUsed();
				light->freeShadowMap();
				light = light->unlinkShadow();
				continue;
			}
			totalUsed += light->getShadowMemoryUsed();
			if (light->genShadowMap(this)) {
				frameUsed += light->getShadowMemoryUsed();
			}
			light = light->m_shadowLink.getNext();
		}

		g_statistic->setValue(stat_shadowPoolSize, totalUsed);
		g_statistic->setValue(stat_shadowPoolUsed, frameUsed);
		g_statistic->setValue(stat_shadowPoolFreed, frameFreed);
	}

	QueuedScene* QueuedScene::addSubScene()
	{
		if (numSubScenes >= MAX_SUB_SCENES) {
			return 0;
		}

		QueuedScene* result = g_renderQueue->allocQueuedScene();
		subScenes[numSubScenes++] = result;
		return result;
	}

	Queue::Queue()
	{}

	Queue::~Queue() {
	}

	void Queue::initialize() {
	//	m_queuedScenes = NULL;
		m_sceneCount = 0;
		m_stack = new MemoryStack();
		m_providingEvent = new SyncEvent();
		m_consumingEvent = new SyncEvent();
		m_cacheEndEvent = new SyncEvent();

		m_target = nullptr;

		// set can providing
		m_providingEvent->setEvent();
	}

	void Queue::finalize() {
		beginProviding();
		delete(m_cacheEndEvent);
		delete(m_consumingEvent);
		delete(m_providingEvent);
		delete(m_stack);
	}

	void Queue::beginProviding() {
		m_providingEvent->lock();
	}

	MemoryStack* Queue::getMemoryStack() {
		return m_stack;
	}

	void Queue::setTarget(Target* target) {
		m_target = target;
	}


	QueuedScene* Queue::allocQueuedScene() {
		QueuedScene* queued_view = new(m_stack) QueuedScene;

		return queued_view;
	}

	void Queue::addScene(QueuedScene* scene)
	{
		if (m_sceneCount >= MAX_VIEW) {
			Errorf("Queue::allocQueuedScene: MAX_VIEW exceeds");
			return;
		}

		m_queuedScenes[m_sceneCount++] = scene;
	}

	Interaction* Queue::allocInteraction() {
		return new(m_stack) Interaction;
	}

	Interaction** Queue::allocInteractionPointer(int num) {
		return new(m_stack) Interaction*[num];
	}


	QueuedLight* Queue::allocQueuedLight() {
		return new(m_stack) QueuedLight;
	}

	QueuedEntity* Queue::allocQueuedActor(int num) {
		return new(m_stack) QueuedEntity[num];
	}

	int* Queue::allocPrimitives(int num) {
		return new(m_stack) int[num];
	}

	void Queue::endProviding() {
		m_providingEvent->resetEvent();
		m_consumingEvent->setEvent();

		// wait render driver cache end
		m_cacheEndEvent->lock();
	}

	void Queue::beginConsuming() {
		m_consumingEvent->lock();
	}

	void Queue::setCacheEnd() {
		m_cacheEndEvent->setEvent();
	}


	QueuedScene* Queue::getScene(int index) {
		AX_ASSERT(index >= 0 && index < m_sceneCount);

		return m_queuedScenes[index];
	}

	void Queue::clear() {
		m_stack->clear();
		m_sceneCount = 0;
	}

	void Queue::endConsuming() {
		m_consumingEvent->resetEvent();
		m_providingEvent->setEvent();
	}

#if 0
	int Queue::allocQueryId() {
		if (m_curQueryId == MAX_QUERIES) {
			Errorf("Queue::allocQueryId: MAX_QUERIES exceeded");
		}
		return m_curQueryId++;
	}

	void Queue::setQueryResult(int num, int* result) {
		memcpy(m_lastFrameResult, result, num * sizeof(int));
	}

	int Queue::getQueryResult(int id) {
		QueryResults::iterator it = m_queryResults[m_readOffset].find(id);

		if (it == m_queryResults[m_readOffset].end()) {
			Debugf("Queue::getQueryResult: not queried");
			return -1;
		}
		return it->second;
	}
#endif

}} // namespace Axon::Render

