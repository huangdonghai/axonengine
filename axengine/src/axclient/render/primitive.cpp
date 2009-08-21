/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon { namespace Render {

	//------------------------------------------------------------------------------
	// class Primitive
	//------------------------------------------------------------------------------

	Primitive::Primitive(Hint hint)
		: m_cachedId(0)
		, m_cachedFrame(-1)
		, m_hint(hint)
		, m_type(NoneType)
	{
		m_material = 0;
		m_lightMap = 0;
		m_isMatrixSet = false;
		m_matrix.setIdentity();

		m_activedIndexes = 0;
	}

	Primitive::~Primitive() {
		if (m_hint != OneFrame) {
			g_primitiveManager->hintUncache(this);
		}
	}

	//------------------------------------------------------------------------------
	// class PointPrim
	//------------------------------------------------------------------------------

	PointPrim::PointPrim(Hint hint) : Primitive(hint) {
		m_type = PointType;
		m_points = NULL;
	}

	PointPrim::~PointPrim() {}

	void PointPrim::initialize(int num_points) {
		m_numPoints = num_points;

		TypeFree(m_points);
		m_points = TypeAlloc<DebugVertex>(m_numPoints);

		m_isDirtied = true;
		m_isVertexBufferDirtied = true;
		m_isIndexBufferDirtied = true;
	}

	int PointPrim::getNumPoints() const {
		return m_numPoints;
	}

	const DebugVertex* PointPrim::getPointsPointer() const {
		return m_points;
	}

	DebugVertex* PointPrim::lock() {
		return m_points;
	}
	void PointPrim::unlock() {
		m_isDirtied = true;
		m_isVertexBufferDirtied = true;
	}

	void PointPrim::setPointSize(float point_size) {
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

	Line::Line(Hint hint)
		: Primitive(hint)
		, m_numVertexes(0)
		, m_numIndexes(0)
		, m_vertexes(NULL)
		, m_indexes(NULL)
		, m_lineWidth(1.0f)
	{
		m_type = LineType;
	}

	Line::~Line() {
		finalize();
	}

	void Line::initialize(int numverts, int numindexes) {
		// clear old data first
		finalize();

		m_numVertexes = numverts;
		m_numIndexes = numindexes;

	//	m_vertexes = (DebugVertex *)Malloc(m_numVertexes * sizeof(DebugVertex));
		m_vertexes = TypeAlloc<DebugVertex>(m_numVertexes);
		m_indexes = TypeAlloc<ushort_t>(numindexes);

		m_isDirtied = true;
		m_isVertexBufferDirtied = true;
		m_isIndexBufferDirtied = true;
	}

	void Line::finalize() {
		m_numVertexes = 0;
		m_numIndexes = 0;
		TypeFree(m_vertexes);
		TypeFree(m_indexes);

	}

	int Line::getNumVertexes() const {
		return m_numVertexes;
	}

	const DebugVertex* Line::getVertexesPointer() const {
		return m_vertexes;
	}

	DebugVertex* Line::lockVertexes() {
		return m_vertexes;
	}

	void Line::unlockVertexes() {
		m_isDirtied = true;
		m_isVertexBufferDirtied = true;
	}

	int Line::getNumIndexes() const {
		return m_numIndexes;
	}

	const ushort_t* Line::getIndexPointer() const {
		return m_indexes;
	}

	ushort_t* Line::lockIndexes() {
		return m_indexes;
	}

	void Line::unlockIndexes() {
		m_isDirtied = true;
		m_isIndexBufferDirtied = true;
	}

	ushort_t Line::getIndex(int order) const {
		AX_ASSERT(order >= 0 && order < m_numIndexes);
		return m_indexes[order];
	}

	const Line::VertexType& Line::getVertex(int order) const {
		AX_ASSERT(order >= 0 && order < m_numVertexes);
		return m_vertexes[order];
	}

	void Line::lock() {}

	void Line::setIndex(int order, int index) {
		AX_ASSERT(order >= 0 && order < m_numIndexes);
		m_indexes[order] = index;
	}

	void Line::setVertex(int order, const VertexType& vert) {
		AX_ASSERT(order >= 0 && order < m_numVertexes);
		m_vertexes[order] = vert;
	}

	Line::VertexType& Line::getVertexRef(int order) {
		AX_ASSERT(order >= 0 && order < m_numVertexes);
		return m_vertexes[order];
	}

	void Line::setLineWidth(float line_width) {
		m_lineWidth = line_width;
	}

	float Line::getLineWidth() const {
		return m_lineWidth;
	}

	void Line::unlock() {
		m_isDirtied = true;
		m_isVertexBufferDirtied = true;
		m_isIndexBufferDirtied = true;
	}

	Line* Line::createAxis(Hint hint, float line_length) {
		Line* line = new Line(hint);

		line->initialize(6, 6);

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

	Line* Line::createAxis(Hint hint, const Vector3& origin, const Matrix3& axis, float line_length) {
		Line* line = new Line(hint);

		line->initialize(6, 6);

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

	Line* Line::createScreenRect(Hint hint, const Rect& rect, const Rgba& color) {
		Line* line = new Line(hint);

		line->initialize(4, 8);

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

	bool Line::setupScreenRect(Line*& line, const Rect& rect, const Rgba& color) {
		if (!line) {
			line = createScreenRect(Dynamic, rect, color);
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

	Line* Line::createLine(Hint hint, const Vector3& from, const Vector3& to, const Rgba& color) {
		Line* line = new Line(hint);

		line->initialize(2, 2);

		// x
		line->m_vertexes[0].xyz = from;
		line->m_vertexes[0].rgba = color;
		line->m_vertexes[1].xyz = to;
		line->m_vertexes[1].rgba = color;

		line->m_indexes[0] = 0;
		line->m_indexes[1] = 1;

		return line;
	}

	bool Line::setupLine(Line*& line, const Vector3& from, const Vector3& to, const Rgba& color) {
		bool isinit = false;

		if (!line) {
			isinit = true;
			line = new Line(Dynamic);
			line->initialize(2, 2);
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

	bool Line::setupAxis(Line*& line, const Vector3& origin, const Matrix3& axis, float length, Rgba xcolor, Rgba ycolor, Rgba zcolor) {
		bool isInit = false;

		if (!line) {
			isInit = true;
			line = new Line(Dynamic);
			line->initialize(6, 6);
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

	bool Line::setupBoundingBox(Line*& line, const Vector3& origin, const Matrix3& axis, const BoundingBox& inbbox, float scale/* =1.0f */, Hint hint /* = Primitive::Dynamic */){
		bool isfirst = false;
		if (!line) {
			line = new Line(hint);
			line->initialize(32, 48);

			ushort_t* indexes = line->lockIndexes();
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

		VertexType* verts = line->lockVertexes();
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

	bool Line::setupCircle(Line*& line, const Vector3& origin, const Vector3& p0, const Vector3& p1, const Rgba& color, int subdivided) {
		return setupCircle(line, origin, p0, p1, color, subdivided, false, Plane());
	}

	bool Line::setupCircle(Line*& line, const Vector3& origin, const Vector3& p0, const Vector3& p1, const Rgba& color, int subdivided, const Plane& plane) {
		return setupCircle(line, origin, p0, p1, color, subdivided, true, plane);
	}

	bool Line::setupCircle(Line*& line, const Vector3& origin, const Vector3& p0, const Vector3& p1, const Rgba& color, int subdivided,Hint hint){
        return setupCircle(line, origin, p0, p1, color, subdivided, false, Plane(), hint);
	}

	bool Line::setupCircle(Line*& line, const Vector3& origin, const Vector3& p0, const Vector3& p1, const Rgba& color, int subdivided, bool clipplane, const Plane& plane,Hint hint /* = Primitive::Dynamic */) {
		AX_ASSERT(subdivided >= 3);
		bool first = false;

		if (!line) {
			line = new Line(Dynamic);
			line->initialize(subdivided, subdivided * 2);
			ushort_t* pidx = line->lockIndexes();
			for (int i = 0; i < subdivided; i++) {
				*pidx++ = i;
				*pidx++ = (i+1) % subdivided;
			}
			line->unlockIndexes();
			first = true;
		}

		VertexType* verts = line->lockVertexes();
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

	Line* Line::createWorldBoundingBox( Hint hint, const BoundingBox& bbox, const Rgba& color )
	{
		Line* line = new Line(hint);

		line->initialize(8, 24);
		VertexType* verts = line->lockVertexes();
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
		ushort_t* indexes = line->lockIndexes();
		memcpy(indexes, s_indices, line->getNumIndexes() * sizeof(ushort_t));
		line->unlockIndexes();

		return line;
	}

	//------------------------------------------------------------------------------
	// class Mesh
	//------------------------------------------------------------------------------

	Mesh::Mesh(Hint hint)
		: Primitive(hint)
		, m_numVertexes(0)
		, m_vertexes(NULL)
		, m_numIndexes(0)
		, m_indexes(NULL)
#if 0
		, m_currentIndexNum(0)
#endif
	{
		m_type = MeshType;
		m_isStriped = false;
	}

	Mesh::~Mesh() {
		finalize();
	}

	void Mesh::initialize(int numverts, int numidxes) {
		if (numverts == m_numVertexes && numidxes == m_numIndexes)
			return;

		// clear old data first
		finalize();

		m_numVertexes = numverts;
		m_numIndexes = numidxes;
		m_vertexes = TypeAlloc<Vertex>(numverts);
		m_indexes = TypeAlloc<ushort_t>(numidxes);
	}

	void Mesh::finalize() {
		m_numVertexes = 0;
		m_numIndexes = 0;
		TypeFree(m_vertexes);
		TypeFree(m_indexes);
	}

	int Mesh::getNumVertexes() const {
		return m_numVertexes;
	}

	const Vertex* Mesh::getVertexesPointer() const {
		return m_vertexes;
	}

	Vertex* Mesh::lockVertexes() {
		return m_vertexes;
	}

	void Mesh::unlockVertexes() {
		m_isDirtied = true;
		m_isVertexBufferDirtied = true;
	}

	const ushort_t* Mesh::getIndexPointer() const {
		return m_indexes;
	}

	int Mesh::getNumIndexes() const {
		return m_numIndexes;
	}

	ushort_t* Mesh::lockIndexes() {
		return m_indexes;
	}

	void Mesh::unlockIndexes() {
		m_isDirtied = true;
		m_isIndexBufferDirtied = true;
	}

	Line* Mesh::getTangentLine(float len) const {
		if (m_numVertexes == 0)
			return NULL;

		Line* line = new Line(OneFrame);

		line->initialize(m_numVertexes * 6, m_numVertexes * 6);
		DebugVertex* verts = line->lockVertexes();
		ushort_t* idxes = line->lockIndexes();

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

	Line* Mesh::getNormalLine(float len) const {
		if (m_numVertexes == 0)
			return NULL;

		Line* line = new Line(OneFrame);

		line->initialize(m_numVertexes * 2, m_numVertexes * 2);
		DebugVertex* verts = line->lockVertexes();
		ushort_t* idxes = line->lockIndexes();

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

	void Mesh::computeTangentSpace() {
		if (m_isStriped) {
			Errorf("Mesh::computeTangentSpace: cann't compute tangent space when is striped");
			return;
		}

		lockVertexes();
		g_simd->computeTangentSpace(m_vertexes, m_numVertexes, m_indexes, m_numIndexes);
		unlockVertexes();
	}

	void Mesh::computeTangentSpaceSlow()
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
	Mesh* Mesh::createScreenQuad(Hint hint, const Rect& rect, const Rgba& color, Material* material, const Vector4& st) {
		Mesh* quad = new Mesh(hint);

		quad->initialize(4, 6);
		Vertex* verts = quad->lockVertexes();
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

		ushort_t* indexes = quad->lockIndexes();
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

	bool Mesh::setupScreenQuad(Mesh*& quad, const Rect& rect, const Rgba& color, Material* material, const Vector4& st) {
		Vertex* verts = quad->lockVertexes();
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

		ushort_t* indexes = quad->lockIndexes();
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

	bool Mesh::setupFan(Mesh*& mesh, const Vector3& center, const Vector3& v0, const Vector3& v1, float start, float end, Rgba color, int subdivided, Material* material) {
		AX_ASSERT(subdivided >= 1);

		bool isinit = false;
		int numverts = subdivided + 2;
		int numidxes = subdivided * 3;
		if (!mesh) {
			isinit = true;
			mesh = new Mesh(Dynamic);
			mesh->initialize(numverts, numidxes);
			mesh->lockIndexes();
			for (int i = 0; i < subdivided; i++) {
				mesh->m_indexes[i*3] = 0;
				mesh->m_indexes[i*3+1] = i + 1;
				mesh->m_indexes[i*3+2] = i + 2;
			}
			memset(mesh->m_vertexes, 0, sizeof(Vertex) * numverts);
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


	bool Mesh::setupPolygon(Mesh*& mesh, int numverts, const Vector3* verts, Rgba color, Material* material) {
		AX_ASSERT(numverts >= 3);
		bool isinit = false;
		int numidxes =(numverts - 2) * 3;
		if (!mesh) {
			isinit = true;
			mesh = new Mesh(Dynamic);
			mesh->initialize(numverts, numidxes);
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

	bool Mesh::setupBox(Mesh*& mesh, const BoundingBox& bbox, const Rgba& color)
	{
		int numverts = 8;
		int numidxes = 36;

		if (!mesh){
			mesh = new Render::Mesh(Dynamic);
			mesh->initialize(8,36);
		}

		Vertex* vertexes = mesh->lockVertexes();
		ushort_t*    indexes  = mesh->lockIndexes();

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
	bool Mesh::setupCone(Mesh*& cone, const Vector3& center, float radius, const Vector3& top, const Rgba& color, int subdivided) {
		int numverts = subdivided + 2;		// number of subdivided + top + center
		int numidxes = subdivided * 2 * 3;
		bool isinit = false;
		if (!cone) {
			isinit = true;
			cone = new Mesh(Dynamic);
			cone->initialize(numverts, numidxes);

			// triangles
			ushort_t* indexes = cone->lockIndexes();
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
		Vertex* verts = cone->lockVertexes();
			// reset to zero
			memset(verts, 0, sizeof(Vertex) * numverts);

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

	bool Mesh::setupHexahedron(Mesh*& mesh, Vector3 volumeverts[8]) {
		int numverts = 8;
		int numindexes = 6 * 2 * 3;

		bool isinit = false;

		if (!mesh) {
			isinit = true;
			mesh = new Mesh(Dynamic);
			mesh->initialize(numverts, numindexes);

			// triangles
			static ushort_t s_idxes[] = {
				0, 2, 1, 1, 2, 3,
				2, 6, 3, 3, 6, 7,
				6, 4, 7, 7, 4, 5,
				4, 0, 5, 5, 0, 1,
				1, 3, 5, 5, 3, 7,
				0, 2, 4, 4, 2, 6
			};

			ushort_t* indexes = mesh->lockIndexes();
			memcpy(indexes, s_idxes, numindexes * sizeof(ushort_t));
			mesh->unlockIndexes();

			// set verts to 0
			Vertex* verts = mesh->lockVertexes();
			memset(verts, 0, numverts * sizeof(Vertex));
			mesh->unlockIndexes();
		}

		if (!volumeverts) {
			return isinit;
		}

		Vertex* verts = mesh->lockVertexes();
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
	// class Text
	//------------------------------------------------------------------------------

	Text::Text(Hint hint) : Primitive(hint), m_horizonAlign(Center), m_verticalAlign(VCenter) {
		m_type = TextType;
	}

	Text::~Text() {
	}

	void Text::initialize(const Rect& rect, Rgba color, float aspect, int format, Font* font, const String& text) {
		m_rect = rect;
		m_color = color;
		m_aspect = aspect;
		m_format = format;
		m_font = font;
		m_text = text;
		m_isSimpleText = false;
	}

	void Text::initializeSimple(const Vector3& xyz, Rgba color, const String& text, bool fixedWidth) {
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

	void Text::finalize() {
	}

	// static helper function
	Text* Text::createSimpleText(Hint hint, const Vector3& xyz, const Rgba& color, const String& text, bool fixedWidth) {
		Text* rp_text = new Text(hint);
		rp_text->initializeSimple(xyz, color, text, fixedWidth);

		return rp_text;
	}

	Text* Text::createText(Hint hint, const Rect& rect, Font* font, const String& text, const Rgba& color, HorizonAlign halign, VerticalAlign valign, int format, float aspect) {
		Text* rp_text = new Text(hint);

		if (text.size() == 0)
			Errorf("Text::CreateText: null text");

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
	// class Chunk
	//------------------------------------------------------------------------------

	Chunk::Chunk(Hint hint)
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

	Chunk::~Chunk() {
		finalize();
	}

	void Chunk::initialize(int numverts, int numidxes) {
		finalize();

		m_numVertexes = numverts;
		m_numIndexes = numidxes;

		m_vertexes = TypeAlloc<ChunkVertex>(numverts);
		m_indexes = TypeAlloc<ushort_t>(numidxes);
	}

	void Chunk::finalize() {
		TypeFree(m_vertexes);
		TypeFree(m_indexes);
		m_numVertexes = 0;
		m_numIndexes = 0;
	}

	int Chunk::getNumVertexes() const {
		return m_numVertexes;
	}

	const ChunkVertex* Chunk::getVertexesPointer() const {
		return m_vertexes;
	}

	ChunkVertex* Chunk::lockVertexes() {
		return m_vertexes;
	}

	void Chunk::unlockVertexes() {
		m_isDirtied = true;
		m_isVertexBufferDirtied = true;
	}

	int Chunk::getNumIndexes() const {
		return m_numIndexes;
	}

	const ushort_t* Chunk::getIndexesPointer() const {
		return m_indexes;
	}

	ushort_t* Chunk::lockIndexes() {
		return m_indexes;
	}

	void Chunk::unlockIndexes() {
		m_isDirtied = true;
		m_isIndexBufferDirtied = true;
	}

	void Chunk::setTerrainRect(const Vector4& rect) {
		m_terrainRect = rect;
	}

	Vector4 Chunk::getTerrainRect() const {
		return m_terrainRect;
	}

	void  Chunk::setColorTexture(Texture* color_texture) {
		m_colorTexture = color_texture;
	}

	Texture* Chunk::getColorTexture() {
		return m_colorTexture;
	}

	void Chunk::setNormalTexture(Texture* dsdt) {
		m_normalTexture = dsdt;
	}

	Texture* Chunk::getNormalTexture() {
		return m_normalTexture;
	}

	void Chunk::setChunkRect(const Vector4& rect) {
		m_chunkRect = rect;
	}

	Vector4 Chunk::getChunkRect() const {
		return m_chunkRect;
	}

	void Chunk::setNumLayers(int n) {
		m_numLayers = std::min<int>(n,MAX_LAYERS);
		m_isDirtied = true;
	}

	int Chunk::getNumLayers() const {
		return m_numLayers;
	}

	void Chunk::setLayers(int index, Texture* alpha, Material* detail, const Vector2& scale, bool isVerticalProjection) {
		if (index >= Chunk::MAX_LAYERS) {
			Debugf("MAX_LAYERS exceeded\n");
			return;
		}

		m_layers[index].alphaTex = alpha;
		m_layers[index].detailMat = detail;
		m_layers[index].scale = scale;
		m_layers[index].isVerticalProjection = isVerticalProjection;
	}

	Texture* Chunk::getLayerAlpha(int index) const {
		if (index >= Chunk::MAX_LAYERS) {
			return nullptr;
		}

		return m_layers[index].alphaTex.get();
	}

	Material* Chunk::getLayerDetail(int index) const {
		if (index >= Chunk::MAX_LAYERS) {
			return nullptr;
		}

		return m_layers[index].detailMat.get();
	}

	Vector2 Chunk::getLayerScale(int index) const {
		if (index >= Chunk::MAX_LAYERS) {
			return Vector2(1,1);
		}

		return m_layers[index].scale;
	}

	bool Chunk::isLayerVerticalProjection(int index) const {

		if (index >= Chunk::MAX_LAYERS) {
			return false;
		}

		return m_layers[index].isVerticalProjection;
	}

	//------------------------------------------------------------------------------
	// class GroupPrim
	//------------------------------------------------------------------------------

	GroupPrim::GroupPrim(Hint hint) : Primitive(hint) {
		m_type = GroupType;
	}

	GroupPrim::~GroupPrim() {
		clear();
	}

	void
	GroupPrim::addPrimitive(Primitive* prim, bool needfree) {
		m_primitives.push_back(prim);
		m_needFrees.push_back(needfree);
	}

	int
	GroupPrim::getPrimitiveCount() const {
		return s2i(m_primitives.size());
	}

	Primitive*
	GroupPrim::getPrimitive(int index) {
		return m_primitives[index];
	}

	void GroupPrim::clear() {
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

	RefPrim::RefPrim(Hint hint) : Primitive(hint) {
		m_type = ReferenceType;
		m_refered = nullptr;
		m_numIndexes = 0;
		m_indexes = nullptr;
	}

	RefPrim::~RefPrim() {
		TypeFree(m_indexes);
	}

	Primitive* RefPrim::getRefered() const {
		return m_refered;
	}

	void RefPrim::setRefered(Primitive* refered) {
		m_refered = refered;
	}

	void RefPrim::initialize(Primitive* refered, int numindexes /*= 0 */) {
		if (m_refered || m_numIndexes) {
			Errorf("RefPrim::initialize: already initialized");
			return;
		}

		m_refered = refered;
		m_numIndexes = numindexes;
		m_indexes = TypeAlloc<ushort_t>(numindexes);
	}
	int RefPrim::getNumIndexes() const {
		return m_numIndexes;
	}

	const ushort_t* RefPrim::getIndexesPointer() const {
		return m_indexes;
	}

	ushort_t* RefPrim::lockIndexes() {
		return m_indexes;
	}

	void RefPrim::unlockIndexes() {
		m_isDirtied = true;
		m_isIndexBufferDirtied = true;
	}

	//--------------------------------------------------------------------------
	// class GeoInstancing
	//
	// geometry instance primitive
	//--------------------------------------------------------------------------

	GeoInstance::GeoInstance(Hint hint) : Primitive(hint) {
		m_type = InstancingType;
		m_instanced = nullptr;
		m_params.reserve(16);
	}

	GeoInstance::~GeoInstance() {
	}

	Primitive* GeoInstance::getInstanced() const {
		return m_instanced;
	}

	void GeoInstance::setInstanced(Primitive* instanced) {
		m_instanced = instanced;
		m_isDirtied = true;
		setMaterial(instanced->getMaterial());
	}

	void GeoInstance::addInstance(const Param& param) {
		m_params.push_back(param);
		m_isDirtied = true;
	}

	void GeoInstance::addInstance( const AffineMat& mtx, const Vector4& user )
	{
		Param param;
		param.worldMatrix = mtx;
		param.userDefined = user;
		addInstance(param);
	}

	int GeoInstance::getNumInstance() const {
		return s2i(m_params.size());
	}

	const GeoInstance::Param& GeoInstance::getInstance(int index) const {
		AX_ASSERT(index < getNumInstance());
		return m_params[index];
	}

	const GeoInstance::ParamSeq& GeoInstance::getAllInstances() const {
		return m_params;
	}

	void GeoInstance::clearAllInstances() {
		m_params.clear();
	}

	void GeoInstance::setInstances(const ParamSeq& params) {
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

	void PrimitiveManager::hintUncache( Primitive* prim )
	{
		int id = prim->getCachedId();

		if (!id)
			return;

		if (isFrameHandle(id))
			return;

		m_waitUncache.push_back(id-1);
	}

}} // namespace Axon::Render
