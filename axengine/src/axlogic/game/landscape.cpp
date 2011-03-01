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
	SafeDelete(m_rigid);
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
	for (int i = 0; i < MaxFixed; i++) {
		m_freeList.push_back(i);
	}
}

Landscape::~Landscape()
{}

void Landscape::addFixed(Fixed *fixed)
{
	AX_ASSERT(fixed->m_num == -1);

	if (m_freeList.empty()) {
		Errorf("MaxFixed exceeded");
		return;
	}

	int num = m_freeList.front();
	m_freeList.pop_front();

	m_fixeds[num] = fixed;
	fixed->m_num = num;
	fixed->m_landscape = this;
	fixed->doSpawn();
}

void Landscape::removeFixed(Fixed *fixed)
{
	int num = fixed->m_num;
	if (num < 0) {
		Errorf("Not a landscape fixed");
		return;
	}

	m_fixeds[num] = 0;

	m_freeList.push_front(num);
	fixed->doRemove();
	fixed->m_num = -1;
	fixed->m_landscape = 0;
}

namespace {
	class UnwrapHelper;

	struct Edge {
		int index0, index1; // edge's index
		int index2; // triangle another index
		float length;

		Edge(UnwrapHelper *obj, int index0, int index1, int index2);
	};

	struct Triangle
	{
		ushort_t p0, p1, p2;
	};

	struct QuadList
	{
		int rows, columns;
		ushort_t indices;
	};

	class UnwrapHelper
	{
	public:
		UnwrapHelper(MeshPrim *prim, float scale, float texelsPerMeter);
		~UnwrapHelper();

	protected:
		int findAdjacentTriangle(int edge0, int edge1, int edge2);
		void process();

	private:
		MeshPrim *m_prim;
		const MeshVertex *m_vertices;
		const ushort_t *m_indices;
		int m_numTris;
		std::vector<bool> m_triUsed;
		float m_scale;
		float m_texelsPerMeter;
	};

	UnwrapHelper::UnwrapHelper(MeshPrim *prim, float scale, float texelsPerMeter)
	{
		m_prim = prim;
		m_vertices = prim->getVertexesPointer();
		m_indices = prim->getIndexPointer();
		m_numTris = prim->getNumIndexes() / 3;
		m_triUsed.resize(m_numTris);
		for (int i = 0; i < m_numTris; i++) {
			m_triUsed[i] = false;
		}
		m_scale = scale;
		m_texelsPerMeter = texelsPerMeter;
	}

	UnwrapHelper::~UnwrapHelper()
	{
	}

	void UnwrapHelper::process()
	{
		for (int i = 0; i < m_numTris; i++) {
			int index0 = m_indices[i*m_numTris+0];
			int index1 = m_indices[i*m_numTris+1];
			int index2 = m_indices[i*m_numTris+2];
			
			Edge e0(this, index0, index1, index2);
			Edge e1(this, index1, index2, index0);
			Edge e2(this, index2, index0, index1);

			Edge *maxLengthEdge = &e0;
			if (e1.length > maxLengthEdge->length)
				maxLengthEdge = &e1;
			if (e2.length > maxLengthEdge->length)
				maxLengthEdge = &e2;

			int adjTri = findAdjacentTriangle(maxLengthEdge->index0, maxLengthEdge->index1, maxLengthEdge->index2);
			if (adjTri < 0) {
				m_triUsed[i] = true;
			}

		}
	}

}

void Landscape::buildKdTree()
{
	int numFixed = 0;
	int numStaticModel = 0;
	int numScaled = 0;
	int numVertices = 0;
	int numElements = 0;
	int numScaledVertices = 0;
	int numScaledElements = 0;
	int numMaterials = 0;
	int numPrimitives = 0;
	int numSmallVertices = 0;
	int numSmallElements = 0;
	int numSmallModel = 0;
	int numLargeVertices = 0;
	int numLargeElements = 0;
	int numLargeModel = 0;
	int numTinyModel = 0;
	float area = 0;
	float smallArea = 0;
	float maxlength = 0;

	Vector2 minmaxTc(0,0);

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

		numMaterials += staticFixed->m_model->numMaterials();
		Primitives prims = staticFixed->m_model->getStaticPrims();
		numPrimitives += prims.size();

		float len = staticFixed->m_model->getBoundingBox().getExtends().getLength();
		maxlength = std::max(len, maxlength);

		if (len < 4) numTinyModel++;

		Primitives::const_iterator it = prims.begin();
		float curArea = 0;
		int curVertices = 0;
		int curElements = 0;
		float scale = staticFixed->m_model->getMatrix().getScales();
		bool scaled = fabsf(scale - 1.0f) > 1e-6;
		if (scaled) {
			numScaled++;
		}
		scale *= scale;

		for (; it != prims.end(); ++it) {
			Primitive *prim = *it;
			if (prim->getType() != Primitive::MeshType)
				continue;
			MeshPrim *mesh = static_cast<MeshPrim *>(prim);

			curVertices += mesh->getNumVertexes();
			curElements += mesh->numElements();
			curArea += mesh->calcArea() * scale;
			mesh->minmaxTc(minmaxTc);
		}
		if (scaled) {
			numScaledVertices += curVertices;
			numScaledElements += curElements;
		}
		if (len < 2) {
			numSmallVertices += curVertices;
			numSmallElements += curElements;
			smallArea += curArea;
			numSmallModel++;
		}

		BoundingBox bbox = staticFixed->m_model->getBoundingBox();
		Vector3 extends = bbox.getExtends();
		float maxface = 0;
		if (extends.x < extends.y && extends.x < extends.z)
			maxface = extends.y * extends.z;
		else if (extends.y < extends.x && extends.y < extends.z)
			maxface = extends.x * extends.z;
		else
			maxface = extends.x * extends.y;

		if (maxface > 20 && curArea > 40) {
			numLargeVertices += curVertices;
			numLargeElements += curElements;
			numLargeModel++;
		}

		numVertices += curVertices;
		numElements += curElements;
		area += curArea;
	}
}




AX_END_NAMESPACE

