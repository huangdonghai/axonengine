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

	struct EdgeInfo {
		int p0, p1; // edge's index
		int p2; // triangle another index
		float length;

		bool operator<(const EdgeInfo &rhs)
		{
			return length < rhs.length;
		}

		EdgeInfo() {}
		void init(UnwrapHelper *obj, int index0, int index1, int index2);
	};

	struct TriangleInfo {
		bool isRightTriangle;
		bool isUsed;
		EdgeInfo edgeInfos[3];

		void init(UnwrapHelper *obj, int index);
	};

	struct Triangle {
		union {
			struct {
				ushort_t p0, p1, p2;
			};
			ushort_t p[3];
		};
	};

	struct Quad {
		union {
			struct {
				ushort_t p0, p1, p2, p3;
			};
			ushort_t p[4];
		};

		Quad *sides[4];
		int chart;

		inline void tryLink(Quad *rhs)
		{
			if (!sides[0] && !rhs->sides[2]) {
				if (p0 == rhs->p2 && p1 == rhs->p3) {
					sides[0] = rhs;
					rhs->sides[2] = this;
					return;
				}
			}

			if (!sides[1] && !rhs->sides[3]) {
				if (p0 == rhs->p1 && p2 == rhs->p3) {
					sides[1] = rhs;
					rhs->sides[3] = this;
					return;
				}
			}

			if (!sides[2] && !rhs->sides[0]) {
				if (p2 == rhs->p0 && p3 == rhs->p1) {
					sides[2] = rhs;
					rhs->sides[0] = this;
					return;
				}
			}

			if (!sides[3] && !rhs->sides[1]) {
				if (p1 == rhs->p0 && p3 == rhs->p2) {
					sides[3] = rhs;
					rhs->sides[1] = this;
					return;
				}
			}
		}

		inline bool isAllSideLinked() const
		{
			return sides[0] && sides[1] && sides[2] && sides[3];
		}

		inline void setChart_r(int _chart)
		{
			if (chart >= 0)
				return;

			chart = _chart;
			for (int i = 0; i < 4; i++) {
				if (sides[i])
					sides[i]->setChart_r(_chart);
			}
		}
	};

	struct UnwrapHelper {
		UnwrapHelper(MeshPrim *prim, float scale, float texelsPerMeter);
		~UnwrapHelper();

		int findAdjacentTriangle(int index);
		void process();
		Quad *allocQuad() { Quad *result = new Quad; TypeZero(result); m_quads.push_back(result); return result; }

		MeshPrim *m_prim;
		const MeshVertex *m_vertices;
		const ushort_t *m_indices;
		const Triangle *m_triangles;
		int m_numTris;
		float m_scale;
		float m_texelsPerMeter;

		std::vector<TriangleInfo> m_triInfos;
		std::vector<int> m_orphanTris;
		std::vector<Quad *> m_quads;
	};

	UnwrapHelper::UnwrapHelper(MeshPrim *prim, float scale, float texelsPerMeter)
	{
		m_prim = prim;
		m_vertices = prim->getVertexesPointer();
		m_indices = prim->getIndexPointer();
		m_triangles = reinterpret_cast<const Triangle *>(m_indices);
		m_numTris = prim->getNumIndexes() / 3;
		m_triInfos.resize(m_numTris);
		for (int i = 0; i < m_numTris; i++) {
			m_triInfos[i].init(this, i);
		}
		m_scale = scale;
		m_texelsPerMeter = texelsPerMeter;

		process();
	}

	UnwrapHelper::~UnwrapHelper()
	{
		{
			std::vector<Quad *>::const_iterator it = m_quads.begin();
			for (; it != m_quads.end(); ++it ) {
				delete *it;
			}
		}
	}

	void UnwrapHelper::process()
	{
		// find quads
		for (int i = 0; i < m_numTris; i++) {
			const Triangle &tri0 = m_triangles[i];
			TriangleInfo &info = m_triInfos[i];

			if (info.isUsed)
				continue;

			info.isUsed = true;

			//if (!info.isRightTriangle) {
			//	m_orphanTris.push_back(i);
			//	continue;
			//}
			
			int adjTri = findAdjacentTriangle(i);
			if (adjTri < 0) {
				m_orphanTris.push_back(i);
				continue;
			}
			const Triangle &tri1 = m_triangles[adjTri];
			m_triInfos[adjTri].isUsed = true;

			const EdgeInfo &e0 = info.edgeInfos[2];
			const EdgeInfo &e1 = m_triInfos[adjTri].edgeInfos[2];

			Quad *quad = allocQuad();
			quad->p0 = e0.p2;
			quad->p1 = e0.p0;
			quad->p2 = e0.p1;
			quad->p3 = e1.p2;
			quad->chart = -1;
		}

		// link quads to each other
		for (std::vector<Quad *>::iterator i = m_quads.begin(); i != m_quads.end(); ++i) {
			Quad *self = *i;

			if (self->isAllSideLinked())
				continue;

			for (std::vector<Quad *>::iterator j = m_quads.begin(); j != m_quads.end(); ++j) {
				Quad *quad = *j;
				if (quad == self)
					continue;

				self->tryLink(quad);

				if (self->isAllSideLinked())
					break;
			}
		}

		// find quad chart
		int chart = 0;
		for (std::vector<Quad *>::iterator i = m_quads.begin(); i != m_quads.end(); ++i) {
			Quad *self = *i;

			if (self->chart >= 0)
				continue;

			self->setChart_r(chart);
			chart++;
		}
	}

	int UnwrapHelper::findAdjacentTriangle(int index)
	{
		const EdgeInfo &edge = m_triInfos[index].edgeInfos[2];
		for (int i = 0; i < m_numTris; i++) {
			if (m_triInfos[i].isUsed)
				continue;
			const EdgeInfo &adjEdge = m_triInfos[i].edgeInfos[2];
			if (adjEdge.p1 == edge.p0 && adjEdge.p0 == edge.p1/* && m_triInfos[i].isRightTriangle*/)
				return i;
		}

		return -1;
	}

	void TriangleInfo::init(UnwrapHelper *obj, int index)
	{
		isUsed = false;

		const Triangle &tri = obj->m_triangles[index];
		edgeInfos[0].init(obj, tri.p0, tri.p1, tri.p2);
		edgeInfos[1].init(obj, tri.p1, tri.p2, tri.p0);
		edgeInfos[2].init(obj, tri.p2, tri.p0, tri.p1);

		std::sort(edgeInfos, edgeInfos+3);

		float c = edgeInfos[0].length * edgeInfos[0].length + edgeInfos[1].length * edgeInfos[1].length;
		if (c <= 0)
			return;

		c = sqrt(c);
		c = edgeInfos[2].length / c;
		if (c > 0.5 && c < 1.5)
			isRightTriangle = true;
	}


	void EdgeInfo::init(UnwrapHelper *obj, int index0, int index1, int index2)
	{
		p0 = index0; p1 = index1; p2 = index2;
		length = (obj->m_vertices[p0].position - obj->m_vertices[p1].position).getLength();
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
//			UnwrapHelper unwarper(mesh, 1, 4);

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

