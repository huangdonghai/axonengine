/*
Copyright (c) 2008-2009 Huang Donghai
Copyright (c) 2010-2011 NetEase.com, Inc.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
std::list<Primitive *> Primitive::ms_framePrim;

Primitive::Primitive(Hint hint)
	: m_hint(hint)
	, m_type(NoneType)
{
	m_isDepthHack = false;

	m_syncFrame = -1;
	m_isWorldSpace = false;
	m_material = 0;
	m_isMatrixSet = false;
	m_matrix.setIdentity();

	m_activedIndexes = 0;

	m_overloadInstanceObject = 0;
	m_overloadMaterial = 0;
	m_overloadIndexObject = 0;

	if (hint == HintFrame)
		ms_framePrim.push_back(this);
}

Primitive::~Primitive()
{
	SafeDelete(m_material);
}

//------------------------------------------------------------------------------

PointPrim::PointPrim(Hint hint) : Primitive(hint)
{
	m_type = PointType;
	m_points = 0;
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

void PointPrim::unlock()
{
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

LinePrim::LinePrim(Hint hint)
	: Primitive(hint)
	, m_numVertexes(0)
	, m_numIndexes(0)
	, m_vertexes(0)
	, m_indexes(0)
	, m_lineWidth(1.0f)
{
	m_type = LineType;

	m_vertexObject = new VertexObject();
	m_indexObject = new IndexObject();
}

LinePrim::~LinePrim()
{
	SafeDelete(m_indexObject);
	SafeDelete(m_vertexObject);
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

const DebugVertex &LinePrim::getVertex(int order) const
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

void LinePrim::setVertex(int order, const DebugVertex &vert)
{
	AX_ASSERT(order >= 0 && order < m_numVertexes);
	m_vertexes[order] = vert;
}

DebugVertex &LinePrim::getVertexRef(int order)
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
	line->m_vertexes[0].position = Vector3(0, 0, 0);
	line->m_vertexes[0].color = Rgba::Red;
	line->m_vertexes[1].position = Vector3(line_length, 0, 0);
	line->m_vertexes[1].color = Rgba::Red;
	line->m_indexes[0] = 0;
	line->m_indexes[1] = 1;

	// y
	line->m_vertexes[2].position = Vector3(0, 0, 0);
	line->m_vertexes[2].color = Rgba::Green;
	line->m_vertexes[3].position = Vector3(0, line_length, 0);
	line->m_vertexes[3].color = Rgba::Green;
	line->m_indexes[2] = 2;
	line->m_indexes[3] = 3;

	// z
	line->m_vertexes[4].position = Vector3(0, 0, 0);
	line->m_vertexes[4].color = Rgba::Blue;
	line->m_vertexes[5].position = Vector3(0, 0, line_length);
	line->m_vertexes[5].color = Rgba::Blue;
	line->m_indexes[4] = 4;
	line->m_indexes[5] = 5;

	return line;
}

LinePrim *LinePrim::createAxis(Hint hint, const Vector3 &origin, const Matrix3 &axis, float line_length)
{
	LinePrim *line = new LinePrim(hint);

	line->init(6, 6);

	// x
	line->m_vertexes[0].position = origin;
	line->m_vertexes[0].color = Rgba::Red;
	line->m_vertexes[1].position = origin + axis[0] * line_length;
	line->m_vertexes[1].color = Rgba::Red;
	line->m_indexes[0] = 0;
	line->m_indexes[1] = 1;

	// y
	line->m_vertexes[2].position = origin;
	line->m_vertexes[2].color = Rgba::Green;
	line->m_vertexes[3].position = origin + axis[1] * line_length;
	line->m_vertexes[3].color = Rgba::Green;
	line->m_indexes[2] = 2;
	line->m_indexes[3] = 3;

	// z
	line->m_vertexes[4].position = origin;
	line->m_vertexes[4].color = Rgba::Blue;
	line->m_vertexes[5].position = origin + axis[2] * line_length;
	line->m_vertexes[5].color = Rgba::Blue;
	line->m_indexes[4] = 4;
	line->m_indexes[5] = 5;

	return line;
}

LinePrim *LinePrim::createScreenRect(Hint hint, const Rect &rect, const Rgba &color)
{
	LinePrim *line = new LinePrim(hint);

	line->init(4, 8);

	// x
	line->m_vertexes[0].position = Vector3(rect.xMin(), rect.yMin(), 0);
	line->m_vertexes[0].color = color;
	line->m_vertexes[1].position = Vector3(rect.xMin(), rect.yMax(), 0);
	line->m_vertexes[1].color = color;
	line->m_vertexes[2].position = Vector3(rect.xMax(), rect.yMax(), 0);
	line->m_vertexes[2].color = color;
	line->m_vertexes[3].position = Vector3(rect.xMax(), rect.yMin(), 0);
	line->m_vertexes[3].color = color;

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
	line->m_vertexes[0].position = Vector3(rect.xMin(), rect.yMin(), 0);
	line->m_vertexes[0].color = color;
	line->m_vertexes[1].position = Vector3(rect.xMin(), rect.yMax(), 0);
	line->m_vertexes[1].color = color;
	line->m_vertexes[2].position = Vector3(rect.xMax(), rect.yMax(), 0);
	line->m_vertexes[2].color = color;
	line->m_vertexes[3].position = Vector3(rect.xMax(), rect.yMin(), 0);
	line->m_vertexes[3].color = color;
	line->unlockVertexes();
	return true;
}

LinePrim *LinePrim::createLine(Hint hint, const Vector3 &from, const Vector3 &to, const Rgba &color)
{
	LinePrim *line = new LinePrim(hint);

	line->init(2, 2);

	// x
	line->m_vertexes[0].position = from;
	line->m_vertexes[0].color = color;
	line->m_vertexes[1].position = to;
	line->m_vertexes[1].color = color;

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
	line->m_vertexes[0].position = from;
	line->m_vertexes[0].color = color;
	line->m_vertexes[1].position = to;
	line->m_vertexes[1].color = color;
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
	line->m_vertexes[0].position = origin;
	line->m_vertexes[0].color = xcolor;
	line->m_vertexes[1].position = origin + axis[0] * length;
	line->m_vertexes[1].color = xcolor;

	// y
	line->m_vertexes[2].position = origin;
	line->m_vertexes[2].color = ycolor;
	line->m_vertexes[3].position = origin + axis[1] * length;
	line->m_vertexes[3].color = ycolor;

	// z
	line->m_vertexes[4].position = origin;
	line->m_vertexes[4].color = zcolor;
	line->m_vertexes[5].position = origin + axis[2] * length;
	line->m_vertexes[5].color = zcolor;
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

	DebugVertex *verts = line->lockVertexes();
	for (int i = 0; i < 8; i++) {
		Vector3 axisscale;
		axisscale.x = i&1;
		axisscale.y = i&2 ? 1 : 0;
		axisscale.z = i&4 ? 1 : 0;

		Vector3 vec = Vector3(0.25f, 0.25f, 0.25f) - axisscale * 0.5f;
		Vector3 width = bbox.getExtends(vec);

		verts[i*4].position = origin + axis * bbox.getLerped(axisscale);
		verts[i*4].color = Rgba::White;
		verts[i*4+1].position = verts[i*4].position + axis * Vector3(width.x, 0, 0);
		verts[i*4+1].color = Rgba::White;
		verts[i*4+2].position = verts[i*4].position + axis * Vector3(0, width.y, 0);
		verts[i*4+2].color = Rgba::White;
		verts[i*4+3].position = verts[i*4].position + axis * Vector3(0, 0, width.z);
		verts[i*4+3].color = Rgba::White;
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

	DebugVertex *verts = line->lockVertexes();
	float step = AX_PI * 2.0f / subdivided;
	float angle = 0;
	for (int i = 0; i < subdivided; i++) {
		float s, c;

		Math::sincos(angle, s, c);
		verts[i].position = origin + p0 * s + p1 * c;
		verts[i].color = color;

		angle += step;

		if (!clipplane)
			continue;

		if (plane.distance(verts[i].position) >= -0.01) {
			verts[i].color.a = 64;
		}
	}
	line->unlockVertexes();
	return first;
}

LinePrim *LinePrim::createWorldBoundingBox( Hint hint, const BoundingBox &bbox, const Rgba &color )
{
	LinePrim *line = new LinePrim(hint);

	line->init(8, 24);
	DebugVertex *verts = line->lockVertexes();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				verts[i*4+j*2+k].position.x = i == 0 ? bbox.min.x : bbox.max.x; 
				verts[i*4+j*2+k].position.y = j == 0 ? bbox.min.y : bbox.max.y; 
				verts[i*4+j*2+k].position.z = k == 0 ? bbox.min.z : bbox.max.z; 
				verts[i*4+j*2+k].color = color; 
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

void LinePrim::draw(Technique tech)
{
	VertexObject *vert = m_vertexObject;
	InstanceObject *inst = m_overloadInstanceObject;
	IndexObject *index = m_overloadIndexObject ? m_overloadIndexObject : m_indexObject;
	Material *mat = m_overloadMaterial ? m_overloadMaterial : m_material;

	g_renderContext->draw(vert, inst, index, mat, tech);
}

void LinePrim::sync()
{
	if (m_syncFrame == g_renderSystem->getFrameNum())
		return;

	if (!m_isDirty && m_hint == Primitive::HintStatic)
		return;

	if (m_isVertexBufferDirty || !isStatic())
		m_vertexObject->init(m_vertexes, m_numVertexes, m_hint, VertexType::kDebug);

	if (m_isIndexBufferDirty || !isStatic())
		m_indexObject->init(m_indexes, m_numIndexes, m_hint, ElementType_LineList, m_activedIndexes);

	m_isDirty = m_isVertexBufferDirty = m_isIndexBufferDirty = 0;

	m_syncFrame = g_renderSystem->getFrameNum();
}

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

	m_vertexObject = new VertexObject();
	m_indexObject = new IndexObject();
}

MeshPrim::~MeshPrim()
{
	SafeDelete(m_indexObject);
	SafeDelete(m_vertexObject);

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
		verts[i*6+0].position = m_vertexes[i].position;
		verts[i*6+0].color = Rgba::Red;

		verts[i*6+1].position = m_vertexes[i].position + m_vertexes[i].tangent.xyz().getNormalized() * len;
		verts[i*6+1].color = Rgba::Red;

		verts[i*6+2].position = m_vertexes[i].position;
		verts[i*6+2].color = Rgba::Green;

		//verts[i*6+3].position = m_vertexes[i].position + m_vertexes[i].binormal.getNormalized() * len;
		verts[i*6+3].color = Rgba::Green;

		verts[i*6+4].position = m_vertexes[i].position;
		verts[i*6+4].color = Rgba::Blue;

		verts[i*6+5].position = m_vertexes[i].position + m_vertexes[i].normal.xyz().getNormalized() * len;
		verts[i*6+5].color = Rgba::Blue;

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
		verts[i*2+0].position = m_vertexes[i].position;
		verts[i*2+0].color = Rgba::Blue;

		verts[i*2+1].position = m_vertexes[i].position + m_vertexes[i].normal.xyz().getNormalized() * len;
		verts[i*2+1].color = Rgba::Blue;

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
	verts[0].position = Vector3(rect.x, rect.y, 0.0f);
	verts[0].streamTc.set(st[0], st[1], 0, 0);
	verts[0].color = color;
	verts[0].normal = Vector4(0, 0, 1, 1);
	verts[0].tangent = Vector4(1, 0, 0, 1);

	verts[1].position = Vector3(rect.x + rect.width, rect.y, 0.0f);
	verts[1].streamTc.set(st[2], st[1], 0, 0);
	verts[1].color = color;
	verts[1].normal = Vector4(0, 0, 1, 1);
	verts[1].tangent = Vector4(1, 0, 0, 1);

	verts[2].position = Vector3(rect.x, rect.y + rect.height, 0.0f);
	verts[2].streamTc.x = st[0];
	verts[2].streamTc.y = st[3];
	verts[2].color = color;
	verts[2].normal = Vector4(0, 0, 1, 1);
	verts[2].tangent = Vector4(1, 0, 0, 1);

	verts[3].position = Vector3(rect.x + rect.width, rect.y + rect.height, 0.0f);
	verts[3].streamTc.x = st[2];
	verts[3].streamTc.y = st[3];
	verts[3].color = color;
	verts[3].normal = Vector4(0, 0, 1, 1);
	verts[3].tangent = Vector4(1, 0, 0, 1);
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
	verts[0].position = Vector3(rect.x, rect.y, 0.0f);
	verts[0].streamTc.x = st[0];
	verts[0].streamTc.y = st[1];
	verts[0].color = color;
	verts[0].normal = Vector4(0, 0, 1, 1);
	verts[0].tangent = Vector4(1, 0, 0, 1);

	verts[1].position = Vector3(rect.x + rect.width, rect.y, 0.0f);
	verts[1].streamTc.x = st[2];
	verts[1].streamTc.y = st[1];
	verts[1].color = color;
	verts[1].normal = Vector4(0, 0, 1, 1);
	verts[1].tangent = Vector4(1, 0, 0, 1);

	verts[2].position = Vector3(rect.x, rect.y + rect.height, 0.0f);
	verts[2].streamTc.x = st[0];
	verts[2].streamTc.y = st[3];
	verts[2].color = color;
	verts[2].normal = Vector4(0, 0, 1, 1);
	verts[2].tangent = Vector4(1, 0, 0, 1);

	verts[3].position = Vector3(rect.x + rect.width, rect.y + rect.height, 0.0f);
	verts[3].streamTc.x = st[2];
	verts[3].streamTc.y = st[3];
	verts[3].color = color;
	verts[3].normal = Vector4(0, 0, 1, 1);
	verts[3].tangent = Vector4(1, 0, 0, 1);
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
	mesh->m_vertexes[0].position = center;
	mesh->m_vertexes[0].color = color;
	float step =(end - start) / subdivided;
	for (int i = 0; i <= subdivided; i++) {
		float angle = start + step * i;
		float s, c;
		Math::sincos(angle, s, c);
		mesh->m_vertexes[i+1].position = center + v0 * c + v1 * s;
		mesh->m_vertexes[i+1].color = color;
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
		memset(mesh->m_vertexes, 0, numverts * sizeof(MeshVertex));
		mesh->setMaterial(material);
	}

	AX_ASSERT(numverts = mesh->getNumVertexes());
	mesh->lockVertexes();
	for (int i = 0; i < numverts; i++) {
		mesh->m_vertexes[i].position = verts[i];
		mesh->m_vertexes[i].color = color;
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
    vertexes[0].position  = bbox.min;
	vertexes[0].color = color;
	vertexes[1].position  = Vector3(bbox.min.x,bbox.min.y,bbox.min.z + bSize.z);
	vertexes[1].color = color;
	vertexes[2].position  = Vector3(bbox.min.x,bbox.min.y + bSize.y,bbox.min.z + bSize.z);
	vertexes[2].color = color;
	vertexes[3].position  = Vector3(bbox.min.x,bbox.min.y + bSize.y,bbox.min.z);
	vertexes[3].color  = color;
	vertexes[4].position  = Vector3(bbox.min.x + bSize.x,bbox.min.y,bbox.min.z);
	vertexes[4].color = color;
	vertexes[5].position  = Vector3(bbox.min.x + bSize.x,bbox.min.y,bbox.min.z + bSize.z);
	vertexes[5].color = color;
	vertexes[6].position  = Vector3(bbox.min.x + bSize.x,bbox.min.y + bSize.y,bbox.min.z + bSize.z);
	vertexes[6].color = color;
	vertexes[7].position  = Vector3(bbox.min.x + bSize.x,bbox.min.y + bSize.y,bbox.min.z);
	vertexes[7].color = color;

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
		verts[0].position = top;
		verts[0].color = color;

		// vertex 1 is center
		verts[1].position = center;
		verts[1].color = color;

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
			verts[2 + i].position = center + right * cosf(alpha) + forward * sinf(alpha);
			verts[2 + i].color = color;
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
		verts[i].position = volumeverts[i];
	}
	mesh->unlockIndexes();

	return isinit;
}

void MeshPrim::draw(Technique tech)
{
	VertexObject *vert = m_vertexObject;
	InstanceObject *inst = m_overloadInstanceObject;
	IndexObject *index = m_overloadIndexObject ? m_overloadIndexObject : m_indexObject;
	Material *mat = m_overloadMaterial ? m_overloadMaterial : m_material;

	g_renderContext->draw(vert, inst, index, mat, tech);
}

void MeshPrim::sync()
{
	if (m_syncFrame == g_renderSystem->getFrameNum())
		return;

	if (!m_isDirty && m_hint == HintStatic)
		return;

	ElementType et = ElementType_TriList;

	if (m_isStriped)
		et = ElementType_TriStrip;

	if (m_isVertexBufferDirty || !isStatic())
		m_vertexObject->init(m_vertexes, m_numVertexes, m_hint, VertexType::kMesh);

	if (m_isIndexBufferDirty || !isStatic())
		m_indexObject->init(m_indexes, m_numIndexes, m_hint, et, m_activedIndexes);

	m_isDirty = m_isVertexBufferDirty = m_isIndexBufferDirty = false;

	m_syncFrame = g_renderSystem->getFrameNum();
}

static float CalcArea(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
	float a = (v1 - v2).getLength();
	float b = (v2 - v3).getLength();
	float c = (v1 - v3).getLength();
	float s = (a + b + c) * 0.5f;
	float result = s * (s - a) * (s - b) * (s - c);
	if (result <= 0.0f) return 0;
	return sqrt(result);
}

float MeshPrim::calcArea() const
{
	if (isStriped()) return 0;

	float result = 0;
	for (int i = 0; i < m_numIndexes/3; i++) {
		result += CalcArea(m_vertexes[m_indexes[i*3]].position,m_vertexes[m_indexes[i*3+1]].position,m_vertexes[m_indexes[i*3+2]].position);
	}
	return result;
}

void MeshPrim::minmaxTc(Vector2 &result) const
{
	for (int i = 0; i < m_numVertexes; i++)
	{
		result.x = std::min(m_vertexes[i].streamTc.x, result.x);
		result.x = std::min(m_vertexes[i].streamTc.y, result.x);
		result.y = std::max(m_vertexes[i].streamTc.x, result.y);
		result.y = std::max(m_vertexes[i].streamTc.y, result.y);
	}
}




//------------------------------------------------------------------------------

TextPrim::TextPrim(Hint hint) : Primitive(hint), m_horizonAlign(Center), m_verticalAlign(VCenter)
{
	m_type = TextType;
}

TextPrim::~TextPrim()
{
}

void TextPrim::init(const Rect &rect, Rgba color, float aspect, int format, Font *font, const std::string &text)
{
	m_rect = rect;
	m_color = color;
	m_aspect = aspect;
	m_format = format;
	m_font = font;
	m_text = text;
	m_isSimpleText = false;
}

void TextPrim::initSimple(const Vector3 &xyz, Rgba color, const std::string &text, bool fixedWidth)
{
	m_isSimpleText = true;
	m_color = color;
	m_position = xyz;
	m_text = text;
	m_format = 0;
	m_aspect = 1.0f;

	if (fixedWidth)
		m_font = g_consoleFont.get();
	else
		m_font = g_defaultFont.get();
}

void TextPrim::clear()
{
}

// static helper function
TextPrim *TextPrim::createSimpleText(Hint hint, const Vector3 &xyz, const Rgba &color, const std::string &text, bool fixedWidth)
{
	TextPrim *rp_text = new TextPrim(hint);
	rp_text->initSimple(xyz, color, text, fixedWidth);

	return rp_text;
}

TextPrim *TextPrim::createText(Hint hint, const Rect &rect, Font *font, const std::string &text, const Rgba &color, HorizonAlign halign, VerticalAlign valign, int format, float aspect)
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

#define BLINK_DIVISOR 0.075
void TextPrim::draw( Technique tech )
{
	if (!r_font.getBool()) {
		return;
	}

	stat_numTextDrawElements.inc();

	if (tech != Technique::Main)
		return;

	TextQuad tq;
	float width, height;

	if (m_isSimpleText) {
		if (m_isSimpleText) {
			m_format = TextPrim::ScaleByVertical;
			m_horizonAlign = TextPrim::Center;
			m_verticalAlign = TextPrim::VCenter;
		}

		width = 1.0f; height = 1.0f;

		const Matrix3 &axis = g_renderContext->getCurCamera()->viewAxis();
		tq.s_vector = -axis[1];
		tq.t_vector = -axis[2];
		tq.origin = m_position - tq.s_vector * width * 0.5f - tq.t_vector * height * 0.5f;
		tq.width = width;
		tq.height = height;
	} else {
		tq.s_vector = Vector3(1, 0, 0);
		tq.t_vector = Vector3(0, 1, 0);
		tq.origin = Vector3(m_rect.x+0.5f, m_rect.y+0.5f, 0);
		tq.width = m_rect.width;
		tq.height = m_rect.height;
	}

	std::wstring wstr = u2w(m_text);
	const wchar_t *pStr = wstr.c_str();
	size_t total_len = wstr.length();
	Vector2 scale, offset;
	Vector2 startpos;

	scale.x = m_aspect;
	scale.y = 1.0;

	float textwidth = scale.x * m_font->getStringWidth(wstr);
	float textheight = scale.y * m_font->getHeight();

	if (m_format & TextPrim::ScaleByVertical) {
		scale.y = tq.height / textheight;
		scale.x = m_aspect * scale.y;
	}

	if (m_format & TextPrim::ScaleByHorizon) {
		scale.x = tq.width / textwidth;
		scale.y = scale.x / m_aspect;
	}

	width = tq.width / scale.x;
	height = tq.height / scale.y;

	bool italic = m_format & TextPrim::Italic ? true : false;

	if (m_horizonAlign == TextPrim::Left) {
		startpos.x = 0;
	} else if (m_horizonAlign == TextPrim::Center) {
		startpos.x =(width - textwidth) * 0.5f;
	} else if (m_horizonAlign == TextPrim::Right) {
		startpos.x = width - textwidth;
	} else {
		startpos.x = 0;
	}

	if (m_verticalAlign == TextPrim::Top) {
		startpos.y = 0;
	} else if (m_verticalAlign == TextPrim::VCenter) {
		startpos.y =(height - textheight) * 0.5f;
	} else if (m_verticalAlign == TextPrim::Bottom) {
		startpos.y = height - textheight;
	} else {
		startpos.y = 0;
	}

	while (total_len) {
		// ulonglong_t t0 = OsUtil::microseconds();

		size_t len = m_font->updateTexture(pStr);

		// for error string, cann't upload or cann't render char

		// ulonglong_t t1 = OsUtil::microseconds();

		if (m_format & TextPrim::Blink) {
			Rgba color = m_color;
			color.a = 128+127*sinf(g_renderContext->getCurCamera()->time() / BLINK_DIVISOR);
			offset = g_renderContext->drawString(m_font, color, tq, startpos, pStr, len, scale, italic);
			goto next;
		}

		// ulonglong_t t2 = OsUtil::microseconds();

		// common
		offset = g_renderContext->drawString(m_font, m_color, tq, startpos, pStr, len, scale, italic);

		// ulonglong_t t3 = OsUtil::microseconds();
		if (m_format & TextPrim::Bold) {
			g_renderContext->drawString(m_font, m_color, tq, startpos+scale, pStr, len, scale, italic);
		}

next:
		total_len -= len;
		pStr += len;
		startpos += offset;
	}
}

void TextPrim::sync()
{
}

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

	m_vertexObject = new VertexObject();
	m_indexObject = new IndexObject();
}

ChunkPrim::~ChunkPrim()
{
	SafeDelete(m_indexObject);
	SafeDelete(m_vertexObject);

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
	m_isDirty = true;
}

Vector4 ChunkPrim::getTerrainRect() const
{
	return m_terrainRect;
}

void  ChunkPrim::setColorTexture(Texture *color_texture)
{
	m_colorTexture = color_texture;
	m_isDirty = true;
}

Texture *ChunkPrim::getColorTexture()
{
	return m_colorTexture;
}

void ChunkPrim::setNormalTexture(Texture *dsdt)
{
	m_normalTexture = dsdt;
	m_isDirty = true;
}

Texture *ChunkPrim::getNormalTexture()
{
	return m_normalTexture;
}

void ChunkPrim::setChunkRect(const Vector4 &rect)
{
	m_chunkRect = rect;
	m_isDirty = true;
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

void ChunkPrim::setLayers(int index, Texture *alpha, Material *detail, float scale, bool isVerticalProjection)
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		Debugf("MAX_LAYERS exceeded\n");
		return;
	}

	m_layers[index].alphaTex = alpha;
	m_layers[index].detailMat = detail;
	m_layers[index].scale = scale;
	m_layers[index].isVerticalProjection = isVerticalProjection;
	m_isDirty = true;
}

Texture *ChunkPrim::getLayerAlpha(int index) const
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		return nullptr;
	}

	return m_layers[index].alphaTex;
}

Material *ChunkPrim::getLayerDetail(int index) const
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		return nullptr;
	}

	return m_layers[index].detailMat;
}

bool ChunkPrim::isLayerVerticalProjection(int index) const
{
	if (index >= ChunkPrim::MAX_LAYERS) {
		return false;
	}

	return m_layers[index].isVerticalProjection;
}

void ChunkPrim::draw(Technique tech)
{
	VertexObject *vert = m_vertexObject;
	InstanceObject *inst = m_overloadInstanceObject;
	IndexObject *index = m_overloadIndexObject ? m_overloadIndexObject : m_indexObject;
	Material *mat = m_overloadMaterial ? m_overloadMaterial : m_material;

	stat_numTerrainDrawElements.inc();

	if (m_overloadMaterial) {
		g_renderContext->draw(vert, inst, index, mat, tech);
		return;
	}

	bool drawlayer = true;

	if (!r_detail.getBool() || !m_layerVisible) {
		drawlayer = false;
	}

	if (m_numLayers == 0)
		drawlayer = false;

	if (g_renderContext->isReflecting())
		drawlayer = false;

	if (tech != Technique::GeoFill)
		drawlayer = false;

	g_renderContext->draw(vert, inst, index, mat, tech);

#if 0
	// draw layer
	for (int i = 0; i < m_numLayers; i++) {
		stat_numTerrainLayeredDrawElements.inc();

		ChunkPrim::Layer &l = m_layers[i];

		AX_SU(g_detailScale, l.scale);
		l.detailMat->setTexture(MaterialTextureId::TerrainColor, m_colorTexture->clone());
		l.detailMat->setTexture(MaterialTextureId::TerrainNormal, m_normalTexture->clone());
		if (l.alphaTex)
			l.detailMat->setTexture(MaterialTextureId::LayerAlpha, l.alphaTex->clone());
		else
			l.detailMat->setTexture(MaterialTextureId::LayerAlpha, 0);

		l.detailMat->setFeature(0, l.isVerticalProjection);

		bool firstLayer = combine && (i == 0);
		l.detailMat->setFeature(1, firstLayer); // if is first layer, set first layer flag
		if (!firstLayer)
			l.detailMat->m_blendMode = Material::BlendMode_Blend;

		l.detailMat->clearParameters();
		l.detailMat->addParameter("g_zoneRect", 4, m_zoneRect.c_ptr());
		l.detailMat->addParameter("g_chunkRect", 4, m_chunkRect.c_ptr());
		g_renderContext->draw(vert, inst, index, l.detailMat, Technique::Layer);
	}
#endif
}

void ChunkPrim::sync()
{
	if (!m_isDirty && m_hint == HintStatic)
		return;

	if (m_syncFrame == g_renderSystem->getFrameNum())
		return;

	if (m_isVertexBufferDirty || m_hint != HintStatic)
		m_vertexObject->init(m_vertexes, m_numVertexes, m_hint, VertexType::kChunk);

	if (m_isIndexBufferDirty || m_hint != HintStatic)
		m_indexObject->init(m_indexes, m_numIndexes, m_hint, ElementType_TriList, m_activedIndexes);

	if (!m_isDirty)
		return;

	m_material->setTexture(MaterialTextureId::TerrainColor, m_colorTexture->clone());
	m_material->setTexture(MaterialTextureId::TerrainNormal, m_normalTexture->clone());

	for (int i = 0; i < m_numLayers; i++) {
		ChunkPrim::Layer &l = m_layers[i];

		Texture *tex = l.detailMat->getTexture(MaterialTextureId::Diffuse);
		if (tex) m_material->setTexture(MaterialTextureId::Detail + i, tex->clone());
		else m_material->setTexture(MaterialTextureId::Detail + i, 0);

		tex = l.detailMat->getTexture(MaterialTextureId::Normal);
		if (tex) m_material->setTexture(MaterialTextureId::DetailNormal+i, tex->clone());
		else m_material->setTexture(MaterialTextureId::DetailNormal+i, 0);

		tex = l.alphaTex;
		if (tex) m_material->setTexture(MaterialTextureId::LayerAlpha+i, tex->clone());
		else m_material->setTexture(MaterialTextureId::LayerAlpha+i, 0);

		m_material->setFeature(i, l.isVerticalProjection);

		m_material->setDetailScale(i, l.scale);
	}

	m_material->clearParameters();
	m_material->addParameter("g_zoneRect", 4, m_zoneRect.c_ptr());
	m_material->addParameter("g_chunkRect", 4, m_chunkRect.c_ptr());

#if 0
	bool combine = false; // r_terrainLayerCombine.getBool();
	// draw layer
	for (int i = 0; i < m_numLayers; i++) {
		stat_numTerrainLayeredDrawElements.inc();

		ChunkPrim::Layer &l = m_layers[i];
		l.detailMat->setTexture(MaterialTextureId::TerrainColor, m_colorTexture->clone());
		l.detailMat->setTexture(MaterialTextureId::TerrainNormal, m_normalTexture->clone());
		if (l.alphaTex)
			l.detailMat->setTexture(MaterialTextureId::LayerAlpha, l.alphaTex->clone());
		else
			l.detailMat->setTexture(MaterialTextureId::LayerAlpha, 0);

		l.detailMat->setFeature(0, l.isVerticalProjection);

		bool firstLayer = combine && (i == 0);
		l.detailMat->setFeature(1, firstLayer); // if is first layer, set first layer flag
		if (!firstLayer)
			l.detailMat->m_blendMode = Material::BlendMode_Blend;

		l.detailMat->clearParameters();
		l.detailMat->addParameter("g_zoneRect", 4, m_zoneRect.c_ptr());
		l.detailMat->addParameter("g_chunkRect", 4, m_chunkRect.c_ptr());
	}
#endif
	m_isDirty = m_isVertexBufferDirty = m_isIndexBufferDirty = 0;
	m_syncFrame = g_renderSystem->getFrameNum();
}

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

void GroupPrim::draw( Technique tech )
{

}

void GroupPrim::sync()
{
	if (m_syncFrame == g_renderSystem->getFrameNum())
		return;

	for (size_t i = 0; i < m_primitives.size(); i++)
		m_primitives[i]->sync();

	m_syncFrame = g_renderSystem->getFrameNum();
}

//--------------------------------------------------------------------------

RefPrim::RefPrim(Hint hint) : Primitive(hint)
{
	m_type = ReferenceType;
	m_refered = 0;
	m_numIndexes = 0;
	m_indexes = 0;

	m_indexObject = new IndexObject();
}

RefPrim::~RefPrim()
{
	SafeDelete(m_indexObject);

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

void RefPrim::draw( Technique tech )
{

}

void RefPrim::sync()
{
	if (!m_isDirty)
		return;

	if (m_syncFrame == g_renderSystem->getFrameNum())
		return;

	m_refered->sync();

	//IndexObject *indexObject = m_refered->m_indexObject;

	if (m_isIndexBufferDirty && m_numIndexes)
		m_indexObject->init(m_indexes, m_numIndexes, m_hint, ElementType_TriList, m_activedIndexes);

	m_isDirty = m_isIndexBufferDirty = false;

	m_syncFrame = g_renderSystem->getFrameNum();
}

//--------------------------------------------------------------------------

InstancePrim::InstancePrim(Hint hint) : Primitive(hint)
{
	m_type = InstancingType;
	m_instanced = 0;
	m_numInstances = 0;
	m_params = 0;

	m_instanceObject = new InstanceObject();
}

InstancePrim::~InstancePrim()
{
	SafeDelete(m_instanceObject);
	SafeDeleteArray(m_params);
}

Primitive *InstancePrim::getInstanced() const
{
	return m_instanced;
}

void InstancePrim::init(Primitive *instanced, int numInstances)
{
	m_instanced = instanced;
	m_isDirty = true;

	m_numInstances = numInstances;
	m_params = new Param[numInstances];
}

void InstancePrim::setInstance(int index, const Param &param)
{
	AX_ASSERT(index >= 0 && index < m_numInstances);
	m_params[index] = (param);
	m_isDirty = true;
}

void InstancePrim::setInstance(int index, const Matrix &mtx, const Vector4 &user)
{
	Param param;
	param.worldMatrix = mtx;
	param.userDefined = user;
	setInstance(index, param);
}

int InstancePrim::getNumInstance() const
{
	return m_numInstances;
}

const InstancePrim::Param &InstancePrim::getInstance(int index) const
{
	AX_ASSERT(index < getNumInstance());
	return m_params[index];
}

const InstancePrim::Param *InstancePrim::getAllInstances() const
{
	return m_params;
}

void InstancePrim::draw(Technique tech)
{
	m_instanced->m_overloadInstanceObject = m_instanceObject;
	m_instanced->draw(tech);
	m_instanced->m_overloadInstanceObject = 0;
}

void InstancePrim::sync()
{
	if (m_syncFrame == g_renderSystem->getFrameNum())
		return;

	m_instanced->sync();

	m_instanceObject->init(m_numInstances, m_params);

	m_syncFrame = g_renderSystem->getFrameNum();
}

AX_END_NAMESPACE
