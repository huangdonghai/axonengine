/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// class Primitive
//------------------------------------------------------------------------------

Primitive::Primitive(Hint hint)
	: m_cachedId(0)
	, m_cachedFrame(-1)
	, m_hint(hint)
	, m_type(NoneType)
{
	m_isWorldSpace = false;
	m_material = 0;
	m_lightMap = 0;
	m_isMatrixSet = false;
	m_matrix.setIdentity();

	m_activedIndexes = 0;
}

Primitive::~Primitive()
{
	if (m_hint != HintFrame) {
		g_primitiveManager->hintUncache(this);
	}
}

//------------------------------------------------------------------------------
// class PointPrim
//------------------------------------------------------------------------------

PointPrim::PointPrim(Hint hint) : Primitive(hint)
{
	m_type = PointType;
	m_points = NULL;
}

PointPrim::~PointPrim()
{}

void PointPrim::initialize(int num_points)
{
	m_numPoints = num_points;

	TypeFree(m_points);
	m_points = TypeAlloc<DebugVertex>(m_numPoints);

	m_isDirty = true;
	m_isVertexBufferDirty = true;
	m_isIndexBufferDirty = true;
}

int PointPrim::getNumPoints() const
{
	return m_numPoints;
}

const DebugVertex *PointPrim::getPointsPointer() const
{
	return m_points;
}

DebugVertex *PointPrim::lock()
{
	return m_points;
}

void PointPrim::unlock() {
	m_isDirty = true;
	m_isVertexBufferDirty = true;
}

void PointPrim::setPointSize(float point_size)
{
	m_pointSize = point_size;
}

float PointPrim::getPointSize() const {
	return m_pointSize;
}

void PointPrim::setDrawOffsetCount(int pointOffset, int pointCount)
{
	m_pointDrawOffset = pointOffset;
	m_pointDrawCount = pointCount;
}

int PointPrim::getDrawOffset()
{
	return m_pointDrawOffset;
}

int PointPrim::getDrawCount()
{
	return m_pointDrawCount;
}

//------------------------------------------------------------------------------
// class LinePrim
//------------------------------------------------------------------------------

LinePrim::LinePrim(Hint hint)
	: Primitive(hint)
	, m_numVertexes(0)
	, m_numIndexes(0)
	, m_vertexes(NULL)
	, m_indexes(NULL)
	, m_lineWidth(1.0f)
{
	m_type = LineType;
}

LinePrim::~LinePrim()
{
	clear();
}

void LinePrim::init(int numverts, int numindexes)
{
	// clear old data first
	clear();

	m_numVertexes = numverts;
	m_numIndexes = numindexes;

//	m_vertexes = (DebugVertex *)Malloc(m_numVertexes * sizeof(DebugVertex));
	m_vertexes = TypeAlloc<DebugVertex>(m_numVertexes);
	m_indexes = TypeAlloc<ushort_t>(numindexes);

	m_isDirty = true;
	m_isVertexBufferDirty = true;
	m_isIndexBufferDirty = true;
}

void LinePrim::clear()
{
	m_numVertexes = 0;
	m_numIndexes = 0;
	TypeFree(m_vertexes);
	TypeFree(m_indexes);

}

int LinePrim::getNumVertexes() const
{
	return m_numVertexes;
}

const DebugVertex *LinePrim::getVertexesPointer() const
{
	return m_vertexes;
}

DebugVertex *LinePrim::lockVertexes()
{
	return m_vertexes;
}

void LinePrim::unlockVertexes()
{
	m_isDirty = true;
	m_isVertexBufferDirty = true;
}

int LinePrim::getNumIndexes() const
{
	return m_numIndexes;
}

const ushort_t *LinePrim::getIndexPointer() const
{
	return m_indexes;
}

ushort_t *LinePrim::lockIndexes()
{
	return m_indexes;
}

void LinePrim::unlockIndexes()
{
	m_isDirty = true;
	m_isIndexBufferDirty = true;
}

ushort_t LinePrim::getIndex(int order) const
{
	AX_ASSERT(order >= 0 && order < m_numIndexes);
	return m_indexes[order];
}

const LinePrim::VertexType &LinePrim::getVertex(int order) const
{
	AX_ASSERT(order >= 0 && order < m_numVertexes);
	return m_vertexes[order];
}

void LinePrim::lock()
{}

void LinePrim::setIndex(int order, int index)
{
	AX_ASSERT(order >= 0 && order < m_numIndexes);
	m_indexes[order] = index;
}

void LinePrim::setVertex(int order, const VertexType &vert)
{
	AX_ASSERT(order >= 0 && order < m_numVertexes);
	m_vertexes[order] = vert;
}

LinePrim::VertexType &LinePrim::getVertexRef(int order)
{
	AX_ASSERT(order >= 0 && order < m_numVertexes);
	return m_vertexes[order];
}

void LinePrim::setLineWidth(float line_width)
{
	m_lineWidth = line_width;
}

float LinePrim::getLineWidth() const
{
	return m_lineWidth;
}

void LinePrim::unlock()
{
	m_isDirty = true;
	m_isVertexBufferDirty = true;
	m_isIndexBufferDirty = true;
}

LinePrim *LinePrim::createAxis(Hint hint, float line_length)
{
	LinePrim *line = new LinePrim(hint);

	line->init(6, 6);

	// x
	line->m_vertexes[0].xyz = Vector3(0, 0, 0);
	line->m_vertexes[0].rgba = Rgba::Red;
	line->m_vertexes[1].xyz = Vector3(line_length, 0, 0);
	line->m_vertexes[1].rgba = Rgba::Red;
	line->m_indexes[0] = 0;
	line->m_indexes[1] = 1;

	// y
	line->m_vertexes[2].xyz = Vector3(0, 0, 0);
	line->m_vertexes[2].rgba = Rgba::Green;
	line->m_vertexes[3].xyz = Vector3(0, line_length, 0);
	line->m_vertexes[3].rgba = Rgba::Green;
	line->m_indexes[2] = 2;
	line->m_indexes[3] = 3;

	// z
	line->m_vertexes[4].xyz = Vector3(0, 0, 0);
	line->m_vertexes[4].rgba = Rgba::Blue;
	line->m_vertexes[5].xyz = Vector3(0, 0, line_length);
	line->m_vertexes[5].rgba = Rgba::Blue;
	line->m_indexes[4] = 4;
	line->m_indexes[5] = 5;

	return line;
}

LinePrim *LinePrim::createAxis(Hint hint, const Vector3 &origin, const Matrix3 &axis, float line_length)
{
	LinePrim *line = new LinePrim(hint);

	line->init(6, 6);

	// x
	line->m_vertexes[0].xyz = origin;
	line->m_vertexes[0].rgba = Rgba::Red;
	line->m_vertexes[1].xyz = origin + axis[0] * line_length;
	line->m_vertexes[1].rgba = Rgba::Red;
	line->m_indexes[0] = 0;
	line->m_indexes[1] = 1;

	// y
	line->m_vertexes[2].xyz = origin;
	line->m_vertexes[2].rgba = Rgba::Green;
	line->m_vertexes[3].xyz = origin + axis[1] * line_length;
	line->m_vertexes[3].rgba = Rgba::Green;
	line->m_indexes[2] = 2;
	line->m_indexes[3] = 3;

	// z
	line->m_vertexes[4].xyz = origin;
	line->m_vertexes[4].rgba = Rgba::Blue;
	line->m_vertexes[5].xyz = origin + axis[2] * line_length;
	line->m_vertexes[5].rgba = Rgba::Blue;
	line->m_indexes[4] = 4;
	line->m_indexes[5] = 5;

	return line;
}

LinePrim *LinePrim::createScreenRect(Hint hint, const Rect &rect, const Rgba &color)
{
	LinePrim *line = new LinePrim(hint);

	line->init(4, 8);

	// x
	line->m_vertexes[0].xyz = Vector3(rect.xMin(), rect.yMin(), 0);
	line->m_vertexes[0].rgba = color;
	line->m_vertexes[1].xyz = Vector3(rect.xMin(), rect.yMax(), 0);
	line->m_vertexes[1].rgba = color;
	line->m_vertexes[2].xyz = Vector3(rect.xMax(), rect.yMax(), 0);
	line->m_vertexes[2].rgba = color;
	line->m_vertexes[3].xyz = Vector3(rect.xMax(), rect.yMin(), 0);
	line->m_vertexes[3].rgba = color;

	line->m_indexes[0] = 0;
	line->m_indexes[1] = 1;
	line->m_indexes[2] = 1;
	line->m_indexes[3] = 2;
	line->m_indexes[4] = 2;
	line->m_indexes[5] = 3;
	line->m_indexes[6] = 3;
	line->m_indexes[7] = 0;

	return line;
}

bool LinePrim::setupScreenRect(LinePrim*& line, const Rect &rect, const Rgba &color)
{
	if (!line) {
		line = createScreenRect(HintDynamic, rect, color);
		return true;
	}
	line->lockVertexes();
	// x
	line->m_vertexes[0].xyz = Vector3(rect.xMin(), rect.yMin(), 0);
	line->m_vertexes[0].rgba = color;
	line->m_vertexes[1].xyz = Vector3(rect.xMin(), rect.yMax(), 0);
	line->m_vertexes[1].rgba = color;
	line->m_vertexes[2].xyz = Vector3(rect.xMax(), rect.yMax(), 0);
	line->m_vertexes[2].rgba = color;
	line->m_vertexes[3].xyz = Vector3(rect.xMax(), rect.yMin(), 0);
	line->m_vertexes[3].rgba = color;
	line->unlockVertexes();
	return true;
}

LinePrim *LinePrim::createLine(Hint hint, const Vector3 &from, const Vector3 &to, const Rgba &color)
{
	LinePrim *line = new LinePrim(hint);

	line->init(2, 2);

	// x
	line->m_vertexes[0].xyz = from;
	line->m_vertexes[0].rgba = color;
	line->m_vertexes[1].xyz = to;
	line->m_vertexes[1].rgba = color;

	line->m_indexes[0] = 0;
	line->m_indexes[1] = 1;

	return line;
}

bool LinePrim::setupLine(LinePrim*& line, const Vector3 &from, const Vector3 &to, const Rgba &color)
{
	bool isinit = false;

	if (!line) {
		isinit = true;
		line = new LinePrim(HintDynamic);
		line->init(2, 2);
		line->lockIndexes();
		line->m_indexes[0] = 0;
		line->m_indexes[1] = 1;
		line->unlockIndexes();
	}

	line->lockVertexes();
	line->m_vertexes[0].xyz = from;
	line->m_vertexes[0].rgba = color;
	line->m_vertexes[1].xyz = to;
	line->m_vertexes[1].rgba = color;
	line->unlockVertexes();

	return isinit;
}

bool LinePrim::setupAxis(LinePrim*& line, const Vector3 &origin, const Matrix3 &axis, float length, Rgba xcolor, Rgba ycolor, Rgba zcolor)
{
	bool isInit = false;

	if (!line) {
		isInit = true;
		line = new LinePrim(HintDynamic);
		line->init(6, 6);
		line->lock();
		line->m_indexes[0] = 0;
		line->m_indexes[1] = 1;
		line->m_indexes[2] = 2;
		line->m_indexes[3] = 3;
		line->m_indexes[4] = 4;
		line->m_indexes[5] = 5;
		line->unlock();
	}

	line->lockVertexes();
	line->m_vertexes[0].xyz = origin;
	line->m_vertexes[0].rgba = xcolor;
	line->m_vertexes[1].xyz = origin + axis[0] * length;
	line->m_vertexes[1].rgba = xcolor;

	// y
	line->m_vertexes[2].xyz = origin;
	line->m_vertexes[2].rgba = ycolor;
	line->m_vertexes[3].xyz = origin + axis[1] * length;
	line->m_vertexes[3].rgba = ycolor;

	// z
	line->m_vertexes[4].xyz = origin;
	line->m_vertexes[4].rgba = zcolor;
	line->m_vertexes[5].xyz = origin + axis[2] * length;
	line->m_vertexes[5].rgba = zcolor;
	line->unlockVertexes();

	return isInit;
}

bool LinePrim::setupBoundingBox(LinePrim*& line, const Vector3 &origin, const Matrix3 &axis, const BoundingBox &inbbox, float scale/* =1.0f */, Hint hint /* = Primitive::HintDynamic */)
{
	bool isfirst = false;
	if (!line) {
		line = new LinePrim(hint);
		line->init(32, 48);

		ushort_t *indexes = line->lockIndexes();
		for (int i = 0; i < 8; i++) {
			indexes[i*6+0] = i * 4;
			indexes[i*6+1] = i * 4 + 1;
			indexes[i*6+2] = i * 4;
			indexes[i*6+3] = i * 4 + 2;
			indexes[i*6+4] = i * 4;
			indexes[i*6+5] = i * 4 + 3;
		}
		line->unlockIndexes();

		isfirst = true;
	}

	AX_ASSERT(line->getNumVertexes() == 32);

	BoundingBox bbox;
	if (scale != 1.0f) {
		bbox.min = inbbox.min -(inbbox.max - inbbox.min) *(scale - 1.0f);
		bbox.max = inbbox.max +(inbbox.max - inbbox.min) *(scale - 1.0f);
	} else {
		bbox = inbbox;
	}

	VertexType *verts = line->lockVertexes();
	for (int i = 0; i < 8; i++) {
		Vector3 axisscale;
		axisscale.x = i&1;
		axisscale.y = i&2 ? 1 : 0;
		axisscale.z = i&4 ? 1 : 0;

		Vector3 vec = Vector3(0.25f, 0.25f, 0.25f) - axisscale * 0.5f;
		Vector3 width = bbox.getExtends(vec);

		verts[i*4].xyz = origin + axis * bbox.getLerped(axisscale);
		verts[i*4].rgba = Rgba::White;
		verts[i*4+1].xyz = verts[i*4].xyz + axis * Vector3(width.x, 0, 0);
		verts[i*4+1].rgba = Rgba::White;
		verts[i*4+2].xyz = verts[i*4].xyz + axis * Vector3(0, width.y, 0);
		verts[i*4+2].rgba = Rgba::White;
		verts[i*4+3].xyz = verts[i*4].xyz + axis * Vector3(0, 0, width.z);
		verts[i*4+3].rgba = Rgba::White;
	}
	line->unlockVertexes();

	return isfirst;
}

bool LinePrim::setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided)
{
	return setupCircle(line, origin, p0, p1, color, subdivided, false, Plane());
}

bool LinePrim::setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided, const Plane &plane)
{
	return setupCircle(line, origin, p0, p1, color, subdivided, true, plane);
}

bool LinePrim::setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided,Hint hint)
{
	return setupCircle(line, origin, p0, p1, color, subdivided, false, Plane(), hint);
}

bool LinePrim::setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided, bool clipplane, const Plane &plane,Hint hint /* = Primitive::HintDynamic */)
{
	AX_ASSERT(subdivided >= 3);
	bool first = false;

	if (!line) {
		line = new LinePrim(HintDynamic);
		line->init(subdivided, subdivided * 2);
		ushort_t *pidx = line->lockIndexes();
		for (int i = 0; i < subdivided; i++) {
			*pidx++ = i;
			*pidx++ = (i+1) % subdivided;
		}
		line->unlockIndexes();
		first = true;
	}

	VertexType *verts = line->lockVertexes();
	float step = AX_PI * 2.0f / subdivided;
	float angle = 0;
	for (int i = 0; i < subdivided; i++) {
		float s, c;

		Math::sincos(angle, s, c);
		verts[i].xyz = origin + p0 * s + p1 * c;
		verts[i].rgba = color;

		angle += step;

		if (!clipplane)
			continue;

		if (plane.distance(verts[i].xyz) >= -0.01) {
			verts[i].rgba.a = 64;
		}
	}
	line->unlockVertexes();
	return first;
}

LinePrim *LinePrim::createWorldBoundingBox( Hint hint, const BoundingBox &bbox, const Rgba &color )
{
	LinePrim *line = new LinePrim(hint);

	line->init(8, 24);
	VertexType *verts = line->lockVertexes();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				verts[i*4+j*2+k].xyz.x = i == 0 ? bbox.min.x : bbox.max.x; 
				verts[i*4+j*2+k].xyz.y = j == 0 ? bbox.min.y : bbox.max.y; 
				verts[i*4+j*2+k].xyz.z = k == 0 ? bbox.min.z : bbox.max.z; 
				verts[i*4+j*2+k].rgba = color; 
			}
		}
	}
	line->unlockVertexes();

	static ushort_t s_indices[] = {
		0, 1, 1, 3, 3, 2, 2, 0,
		4, 5, 5, 7, 7, 6, 6, 4,
		0, 4, 1, 5, 3, 7, 2, 6
	};
	AX_ASSERT(ArraySize(s_indices) == line->getNumIndexes());
	ushort_t *indexes = line->lockIndexes();
	memcpy(indexes, s_indices, line->getNumIndexes() * sizeof(ushort_t));
	line->unlockIndexes();

	return line;
}

//------------------------------------------------------------------------------
// class MeshPrim
//------------------------------------------------------------------------------

MeshPrim::MeshPrim(Hint hint)
	: Primitive(hint)
	, m_numVertexes(0)
	, m_vertexes(NULL)
	, m_numIndexes(0)
	, m_indexes(NULL)
{
	m_type = MeshType;
	m_isStriped = false;
}

MeshPrim::~MeshPrim()
{
	clear();
}

void MeshPrim::init(int numverts, int numidxes)
{
	if (numverts <= m_numVertexes && numidxes <= m_numIndexes)
		return;

	// clear old data first
	clear();

	m_numVertexes = numverts;
	m_numIndexes = numidxes;
	m_vertexes = TypeAlloc<MeshVertex>(numverts);
	m_indexes = TypeAlloc<ushort_t>(numidxes);
}

void MeshPrim::clear()
{
	m_numVertexes = 0;
	m_numIndexes = 0;
	TypeFree(m_vertexes);
	TypeFree(m_indexes);
}

int MeshPrim::getNumVertexes() const
{
	return m_numVertexes;
}

const MeshVertex *MeshPrim::getVertexesPointer() const
{
	return m_vertexes;
}

MeshVertex *MeshPrim::lockVertexes()
{
	return m_vertexes;
}

void MeshPrim::unlockVertexes()
{
	m_isDirty = true;
	m_isVertexBufferDirty = true;
}

const ushort_t *MeshPrim::getIndexPointer() const
{
	return m_indexes;
}

int MeshPrim::getNumIndexes() const
{
	return m_numIndexes;
}

ushort_t *MeshPrim::lockIndexes()
{
	return m_indexes;
}

void MeshPrim::unlockIndexes()
{
	m_isDirty = true;
	m_isIndexBufferDirty = true;
}

LinePrim *MeshPrim::getTangentLine(float len) const
{
	if (m_numVertexes == 0)
		return NULL;

	LinePrim *line = new LinePrim(HintFrame);

	line->init(m_numVertexes * 6, m_numVertexes * 6);
	DebugVertex *verts = line->lockVertexes();
	ushort_t *idxes = line->lockIndexes();

	for (int i = 0; i < m_numVertexes; i++) {
		verts[i*6+0].xyz = m_vertexes[i].xyz;
		verts[i*6+0].rgba = Rgba::Red;

		verts[i*6+1].xyz = m_vertexes[i].xyz + m_vertexes[i].tangent.getNormalized() * len;
		verts[i*6+1].rgba = Rgba::Red;

		verts[i*6+2].xyz = m_vertexes[i].xyz;
		verts[i*6+2].rgba = Rgba::Green;

		verts[i*6+3].xyz = m_vertexes[i].xyz + m_vertexes[i].binormal.getNormalized() * len;
		verts[i*6+3].rgba = Rgba::Green;

		verts[i*6+4].xyz = m_vertexes[i].xyz;
		verts[i*6+4].rgba = Rgba::Blue;

		verts[i*6+5].xyz = m_vertexes[i].xyz + m_vertexes[i].normal.getNormalized() * len;
		verts[i*6+5].rgba = Rgba::Blue;

		idxes[i*6+0] = i*6+0;
		idxes[i*6+1] = i*6+1;
		idxes[i*6+2] = i*6+2;
		idxes[i*6+3] = i*6+3;
		idxes[i*6+4] = i*6+4;
		idxes[i*6+5] = i*6+5;
	}

	line->unlockVertexes();
	line->unlockIndexes();

	return line;
}

LinePrim *MeshPrim::getNormalLine(float len) const
{
	if (m_numVertexes == 0)
		return NULL;

	LinePrim *line = new LinePrim(HintFrame);

	line->init(m_numVertexes * 2, m_numVertexes * 2);
	DebugVertex *verts = line->lockVertexes();
	ushort_t *idxes = line->lockIndexes();

	for (int i = 0; i < m_numVertexes; i++) {
		verts[i*2+0].xyz = m_vertexes[i].xyz;
		verts[i*2+0].rgba = Rgba::Blue;

		verts[i*2+1].xyz = m_vertexes[i].xyz + m_vertexes[i].normal.getNormalized() * len;
		verts[i*2+1].rgba = Rgba::Blue;

		idxes[i*2+0] = i*2+0;
		idxes[i*2+1] = i*2+1;
	}

	line->unlockVertexes();
	line->unlockIndexes();

	return line;
}

void MeshPrim::computeTangentSpace()
{
	if (m_isStriped) {
		Errorf("MeshPrim::computeTangentSpace: cann't compute tangent space when is striped");
		return;
	}

	lockVertexes();
	g_simd->computeTangentSpace(m_vertexes, m_numVertexes, m_indexes, m_numIndexes);
	unlockVertexes();
}

void MeshPrim::computeTangentSpaceSlow()
{
	if (m_isStriped) {
		Errorf("can't compute tangent space when is striped");
		return;
	}

	lockVertexes();
	g_simd->computeTangentSpaceSlow(m_vertexes, m_numVertexes, m_indexes, m_numIndexes);
	unlockVertexes();
}



// static helper function
MeshPrim *MeshPrim::createScreenQuad(Hint hint, const Rect &rect, const Rgba &color, Material *material, const Vector4 &st)
{
	MeshPrim *quad = new MeshPrim(hint);

	quad->init(4, 6);
	MeshVertex *verts = quad->lockVertexes();
	verts[0].xyz = Vector3(rect.x, rect.y, 0.0f);
	verts[0].st.x = st[0];
	verts[0].st.y = st[1];
	verts[0].rgba = color;
	verts[0].st2 = Vector2(0, 0);
	verts[0].normal = Vector3(0, 0, 1);
	verts[0].tangent = Vector3(1, 0, 0);
	verts[0].binormal = Vector3(0, 1, 0);

	verts[1].xyz = Vector3(rect.x + rect.width, rect.y, 0.0f);
	verts[1].st.x = st[2];
	verts[1].st.y = st[1];
	verts[1].rgba = color;
	verts[1].st2 = Vector2(0, 0);
	verts[1].normal = Vector3(0, 0, 1);
	verts[1].tangent = Vector3(1, 0, 0);
	verts[1].binormal = Vector3(0, 1, 0);

	verts[2].xyz = Vector3(rect.x, rect.y + rect.height, 0.0f);
	verts[2].st.x = st[0];
	verts[2].st.y = st[3];
	verts[2].rgba = color;
	verts[2].st2 = Vector2(0, 0);
	verts[2].normal = Vector3(0, 0, 1);
	verts[2].tangent = Vector3(1, 0, 0);
	verts[2].binormal = Vector3(0, 1, 0);

	verts[3].xyz = Vector3(rect.x + rect.width, rect.y + rect.height, 0.0f);
	verts[3].st.x = st[2];
	verts[3].st.y = st[3];
	verts[3].rgba = color;
	verts[3].st2 = Vector2(0, 0);
	verts[3].normal = Vector3(0, 0, 1);
	verts[3].tangent = Vector3(1, 0, 0);
	verts[3].binormal = Vector3(0, 1, 0);
	quad->unlockVertexes();

	ushort_t *indexes = quad->lockIndexes();
	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 2;
	indexes[4] = 1;
	indexes[5] = 3;
	quad->unlockIndexes();

	quad->setMaterial(material);

	return quad;
}

bool MeshPrim::setupScreenQuad(MeshPrim*& quad, const Rect &rect, const Rgba &color, Material *material, const Vector4 &st)
{
	MeshVertex *verts = quad->lockVertexes();
	verts[0].xyz = Vector3(rect.x, rect.y, 0.0f);
	verts[0].st.x = st[0];
	verts[0].st.y = st[1];
	verts[0].rgba = color;
	verts[0].st2 = Vector2(0, 0);
	verts[0].normal = Vector3(0, 0, 1);
	verts[0].tangent = Vector3(1, 0, 0);
	verts[0].binormal = Vector3(0, 1, 0);

	verts[1].xyz = Vector3(rect.x + rect.width, rect.y, 0.0f);
	verts[1].st.x = st[2];
	verts[1].st.y = st[1];
	verts[1].rgba = color;
	verts[1].st2 = Vector2(0, 0);
	verts[1].normal = Vector3(0, 0, 1);
	verts[1].tangent = Vector3(1, 0, 0);
	verts[1].binormal = Vector3(0, 1, 0);

	verts[2].xyz = Vector3(rect.x, rect.y + rect.height, 0.0f);
	verts[2].st.x = st[0];
	verts[2].st.y = st[3];
	verts[2].rgba = color;
	verts[2].st2 = Vector2(0, 0);
	verts[2].normal = Vector3(0, 0, 1);
	verts[2].tangent = Vector3(1, 0, 0);
	verts[2].binormal = Vector3(0, 1, 0);

	verts[3].xyz = Vector3(rect.x + rect.width, rect.y + rect.height, 0.0f);
	verts[3].st.x = st[2];
	verts[3].st.y = st[3];
	verts[3].rgba = color;
	verts[3].st2 = Vector2(0, 0);
	verts[3].normal = Vector3(0, 0, 1);
	verts[3].tangent = Vector3(1, 0, 0);
	verts[3].binormal = Vector3(0, 1, 0);
	quad->unlockVertexes();

	ushort_t *indexes = quad->lockIndexes();
	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 2;
	indexes[4] = 1;
	indexes[5] = 3;
	quad->unlockIndexes();

	quad->m_material = material;

	return true;
}

bool MeshPrim::setupFan(MeshPrim*& mesh, const Vector3 &center, const Vector3 &v0, const Vector3 &v1, float start, float end, Rgba color, int subdivided, Material *material)
{
	AX_ASSERT(subdivided >= 1);

	bool isinit = false;
	int numverts = subdivided + 2;
	int numidxes = subdivided * 3;
	if (!mesh) {
		isinit = true;
		mesh = new MeshPrim(HintDynamic);
		mesh->init(numverts, numidxes);
		mesh->lockIndexes();
		for (int i = 0; i < subdivided; i++) {
			mesh->m_indexes[i*3] = 0;
			mesh->m_indexes[i*3+1] = i + 1;
			mesh->m_indexes[i*3+2] = i + 2;
		}
		memset(mesh->m_vertexes, 0, sizeof(MeshVertex) * numverts);
		mesh->setMaterial(material);
		mesh->unlockIndexes();
	}

	mesh->lockVertexes();
	mesh->m_vertexes[0].xyz = center;
	mesh->m_vertexes[0].rgba = color;
	float step =(end - start) / subdivided;
	for (int i = 0; i <= subdivided; i++) {
		float angle = start + step * i;
		float s, c;
		Math::sincos(angle, s, c);
		mesh->m_vertexes[i+1].xyz = center + v0 * c + v1 * s;
		mesh->m_vertexes[i+1].rgba = color;
	}
	mesh->unlockVertexes();

	return true;
}


bool MeshPrim::setupPolygon(MeshPrim*& mesh, int numverts, const Vector3 *verts, Rgba color, Material *material)
{
	AX_ASSERT(numverts >= 3);
	bool isinit = false;
	int numidxes =(numverts - 2) * 3;
	if (!mesh) {
		isinit = true;
		mesh = new MeshPrim(HintDynamic);
		mesh->init(numverts, numidxes);
		mesh->lockIndexes();
		for (int i = 0; i < numverts - 2; i++) {
			mesh->m_indexes[i*3] = 0;
			mesh->m_indexes[i*3+1] = i + 1;
			mesh->m_indexes[i*3+2] = i + 2;
		}
		mesh->unlockVertexes();
		memset(mesh->m_vertexes, 0, numverts * sizeof(VertexType));
		mesh->setMaterial(material);
	}

	AX_ASSERT(numverts = mesh->getNumVertexes());
	mesh->lockVertexes();
	for (int i = 0; i < numverts; i++) {
		mesh->m_vertexes[i].xyz = verts[i];
		mesh->m_vertexes[i].rgba = color;
	}
	mesh->unlockVertexes();

	return isinit;
}

bool MeshPrim::setupBox(MeshPrim*& mesh, const BoundingBox &bbox, const Rgba &color)
{
	int numverts = 8;
	int numidxes = 36;

	if (!mesh){
		mesh = new MeshPrim(HintDynamic);
		mesh->init(8,36);
	}

	MeshVertex *vertexes = mesh->lockVertexes();
	ushort_t *indexes  = mesh->lockIndexes();

	Vector3 bSize = bbox.max - bbox.min;
    vertexes[0].xyz  = bbox.min;
	vertexes[0].rgba = color;
	vertexes[1].xyz  = Vector3(bbox.min.x,bbox.min.y,bbox.min.z + bSize.z);
	vertexes[1].rgba = color;
	vertexes[2].xyz  = Vector3(bbox.min.x,bbox.min.y + bSize.y,bbox.min.z + bSize.z);
	vertexes[2].rgba = color;
	vertexes[3].xyz  = Vector3(bbox.min.x,bbox.min.y + bSize.y,bbox.min.z);
	vertexes[3].rgba  = color;
	vertexes[4].xyz  = Vector3(bbox.min.x + bSize.x,bbox.min.y,bbox.min.z);
	vertexes[4].rgba = color;
	vertexes[5].xyz  = Vector3(bbox.min.x + bSize.x,bbox.min.y,bbox.min.z + bSize.z);
	vertexes[5].rgba = color;
	vertexes[6].xyz  = Vector3(bbox.min.x + bSize.x,bbox.min.y + bSize.y,bbox.min.z + bSize.z);
	vertexes[6].rgba = color;
	vertexes[7].xyz  = Vector3(bbox.min.x + bSize.x,bbox.min.y + bSize.y,bbox.min.z);
	vertexes[7].rgba = color;

	int boxindexes[36] = {0,1,3,3,1,2,3,2,7,7,2,6,7,6,4,4,6,5,4,5,0,0,5,1,1,5,2,2,5,6,3,7,0,0,7,4};
	memcpy(indexes,boxindexes,sizeof(boxindexes));

    mesh->unlockVertexes();
	mesh->unlockIndexes();

	return true;
 	}

// NOTES: debug cone, no texture coords
bool MeshPrim::setupCone(MeshPrim*& cone, const Vector3 &center, float radius, const Vector3 &top, const Rgba &color, int subdivided)
{
	int numverts = subdivided + 2;		// number of subdivided + top + center
	int numidxes = subdivided * 2 * 3;
	bool isinit = false;
	if (!cone) {
		isinit = true;
		cone = new MeshPrim(HintDynamic);
		cone->init(numverts, numidxes);

		// triangles
		ushort_t *indexes = cone->lockIndexes();
		for (int i = 0; i < subdivided; i++) {
			int idx0 = i + 2;
			int idx1 =(i + 1) % subdivided + 2;

			*indexes++ = idx0;
			*indexes++ = idx1;
			*indexes++ = 0;
			*indexes++ = idx1;
			*indexes++ = idx0;
			*indexes++ = 1;
		}
		cone->unlockIndexes();
	}

	AX_ASSERT(numverts == cone->getNumVertexes());

	// vertexes
	MeshVertex *verts = cone->lockVertexes();
		// reset to zero
		memset(verts, 0, sizeof(MeshVertex) * numverts);

		// vertex 0 is top
		verts[0].xyz = top;
		verts[0].rgba = color;

		// vertex 1 is center
		verts[1].xyz = center;
		verts[1].rgba = color;

		// round subdivided

		Vector3 right, forward;
		Vector3 up = top - center;		// up is cone point to

		// calculate right and forward
		if (up.x == 0.0f && up.y == 0.0f)
			forward = Vector3(0.0f, 1.0f, 0.0f);
		else
			forward = Vector3(0.0f, 0.0f, 1.0f);

		right = up ^ forward;
		forward = right ^ up;

		right.normalize();
		forward.normalize();

		right *= radius;
		forward *= radius;

		int i;
		for (i = 0; i < subdivided; i++) {
			float alpha = AX_PI * 2.0f / subdivided * i;
			verts[2 + i].xyz = center + right * cosf(alpha) + forward * sinf(alpha);
			verts[2 + i].rgba = color;
		}

	cone->unlockVertexes();

	return isinit;
}

bool MeshPrim::setupHexahedron(MeshPrim*& mesh, Vector3 volumeverts[8])
{
	int numverts = 8;
	int numindexes = 6 * 2 * 3;

	bool isinit = false;

	if (!mesh) {
		isinit = true;
		mesh = new MeshPrim(HintDynamic);
		mesh->init(numverts, numindexes);

		// triangles
		static ushort_t s_idxes[] = {
			0, 2, 1, 1, 2, 3,
			2, 6, 3, 3, 6, 7,
			6, 4, 7, 7, 4, 5,
			4, 0, 5, 5, 0, 1,
			1, 3, 5, 5, 3, 7,
			0, 2, 4, 4, 2, 6
		};

		ushort_t *indexes = mesh->lockIndexes();
		memcpy(indexes, s_idxes, numindexes * sizeof(ushort_t));
		mesh->unlockIndexes();

		// set verts to 0
		MeshVertex *verts = mesh->lockVertexes();
		memset(verts, 0, numverts * sizeof(MeshVertex));
		mesh->unlockIndexes();
	}

	if (!volumeverts) {
		return isinit;
	}

	MeshVertex *verts = mesh->lockVertexes();
	for (int i = 0; i < numverts; i++) {
		verts[i].xyz = volumeverts[i];
	}
	mesh->unlockIndexes();

	return isinit;
}

#if 0
void Mesh::setCurrentIndexNum(int currentIndexNum)
{
	m_currentIndexNum = currentIndexNum;
}

int Mesh::getCurrentIndexNum() const
{
	return m_currentIndexNum;
}
#endif

//------------------------------------------------------------------------------
// class TextPrim
//------------------------------------------------------------------------------

TextPrim::TextPrim(Hint hint) : Primitive(hint), m_horizonAlign(Center), m_verticalAlign(VCenter)
{
	m_type = TextType;
}

TextPrim::~TextPrim() {
}

void TextPrim::init(const Rect &rect, Rgba color, float aspect, int format, Font *font, const String &text)
{
	m_rect = rect;
	m_color = color;
	m_aspect = aspect;
	m_format = format;
	m_font = font;
	m_text = text;
	m_isSimpleText = false;
}

void TextPrim::initSimple(const Vector3 &xyz, Rgba color, const String &text, bool fixedWidth)
{
	m_isSimpleText = true;
	m_color = color;
	m_position = xyz;
	m_text = text;
	m_format = 0;
	m_aspect = 1.0f;

	if (fixedWidth)
		m_font = g_consoleFont;
	else
		m_font = g_defaultFont;
}

void TextPrim::clear()
{
}

// static helper function
TextPrim *TextPrim::createSimpleText(Hint hint, const Vector3 &xyz, const Rgba &color, const String &text, bool fixedWidth)
{
	TextPrim *rp_text = new TextPrim(hint);
	rp_text->initSimple(xyz, color, text, fixedWidth);

	return rp_text;
}

TextPrim *TextPrim::createText(Hint hint, const Rect &rect, Font *font, const String &text, const Rgba &color, HorizonAlign halign, VerticalAlign valign, int format, float aspect)
{
	TextPrim *rp_text = new TextPrim(hint);

	if (text.size() == 0)
		Errorf("TextPrim::CreateText: null text");

	rp_text->m_rect = rect;
	rp_text->m_font = font;
	rp_text->m_text = text;
	rp_text->m_color = color;
	rp_text->m_format = format;
	rp_text->m_aspect = aspect;
	rp_text->m_isSimpleText = false;
	rp_text->m_horizonAlign = halign;
	rp_text->m_verticalAlign = valign;

	return rp_text;
}


//------------------------------------------------------------------------------
// class ChunkPrim
//------------------------------------------------------------------------------

ChunkPrim::ChunkPrim(Hint hint)
	: Primitive(hint)
	, m_numVertexes(0)
	, m_vertexes(NULL)
	, m_numIndexes(0)
	, m_indexes(NULL)
{
	m_type = ChunkType;
	m_layerVisible = false;
	m_isZonePrim = false;
}

ChunkPrim::~ChunkPrim()
{
	clear();
}

void ChunkPrim::init(int numverts, int numidxes)
{
	clear();

	m_numVertexes = numverts;
	m_numIndexes = numidxes;

	m_vertexes = TypeAlloc<ChunkVertex>(numverts);
	m_indexes = TypeAlloc<ushort_t>(numidxes);
}

void ChunkPrim::clear()
{
	TypeFree(m_vertexes);
	TypeFree(m_indexes);
	m_numVertexes = 0;
	m_numIndexes = 0;
}

int ChunkPrim::getNumVertexes() const
{
	return m_numVertexes;
}

const ChunkVertex *ChunkPrim::getVertexesPointer() const
{
	return m_vertexes;
}

ChunkVertex *ChunkPrim::lockVertexes()
{
	return m_vertexes;
}

void ChunkPrim::unlockVertexes()
{
	m_isDirty = true;
	m_isVertexBufferDirty = true;
}

int ChunkPrim::getNumIndexes() const
{
	return m_numIndexes;
}

const ushort_t *ChunkPrim::getIndexesPointer() const
{
	return m_indexes;
}

ushort_t *ChunkPrim::lockIndexes()
{
	return m_indexes;
}

void ChunkPrim::unlockIndexes()
{
	m_isDirty = true;
	m_isIndexBufferDirty = true;
}

void ChunkPrim::setTerrainRect(const Vector4 &rect)
{
	m_terrainRect = rect;
}

Vector4 ChunkPrim::getTerrainRect() const
{
	return m_terrainRect;
}

void  ChunkPrim::setColorTexture(Texture *color_texture)
{
	m_colorTexture = color_texture;
}

Texture *ChunkPrim::getColorTexture()
{
	return m_colorTexture;
}

void ChunkPrim::setNormalTexture(Texture *dsdt)
{
	m_normalTexture = dsdt;
}

Texture *ChunkPrim::getNormalTexture()
{
	return m_normalTexture;
}

void ChunkPrim::setChunkRect(const Vector4 &rect)
{
	m_chunkRect = rect;
}

Vector4 ChunkPrim::getChunkRect() const
{
	return m_chunkRect;
}

void ChunkPrim::setNumLayers(int n)
{
	m_numLayers = std::min<int>(n,MAX_LAYERS);
	m_isDirty = true;
}

int ChunkPrim::getNumLayers() const
{
	return m_numLayers;
}

void ChunkPrim::setLayers(int index, Texture *alpha, Material *detail, const Vector2 &scale, bool isVerticalProjection)
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		Debugf("MAX_LAYERS exceeded\n");
		return;
	}

	m_layers[index].alphaTex = alpha;
	m_layers[index].detailMat = detail;
	m_layers[index].scale = scale;
	m_layers[index].isVerticalProjection = isVerticalProjection;
}

Texture *ChunkPrim::getLayerAlpha(int index) const
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		return nullptr;
	}

	return m_layers[index].alphaTex.get();
}

Material *ChunkPrim::getLayerDetail(int index) const
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		return nullptr;
	}

	return m_layers[index].detailMat.get();
}

Vector2 ChunkPrim::getLayerScale(int index) const
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		return Vector2(1,1);
	}

	return m_layers[index].scale;
}

bool ChunkPrim::isLayerVerticalProjection(int index) const
{

	if (index >= ChunkPrim::MAX_LAYERS) {
		return false;
	}

	return m_layers[index].isVerticalProjection;
}

//------------------------------------------------------------------------------
// class GroupPrim
//------------------------------------------------------------------------------

GroupPrim::GroupPrim(Hint hint) : Primitive(hint)
{
	m_type = GroupType;
}

GroupPrim::~GroupPrim()
{
	clear();
}

void GroupPrim::addPrimitive(Primitive *prim, bool needfree)
{
	m_primitives.push_back(prim);
	m_needFrees.push_back(needfree);
}

int GroupPrim::getPrimitiveCount() const
{
	return s2i(m_primitives.size());
}

Primitive *GroupPrim::getPrimitive(int index)
{
	return m_primitives[index];
}

void GroupPrim::clear()
{
	for (size_t i = 0; i < m_primitives.size(); i++) {
		if (m_needFrees[i]) {
			delete m_primitives[i];
		}
	}

	m_primitives.clear();
	m_needFrees.clear();
}

//--------------------------------------------------------------------------
// class RefPrim
//--------------------------------------------------------------------------

RefPrim::RefPrim(Hint hint) : Primitive(hint)
{
	m_type = ReferenceType;
	m_refered = nullptr;
	m_numIndexes = 0;
	m_indexes = nullptr;
}

RefPrim::~RefPrim()
{
	TypeFree(m_indexes);
}

Primitive *RefPrim::getRefered() const
{
	return m_refered;
}

void RefPrim::setRefered(Primitive *refered)
{
	m_refered = refered;
}

void RefPrim::init(Primitive *refered, int numindexes /*= 0 */)
{
	if (m_refered || m_numIndexes) {
		Errorf("RefPrim::init: already initialized");
		return;
	}

	m_refered = refered;
	m_numIndexes = numindexes;
	m_indexes = TypeAlloc<ushort_t>(numindexes);
}

int RefPrim::getNumIndexes() const
{
	return m_numIndexes;
}

const ushort_t *RefPrim::getIndexesPointer() const
{
	return m_indexes;
}

ushort_t *RefPrim::lockIndexes()
{
	return m_indexes;
}

void RefPrim::unlockIndexes()
{
	m_isDirty = true;
	m_isIndexBufferDirty = true;
}

//--------------------------------------------------------------------------
// class InstancePrim
//
// geometry instance primitive
//--------------------------------------------------------------------------

InstancePrim::InstancePrim(Hint hint) : Primitive(hint)
{
	m_type = InstancingType;
	m_instanced = nullptr;
	m_params.reserve(16);
}

InstancePrim::~InstancePrim()
{
}

Primitive *InstancePrim::getInstanced() const
{
	return m_instanced;
}

void InstancePrim::setInstanced(Primitive *instanced)
{
	m_instanced = instanced;
	m_isDirty = true;
	setMaterial(instanced->getMaterial());
}

void InstancePrim::addInstance(const Param &param)
{
	m_params.push_back(param);
	m_isDirty = true;
}

void InstancePrim::addInstance(const Matrix &mtx, const Vector4 &user)
{
	Param param;
	param.worldMatrix = mtx;
	param.userDefined = user;
	addInstance(param);
}

int InstancePrim::getNumInstance() const
{
	return s2i(m_params.size());
}

const InstancePrim::Param &InstancePrim::getInstance(int index) const
{
	AX_ASSERT(index < getNumInstance());
	return m_params[index];
}

const InstancePrim::ParamSeq &InstancePrim::getAllInstances() const
{
	return m_params;
}

void InstancePrim::clearAllInstances()
{
	m_params.clear();
}

void InstancePrim::setInstances(const ParamSeq &params)
{
	m_params = params;
}



//--------------------------------------------------------------------------
// class PrimitiveManager
//--------------------------------------------------------------------------

PrimitiveManager::PrimitiveManager()
{
}

PrimitiveManager::~PrimitiveManager()
{
}

void PrimitiveManager::hintUncache(Primitive *prim)
{
	int id = prim->getCachedId();

	if (!id)
		return;

	if (isFrameHandle(id))
		return;

	m_waitUncache.push_back(id-1);
}

AX_END_NAMESPACE
