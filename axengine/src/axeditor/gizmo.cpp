/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

AX_BEGIN_NAMESPACE

enum {
	GIZMO_SIZE = 100
};

//--------------------------------------------------------------------------
// class MoveGizmo, editor manipulator
//--------------------------------------------------------------------------

MoveGizmo::MoveGizmo()
{
	TypeZeroArray(m_lines);
	TypeZeroArray(m_meshs);
	m_highlit = None;
	m_material = new Material("_blend");
}

MoveGizmo::~MoveGizmo()
{
	clear();
}

inline Rgba MoveGizmo::getColor(int axis, Rgba c)
{
	if (axis == m_highlit)
		return Rgba::Yellow;
	return c;
}

void MoveGizmo::setup(const RenderCamera &camera, const Vector3 &pos, const Matrix3 &axis, float scale)
{
	m_pos = pos;
	m_axis = axis;
	m_scale = scale;
	m_length = GIZMO_SIZE * scale;

#if 0
	xSetupAxis(m_lines[X], pos, pos + axis[0] * length, getColor(X, Rgba::Red));
	xSetupAxis(m_lines[Y], pos, pos + axis[1] * length, getColor(Y, Rgba::Green));
	xSetupAxis(m_lines[Z], pos, pos + axis[2] * length, getColor(Z, Rgba::Blue));

	Render::Mesh::setupCone(m_meshs[X], pos + axis[0] * length, length * 0.03f, pos + axis[0] * (length*1.3f), getColor(X, Rgba::Red) );
	Render::Mesh::setupCone(m_meshs[Y], pos + axis[1] * length, length * 0.03f, pos + axis[1] * (length*1.3f), getColor(Y, Rgba::Green));
	Render::Mesh::setupCone(m_meshs[Z], pos + axis[2] * length, length * 0.03f, pos + axis[2] * (length*1.3f), getColor(Z, Rgba::Blue));
#else
	setupAxis(X);
	setupAxis(Y);
	setupAxis(Z);
#endif
	setupPlane(XY);
	setupPlane(YZ);
	setupPlane(XZ);

	setupXYZ(camera);

	for (int i = 0; i < NumberId; i++) {
//		m_lines[i]->setDepthHack(true);
//		m_meshs[i]->setDepthHack(true);
	}
}

void MoveGizmo::clear()
{
	for (int i = 0; i < NumberId; i++) {
		SafeDelete(m_lines[i]);
		SafeDelete(m_meshs[i]);
	}
}

void MoveGizmo::doRender()
{
	for (int i = 0; i < NumberId; i++) {
		if (m_meshs[i]) {
			m_meshs[i]->setMaterial(m_material);
			g_renderSystem->addToScene(m_meshs[i]);
		}
		if (m_lines[i]) {
			m_lines[i]->setMaterial(m_material);
			g_renderSystem->addToScene(m_lines[i]);
		}
	}
}

int MoveGizmo::doSelect(RenderCamera *camera, int x, int y,int selectedSize)
{
	if (!m_lines[0])
		return -1;

	int size = selectedSize;
	Rect r(x-size, y-size, size*2+1, size*2+1);
	RenderCamera cam = camera->createSelectionCamera(r);

	g_renderSystem->beginHitTest(cam);

	g_renderSystem->loadHitId(X);
	g_renderSystem->hitTest(m_lines[X]);
	g_renderSystem->hitTest(m_meshs[X]);

	g_renderSystem->loadHitId(Y);
	g_renderSystem->hitTest(m_lines[Y]);
	g_renderSystem->hitTest(m_meshs[Y]);

	g_renderSystem->loadHitId(Z);
	g_renderSystem->hitTest(m_lines[Z]);
	g_renderSystem->hitTest(m_meshs[Z]);

	g_renderSystem->loadHitId(XY);
	g_renderSystem->hitTest(m_lines[XY]);

	g_renderSystem->loadHitId(YZ);
	g_renderSystem->hitTest(m_lines[YZ]);

	g_renderSystem->loadHitId(XZ);
	g_renderSystem->hitTest(m_lines[XZ]);

	g_renderSystem->loadHitId(XYZ);
	g_renderSystem->hitTest(m_lines[XYZ]);

	HitRecords records = g_renderSystem->endHitTest();

	if (records.empty())
		return -1;

	float minz = 1.0f;
	int nearest = -1;
	for (size_t i = 0; i < records.size(); i++) {
		int id = records[i].name;

		if (records[i].minz < minz) {
			minz = records[i].minz;
			nearest = id;
		}
	}


	if (nearest == -1) nearest = None;
	return nearest;
}

int MoveGizmo::doSelect(View *view, int x, int y)
{
	if (!m_lines[0])
		return -1;

	int l = GIZMO_SIZE;
	int size = std::max<int>(1, l / 20);
	Rect r(x-size, y-size, size*2+1, size*2+1);
	view->beginSelect(r);

	g_renderSystem->loadHitId(X);
	g_renderSystem->hitTest(m_lines[X]);
	g_renderSystem->hitTest(m_meshs[X]);

	g_renderSystem->loadHitId(Y);
	g_renderSystem->hitTest(m_lines[Y]);
	g_renderSystem->hitTest(m_meshs[Y]);

	g_renderSystem->loadHitId(Z);
	g_renderSystem->hitTest(m_lines[Z]);
	g_renderSystem->hitTest(m_meshs[Z]);

	g_renderSystem->loadHitId(XY);
	g_renderSystem->hitTest(m_lines[XY]);

	g_renderSystem->loadHitId(YZ);
	g_renderSystem->hitTest(m_lines[YZ]);

	g_renderSystem->loadHitId(XZ);
	g_renderSystem->hitTest(m_lines[XZ]);

	g_renderSystem->loadHitId(XYZ);
	g_renderSystem->hitTest(m_lines[XYZ]);

	int axis = view->endSelect();

	if (axis == -1) axis = None;
	return axis;
}

void MoveGizmo::setHighlight(int axis)
{
	m_highlit = (SelectId)axis;
}

void MoveGizmo::setupAxis(int axis)
{
	Vector3 p0, p1, p2;
	Rgba color;

	float len0 = m_length * 0.3f, len1 = m_length, len2 = m_length * 1.3f;
	if (axis == X) {
		p0 = m_pos + m_axis[0] * len0;
		p1 = m_pos + m_axis[0] * len1;
		p2 = m_pos + m_axis[0] * len2;
		color = Rgba::Red;
	} else if (axis == Y) {
		p0 = m_pos + m_axis[1] * len0;
		p1 = m_pos + m_axis[1] * len1;
		p2 = m_pos + m_axis[1] * len2;
		color = Rgba::Green;
	} else if (axis == Z) {
		p0 = m_pos + m_axis[2] * len0;
		p1 = m_pos + m_axis[2] * len1;
		p2 = m_pos + m_axis[2] * len2;
		color = Rgba::Blue;
	} else {
		Errorf("MoveGizmo::setupAxis: error axis");
	}

	if (axis == m_highlit) {
		color = Rgba::Yellow;
	}

	LinePrim*& line = m_lines[axis];
	if (!line) {
		line = new LinePrim(Primitive::HintDynamic);
		line->init(2, 2);
		line->lock();
		line->setIndex(0, 0);
		line->setIndex(1, 1);
		line->unlock();
	}

	line->lock();
	DebugVertex &v0 = line->getVertexRef(0);
	DebugVertex &v1 = line->getVertexRef(1);
	v0.position = p0;
	v0.color = color;
	v1.position = p1;
	v1.color = color;
	line->unlock();

	MeshPrim::setupCone(m_meshs[axis], p1, m_length * 0.03f, p2, color );
}


void MoveGizmo::setupPlane(int axis)
{
	Vector3 v0;
	Vector3 v1;
	Rgba color0, color1;

	switch (axis) {
	case XY:
		v0 = m_axis[0]; color0 = Rgba::Red;
		v1 = m_axis[1]; color1 = Rgba::Green;
		break;
	case YZ:
		v0 = m_axis[1]; color0 = Rgba::Green;
		v1 = m_axis[2]; color1 = Rgba::Blue;
		break;
	case XZ:
		v0 = m_axis[0]; color0 = Rgba::Red;
		v1 = m_axis[2]; color1 = Rgba::Blue;
		break;
	default:
		AX_WRONGPLACE;
		break;
	}

	if (axis == m_highlit) {
		color0 = Rgba::Yellow;
		color1 = Rgba::Yellow;
	}

	LinePrim*& line = m_lines[axis];
	if (!line) {
		line = new LinePrim(Primitive::HintDynamic);
		line->init(4, 4);
		ushort_t *idxes = line->lockIndexes();
		idxes[0] = 0;
		idxes[1] = 1;
		idxes[2] = 2;
		idxes[3] = 3;
		line->unlockIndexes();
	}

	float length = m_length * 0.5f;

	DebugVertex *verts = line->lockVertexes();
	verts[0].position = m_pos + v0 * length;
	verts[0].color = color0;
	verts[1].position = verts[0].position + v1 * length;
	verts[1].color = color0;
	verts[2].position = m_pos + v1 * length;
	verts[2].color = color1;
	verts[3].position = verts[1].position;
	verts[3].color = color1;
	line->unlockVertexes();

	MeshPrim*& mesh = m_meshs[axis];
	bool first = false;
	if (!mesh) {
		mesh = new MeshPrim(Primitive::HintDynamic);
		mesh->init(4, 6);
		ushort_t *idxes = mesh->lockIndexes();
		idxes[0] = 0;
		idxes[1] = 1;
		idxes[2] = 2;
		idxes[3] = 2;
		idxes[4] = 1;
		idxes[5] = 3;
		mesh->unlockIndexes();
		first = true;
	}

	Rgba meshcolor(255,255,0,0);

	if (axis == m_highlit) meshcolor.a = 64;

	MeshVertex *mverts = mesh->lockVertexes();
	if (first) memset(mverts, 0, sizeof(MeshVertex) * 4);
	mverts[0].position = m_pos;
	mverts[0].color = meshcolor;
	mverts[1].position = m_pos + v0 * length;
	mverts[1].color = meshcolor;
	mverts[2].position = m_pos + v1 * length;
	mverts[2].color = meshcolor;
	mverts[3].position = m_pos + v0 * length + v1 * length;
	mverts[3].color = meshcolor;
	mesh->unlockVertexes();
}

void MoveGizmo::setupXYZ(const RenderCamera &camera)
{
	float len = 6.0f * m_scale;
	Rgba color = Rgba::LtGrey;

	if (m_highlit == XYZ)
		color = Rgba::Yellow;

	LinePrim*& line = m_lines[XYZ];

	if (!line) {
		line = new LinePrim(Primitive::HintDynamic);
		line->init(4, 8);
		ushort_t *idxes = line->lockIndexes();
		idxes[0] = 0;
		idxes[1] = 1;
		idxes[2] = 1;
		idxes[3] = 2;
		idxes[4] = 2;
		idxes[5] = 3;
		idxes[6] = 3;
		idxes[7] = 0;
		line->unlockIndexes();
	}

	const Matrix3 &axis = camera.viewAxis();
	DebugVertex *verts = line->lockVertexes();
	verts[0].position = m_pos - axis[1] * len - axis[2] * len;
	verts[0].color = color;
	verts[1].position = m_pos - axis[1] * len + axis[2] * len;
	verts[1].color = color;
	verts[2].position = m_pos + axis[1] * len + axis[2] * len;
	verts[2].color = color;
	verts[3].position = m_pos + axis[1] * len - axis[2] * len;
	verts[3].color = color;
	line->unlockVertexes();
}

//--------------------------------------------------------------------------
// class RotateGizmo, rotate gizmo
//--------------------------------------------------------------------------

RotateGizmo::RotateGizmo()
{
	m_centerLine = nullptr;
	TypeZeroArray(m_circles);
	m_innerBound = nullptr;
	m_outerBound = nullptr;
	m_crank = nullptr;

	m_highlit = None;
	m_material = new Material("_blend");

	m_enabledCrank = false;
}

RotateGizmo::~RotateGizmo()
{
	SafeDelete(m_centerLine);
	SafeDelete(m_circles[0]);
	SafeDelete(m_circles[1]);
	SafeDelete(m_circles[2]);
	SafeDelete(m_innerBound);
	SafeDelete(m_outerBound);
}

void RotateGizmo::setup(const RenderCamera &camera, const Vector3 &pos, const Matrix3 &axis, float scale)
{
	m_pos = pos;
	m_axis = axis;
	m_scale = scale;
	m_length = GIZMO_SIZE * scale;

	const Matrix3 &camaxis = camera.viewAxis();
	Plane plane(pos, camaxis[0]);

	float length = m_length;
	if (LinePrim::setupAxis(m_centerLine, pos, axis, length * 0.5f, getCenterColor(X), getCenterColor(Y), getCenterColor(Z))) {
		m_centerLine->setMaterial(m_material);
	}

	if (LinePrim::setupCircle(m_circles[0], pos, axis[1] * length, axis[2] * length, getColor(X), CirculSubdivided, plane)) {
		m_circles[0]->setMaterial(m_material);
	}
	if (LinePrim::setupCircle(m_circles[1], pos, axis[0] * length, axis[2] * length, getColor(Y), CirculSubdivided, plane)) {
		m_circles[1]->setMaterial(m_material);
	}
	if (LinePrim::setupCircle(m_circles[2], pos, axis[0] * length, axis[1] * length, getColor(Z), CirculSubdivided, plane)) {
		m_circles[2]->setMaterial(m_material);
	}

	if (LinePrim::setupCircle(m_innerBound, pos, camaxis[1] * length, camaxis[2] * length, Rgba::DkGrey, CirculSubdivided)) {
		m_innerBound->setMaterial(m_material);
	}
	if (LinePrim::setupCircle(m_outerBound, pos, camaxis[1] * length * 1.3f, camaxis[2] * length * 1.3f, getColor(Screen), CirculSubdivided)) {
		m_outerBound->setMaterial(m_material);
	}

	if (!m_enabledCrank)
		return;

	setupCrank(camera);
}

void RotateGizmo::doRender()
{
	if (m_centerLine)
		g_renderSystem->addToScene(m_centerLine);
	if (m_circles[0])
		g_renderSystem->addToScene(m_circles[0]);
	if (m_circles[1])
		g_renderSystem->addToScene(m_circles[1]);
	if (m_circles[2])
		g_renderSystem->addToScene(m_circles[2]);
	if (m_innerBound)
		g_renderSystem->addToScene(m_innerBound);
	if (m_outerBound )
		g_renderSystem->addToScene(m_outerBound);
	if (m_enabledCrank && m_crank)
		g_renderSystem->addToScene(m_crank);
}

int RotateGizmo::doSelect(View *view, int x, int y)
{
	// check if initialized
	if (!m_circles[0])
		return -1;

	int l = GIZMO_SIZE;
	int size = std::max<int>(1, l / 20);
	Rect r(x-size, y-size, size*2+1, size*2+1);
	view->beginSelect(r);

	g_renderSystem->loadHitId(X);
	g_renderSystem->hitTest(m_circles[0]);

	g_renderSystem->loadHitId(Y);
	g_renderSystem->hitTest(m_circles[1]);

	g_renderSystem->loadHitId(Z);
	g_renderSystem->hitTest(m_circles[2]);

	g_renderSystem->loadHitId(Screen);
	g_renderSystem->hitTest(m_outerBound);

	int axis = view->endSelect();

	if (axis == -1) axis = None;
	return axis;
}

void RotateGizmo::setHighlight(int id)
{
	m_highlit = (SelectId)id;
}

void RotateGizmo::setCrank(float start, float end)
{
	m_enabledCrank = true;
	m_crankStart = start;
	m_crankEnd = end;
}

void RotateGizmo::disableCrank()
{
	m_enabledCrank = false;
}

void RotateGizmo::setupCrank(const RenderCamera &camera)
{
	if (m_highlit == None)
		return;

	Vector3 left, up;
	Rgba color;
	const Matrix3 &camaxis = camera.viewAxis();

	switch (m_highlit) {
	case X:
		left = m_axis[1] * m_length;
		up = m_axis[2] * m_length;
		color = Rgba::Red;
		break;
	case Y:
		left = m_axis[2] * m_length;
		up = m_axis[0] * m_length;
		color = Rgba::Green;
		break;
	case Z:
		left = m_axis[0] * m_length;
		up = m_axis[1] * m_length;
		color = Rgba::Blue;
		break;
	case Screen:
		left = camaxis[1] * m_length * 1.3f;
		up = camaxis[2] * m_length * 1.3f;
		color = Rgba::LtGrey;
		break;
	}

	color.a = 64;

	MeshPrim::setupFan(m_crank, m_pos, left, up, m_crankStart, m_crankEnd, color, CrankSubdivided, m_material);
}

Rgba RotateGizmo::getColor(int id)
{
	if (m_highlit == id)
		return Rgba::Yellow;

	if (id == X) {
		return Rgba::Red;
	} else if (id == Y) {
		return Rgba::Green;
	} else if (id == Z) {
		return Rgba::Blue;
	} else if (id == Screen) {
		return Rgba::LtGrey;
	}

	return Rgba::White;
}

Rgba RotateGizmo::getCenterColor(int id)
{
	if (m_highlit != id)
		return Rgba::MdGrey;

	if (id == X) {
		return Rgba::Red;
	} else if (id == Y) {
		return Rgba::Green;
	} else if (id == Z) {
		return Rgba::Blue;
	} else if (id == Screen) {
		return Rgba::LtGrey;
	}

	return Rgba::White;
}


//--------------------------------------------------------------------------
// class ScaleGizmo, rotate gizmo
//--------------------------------------------------------------------------

ScaleGizmo::ScaleGizmo()
{
	TypeZeroArray(m_lines);
	TypeZeroArray(m_meshs);
	m_highlit = None;
	m_material = new Material("_blend");
}

ScaleGizmo::~ScaleGizmo()
{
	for (int i = 0; i < NumberId; i++) {
		SafeDelete(m_lines[i]);
		SafeDelete(m_meshs[i]);
	}
}

void ScaleGizmo::setup(const RenderCamera &camera, const Vector3 &pos, const Matrix3 &axis, float scale)
{
	m_pos = pos;
	m_axis = axis;
	m_scale = scale;
	m_length = GIZMO_SIZE * scale;
	float space = std::max(m_scale * 8, m_length * 0.1f);

	LinePrim::setupLine(m_lines[X], pos+axis[0]*space, pos+axis[0]*m_length, getColor(X));
	setupScreenQuad(camera, m_meshs[X], pos+axis[0]*m_length, getColor(X));
	LinePrim::setupLine(m_lines[Y], pos+axis[1]*space, pos+axis[1]*m_length, getColor(Y));
	setupScreenQuad(camera, m_meshs[Y], pos+axis[1]*m_length, getColor(Y));
	LinePrim::setupLine(m_lines[Z], pos+axis[2]*space, pos+axis[2]*m_length, getColor(Z));
	setupScreenQuad(camera, m_meshs[Z], pos+axis[2]*m_length, getColor(Z));

	// ÆÁ±Îµô´íÇÐ·ÅËõ·½¿ò --timlly add
	setupScreenQuad(camera, m_meshs[XYZ], pos, getColor(XYZ));
}

int ScaleGizmo::doSelect(View *view, int x, int y)
{
	// check if initialized
	if (!m_lines[0])
		return -1;

	int l = GIZMO_SIZE;
	int size = std::max<int>(1, l / 20);
	Rect r(x-size, y-size, size*2+1, size*2+1);

//		view->beginSelect(r);

	//gRenderSystem->loadHitId(XYZ);
	//gRenderSystem->hitTest(m_meshs[XYZ]);

//		int axis = view->endSelect();
	/*if (axis != -1) {
		return XYZ;
	}*/

	view->beginSelect(r);

	g_renderSystem->loadHitId(X);
	g_renderSystem->hitTest(m_lines[X]);
	g_renderSystem->hitTest(m_meshs[X]);

	g_renderSystem->loadHitId(Y);
	g_renderSystem->hitTest(m_lines[Y]);
	g_renderSystem->hitTest(m_meshs[Y]);

	g_renderSystem->loadHitId(Z);
	g_renderSystem->hitTest(m_lines[Z]);
	g_renderSystem->hitTest(m_meshs[Z]);

	int axis = view->endSelect();

	if (axis == -1) axis = None;
	return axis;
}

void ScaleGizmo::doRender()
{
	for (int i = 0; i < NumberId; i++) {
		if (m_meshs[i]) {
			m_meshs[i]->setMaterial(m_material);
			g_renderSystem->addToScene(m_meshs[i]);
		}
		if (m_lines[i]) {
			m_lines[i]->setMaterial(m_material);
			g_renderSystem->addToScene(m_lines[i]);
		}
	}
}


void ScaleGizmo::setHighlight(int id)
{
	m_highlit = (SelectId)id;
}

Rgba ScaleGizmo::getColor(int id)
{
	if (m_highlit == id)
		return Rgba::Yellow;

	if (id == X) {
		return Rgba::Red;
	} else if (id == Y) {
		return Rgba::Green;
	} else if (id == Z) {
		return Rgba::Blue;
	} else if (id == XYZ) {
		return Rgba::LtGrey;
	}

	return Rgba::White;
}
void ScaleGizmo::setupScreenQuad(const RenderCamera &camera, MeshPrim*& mesh, const Vector3 &pos, Rgba color)
{
	if (!mesh) {
		mesh = new MeshPrim(Primitive::HintDynamic);
		mesh->init(4, 6);
		ushort_t *idx = mesh->lockIndexes();
		idx[0] = 0; idx[1] = 1; idx[2] = 2;
		idx[3] = 2; idx[4] = 1; idx[5] = 3;
		mesh->unlockIndexes();
		MeshVertex *vert = mesh->lockVertexes();
		memset(vert, 0, sizeof(MeshVertex) * 4);
		mesh->unlockVertexes();
	}

	float len = 6.0f * m_scale;
	const Matrix3 &axis = camera.viewAxis();
	MeshVertex *vert = mesh->lockVertexes();
	vert[0].position = pos + axis[1] * len - axis[2] * len;
	vert[0].color = color;
	vert[1].position = pos - axis[1] * len - axis[2] * len;
	vert[1].color = color;
	vert[2].position = pos + axis[1] * len + axis[2] * len;
	vert[2].color = color;
	vert[3].position = pos - axis[1] * len + axis[2] * len;
	vert[3].color = color;
	mesh->unlockVertexes();
}

AX_END_NAMESPACE
