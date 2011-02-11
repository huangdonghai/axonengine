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
static bool s_drawEntity = true;

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

	s_drawTerrain = true;
	s_drawEntity = true;

	m_updateShadowVis = qscene->isLastCsmSplits();

	if (qscene->sceneType == RenderScene::Reflection) {
		int ref = r_reflection.getInteger();

		s_drawTerrain = false;
		s_drawEntity = false;

		switch (ref) {
			case 2:
				s_drawEntity = true;
			case 1:
				s_drawTerrain = true;
		}
	} else if (qscene->sceneType == RenderScene::ShadowGen) {
		if (r_terrainShadow.getBool()) {
			s_drawTerrain = true;
		} else {
			s_drawTerrain = false;
		}
	} else if(qscene->sceneType == RenderScene::WorldMain) {
		m_visFrameId++;
	} else {
		AX_WRONGPLACE;
	}

	if (!r_entity.getBool()) {
		s_drawEntity = false;
	}

	const RenderCamera &cam = qscene->camera;

	// outdoor environment
	if (m_outdoorEnv) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_outdoorEnv->update(qscene, false);
		}

		qscene->addEntity(m_outdoorEnv);
		m_outdoorEnv->issueToScene(qscene);

		if (m_shadowDir != m_outdoorEnv->getGlobalLight()->lightDirection()) {
			m_shadowDir = m_outdoorEnv->getGlobalLight()->lightDirection();
			m_shadowFrameId++;
		}
	}

	// terrain
	if (m_terrain && r_terrain.getBool() && s_drawTerrain) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_terrain->update(qscene, false);
		}

		m_terrain->issueToScene(qscene);
	}

	bool frustumCull = r_frustumCull.getBool();

	Plane::Side side;
	if (frustumCull) {
		side = Plane::Cross;
	} else {
		side = Plane::Front;
	}

	if (s_drawEntity)
		markVisible_r(qscene, m_rootNode, side);

	// issue entity manager
	for (int i = 0; i < g_renderSystem->getNumEntityManager(); i++) {
		IEntityManager *am = g_renderSystem->getEntityManager(i);
		AX_ASSERT(am);

		am->issueToScene(qscene);
	}

	qscene->finalProcess();

	if (qscene->sceneType != RenderScene::WorldMain)
		return;

	stat_worldGlobalLights.setInt(qscene->globalLight ? 1 : 0);
	stat_worldLights.setInt(qscene->numLights);
	stat_worldActors.setInt(qscene->numEntities);
	stat_worldInteractions.setInt(qscene->numInteractions);

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
			mat->setTexture(MaterialTextureId::Reflection, refl->m_target);

		if (qscene->numSubScenes == RenderScene::MAX_SUB_SCENES) {
			Errorf("MAX_SUB_SCENES exceeds");
			break;
		}
	}

	for (int i = 0; i < qscene->numSubScenes; i++) {
		// render to texture no world
		RenderScene *subscene = qscene->subScenes[i];
		if (subscene->sceneType == RenderScene::RenderToTexture)
			continue;

		QuadNode *node = m_rootNode;

		if (subscene->sceneType == RenderScene::ShadowGen) {
			RenderLight *light = subscene->sourceLight;

			if (light->lightType() != RenderLight::kGlobal) {
				node = light->m_linkedNode;
			}
		}
		renderTo(qscene->subScenes[i], node);
	}
}

void RenderWorld::renderTo(RenderScene *qscene, QuadNode *node)
{
	m_frameNum++;

	qscene->worldFrameId = m_frameNum;

	s_drawTerrain = true;
	s_drawEntity = true;

	m_updateShadowVis = qscene->isLastCsmSplits();

	if (qscene->sceneType == RenderScene::Reflection) {
		int ref = r_reflection.getInteger();

		s_drawTerrain = false;
		s_drawEntity = false;

		switch (ref) {
		case 2:
			s_drawEntity = true;
		case 1:
			s_drawTerrain = true;
		}
	} else if (qscene->sceneType == RenderScene::ShadowGen) {
		if (r_terrainShadow.getBool()) {
			s_drawTerrain = true;
		} else {
			s_drawTerrain = false;
		}
	} else if(qscene->sceneType == RenderScene::WorldMain) {
		m_visFrameId++;
	} else {
		AX_WRONGPLACE;
	}

	if (!r_entity.getBool()) {
		s_drawEntity = false;
	}

	const RenderCamera &cam = qscene->camera;

	// outdoor environment
	if (m_outdoorEnv) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_outdoorEnv->update(qscene, false);
		}

		qscene->addEntity(m_outdoorEnv);
		m_outdoorEnv->issueToScene(qscene);

		if (m_shadowDir != m_outdoorEnv->getGlobalLight()->lightDirection()) {
			m_shadowDir = m_outdoorEnv->getGlobalLight()->lightDirection();
			m_shadowFrameId++;
		}
	}

	// terrain
	if (m_terrain && r_terrain.getBool() && s_drawTerrain) {
		if (qscene->sceneType == RenderScene::WorldMain) {
			m_terrain->update(qscene, false);
		}
		m_terrain->issueToScene(qscene);
	}

	bool frustumCull = r_frustumCull.getBool();

	Plane::Side side;
	if (frustumCull) {
		side = Plane::Cross;
	} else {
		side = Plane::Front;
	}

	if (s_drawEntity)
		markVisible_r(qscene, node, side);

	// issue entity manager
	for (int i = 0; i < g_renderSystem->getNumEntityManager(); i++) {
		IEntityManager *am = g_renderSystem->getEntityManager(i);
		AX_ASSERT(am);

		am->issueToScene(qscene);
	}

	qscene->finalProcess();
}

// mark visible
void RenderWorld::markVisible_r(RenderScene *qscene, QuadNode *node, Plane::Side parentSide)
{
	if (!node)
		return;

	if (node->bbox.empty())
		return;

	const RenderCamera &cam = qscene->camera;

	Plane::Side side = parentSide;
#if 1
	if (side == Plane::Cross) {
		side = cam.checkBox(node->bbox);
		if (side == Plane::Back) {
			return;
		}
	}
#endif
	for (IntrusiveList<RenderEntity>::iterator it = node->linkHead.begin(); it != node->linkHead.end(); ++it) {
		RenderEntity *entity = &*it;
		if (qscene->sceneType == RenderScene::ShadowGen && qscene->sourceLight->isGlobal()) {
			if (entity->isCsmCulled()) {
				stat_csmCulled.inc();
				continue;
			} else {
				stat_csmPassed.inc();
			}
		}

		const BoundingBox &bbox = entity->m_linkedBbox;

		Plane::Side actorSide = side;
		if (r_lockPvs.getBool() && qscene->sceneType == RenderScene::WorldMain) {
			if (entity->m_visFrameId != m_visFrameId - 1)
				continue;
		} else {
			// if node is cross frustum, we check entity's bbox
			if (side == Plane::Cross && r_cullEntity.getBool()) {
				actorSide = cam.checkBox(bbox);
				if (actorSide == Plane::Back) {
					continue;
				}
			}
		}

		bool allInFrustum = actorSide == Plane::Front;
		if (qscene->sceneType == RenderScene::WorldMain) {
			entity->update(qscene, allInFrustum);

			if (entity->m_queryCulled) {
				stat_numVisQueryCulled.inc();
				continue;
			}
		}

		if (m_updateShadowVis)
			entity->updateCsm(qscene, allInFrustum);

		if (entity->m_viewDistCulled)
			continue;

		// check if is light
		if (entity->getKind() == RenderEntity::kLight || s_drawEntity)
			qscene->addEntity(entity);

		if (!s_drawEntity)
			continue;

		entity->issueToScene(qscene);
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
	node->linkHead.push_back(entity);
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

void RenderWorld::buildKdTree()
{

}


AX_END_NAMESPACE


