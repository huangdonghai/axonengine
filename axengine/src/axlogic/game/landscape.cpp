/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

Fixed::Fixed()
{
	m_num = -1;
	m_landscape = 0;
}

Fixed::~Fixed()
{
}

//--------------------------------------------------------------------------
// class StaticFixed
//--------------------------------------------------------------------------

StaticFixed::StaticFixed()
{
	m_model = 0;
	m_rigid = 0;
}

StaticFixed::~StaticFixed()
{
	onReset();
}

void StaticFixed::set_model(const std::string &val)
{
	m_modelName = val;
}

void StaticFixed::onReload()
{
	onReset();

	if (m_modelName.empty())
		return;

	m_model = new HavokModel(m_modelName);
	m_model->setStatic(true);
	m_rigid = new PhysicsRigid(m_modelName);

	m_model->setMatrix(m_matrix_p);
	m_model->setInstanceColor(m_instanceColor_p);
	m_rigid->setMatrix(m_matrix_p);
	m_rigid->setMotionType(PhysicsEntity::Motion_Fixed);

	m_landscape->getGameWorld()->getRenderWorld()->addEntity(m_model);
	m_landscape->getGameWorld()->getPhysicsWorld()->addEntity(m_rigid);

	setRenderEntity(m_model);
	setPhysicsEntity(m_rigid);
}

void StaticFixed::onReset()
{
	setRenderEntity(0);
	setPhysicsEntity(0);

	SafeDelete(m_model);
	SafeDelete (m_rigid);
}

//--------------------------------------------------------------------------
// class TerrainFixed
//--------------------------------------------------------------------------

TerrainFixed::TerrainFixed(RenderTerrain *terr)
{
	m_renderTerrain = terr;
	terr->setStatic(true);
	m_physicsTerrain = 0;

	m_renderTerrain->addObserver(this);
}

TerrainFixed::~TerrainFixed()
{
	m_renderTerrain->removeObserver(this);
	onReset();
}

void TerrainFixed::onReload()
{
	if (!m_landscape || !m_renderTerrain) return;
	m_landscape->getGameWorld()->getRenderWorld()->addEntity(m_renderTerrain);
	if (!m_physicsTerrain) return;
	m_landscape->getGameWorld()->getPhysicsWorld()->addEntity(m_physicsTerrain);
}

void TerrainFixed::onReset()
{
	if (!m_landscape || !m_renderTerrain) return;
	m_landscape->getGameWorld()->getRenderWorld()->removeEntity(m_renderTerrain);
	if (!m_physicsTerrain) return;
	m_landscape->getGameWorld()->getPhysicsWorld()->removeEntity(m_physicsTerrain);
}

void TerrainFixed::beNotified(IObservable *subject, int arg)
{
	if (subject != m_renderTerrain) {
		return;
	}

	if (arg != RenderTerrain::HeightfieldSetted) {
		return;
	}

	if (m_physicsTerrain) {
		if (m_landscape) {
			m_landscape->getGameWorld()->getPhysicsWorld()->removeEntity(m_physicsTerrain);
		}
		SafeDelete(m_physicsTerrain);
	}

	ushort_t *datap;
	int size;
	float tilemeters;

	m_renderTerrain->getHeightinfo(datap, size, tilemeters);
	m_physicsTerrain = new PhysicsTerrain(datap, size, tilemeters);

	if (m_landscape) {
		m_landscape->getGameWorld()->getPhysicsWorld()->addEntity(m_physicsTerrain);
	}
}

//--------------------------------------------------------------------------
// class TreeFixed
//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
TreeFixed::TreeFixed()
{
	m_renderTree = 0;
}

TreeFixed::~TreeFixed()
{
	onReset();
}

void TreeFixed::onReload()
{
	onReset();

	if (m_treeName.empty())
		return;

	m_renderTree = new RenderTree(m_treeName);
	m_renderTree->setMatrix(m_matrix_p);
	m_landscape->getGameWorld()->getRenderWorld()->addEntity(m_renderTree);

	setRenderEntity(m_renderTree);
}

void TreeFixed::onReset()
{
	setRenderEntity(0);
	SafeDelete(m_renderTree);
}

void TreeFixed::set_tree(const std::string &val)
{
	m_treeName = val;
}
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

//--------------------------------------------------------------------------
// class Landscape
//--------------------------------------------------------------------------

Landscape::Landscape(GameWorld *world)
{
	m_world = world;

	TypeZeroArray(m_fixeds);
/* for (int i = MaxFixed - 1; i >= 0; i--) {
		m_freeList.add(m_fixeds[i]);
	}*/
}

Landscape::~Landscape()
{}

void Landscape::addFixed(Fixed *fixed)
{
	AX_ASSERT(fixed->m_num == -1);

	/*Fixed** free = m_freeList.get();
	if (!free) {
		return;
	}*/
	int num = -1;
	for (int i = 0; i < MaxFixed;++ i){
		if (m_fixeds[i] == 0) {
			num = i;
			break;
		}
	}

	if (num == -1) {
		Errorf("MaxFixed exceeded");
	}

	m_fixeds[num] = fixed;
	fixed->m_num = num;
	fixed->m_landscape = this;
	fixed->doSpawn();
}

void Landscape::removeFixed(Fixed *fixed)
{
	int num = fixed->m_num;
	if (num < 0) {
		return;
	}

	//Fixed*& befree = m_fixeds[num];
	//AX_ASSERT(fixed == befree);
	//befree = 0;
	m_fixeds[num] = 0;

	//m_freeList.add(befree);
	fixed->doRemove();
	fixed->m_num = -1;
	fixed->m_landscape = 0;
}

void Landscape::buildKdTree()
{
	int numFixed = 0;
	int numStaticModel = 0;
	int numVertices = 0;
	int numElements = 0;
	float area = 0;

	for (int i = 0; i < MaxFixed; i++) {
		Fixed *fixed = m_fixeds[i];
		if (!fixed)
			continue;

		numFixed++;
		StaticFixed *staticFixed = object_cast<StaticFixed *>(fixed);
		if (!staticFixed)
			continue;

		numStaticModel++;
		if (!staticFixed->m_model)
			continue;

		Primitives prims = staticFixed->m_model->getStaticPrims();
		Primitives::const_iterator it = prims.begin();
		float curArea = 0;
		for (; it != prims.end(); ++it) {
			Primitive *prim = *it;
			if (prim->getType() != Primitive::MeshType)
				continue;
			MeshPrim *mesh = static_cast<MeshPrim *>(prim);

			numVertices += mesh->getNumVertexes();
			numElements += mesh->numElements();
			curArea += mesh->calcArea();
		}
		float scale = staticFixed->m_model->getMatrix().getScales();
		if (scale != 1.0f)
			curArea *= scale * scale;
		area += curArea;
	}
}




AX_END_NAMESPACE

