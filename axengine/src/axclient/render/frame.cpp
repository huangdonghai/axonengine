/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

void RenderScene::addLight(RenderLight *light)
{
	AX_RELEASE_ASSERT(numLights < MAX_LIGHTS);
	lights[numLights++] = light;

	if (light->isGlobal())
		globalLight = light;
}

void RenderScene::addEntity(RenderEntity *entity)
{
	AX_RELEASE_ASSERT(numEntities < MAX_ENTITIES);

	entities[numEntities++] = entity;

	addHelperPrims(entity);

	if (entity->getKind() == RenderEntity::kLight) {
		RenderLight *light = dynamic_cast<RenderLight*>(entity);
		AX_ASSERT(light);
		addLight(light);
	}
#if 0
	if (entity->getKind() == RenderEntity::kFog) {
		RenderFog *fog = dynamic_cast<RenderFog*>(entity);
		AX_ASSERT(fog);
		QueuedFog *qf = g_renderQueue->allocType<QueuedFog>();
		fog->fillQueuedFog(qf);
	}
#endif
}

Interaction *RenderScene::addInteraction(RenderEntity *entity, Primitive *prim, bool chain)
{
	AX_RELEASE_ASSERT(numInteractions < MAX_INTERACTIONS);

	Interaction *ia = g_renderFrame->allocInteraction();
	ia->primitive = prim;
	if (entity)
		ia->entity = entity;
	else
		ia->entity = 0;

	if (addInteraction(ia)) {
		if (chain) {
			if (ia->primitive)
				prim->interactionChain(ia, worldFrameId);

			if (ia->entity)
				ia->entity->interactionChain(ia, worldFrameId);
		}

		return ia;
	}

	return 0;
}

bool RenderScene::addInteraction(Interaction *ia)
{
	interactions[numInteractions++] = ia;

	Material *mat = ia->primitive->getMaterial();
	if (!mat) {
		return true;
	}

	if (!r_showPhysics.getBool() && mat->isPhysicsHelper()) {
		numInteractions--;
		return false;
	}

	const ShaderInfo *shader = mat->getShaderInfo();
	if (!shader) {
		return true;
	}

	switch (sceneType) {
	case ShadowGen:
		if (!shader->m_haveTechnique[Technique::ShadowGen]) {
			numInteractions--;
			return false;
		}
	}

	if (shader->m_needReflection && sceneType != WorldMain) {
		numInteractions--;
		return false;
	}

	return true;
}

void RenderScene::addHelperPrims(RenderEntity *entity)
{
	if (!r_helper.getBool())
		return;

	const Primitives &prims = entity->getHelperPrims();

	AX_FOREACH(Primitive *prim, prims) {
		addHelperInteraction(entity, prim);
	}
}

void RenderScene::addHelperInteraction(RenderEntity *entity, Primitive *prim)
{
	AX_RELEASE_ASSERT(numDebugInteractions < MAX_DEBUG_INTERACTIONS);

	Interaction *ia = g_renderFrame->allocInteraction();
	ia->primitive = prim;
	ia->entity = entity;
	debugInteractions[numDebugInteractions++] = ia;
}

inline static bool Lesser(const Interaction *a, const Interaction *b)
{
	return a->sortkey < b->sortkey;
}

inline static bool LesserLight(const RenderLight *a, const RenderLight *b)
{
	return a->getVisSize() < b->getVisSize();
}

void RenderScene::finalProcess()
{
	findInstance();

	for (int i = 0; i < numInteractions; i++) {
		interactions[i]->calcSort(isEyeInWater);
	}

	sortInteractions();

	checkLights();
}

void RenderScene::findInstance()
{
	if (r_geoInstancing.getInteger() < 2)
		return;

	int oldNumInteractions = numInteractions;
	Interaction** oldInteractions = g_renderFrame->allocType<Interaction*>(numInteractions);
	memcpy(oldInteractions, interactions, numInteractions*sizeof(oldInteractions[0]));

	numInteractions = 0;
	int numInstancedIA = 0;

	for (int i = 0; i < oldNumInteractions; i++) {
		Interaction *ia = oldInteractions[i];

		if (ia->instanced) {
			continue;
		}

		Primitive *prim = ia->primitive;
		Interaction *head = prim->getHeadInteraction();
		int numChained = prim->getNumChainedInteractions();

		if (numChained < 2) {
			interactions[numInteractions++] = ia;
			continue;
		}

		numInstancedIA++;

		InstancePrim *geoins = new InstancePrim(Primitive::HintFrame);
		geoins->init(head->primitive, numChained);

		int index = 0;
		for (ia = head; ia; ia = ia->primNext, index++) {
			if (!ia->entity) {
#if 0
				interactions[numInteractions++] = ia;
#else
				geoins->setInstance(index, Matrix::getIdentity(), Vector4::One);
#endif
				continue;
			}

			geoins->setInstance(index, ia->entity->getMatrix(), ia->entity->getInstanceParam());
			ia->instanced = true;
		}

		addInteraction(0, geoins, false);
	}

	stat_instancedIA.add(numInstancedIA);
}

void RenderScene::sortInteractions()
{
	std::sort(&interactions[0], &interactions[numInteractions], Lesser);
}

void RenderScene::checkLights()
{
	// check lights shadow map gen
	if (sceneType != WorldMain)
		return;

	if (!r_shadow.getInteger())
		return;

	if (!numLights)
		return;

	// copy shadowed light
	RenderLight **shadowed = g_renderFrame->allocType<RenderLight*>(numLights);
	int numShadowed = 0;
	for (int i = 0; i < numLights; i++) {}

	for (int i = 0; i < numLights; i++) {
		bool v = lights[i]->checkShadow(this);
		if (!v)
			continue;

		shadowed[numShadowed++] = lights[i];
	}

	if (!numShadowed)
		return;

	// sort by visSize
	std::sort(&shadowed[0], &shadowed[numShadowed], LesserLight);
	RenderWorld *world = shadowed[0]->getWorld();

	// add to world's shadow link
	for (int i = 0; i < numShadowed; i++) {
		world->m_shadowLink.push_front(shadowed[i]);
	}

	// check shadow map memory used
	int totalUsed = 0;
	int frameUsed = 0;
	int frameFreed = 0;

	int desired = r_shadowPoolSize.getInteger();
	desired = Math::clamp(desired, 8, 128) * 1024 * 1024;

	RenderWorld::ShadowList::iterator it = world->m_shadowLink.begin();
	while (it != world->m_shadowLink.end()) {
		RenderLight *light = &*it;
		if (totalUsed > desired) {
			frameFreed += light->getShadowMemoryUsed();
			light->freeShadowMap();
			it = world->m_shadowLink.erase(it);
			continue;
		}
		totalUsed += light->getShadowMemoryUsed();
		if (light->genShadowMap(this)) {
			frameUsed += light->getShadowMemoryUsed();
		}
		++it;
	}

	stat_shadowPoolSize.setInt(totalUsed);
	stat_shadowPoolUsed.setInt(frameUsed);
	stat_shadowPoolFreed.setInt(frameFreed);
}

RenderScene *RenderScene::addSubScene()
{
	AX_RELEASE_ASSERT(numSubScenes < MAX_SUB_SCENES);

	RenderScene *result = g_renderFrame->allocScene();
	subScenes[numSubScenes++] = result;
	return result;
}

void RenderScene::addPrimitive( Primitive *prim )
{
	AX_RELEASE_ASSERT(numPrimitives < MAX_PRIMITIVES);

	primtives[numPrimitives++] = prim;
}

void RenderScene::addOverlayPrimitive( Primitive *prim )
{
	AX_RELEASE_ASSERT(numOverlayPrimitives < MAX_OVERLAY_PRIMITIVES);

	overlayPrimitives[numOverlayPrimitives++] = prim;
}

RenderFrame::RenderFrame()
{
	m_sceneCount = 0;
	m_stack = new MemoryStack();
	m_target = nullptr;
}

RenderFrame::~RenderFrame()
{
	delete(m_stack);
}


MemoryStack *RenderFrame::getMemoryStack()
{
	return m_stack;
}

void RenderFrame::setTarget(RenderTarget *target)
{
	m_target = target;
}


RenderScene *RenderFrame::allocScene()
{
	RenderScene *queued_view = new(m_stack) RenderScene;

	return queued_view;
}

void RenderFrame::addScene(RenderScene *scene)
{
	AX_RELEASE_ASSERT(m_sceneCount < MAX_VIEW);

	m_queuedScenes[m_sceneCount++] = scene;
}

Interaction *RenderFrame::allocInteraction()
{
	return new(m_stack) Interaction;
}

RenderScene *RenderFrame::getScene(int index)
{
	AX_ASSERT(index >= 0 && index < m_sceneCount);

	return m_queuedScenes[index];
}

void RenderFrame::clear()
{
	m_stack->clear();
	m_sceneCount = 0;
}

AX_END_NAMESPACE

