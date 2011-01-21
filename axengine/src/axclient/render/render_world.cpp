/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

static bool s_drawTerrain = true;
static bool s_drawActor = true;

enum {
	MininumNodeSize = 32
};

int RenderWorld::m_frameNum = 0;

RenderWorld::RenderWorld()
{
	m_outdoorEnv = nullptr;

	m_visFrameId = 1;
	m_shadowFrameId = 1;
	m_shadowDir.set(0,0,0);
}

RenderWorld::~RenderWorld()
{}

void RenderWorld::initialize(int worldSize)
{
	worldSize = Math::clamp(worldSize, 1024, 8192);
	worldSize = Math::nextPowerOfTwo(worldSize);
	m_worldSize = worldSize;

	m_terrain = nullptr;

	m_outdoorEnv = new OutdoorEnv(this);

	// init quadtree
	generateQuadNode();
}

void RenderWorld::finalize()
{
//		TypeFreeContainer(m_linkedActorSeq);
}

void RenderWorld::addEntity(RenderEntity *entity)
{
	if (entity->m_world) {
		if (entity->m_world == this) {
			updateEntity(entity);
			return;
		} else {
			Errorf("Add a entity belongs to other world.");
			return;
		}
	}

	entity->m_world = this;

	if (entity->getKind() == RenderEntity::kTerrain) {
		AX_ASSERT(m_terrain == nullptr);
		m_terrain = dynamic_cast<RenderTerrain*>(entity);
		AX_ASSERT(m_terrain);

		return;
	}

	linkEntity(entity);
}

void RenderWorld::updateEntity(RenderEntity *entity)
{
	if (entity->getKind() == RenderEntity::kTerrain) {
		return;
	}

	unlinkEntity(entity);
	linkEntity(entity);
}

void RenderWorld::removeEntity(RenderEntity *entity)
{
	if (entity->m_world != this) {
		return;
	}

	if (entity->getKind() == RenderEntity::kTerrain) {
		m_terrain = nullptr;
		return;
	}

	unlinkEntity(entity);

	entity->m_world = 0;
}


void RenderWorld::renderTo(RenderScene *qscene)
{
	m_frameNum++;

	qscene->worldFrameId = m_frameNum;

	s_drawTerrain = false;
	s_drawActor = true;

	m_updateShadowVis = qscene->isLastCsmSplits();

	if (qscene->sceneType == RenderScene::Reflection) {
		int ref = r_reflection.getInteger();

		s_drawTerrain = false;
		s_drawActor = false;

		switch (ref) {
			case 2:
				s_drawActor = true;
			case 1:
				s_drawTerrain = true;
		}
	} else if (qscene->sceneType == RenderScene::ShadowGen) {
		if (r_terrainShadow.getBool()) {
			s_drawTerrain = true;
		} else {
			s_drawTerrain = false;
		}
	} else {
		m_visFrameId++;
	}

	if (!r_actor.getBool()) {
		s_drawActor = false;
	}

	const RenderCamera &cam = qscene->camera;

	// outdoor environment
	if (m_outdoorEnv) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_outdoorEnv->update(qscene, Plane::Cross);
		}

		qscene->addEntity(m_outdoorEnv);
		m_outdoorEnv->issueToQueue(qscene);

		if (m_shadowDir != m_outdoorEnv->getGlobalLight()->getGlobalLightDirection()) {
			m_shadowDir = m_outdoorEnv->getGlobalLight()->getGlobalLightDirection();
			m_shadowFrameId++;
		}
	}

	// terrain
	if (m_terrain && r_terrain.getBool() && s_drawTerrain) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_terrain->update(qscene, Plane::Cross);
		}
#if 0
		Primitives prims = m_terrain->getViewedPrimitives();

		for (size_t i = 0; i < prims.size(); i++) {
			qscene->addInteraction(0, prims[i]);
		}
#else
		m_terrain->issueToQueue(qscene);
#endif
	}

	bool frustumCull = r_frustumCull.getBool();

	Plane::Side side;
	if (frustumCull) {
		side = Plane::Cross;
	} else {
		side = Plane::Front;
	}

	if (s_drawActor) {
		markVisible_r(qscene, m_rootNode, side);
	}

	// issue entity manager
	for (int i = 0; i < g_renderSystem->getNumEntityManager(); i++) {
		IEntityManager *am = g_renderSystem->getEntityManager(i);
		AX_ASSERT(am);

		am->issueToQueue(qscene);
	}

	qscene->finalProcess();

	if (qscene->sceneType != RenderScene::WorldMain) {
		return;
	}

#if 0
	g_statistic->setValue(stat_worldGlobalLights, qscene->globalLight ? 1 : 0);
	g_statistic->setValue(stat_worldLights, qscene->numLights);
	g_statistic->setValue(stat_worldActors, qscene->numEntities);
	g_statistic->setValue(stat_worldInteractions, qscene->numInteractions);
#else
	stat_worldGlobalLights.setInt(qscene->globalLight ? 1 : 0);
	stat_worldLights.setInt(qscene->numLights);
	stat_worldActors.setInt(qscene->numEntities);
	stat_worldInteractions.setInt(qscene->numInteractions);
#endif

	// check interactions, if need subscene
	for (int i = 0; i < qscene->numInteractions; i++) {
		Interaction *ia = qscene->interactions[i];
		Material *mat = ia->primitive->getMaterial();

		if (!mat)
			continue;

		const ShaderInfo *shader = mat->getShaderInfo();
		if (!shader)
			continue;

		if (!shader->m_needReflection)
			continue;

		ReflectionMap *refl = g_renderSystem->findReflection(this, 0, ia->primitive, 512, 512);
		refl->update(qscene);

		if (refl->m_target)
			mat->setTexture(MaterialTextureId::Reflection, refl->m_target->getTexture());

		if (qscene->numSubScenes == RenderScene::MAX_SUB_SCENES) {
			Errorf("MAX_SUB_SCENES exceeds");
			break;
		}
	}

	for (int i = 0; i < qscene->numSubScenes; i++) {
		// render to texture no world
		RenderScene *subscene = qscene->subScenes[i];
		if (subscene->sceneType == RenderScene::RenderToTexture) {
			continue;
		}

		QuadNode *node = m_rootNode;

		if (subscene->sceneType == RenderScene::ShadowGen) {
			RenderLight *light = subscene->sourceLight;

			if (light->getLightType() != RenderLight::kGlobal) {
				node = light->m_linkedNode;
			}
		}
		renderTo(qscene->subScenes[i], node);
	}
}

void RenderWorld::renderTo( RenderScene *qscene, QuadNode *node )
{
	m_frameNum++;

	qscene->worldFrameId = m_frameNum;

	s_drawTerrain = true;
	s_drawActor = true;

	m_updateShadowVis = qscene->isLastCsmSplits();

	if (qscene->sceneType == RenderScene::Reflection) {
		int ref = r_reflection.getInteger();

		s_drawTerrain = false;
		s_drawActor = false;

		switch (ref) {
		case 2:
			s_drawActor = true;
		case 1:
			s_drawTerrain = true;
		}
	} else if (qscene->sceneType == RenderScene::ShadowGen) {
		if (r_terrainShadow.getBool()) {
			s_drawTerrain = true;
		} else {
			s_drawTerrain = false;
		}
	} else {
		m_visFrameId++;
	}

	if (!r_actor.getBool()) {
		s_drawActor = false;
	}

	const RenderCamera &cam = qscene->camera;

	// outdoor environment
	if (m_outdoorEnv) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_outdoorEnv->update(qscene, Plane::Cross);
		}

		qscene->addEntity(m_outdoorEnv);
		m_outdoorEnv->issueToQueue(qscene);

		if (m_shadowDir != m_outdoorEnv->getGlobalLight()->getGlobalLightDirection()) {
			m_shadowDir = m_outdoorEnv->getGlobalLight()->getGlobalLightDirection();
			m_shadowFrameId++;
		}
	}

	// terrain
	if (m_terrain && r_terrain.getBool() && s_drawTerrain) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_terrain->update(qscene, Plane::Cross);
		}
#if 0
		Primitives prims = m_terrain->getViewedPrimitives();

		for (size_t i = 0; i < prims.size(); i++) {
			qscene->addInteraction(0, prims[i]);
		}
#else
		m_terrain->issueToQueue(qscene);
#endif
	}

	bool frustumCull = r_frustumCull.getBool();

	Plane::Side side;
	if (frustumCull) {
		side = Plane::Cross;
	} else {
		side = Plane::Front;
	}

	if (s_drawActor)
		markVisible_r(qscene, node, side);

	// issue entity manager
	for (int i = 0; i < g_renderSystem->getNumEntityManager(); i++) {
		IEntityManager *am = g_renderSystem->getEntityManager(i);
		AX_ASSERT(am);

		am->issueToQueue(qscene);
	}

	qscene->finalProcess();
}

// mark visible
void RenderWorld::markVisible_r(RenderScene *qscene, QuadNode *node, Plane::Side parentSide)
{
	if (node == NULL)
		return;

	if (node->bbox.empty())
		return;

	const RenderCamera &cam = qscene->camera;

	Plane::Side side = parentSide;
	
	if (side == Plane::Cross) {
		side = cam.checkBox(node->bbox);
		if (side == Plane::Back) {
			return;
		}
	}

	for (IntrusiveList<RenderEntity>::iterator it = node->linkHead.begin(); it != node->linkHead.end(); ++it) {
		RenderEntity *entity = &*it;
		if (qscene->sceneType == RenderScene::ShadowGen && qscene->sourceLight->getLightType() == RenderLight::kGlobal) {
			if (entity->isCsmCulled()) {
#if 0
				g_statistic->incValue(stat_csmCulled);
#else
				stat_csmCulled.inc();
#endif
				continue;
			} else {
				// g_statistic->incValue(stat_csmPassed);
				stat_csmPassed.inc();
			}
		}

		const BoundingBox &bbox = entity->m_linkedBbox;
		// if node is cross frustum, we check entity's bbox
		Plane::Side actorSide = side;
		if (side == Plane::Cross && r_cullActor.getBool()) {
			actorSide = cam.checkBox(bbox);
			if ( actorSide == Plane::Back) {
				continue;
			}
		}

		if (qscene->sceneType == RenderScene::WorldMain) {
			entity->update(qscene, actorSide);

			if (entity->m_queryCulled)
				continue;
		}

		if (m_updateShadowVis) {
			entity->updateCsm(qscene, actorSide);
		}

		if (entity->m_viewDistCulled)
			continue;

		// check if is light
		if (entity->getKind() == RenderEntity::kLight || s_drawActor)
			qscene->addEntity(entity);

		if (!s_drawActor)
			continue;

#if 0
 		Primitives prims = la->actor->getViewedPrimitives();

		for (size_t i = 0; i < prims.size(); i++) {
			qscene->addInteraction(la->queued, prims[i]);
		}
#endif
		entity->issueToQueue(qscene);
	}

	if (node->children[0]) {
		markVisible_r(qscene, node->children[0], side);
		markVisible_r(qscene, node->children[1], side);
		markVisible_r(qscene, node->children[2], side);
		markVisible_r(qscene, node->children[3], side);
	}

	if (!r_showNode.getBool())
		return;

	Rgba color = Rgba::Green;

	if (node->children[0])
		color *= 0.5f;

	LinePrim *line = LinePrim::createWorldBoundingBox(LinePrim::HintFrame, node->bbox, color);
	qscene->addHelperInteraction(0, line);
}

void RenderWorld::generateQuadNode()
{
	uint_t size = m_worldSize;

	// setup root
	m_rootNode = new QuadNode();
	m_rootNode->dist[0] = 0;
	m_rootNode->dist[1] = 0;
	m_rootNode->parent = 0;
	m_rootNode->size = size;

	generateChildNode_r(m_rootNode);
}

void RenderWorld::generateChildNode_r(QuadNode *node)
{
	float size = node->size * 0.5f;
	float offset = size * 0.5f;

	// first quadrant
	QuadNode *child = new QuadNode();
	child->dist[0] = node->dist[0] + offset;
	child->dist[1] = node->dist[1] + offset;
	child->size = size;
	child->parent = node;
	node->children[0] = child;

	// 2nd quadrant
	child = new QuadNode();
	child->dist[0] = node->dist[0] - offset;
	child->dist[1] = node->dist[1] + offset;
	child->size = size;
	child->parent = node;
	node->children[1] = child;

	// 3rd quadrant
	child = new QuadNode();
	child->dist[0] = node->dist[0] - offset;
	child->dist[1] = node->dist[1] - offset;
	child->size = size;
	child->parent = node;
	node->children[2] = child;

	// 4th quadrant
	child = new QuadNode();
	child->dist[0] = node->dist[0] + offset;
	child->dist[1] = node->dist[1] - offset;
	child->size = size;
	child->parent = node;
	node->children[3] = child;

	if (size > MininumNodeSize) {
		generateChildNode_r(node->children[0]);
		generateChildNode_r(node->children[1]);
		generateChildNode_r(node->children[2]);
		generateChildNode_r(node->children[3]);
	}
}


void RenderWorld::linkEntity(RenderEntity *entity)
{
	entity->m_linkedBbox = entity->getBoundingBox();
	entity->m_linkedExtends = entity->m_linkedBbox.getExtends().getLength();
	entity->m_linkedFrame = m_visFrameId;

	const BoundingBox &bbox = entity->m_linkedBbox;

	// find quad node
	QuadNode *node = m_rootNode;
	while (1) {
		if (node->children[0] == NULL)
			break;

		if (bbox.min.x >= node->dist[0]) {
			if (bbox.min.y >= node->dist[1]) {
				node = node->children[0];
			} else if (bbox.max.y <= node->dist[1]) {
				node = node->children[3];
			} else {
				break;
			}
		} else if (bbox.max.x <= node->dist[0]) {
			if (bbox.max.y <= node->dist[1]) {
				node = node->children[2];
			} else if (bbox.min.y >= node->dist[1]) {
				node = node->children[1];
			} else {
				break;
			}
		} else {
			break;
		}
	}

	// link to it
#if 0
	entity->m_link.addToEnd(node->linkHead);
#else
	node->linkHead.push_back(entity);
#endif
	entity->m_linkedNode = node;

	// expand node's boundingbox
	node->expandBbox(bbox);

	if (!entity->isLight())
		node->frameUpdated(m_visFrameId);
}

void RenderWorld::unlinkEntity(RenderEntity *la)
{
	QuadNode *node = la->m_linkedNode;

	if (!la->isLight())
		node->frameUpdated(m_visFrameId);

#if 0
	la->m_link.removeFromList();
#else
	node->linkHead.erase(la);
#endif
}

AX_END_NAMESPACE


