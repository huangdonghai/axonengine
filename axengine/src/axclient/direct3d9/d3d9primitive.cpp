/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

AX_BEGIN_NAMESPACE

//------------------------------------------------------------------------------
// class D3D9Primitive
//------------------------------------------------------------------------------

D3D9Primitive::D3D9Primitive()
{
	m_src = nullptr;
	m_material = nullptr;

	m_overloadMaterial = nullptr;
	m_overloadedIndexObj = nullptr;
	m_instanceBuffer = nullptr;
	m_lightmap = nullptr;

	m_activeIndexes = 0;
}

D3D9Primitive::~D3D9Primitive()
{}

void D3D9Primitive::initialize(Primitive *src)
{
	m_src = src;
	m_hint = src->getHint();

	D3D9Primitive::update();
}

void D3D9Primitive::finalize()
{}

void D3D9Primitive::update()
{
	m_material = m_src->getMaterial();
	m_lightmap = m_src->getLightMap();
	m_isMatrixSet = m_src->isMatrixSet();
	m_matrix = m_src->getMatrix();
	m_activeIndexes = m_src->getActivedIndexes();
}

//------------------------------------------------------------------------------
// class D3D9geometry
//------------------------------------------------------------------------------


D3D9geometry::D3D9geometry()
	: m_geometrySize(1.0f)
{
}

D3D9geometry::~D3D9geometry()
{
	finalize();
}


void D3D9geometry::initialize(Primitive *src)
{
	D3D9Primitive::initialize(src);

	switch (m_src->getType()) {
	case Primitive::PointType:
		break;
	case Primitive::LineType:
		initLine();
		break;
	case Primitive::MeshType:
		initMesh();
		break;
	case Primitive::GuiType:
		break;
	case Primitive::ChunkType:
		break;
	default:
		AX_NO_DEFAULT;
	}
}

void D3D9geometry::finalize()
{
	D3D9Primitive::finalize();
}

void D3D9geometry::update()
{
	if (!m_src->isDirty() && m_hint == Primitive::HintStatic)
		return;

	D3D9Primitive::update();

	switch (m_src->getType()) {
	case Primitive::PointType:
		updatePoint();
		break;
	case Primitive::LineType:
		updateLine();
		break;
	case Primitive::MeshType:
		updateMesh();
		break;
	default:
		AX_NO_DEFAULT;
	}

	m_src->clearDirty();
}

void D3D9geometry::draw(Technique tech)
{
	bindVertexBuffer();

	if (m_instanceBuffer) {
		g_shaderMacro.setMacro(ShaderMacro::G_GEOMETRY_INSTANCING);
	}

	if (m_overloadMaterial)
		D3D9Draw::draw(m_overloadMaterial, tech, this);
	else
		D3D9Draw::draw(m_material, tech, this);

	if (m_instanceBuffer) {
		g_shaderMacro.resetMacro(ShaderMacro::G_GEOMETRY_INSTANCING);
	}

	unbindVertexBuffer();
}

void D3D9geometry::bindVertexBuffer()
{
	if (!m_instanceBuffer) {
		m_vertexObject.bind();
	} else {
		m_vertexObject.bindInstanced(m_instanceBuffer);
	}
}


void D3D9geometry::unbindVertexBuffer()
{
	if (!m_instanceBuffer) {
	} else {
		m_vertexObject.unbindInstanced();
	}
}

void D3D9geometry::drawElements()
{
	D3D9IndexObject *ib = &m_indexObject;

	if (m_overloadedIndexObj) {
		ib = m_overloadedIndexObj;
	}

	ib->drawElements(m_d3dPrimitiveType, m_vertexObject.getNumVerts());
}

void D3D9geometry::initPoint()
{
	m_vertexType = D3D9VertexObject::VertexDebug;
	m_d3dPrimitiveType = D3DPT_POINTLIST;

	updatePoint();
}

void D3D9geometry::initLine()
{
	m_vertexType = D3D9VertexObject::VertexDebug;
	m_d3dPrimitiveType = D3DPT_LINELIST;

	updateLine();
}

void D3D9geometry::initMesh()
{
	m_vertexType = D3D9VertexObject::VertexGeneric;

	MeshPrim *src = (MeshPrim*)m_src;

	if (src->isStriped()) {
		m_d3dPrimitiveType = D3DPT_TRIANGLESTRIP;
	} else {
		m_d3dPrimitiveType = D3DPT_TRIANGLELIST;
	}

	updateMesh();
}


void D3D9geometry::updatePoint()
{
	PointPrim *Point = static_cast<PointPrim*>(m_src);
	AX_ASSERT(Point);

	int refOffset = Point->getDrawOffset();
	int refCount = Point->getDrawCount();

	int m_allCount = Point->getNumPoints();
	AX_ASSERT(refOffset + refCount <= m_allCount);

	//		m_indexCount = line->getNumIndexes();
	//		m_currentIndexNum = 0;

	// create vertex buffer
//		m_vertexBuffer.setData(Point->getPointsPointer() + refOffset, m_vertexCount*m_vertexStride, m_hint);
//		m_vertexObject.setData(P)

	// create index buffer
	//		m_indexBuffer.setData(Point->getPointsPointer(), m_indexCount, m_hint, m_indexDataType);
}

void D3D9geometry::updateLine()
{
	LinePrim *line = static_cast<LinePrim*>(m_src);
	AX_ASSERT(line);

	int m_vertexCount = line->getNumVertexes();

	int m_indexCount = line->getNumIndexes();

	// create vertex buffer
	m_vertexObject.setData(line->getVertexesPointer(), m_vertexCount, m_hint, m_vertexType);

	// create index buffer
	m_indexObject.setData(line->getIndexPointer(), m_indexCount, m_hint, m_activeIndexes);
}


void D3D9geometry::updateMesh()
{
	MeshPrim *mesh = static_cast<MeshPrim*>(m_src);
	AX_ASSERT(mesh);

	int m_vertexCount = mesh->getNumVertexes();
	int m_indexCount = mesh->getNumIndexes();

	m_vertexObject.setData(mesh->getVertexesPointer(), m_vertexCount, m_hint, m_vertexType);
	m_indexObject.setData(mesh->getIndexPointer(), m_indexCount, m_hint, m_activeIndexes);
}


//------------------------------------------------------------------------------
// class D3D9text
//------------------------------------------------------------------------------

//	Material *D3D9text::m_fontMtr;
D3D9text::D3D9text()
{
}

D3D9text::~D3D9text()
{
}


void D3D9text::initialize(Primitive *src)
{
	D3D9Primitive::initialize(src);

	TextPrim *text = static_cast<TextPrim*>(m_src);
	AX_ASSERT(text);

	// copy text info from source primitive
	m_rect = text->getRect();
	m_position = text->getPosition();
	m_isSimpleText = text->getIsSimpleText();
	m_color = text->getColor();
	m_aspect = text->getAspect();
	m_format = text->getFormat();
	m_font = text->getFont();
	m_text = text->getText();
	m_horizonAlign = text->getHorizonAlign();
	m_verticalAlign = text->getVerticalAlign();

	if (m_isSimpleText) {
		m_format = TextPrim::ScaleByVertical;
		m_horizonAlign = TextPrim::Center;
		m_verticalAlign = TextPrim::VCenter;
	}

	int numchars = s2i(m_text.size());
}

void D3D9text::finalize()
{
	D3D9Primitive::finalize();
}

void D3D9text::update()
{
	D3D9Primitive::update();
}

#define BLINK_DIVISOR 0.075

void D3D9text::draw(Technique tech)
{
	if (!r_font.getBool()) {
		return;
	}
#if 0
	g_statistic->incValue(stat_numTextDrawElements);
#else
	stat_numTextDrawElements.inc();
#endif
	if (tech != Technique::Main)
		return;

	TextQuad tq;
	float width, height;

	if (m_isSimpleText) {
		width = 1.0f; height = 1.0f;

		const Matrix3 &axis = d3d9Camera->getViewAxis();
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

	WString wstr = u2w(m_text);
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
//			ulonglong_t t0 = OsUtil::microseconds();

		size_t len = m_font->updateTexture(pStr);

		// for error string, cann't upload or cann't render char

//			ulonglong_t t1 = OsUtil::microseconds();

		if (m_format & TextPrim::Blink) {
			Rgba color = m_color;
			color.a = 128+127*sinf(d3d9Camera->getTime() / BLINK_DIVISOR);
			offset = d3d9Draw->drawString(m_font, color, tq, startpos, pStr, len, scale, italic);
			goto next;
		}

//			ulonglong_t t2 = OsUtil::microseconds();

		// common
		offset = d3d9Draw->drawString(m_font, m_color, tq, startpos, pStr, len, scale, italic);

//			ulonglong_t t3 = OsUtil::microseconds();
		if (m_format & TextPrim::Bold) {
			d3d9Draw->drawString(m_font, m_color, tq, startpos+scale, pStr, len, scale, italic);
		}

next:
		total_len -= len;
		pStr += len;
		startpos += offset;
	}
}

//------------------------------------------------------------------------------
// class D3D9terrain
//------------------------------------------------------------------------------

D3D9terrain::D3D9terrain()
{
}

D3D9terrain::~D3D9terrain()
{
}

// implement D3D9Primitive
void D3D9terrain::initialize(Primitive *src)
{
	D3D9Primitive::initialize(src);

	m_vertexType = D3D9VertexObject::VertexChunk;
	m_d3dPrimitiveType = D3DPT_TRIANGLELIST;

	update();
}

void D3D9terrain::finalize()
{
	D3D9Primitive::finalize();
}

void D3D9terrain::update()
{
	if (!m_src->isDirty() && m_hint == Primitive::HintStatic)
		return;

	D3D9Primitive::update();

	ChunkPrim *chunk = static_cast<ChunkPrim*>(m_src);
	AX_ASSERT(chunk);

	m_isZonePrim = chunk->isZonePrim();
	Primitive::Hint hint = m_src->getHint();

	finalize();

	m_indexHint = m_hint;
#if 0
	if (m_isZonePrim) {
		m_indexHint = Primitive::Dynamic;
	}
#endif
	if (m_src->isDirty()) {
		m_terrainRect = chunk->getTerrainRect();

		m_zoneRect = chunk->getZoneRect();
		m_normalTexture = chunk->getNormalTexture();
		m_colorTexture = chunk->getColorTexture();

		m_chunkRect = chunk->getChunkRect();
		m_numLayers = chunk->getNumLayers();
		m_layerVisible = chunk->getLayerVisible();

		for (int i = 0; i < m_numLayers; i++) {
			m_layers[i].alphaTex = chunk->getLayerAlpha(i);
			m_layers[i].detailMat = chunk->getLayerDetail(i);
			m_layers[i].scale = chunk->getLayerScale(i);
			m_layers[i].isVerticalProjection = chunk->isLayerVerticalProjection(i);
		}
	}

	if (m_src->isVertexBufferDirty() || hint != Primitive::HintStatic) {
		int m_vertexCount = chunk->getNumVertexes();
		// create vertex buffer
		m_vertexObject.setData(chunk->getVertexesPointer(), m_vertexCount, m_hint, m_vertexType);
	}

	if (m_src->isIndexBufferDirty() || hint != Primitive::HintStatic) {
		int m_indexCount = chunk->getNumIndexes();

		m_indexObject.setData(chunk->getIndexesPointer(), m_indexCount, m_indexHint, m_activeIndexes);
	}

	m_src->clearDirty();
}

void D3D9terrain::draw(Technique tech)
{
#if 0
	g_statistic->incValue(stat_numTerrainDrawElements);
#else
	stat_numTerrainDrawElements.inc();
#endif

	if (m_overloadMaterial) {
		D3D9geometry::draw(tech);
		return;
	}

	bindVertexBuffer();

	bool combine = r_terrainLayerCombine.getBool();
	bool drawlayer = true;

	if (!r_detail.getBool() || !m_layerVisible) {
		drawlayer = false;
	}

	if (m_numLayers == 0) {
		drawlayer = false;
	}

	if (d3d9IsReflecting) {
		drawlayer = false;
	}

	if (tech != Technique::Main) {
		drawlayer = false;
	}

	m_material->setTexture(SamplerType::TerrainColor, m_colorTexture);
	m_material->setTexture(SamplerType::TerrainNormal, m_normalTexture);

	AX_SU(g_terrainRect, m_terrainRect);
	AX_SU(g_zoneRect, m_zoneRect);
	AX_SU(g_chunkRect, m_chunkRect);

	if (!drawlayer || !combine) {
		//D3D9geometry::draw(tech);
		D3D9Draw::draw(m_material, tech, this);

		if (!drawlayer) {
			return;
		}
	}

	// draw layer
	for (int i = 0; i < m_numLayers; i++) {
#if 0
		g_statistic->incValue(stat_numTerrainLayeredDrawElements);
#else
		stat_numTerrainLayeredDrawElements.inc();
#endif
		ChunkPrim::Layer &l = m_layers[i];
		l.detailMat->setTexture(SamplerType::TerrainColor, m_colorTexture);
		l.detailMat->setTexture(SamplerType::TerrainNormal, m_normalTexture);
		l.detailMat->setTexture(SamplerType::LayerAlpha, l.alphaTex.get());

		l.detailMat->setFeature(0, l.isVerticalProjection);
		l.detailMat->setFeature(1, combine && (i == 0)); // if is first layer, set first layer flag

		AX_SU(g_layerScale, l.scale);
		D3D9Draw::draw(l.detailMat.get(), Technique::Layer, this);
	}
}


//------------------------------------------------------------------------------
// class D3D9group
//------------------------------------------------------------------------------

D3D9group::D3D9group()
{
}

D3D9group::~D3D9group()
{
}

void D3D9group::initialize(Primitive *src)
{
	D3D9Primitive::initialize(src);

	GroupPrim *rpgroup = static_cast<GroupPrim*>(src);
	AX_ASSERT(rpgroup);

	uint_t count = rpgroup->getPrimitiveCount();

	for (uint_t i = 0; i < count; i++) {
		Primitive *s_prim = rpgroup->getPrimitive(i);
		int id = d3d9PrimitiveManager->cachePrimitive(s_prim);
		m_primitives.push_back(id);
	}
}

void D3D9group::finalize()
{
	m_primitives.clear();
	D3D9Primitive::finalize();
}

void D3D9group::update()
{
	D3D9Primitive::update();
}

void D3D9group::draw(Technique tech)
{
	for (size_t i = 0; i < m_primitives.size(); i++) {
		D3D9Primitive *prim = d3d9PrimitiveManager->getPrimitive(m_primitives[i]);
		prim->draw(tech);
	}
}

//------------------------------------------------------------------------------
// class D3D9ref
//------------------------------------------------------------------------------

D3D9ref::D3D9ref()
{}

D3D9ref::~D3D9ref()
{}

void D3D9ref::initialize(Primitive *src)
{
	D3D9Primitive::initialize(src);

	RefPrim *ref = static_cast<RefPrim*>(src);
	AX_ASSERT(ref);

	m_refed = d3d9PrimitiveManager->cachePrimitive(ref->getRefered());

	int indexcount = ref->getNumIndexes();

	if (!indexcount) {
		return;
	}

	m_indexObject.setData(ref->getIndexesPointer(), indexcount, ref->getHint(), m_activeIndexes);
}

void D3D9ref::finalize()
{
	D3D9Primitive::finalize();
}

void D3D9ref::update()
{
	D3D9Primitive::update();
}

void D3D9ref::draw(Technique tech)
{
	D3D9Primitive *prim = d3d9PrimitiveManager->getPrimitive(m_refed);
	if (m_material) {
		prim->setOverloadMaterial(m_material);
	}

	if (m_indexObject.haveData()) {
		prim->m_overloadedIndexObj = &m_indexObject;
	}

	if (m_instanceBuffer) {
		prim->m_instanceBuffer = m_instanceBuffer;
	}

	prim->draw(tech);

	prim->m_instanceBuffer = 0;

	if (m_material) {
		prim->unsetOverloadMaterial();
	}

	if (m_indexObject.haveData()) {
		prim->m_overloadedIndexObj = nullptr;
	}
}

//--------------------------------------------------------------------------
// class D3D9instance
//--------------------------------------------------------------------------

D3D9instance::D3D9instance()
{}

D3D9instance::~D3D9instance()
{}

void D3D9instance::initialize(Primitive *src)
{
	D3D9Primitive::initialize(src);

	update();
}

void D3D9instance::finalize()
{}

void D3D9instance::update()
{
	D3D9Primitive::update();
	InstancePrim *gi = static_cast<InstancePrim*>(m_src);
	AX_ASSERT(gi);

	m_instanced = d3d9PrimitiveManager->cachePrimitive(gi->getInstanced());
	m_buffer.setData(gi->getNumInstance(), gi->getAllInstances());
	m_src->clearDirty();
}

void D3D9instance::draw(Technique tech)
{
	D3D9Primitive *prim = d3d9PrimitiveManager->getPrimitive(m_instanced);

	prim->m_instanceBuffer = &m_buffer;
	prim->draw(tech);
	prim->m_instanceBuffer = 0;
}


//--------------------------------------------------------------------------
// class D3D9primitivemanager
//--------------------------------------------------------------------------

D3D9primitivemanager::D3D9primitivemanager()
	: m_numStaticPrims(0)
	, m_numFramePrims(0)
	, m_freePrimLink(NULL)
	, m_numWaitForDelete(0)
{
	m_frameId = 0;

	memset(m_staticPrims, 0, sizeof(m_staticPrims));
	memset(m_framePrims, 0, sizeof(m_staticPrims));
	memset(m_waitForDelete, 0, sizeof(m_staticPrims));

	// setup free list
	m_freePrimLink = &m_staticPrims[0];
	for (int i = 0; i < MAX_PRIMITIVES-1; i++) {
		m_staticPrims[i] = (D3D9geometry*)&m_staticPrims[i+1];
	}
	m_staticPrims[MAX_PRIMITIVES-1] = NULL;
}

D3D9primitivemanager::~D3D9primitivemanager()
{
}

int D3D9primitivemanager::cachePrimitive(Primitive *prim)
{
#if 0
	g_statistic->incValue(stat_numElements);
#else
	stat_numElements.inc();
#endif

	int f = prim->getCachedFrame();
	int h = prim->getCachedId();

	if (f == m_frameId)
		return h;

	int new_id;

	if (prim->getHint() != Primitive::HintFrame) {
		if (h == 0) {
			// init
			D3D9Primitive *glprim = createPrim(prim);

			findStaticFreeSlot(new_id);
			linkId(new_id, glprim);

			prim->setCachedId(new_id);
		} else {
			// update
			D3D9Primitive *glprim = getPrimitive(h);
			glprim->update();

			new_id = h;
		}
	} else {
		// FIXME: if have a static gl primitive for this prim, we uncache it first
		if (isStatic(h))
			uncachePrimitive(prim);

		D3D9Primitive *glprim = createPrim(prim);

		new_id = (int)((m_numFramePrims+1) | FRAME_FLAG);
		m_numFramePrims++;
		linkId(new_id, glprim);

		prim->setCachedId(new_id);

#if 0
		g_statistic->setValue(stat_framePrims, m_numFramePrims);
#else
		stat_framePrims.setInt(m_numFramePrims);
#endif
	}

	prim->setCachedFrame(m_frameId);
#if 0
	g_statistic->incValue(stat_changedPrims);
#else
	stat_changedPrims.inc();
#endif
	if (prim->getHint() == Primitive::HintFrame)
		delete(prim);

	return new_id;
}

void D3D9primitivemanager::uncachePrimitive(Primitive *prim)
{
	int id = prim->getCachedId();

	if (!id)
		return;

	if (isFrameHandle(id))
		return;

	if (m_numWaitForDelete >= MAX_PRIMITIVES) {
		Errorf("D3D9primitivemanager::uncachePrimitive: overflowed");
		return;
	}

	m_waitForDelete[m_numWaitForDelete++] = uint_t(id)-1;
#if 0
	g_statistic->decValue(stat_staticPrims);
#else
	stat_staticPrims.dec();
#endif
}

void D3D9primitivemanager::syncFrame()
{
	// clear frame primitive
	for (int i = 0; i < m_numFramePrims; i++) {
		freePrim(m_framePrims[i]);
	}
	m_numFramePrims = 0;

	// clear wait for delete
	for (int i = 0; i < m_numWaitForDelete; i++) {
		freePrim(m_staticPrims[m_waitForDelete[i]]);

		// add to free link
		m_staticPrims[m_waitForDelete[i]] = (D3D9geometry*)m_freePrimLink;
		m_freePrimLink = &m_staticPrims[m_waitForDelete[i]];
	}
	m_numWaitForDelete = 0;

	// clear main thread's uncached
	List<int>::const_iterator it = m_waitUncache.begin();
	for (; it != m_waitUncache.end(); ++it) {
		int id = *it;
		freePrim(m_staticPrims[id]);

		// add to free link
		m_staticPrims[id] = (D3D9geometry*)m_freePrimLink;
		m_freePrimLink = &m_staticPrims[id];
	}
	m_waitUncache.clear();

	m_frameId++;
}

void D3D9primitivemanager::findStaticFreeSlot(int &handle)
{
	if (m_freePrimLink == NULL) {
		Errorf("no free primitive slot");
		return;
	}

	// pop a free slot
	D3D9Primitive** ptr = (D3D9Primitive**)m_freePrimLink;
	intptr_t *link = (intptr_t*)m_freePrimLink;
	m_freePrimLink = (void*)(*link);

	handle = int((ptr - m_staticPrims) + 1);
#if 0
	g_statistic->incValue(stat_staticPrims);
#else
	stat_staticPrims.inc();
#endif
}

void D3D9primitivemanager::linkId(int id, D3D9Primitive *glprim)
{
	if (id == 0)
		Errorf("D3D9primitivemanager::linkId: error id");

	int index =(id & INDEX_MASK) - 1;
	AX_ASSURE(index < MAX_PRIMITIVES);

	if (isStatic(id)) {
		m_staticPrims[index] = glprim;
	} else {
		m_framePrims[index] = glprim;
	}
}

D3D9Primitive *D3D9primitivemanager::createPrim(Primitive *prim)
{
	D3D9Primitive *glprim;

	Primitive::Type primType = prim->getType();
	if (primType == Primitive::TextType)
		glprim = m_textAlloc.alloc();
	else if (primType == Primitive::GroupType)
		glprim = m_groupAlloc.alloc();
	else if (primType == Primitive::ChunkType)
		glprim = m_terrainAlloc.alloc();
	else if (primType == Primitive::ReferenceType)
		glprim = m_refAlloc.alloc();
	else if (primType == Primitive::InstancingType)
		glprim = m_instanceAlloc.alloc();
	else
		glprim = m_geometryAlloc.alloc();

	glprim->initialize(prim);

	return glprim;
}

void D3D9primitivemanager::freePrim(D3D9Primitive *prim)
{
	D3D9Primitive::Type type = prim->getType();

	switch (type) {
	case D3D9Primitive::kGeometry:
		return m_geometryAlloc.free((D3D9geometry*)prim);
	case D3D9Primitive::kText:
		return m_textAlloc.free((D3D9text*)prim);
	case D3D9Primitive::kTerrain:
		return m_terrainAlloc.free((D3D9terrain*)prim);
	case D3D9Primitive::kGroup:
		return m_groupAlloc.free((D3D9group*)prim);
	case D3D9Primitive::kRef:
		return m_refAlloc.free((D3D9ref*)prim);
	case D3D9Primitive::kInstance:
		return m_instanceAlloc.free((D3D9instance*)prim);
	default:
		return;
	}
}

void D3D9primitivemanager::onDeviceLost()
{
}

void D3D9primitivemanager::onReset()
{
}

AX_END_NAMESPACE

