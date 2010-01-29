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

RenderWorld::RenderWorld() {
	m_outdoorEnv = nullptr;
	TypeZeroArray(m_histogram);
	m_curHistogramIndex = 0;
	m_histogramQuery = g_queryManager->allocQuery();
	m_lastExposure = 1;

	m_visFrameId = 1;
	m_shadowFrameId = 1;
	m_shadowDir.set(0,0,0);
}

RenderWorld::~RenderWorld() {
	g_queryManager->freeQuery(m_histogramQuery);
}

void RenderWorld::initialize(int worldSize) {
	worldSize = Math::clamp(worldSize, 1024, 8192);
	worldSize = Math::nextPowerOfTwo(worldSize);
	m_worldSize = worldSize;

	m_terrain = nullptr;

	m_outdoorEnv = new OutdoorEnv(this);

	// init quadtree
	generateQuadNode();
}

void RenderWorld::finalize() {
//		TypeFreeContainer(m_linkedActorSeq);
}

void RenderWorld::addEntity(RenderEntity *entity) {
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

void RenderWorld::updateEntity(RenderEntity *entity) {
	if (entity->getKind() == RenderEntity::kTerrain) {
		return;
	}

	unlinkEntity(entity);
	linkEntity(entity);
}

void RenderWorld::removeEntity(RenderEntity *entity) {
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


void RenderWorld::renderTo(QueuedScene *qscene) {
	m_frameNum++;

	qscene->worldFrameId = m_frameNum;

	s_drawTerrain = true;
	s_drawActor = true;

	m_updateShadowVis = qscene->isLastCsmSplits();

	if (qscene->sceneType == QueuedScene::Reflection) {
		int ref = r_reflection->getInteger();

		s_drawTerrain = false;
		s_drawActor = false;

		switch (ref) {
			case 2:
				s_drawActor = true;
			case 1:
				s_drawTerrain = true;
		}
	} else if (qscene->sceneType == QueuedScene::ShadowGen) {
		if (r_terrainShadow->getBool()) {
			s_drawTerrain = true;
		} else {
			s_drawTerrain = false;
		}
	} else {
		m_visFrameId++;
	}

	if (!r_actor->getBool()) {
		s_drawActor = false;
	}

	const RenderCamera &cam = qscene->camera;

	// outdoor environment
	if (m_outdoorEnv) {
		if (qscene->sceneType == QueuedScene::WorldMain) {
			updateExposure(qscene);
			m_outdoorEnv->update(qscene, Plane::Cross);
		} else {
			qscene->exposure = m_lastExposure;
		}

		qscene->addEntity(m_outdoorEnv);
		m_outdoorEnv->issueToQueue(qscene);

		if (m_shadowDir != m_outdoorEnv->getGlobalLight()->getGlobalLightDirection()) {
			m_shadowDir = m_outdoorEnv->getGlobalLight()->getGlobalLightDirection();
			m_shadowFrameId++;
		}
	}

	// terrain
	if (m_terrain && r_terrain->getBool() && s_drawTerrain) {
		if (qscene->sceneType == QueuedScene::WorldMain) {
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

	bool frustumCull = r_frustumCull->getBool();

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

	if (qscene->sceneType != QueuedScene::WorldMain) {
		return;
	}

	g_statistic->setValue(stat_worldGlobalLights, qscene->globalLight ? 1 : 0);
	g_statistic->setValue(stat_worldLights, qscene->numLights);
	g_statistic->setValue(stat_worldActors, qscene->numEntities);
	g_statistic->setValue(stat_worldInteractions, qscene->numInteractions);

	// check interactions, if need subscene
	for (int i = 0; i < qscene->numInteractions; i++) {
		Interaction *ia = qscene->interactions[i];
		Material *mat = ia->primitive->getMaterial();
		if (!mat) {
			continue;
		}

		Shader *shader = mat->getShaderTemplate();
		if (!shader) {
			continue;
		}

		for (int j = 0; j < shader->getNumSampler(); j++) {
			SamplerAnno *sa = shader->getSamplerAnno(j);
			if (sa->m_renderType != SamplerAnno::Reflection) {
				continue;
			}

#if 0
			QueuedScene *subscene = gRenderQueue->allocQueuedScene();
			qscene->subScenes[qscene->numSubScenes++] = subscene;
			subscene->sceneType = QueuedScene::Reflection;

			subscene->camera = cam.createMirrorCamera(Plane(0, 0, 1, 0));
			Target *target = gTargetManager->allocTarget(Target::FrameAlloc, 512, 512, TexFormat::BGRA8);
			subscene->camera.setTarget(target);
			subscene->camera.setViewRect(Rect(0, 0, 512, 512));
#else
			ReflectionTarget *refl = g_targetManager->findReflection(this, 0, ia->primitive, 512, 512);
			refl->update(qscene);
#endif

			if (refl->m_target->m_realAllocated)
				ia->targets[ia->numTargets++] = refl->m_target;

			if (qscene->numSubScenes == QueuedScene::MAX_SUB_SCENES) {
				Errorf("MAX_SUB_SCENES exceeds");
				break;
			}
		}

		if (qscene->numSubScenes == QueuedScene::MAX_SUB_SCENES) {
			Errorf("MAX_SUB_SCENES exceeds");
			break;
		}
	}

	for (int i = 0; i < qscene->numSubScenes; i++) {
		// render to texture no world
		QueuedScene *subscene = qscene->subScenes[i];
		if (subscene->sceneType == QueuedScene::RenderToTexture) {
			continue;
		}

		QuadNode *node = m_rootNode;

		if (subscene->sceneType == QueuedScene::ShadowGen) {
			RenderLight *light = subscene->sourceLight->preQueued;

			if (light->getLightType() != RenderLight::kGlobal) {
				node = subscene->sourceLight->preQueued->m_linkedNode;
			}
		}
		renderTo(qscene->subScenes[i], node);
	}
}

void RenderWorld::renderTo( QueuedScene *qscene, QuadNode *node )
{
	m_frameNum++;

	qscene->worldFrameId = m_frameNum;

	s_drawTerrain = true;
	s_drawActor = true;

	m_updateShadowVis = qscene->isLastCsmSplits();

	if (qscene->sceneType == QueuedScene::Reflection) {
		int ref = r_reflection->getInteger();

		s_drawTerrain = false;
		s_drawActor = false;

		switch (ref) {
			case 2:
				s_drawActor = true;
			case 1:
				s_drawTerrain = true;
		}
	} else if (qscene->sceneType == QueuedScene::ShadowGen) {
		if (r_terrainShadow->getBool()) {
			s_drawTerrain = true;
		} else {
			s_drawTerrain = false;
		}
	} else {
		m_visFrameId++;
	}

	if (!r_actor->getBool()) {
		s_drawActor = false;
	}

	const RenderCamera &cam = qscene->camera;

	// outdoor environment
	if (m_outdoorEnv) {
		if (qscene->sceneType == QueuedScene::WorldMain) {
			updateExposure(qscene);
			m_outdoorEnv->update(qscene, Plane::Cross);
		} else {
			qscene->exposure = m_lastExposure;
		}

		qscene->addEntity(m_outdoorEnv);
		m_outdoorEnv->issueToQueue(qscene);

		if (m_shadowDir != m_outdoorEnv->getGlobalLight()->getGlobalLightDirection()) {
			m_shadowDir = m_outdoorEnv->getGlobalLight()->getGlobalLightDirection();
			m_shadowFrameId++;
		}
	}

	// terrain
	if (m_terrain && r_terrain->getBool() && s_drawTerrain) {
		if (qscene->sceneType == QueuedScene::WorldMain) {
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

	bool frustumCull = r_frustumCull->getBool();

	Plane::Side side;
	if (frustumCull) {
		side = Plane::Cross;
	} else {
		side = Plane::Front;
	}

	if (s_drawActor) {
		markVisible_r(qscene, node, side);
	}

	// issue entity manager
	for (int i = 0; i < g_renderSystem->getNumEntityManager(); i++) {
		IEntityManager *am = g_renderSystem->getEntityManager(i);
		AX_ASSERT(am);

		am->issueToQueue(qscene);
	}

	qscene->finalProcess();
}

// mark visible
void RenderWorld::markVisible_r(QueuedScene *qscene, QuadNode *node, Plane::Side parentSide) {
	if (node == NULL)
		return;

	if (node->bbox.empty()) {
		return;
	}

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
		if (qscene->sceneType == QueuedScene::ShadowGen && qscene->sourceLight->type == RenderLight::kGlobal) {
			if (entity->isCsmCulled()) {
				g_statistic->incValue(stat_csmCulled);
				continue;
			} else {
				g_statistic->incValue(stat_csmPassed);
			}
		}

		const BoundingBox &bbox = entity->m_linkedBbox;
		// if node is cross frustum, we check entity's bbox
		Plane::Side actorSide = side;
		if (side == Plane::Cross && r_cullActor->getBool()) {
			actorSide = cam.checkBox(bbox);
			if ( actorSide == Plane::Back) {
				continue;
			}
		}

		if (qscene->sceneType == QueuedScene::WorldMain) {
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
		if (entity->getKind() == RenderEntity::kLight || s_drawActor) {
			qscene->addEntity(entity);
		}

		if (!s_drawActor) {
			continue;
		}

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
	} else {
	}

	if (!r_showNode->getBool())
		return;

	Rgba color = Rgba::Green;

	if (node->children[0])
		color *= 0.5f;

	LinePrim *line = LinePrim::createWorldBoundingBox(LinePrim::HintFrame, node->bbox, color);
	qscene->addHelperInteraction(0, line);
}

void RenderWorld::generateQuadNode() {
	uint_t size = m_worldSize;

	// setup root
	m_rootNode = new QuadNode();
	m_rootNode->dist[0] = 0;
	m_rootNode->dist[1] = 0;
	m_rootNode->parent = 0;
	m_rootNode->size = size;

	generateChildNode_r(m_rootNode);
}

void RenderWorld::generateChildNode_r(QuadNode *node) {
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


void RenderWorld::linkEntity(RenderEntity *entity) {
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

void RenderWorld::unlinkEntity(RenderEntity *la) {
	QuadNode *node = la->m_linkedNode;

	if (!la->isLight())
		node->frameUpdated(m_visFrameId);

#if 0
	la->m_link.removeFromList();
#else
	node->linkHead.erase(la);
#endif
}

void RenderWorld::updateExposure(QueuedScene *qscene) {
	if (1 || !r_hdr->getBool()) {
		qscene->m_histogramIndex = -1;
		qscene->m_histogramQueryId = 0;
		return;
	}

#if 0
	Target *lasttarget = gRenderQueue->getQueryResultTarget();
	if (lasttarget != qscene->camera.getTarget()) {
		qscene->exposure = m_lastExposure;
		return;
	}
#endif
	m_curHistogramIndex++;
	qscene->m_histogramIndex = m_curHistogramIndex % HISTOGRAM_WIDTH;
//		qscene->m_histogramQueryId = m_histogramQueryId;

	if (m_curHistogramIndex < HISTOGRAM_WIDTH + 3) {
		return;
	}

	int resultIndex = (m_curHistogramIndex - 2) % HISTOGRAM_WIDTH;
	int queryResult = -1; // gRenderQueue->getQueryResult(m_histogramQueryId);

	if (queryResult >= 0) {
		m_histogram[resultIndex] = queryResult;
	} else {
		qscene->exposure = m_lastExposure;
		return;
	}

	int totalsamplers = 0;
	for (int i = 0; i < HISTOGRAM_WIDTH; i++) {
		totalsamplers += m_histogram[i];
		m_histogramAccumed[i] = totalsamplers;
	}

	if (totalsamplers == 0) {
		return;
	}

	float midexposure = 0;
	float overexposure = 0;

	int halfsamplers = totalsamplers / 2;
	int s, e;
	for (s = 0; s < HISTOGRAM_WIDTH; s++) {
		if (m_histogramAccumed[s] > halfsamplers) {
			break;
		}
	}
	for (e = HISTOGRAM_WIDTH-1; e >= 0; e--) {
		if (m_histogramAccumed[e] < halfsamplers) {
			break;
		}
	}

	float scale = float(s + e + 2) / HISTOGRAM_WIDTH;

	midexposure = m_lastExposure * scale;

	int overbrightsamplers = totalsamplers * 0.95f;

	int i;
	for (i = 0; i < HISTOGRAM_WIDTH; i++) {
		if (m_histogramAccumed[i] > overbrightsamplers) {
			break;
		}
	}

	if (i >= HISTOGRAM_WIDTH - 1) {
		scale = (float)m_histogram[HISTOGRAM_WIDTH - 1] / totalsamplers;
		scale *= 20.0f;
		if (scale > 1) {
			scale = 1 + logf(scale);
		}
	} else {
		scale = float(i+1) / HISTOGRAM_WIDTH;
	}


//		scale = float(i+1) / HISTOGRAM_WIDTH * 1.2f;

	overexposure = m_lastExposure * scale;

//		float disired = std::max(midexposure, overexposure);
	float disired = midexposure;

	disired = Math::clamp(disired, 0.25f, 4.0f);

	if (r_exposure->getFloat()) {
		disired = r_exposure->getFloat();
	}

	float frametime = qscene->camera.getFrameTime();
	float delta = fabs(disired - m_lastExposure) * frametime;
	float exposure = Math::clamp(disired, m_lastExposure * (1-delta), m_lastExposure * (1+delta));
	qscene->exposure = exposure;
	m_lastExposure = qscene->exposure;
}


AX_END_NAMESPACE


