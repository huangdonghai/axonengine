/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE

int GLdriver::cachePrimitive(Primitive *prim) {
	return glPrimitiveManager->cachePrimitive(prim);
}

void GLdriver::uncachePrimitive(Primitive *prim) {
	glPrimitiveManager->uncachePrimitive(prim);
}


//------------------------------------------------------------------------------
// class GLprimitive
//------------------------------------------------------------------------------

GLprimitive::GLprimitive() {
	m_source = nullptr;
	m_material = nullptr;

	m_overloadMaterial = nullptr;
	m_overloadedIndexbuffer = nullptr;
	m_instanceParams = nullptr;
	m_lightmap = nullptr;

	m_activeIndexes = 0;
}

GLprimitive::~GLprimitive() {}

void GLprimitive::initialize(Primitive *src) {
	m_source = src;
	m_hint = src->getHint();
#if 0
	m_material = m_source->getMaterial();
	m_lightmap = m_source->getLightMap();
	m_isMatrixSet = src->isMatrixSet();
	m_matrix = src->getMatrix();
	m_flags = src->getFlags();
	m_activeIndexes = src->getActivedIndexes();
#else
	GLprimitive::update();
#endif
}

void GLprimitive::finalize() {}

void GLprimitive::update() {
	m_material = m_source->getMaterial();
	m_lightmap = m_source->getLightMap();
	m_isMatrixSet = m_source->isMatrixSet();
	m_matrix = m_source->getMatrix();
	m_activeIndexes = m_source->getActivedIndexes();
}

//------------------------------------------------------------------------------
// class GLgeometry
//------------------------------------------------------------------------------


GLgeometry::GLgeometry()
	: m_isFrameLieftime(false)
	, m_vertexOffset(0)
	, m_vertexStride(0)
	, m_vertexCount(0)
#if 0
	, m_indexCount(0)
	, m_currentIndexNum(0)
	, m_indexOffset(0)
#endif
	, m_color(255, 255, 255, 255)
	, m_geometrySize(1.0f)
{
}

GLgeometry::~GLgeometry() {
	finalize();
}


void GLgeometry::initialize(Primitive *src) {
	GLprimitive::initialize(src);

	switch (m_source->getType()) {
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
		AX_WRONGPLACE;
	}
}

void GLgeometry::finalize() {
	if (m_isFrameLieftime)
		return;
	GLprimitive::finalize();
}

void GLgeometry::update() {
	GLprimitive::update();

	switch (m_source->getType()) {
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
		AX_WRONGPLACE;
	}
}

void GLgeometry::draw(Technique tech) {
	bindVertexBuffer();

	if (m_instanceParams) {
		g_shaderMacro.setMacro(ShaderMacro::G_GEOMETRY_INSTANCING);
	}

	if (m_overloadMaterial)
		GLrender::draw(m_overloadMaterial, tech, this);
	else
		GLrender::draw(m_material, tech, this);

	if (m_instanceParams) {
		g_shaderMacro.resetMacro(ShaderMacro::G_GEOMETRY_INSTANCING);
	}
}

void GLgeometry::bindVertexBuffer() {
	GLrender::bindVertexBuffer(m_vertexDefs, m_vertexStride, m_vertexBuffer.getObject(), m_vertexOffset);
}


void GLgeometry::drawElements() {

	if (!m_instanceParams) {
		GLindexbuffer *ib = &m_indexBuffer;

		if (m_overloadedIndexbuffer) {
			ib = m_overloadedIndexbuffer;
		}

		ib->drawElements(m_elementType);
	} else {
		drawElementsInstanced();
	}
}

void GLgeometry::drawElementsInstanced() {
#if 0
	GLindexbuffer *ib = &m_indexBuffer;

	if (m_overloadedIndexbuffer) {
		ib = m_overloadedIndexbuffer;
	}

	ib->bind();

	const InstancePrim::Param *params = m_instanceParams;

	for (size_t i = 0; i < params.size(); i++) {
		const InstancePrim::Param &param = params[i];

		glMultiTexCoord4fv(GL_TEXTURE2, param.worldMatrix.getRow(0).c_ptr());
		glMultiTexCoord4fv(GL_TEXTURE3, param.worldMatrix.getRow(1).c_ptr());
		glMultiTexCoord4fv(GL_TEXTURE4, param.worldMatrix.getRow(2).c_ptr());
		glMultiTexCoord4fv(GL_TEXTURE5, param.userDefined.c_ptr());

		ib->drawElementsWithoutBind(m_elementType);
	}
#endif
}


void GLgeometry::initPoint() {
	m_vertexStride = sizeof(DebugVertex);

	m_vertexDefs.push_back(VDF_position);
	m_vertexDefs.push_back(VDF_color);

	m_elementType = GL_POINTS;

	updatePoint();
}

void GLgeometry::initPointSprite() {
}

void GLgeometry::initLine() {
	m_vertexStride = sizeof(DebugVertex);

	m_vertexDefs.push_back(VDF_position);
	m_vertexDefs.push_back(VDF_color);

	m_elementType = GL_LINES;

	updateLine();
}


void GLgeometry::initLineStrip() {
}


void GLgeometry::initMesh() {
	m_vertexStride = sizeof(MeshVertex);

	m_vertexDefs.push_back(VDF_position);
	m_vertexDefs.push_back(VDF_baseTc);
	m_vertexDefs.push_back(VDF_color);
	m_vertexDefs.push_back(VDF_lightmapTc);
	m_vertexDefs.push_back(VDF_normal);
	m_vertexDefs.push_back(VDF_tangent);
	m_vertexDefs.push_back(VDF_binormal);

	MeshPrim *src = (MeshPrim*)m_source;

	if (src->isStriped()) {
		m_elementType = GL_TRIANGLE_STRIP;
	} else {
		m_elementType = GL_TRIANGLES;
	}

	updateMesh();
}


void GLgeometry::updatePoint() {
	if (!m_source->isDirty())
		return;

	GLprimitive::update();

	m_source->clearDirty();

	PointPrim *Point = dynamic_cast<PointPrim*>(m_source);
	AX_ASSERT(Point);

	int refOffset = Point->getDrawOffset();
	int refCount = Point->getDrawCount();

	int m_allCount = Point->getNumPoints();
	AX_ASSERT(refOffset + refCount <= m_allCount);
	m_vertexCount = refCount;

//		m_indexCount = line->getNumIndexes();
//		m_currentIndexNum = 0;

	// create vertex buffer
	m_vertexBuffer.setData(Point->getPointsPointer() + refOffset, m_vertexCount*m_vertexStride, m_hint);

	// create index buffer
//		m_indexBuffer.setData(Point->getPointsPointer(), m_indexCount, m_hint, m_indexDataType);
}

void GLgeometry::updatePointSprite() {
}


void GLgeometry::updateLine() {
	if (!m_source->isDirty())
		return;

	GLprimitive::update();

	m_source->clearDirty();

	LinePrim *line = dynamic_cast<LinePrim*>(m_source);
	AX_ASSERT(line);

	m_vertexCount = line->getNumVertexes();

	int m_indexCount = line->getNumIndexes();

	// create vertex buffer
	m_vertexBuffer.setData(line->getVertexesPointer(), m_vertexCount*m_vertexStride, m_hint);

	// create index buffer
	m_indexBuffer.setData(line->getIndexPointer(), m_indexCount, m_hint, m_activeIndexes);
//		m_indexBuffer.setActiveCount(m_activeIndexes);
}


void GLgeometry::updateLineStrip() {
}


void GLgeometry::updateMesh() {
//	m_currentIndexNum = m_source->getCurrentIndexNum();

	if (!m_source->isDirty())
		return;

	GLprimitive::update();

	m_source->clearDirty();

	MeshPrim *mesh = dynamic_cast<MeshPrim*>(m_source);
	AX_ASSERT(mesh);

	m_vertexCount = mesh->getNumVertexes();

	int m_indexCount = mesh->getNumIndexes();
#if 0
	int m_currentIndexNum = mesh->getCurrentIndexNum();
#endif
	m_vertexBuffer.setData(mesh->getVertexesPointer(), m_vertexCount*m_vertexStride, m_hint);
	m_indexBuffer.setData(mesh->getIndexPointer(), m_indexCount, m_hint, m_activeIndexes);
#if 0
	m_indexBuffer.setActiveCount(m_currentIndexNum);
#else
//		m_indexBuffer.setActiveCount(m_activeIndexes);
#endif
	m_material = mesh->getMaterial();
}


//------------------------------------------------------------------------------
// class GLtext
//------------------------------------------------------------------------------

//	Material *GLtext::m_fontMtr;
GLtext::GLtext() {
}

GLtext::~GLtext() {
}


void GLtext::initialize(Primitive *src) {
	GLprimitive::initialize(src);

	TextPrim *text = dynamic_cast<TextPrim*>(m_source);
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

void GLtext::finalize() {
	GLprimitive::finalize();
}

void GLtext::update() {
	GLprimitive::update();
}

#define BLINK_DIVISOR 0.075

void GLtext::draw(Technique tech)
{
	//g_statistic->incValue(stat_numTextDrawElements);
	stat_numTextDrawElements.inc();

	if (tech != Technique::Main)
		return;

	TextQuad tq;
	float width, height;

	if (m_isSimpleText) {
		width = 1.0f; height = 1.0f;

		const Matrix3 &axis = gCamera->getViewAxis();
		tq.s_vector = -axis[1];
		tq.t_vector = -axis[2];
		tq.origin = m_position - tq.s_vector * width * 0.5f - tq.t_vector * height * 0.5f;
		tq.width = width;
		tq.height = height;
	} else {
		tq.s_vector = Vector3(1, 0, 0);
		tq.t_vector = Vector3(0, 1, 0);
		tq.origin = Vector3(m_rect.x, m_rect.y, 0);
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
		size_t len = m_font->updateTexture(pStr);
		Vector2 offset;

		// for error string, cann't upload or cann't render char

		if (m_format & TextPrim::Blink) {
			Rgba color = m_color;
			color.a = 128+127*sinf(gCamera->getTime() / BLINK_DIVISOR);
			offset = glFontRender->drawString(m_font, color, tq, startpos, pStr, len, scale, italic);
			goto next;
		}

		// common
		offset = glFontRender->drawString(m_font, m_color, tq, startpos, pStr, len, scale, italic);

		if (m_format & TextPrim::Bold) {
			glFontRender->drawString(m_font, m_color, tq, startpos+scale, pStr, len, scale, italic);
		}

next:
		total_len -= len;
		pStr += len;
		startpos += offset;
	}

	return;
}

//------------------------------------------------------------------------------
// class GLterrain
//------------------------------------------------------------------------------

GLterrain::GLterrain() {
}

GLterrain::~GLterrain() {
}

// implement GLprimitive
void GLterrain::initialize(Primitive *src) {
	GLprimitive::initialize(src);

	m_vertexStride = sizeof(ChunkVertex);

	m_vertexDefs.push_back(VDF_position);
	m_elementType = GL_TRIANGLES;

	update();
}

void GLterrain::finalize() {
	if (m_isFrameLieftime)
		return;

	GLprimitive::finalize();
}

void GLterrain::update() {
	GLprimitive::update();

	ChunkPrim *chunk = dynamic_cast<ChunkPrim*>(m_source);
	AX_ASSERT(chunk);

	if (!m_source->isDirty())
		return;

	finalize();

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

	if (m_source->isVertexBufferDirty()) {
		m_vertexCount = chunk->getNumVertexes();
		// create vertex buffer
		m_vertexBuffer.setData(chunk->getVertexesPointer(), m_vertexCount*m_vertexStride, m_hint);
	}

	if (m_source->isIndexBufferDirty()) {
		int m_indexCount = chunk->getNumIndexes();

		m_indexBuffer.setData(chunk->getIndexesPointer(), m_indexCount, m_hint, m_activeIndexes);
	}

	m_source->clearDirty();
}

void GLterrain::draw(Technique tech) {
	//g_statistic->incValue(stat_numTerrainDrawElements);
	stat_numTerrainDrawElements.inc();

	if (m_overloadMaterial) {
		GLgeometry::draw(tech);
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

	if (gIsReflecting) {
		drawlayer = false;
	}

	if (tech != Technique::Main) {
		drawlayer = false;
	}

	m_material->setTexture(MaterialTextureId::TerrainColor, m_colorTexture);
	m_material->setTexture(MaterialTextureId::TerrainNormal, m_normalTexture);

	AX_SU(g_terrainRect, m_terrainRect);
	AX_SU(g_zoneRect, m_zoneRect);
	AX_SU(g_chunkRect, m_chunkRect);

	if (!drawlayer || !combine) {
		//GLgeometry::draw(tech);
		GLrender::draw(m_material, tech, this);

		if (!drawlayer) {
			return;
		}
	}

#if 0
	if (tech != Technique::Main)
		return;

	if (gIsReflecting) {
		return;
	}

	if (!r_terrainDetail->getBool() || !m_layerVisible)
		return;
#endif

	// draw layer
	for (int i = 0; i < m_numLayers; i++) {
		//g_statistic->incValue(stat_numTerrainLayeredDrawElements);
		stat_numTerrainLayeredDrawElements.inc();

		ChunkPrim::Layer &l = m_layers[i];
		l.detailMat->setTexture(MaterialTextureId::TerrainColor, m_colorTexture);
		l.detailMat->setTexture(MaterialTextureId::TerrainNormal, m_normalTexture);
		l.detailMat->setTexture(MaterialTextureId::LayerAlpha, l.alphaTex.get());

		l.detailMat->setFeature(0, l.isVerticalProjection);
		l.detailMat->setFeature(1, combine && (i == 0)); // if is first layer, set first layer flag

		AX_SU(g_layerScale, l.scale);
		GLrender::draw(l.detailMat.get(), Technique::Layer, this);
	}
}


//------------------------------------------------------------------------------
// class GLgroup
//------------------------------------------------------------------------------

GLgroup::GLgroup() {
}

GLgroup::~GLgroup() {
}

void GLgroup::initialize(Primitive *src) {
	GLprimitive::initialize(src);

	GroupPrim *rpgroup = dynamic_cast<GroupPrim*>(src);
	AX_ASSERT(rpgroup);

	uint_t count = rpgroup->getPrimitiveCount();

	for (uint_t i = 0; i < count; i++) {
		Primitive *s_prim = rpgroup->getPrimitive(i);
		int id = glPrimitiveManager->cachePrimitive(s_prim);
		m_primitives.push_back(id);
	}
}

void GLgroup::finalize() {
	m_primitives.clear();
	GLprimitive::finalize();
}

void GLgroup::update() {
	GLprimitive::update();
}

void GLgroup::draw(Technique tech) {
	for (size_t i = 0; i < m_primitives.size(); i++) {
		GLprimitive *prim = glPrimitiveManager->getPrimitive(m_primitives[i]);
		prim->draw(tech);
	}
}
#if 0
void GLgroup::drawElements() {
	for (size_t i = 0; i < m_primitives.size(); i++) {
		GLprimitive *prim = glPrimitiveManager->getPrimitive(m_primitives[i]);
		prim->drawElements();
	}
}
#endif
//------------------------------------------------------------------------------
// class GLref
//------------------------------------------------------------------------------

GLref::GLref() {}
GLref::~GLref() {}

void GLref::initialize(Primitive *src) {
	GLprimitive::initialize(src);

	RefPrim *ref = dynamic_cast<RefPrim*>(src);
	AX_ASSERT(ref);

	m_cachedId = glPrimitiveManager->cachePrimitive(ref->getRefered());

	int indexcount = ref->getNumIndexes();

	if (!indexcount) {
		return;
	}

	m_indexBuffer.setData(ref->getIndexesPointer(), indexcount, ref->getHint(), m_activeIndexes);
//		m_indexBuffer.setActiveCount(m_activeIndexes);
}

void GLref::finalize() {
	GLprimitive::finalize();
}

void GLref::update() {
	GLprimitive::update();
}

void GLref::draw(Technique tech) {
	GLprimitive *prim = glPrimitiveManager->getPrimitive(m_cachedId);
	if (m_material) {
		prim->setOverloadMaterial(m_material);
	}

	if (m_indexBuffer.haveData()) {
		prim->m_overloadedIndexbuffer = &m_indexBuffer;
	}

	if (m_instanceParams) {
		prim->m_instanceParams = m_instanceParams;
	}

	prim->draw(tech);

	prim->m_instanceParams = 0;

	if (m_material) {
		prim->unsetOverloadMaterial();
	}

	if (m_indexBuffer.haveData()) {
		prim->m_overloadedIndexbuffer = nullptr;
	}
}

//--------------------------------------------------------------------------
// class GLinstance
//--------------------------------------------------------------------------

GLinstance::GLinstance() {}
GLinstance::~GLinstance() {}

void GLinstance::initialize(Primitive *src) {
	GLprimitive::initialize(src);

	update();
}

void GLinstance::finalize() {}

void GLinstance::update() {
	GLprimitive::update();
	InstancePrim *gi = dynamic_cast<InstancePrim*>(m_source);
	AX_ASSERT(gi);

	m_instanced = glPrimitiveManager->cachePrimitive(gi->getInstanced());

	m_params = gi->getAllInstances();

	m_source->clearDirty();
}

void GLinstance::draw(Technique tech) {
	GLprimitive *prim = glPrimitiveManager->getPrimitive(m_instanced);

	prim->m_instanceParams = m_params;
	prim->draw(tech);
	prim->m_instanceParams = 0;
}


//--------------------------------------------------------------------------
// class GLprimitivemanager
//--------------------------------------------------------------------------

GLprimitivemanager::GLprimitivemanager()
	: m_numStaticPrims(0)
	, m_numFramePrims(0)
	, m_freePrimLink(NULL)
	, m_numWaitForDelete(0)
	, m_frameVertexBuffers(NULL)
	, m_frameIndexBuffers(NULL)
{
	m_framenum = 0;

	memset(m_staticPrims, 0, sizeof(m_staticPrims));
	memset(m_framePrims, 0, sizeof(m_staticPrims));
	memset(m_waitForDelete, 0, sizeof(m_staticPrims));

	// setup free list
	m_freePrimLink = &m_staticPrims[0];
	for (int i = 0; i < MAX_PRIMITIVES-1; i++) {
		m_staticPrims[i] = (GLgeometry*)&m_staticPrims[i+1];
	}
	m_staticPrims[MAX_PRIMITIVES-1] = NULL;
}

GLprimitivemanager::~GLprimitivemanager() {
}


void GLprimitivemanager::initialize() {
}

void GLprimitivemanager::finalize() {
}

void GLprimitivemanager::beginFrame() {
}

int GLprimitivemanager::cachePrimitive(Primitive *prim) {
	//g_statistic->incValue(stat_numElements);
	stat_numElements.inc();

	int f = prim->getCachedFrame();
	int h = prim->getCachedId();

	if (f == m_framenum)
		return h;

	int new_id;

	if (prim->getHint() != Primitive::HintFrame) {
		if (h == 0) {
			// init
			GLprimitive *glprim = createPrim(prim);

			findStaticFreeSlot(new_id);
			linkId(new_id, glprim);

			prim->setCachedId(new_id);
		} else {
			// update
			GLprimitive *glprim = getPrimitive(h);
			glprim->update();

			new_id = h;
		}
	} else {
		// FIXME: if have a static gl primitive for this prim, we uncache it first
		if (isStatic(h))
			uncachePrimitive(prim);

		GLprimitive *glprim = createPrim(prim);

		new_id = (int)((m_numFramePrims+1) | FRAME_FLAG);
		m_numFramePrims++;
		linkId(new_id, glprim);

		prim->setCachedId(new_id);

		//g_statistic->setValue(stat_framePrims, m_numFramePrims);
		stat_framePrims.setInt(m_numFramePrims);
	}

	prim->setCachedFrame(m_framenum);
	//g_statistic->incValue(stat_changedPrims);
	stat_changedPrims.inc();

	if (prim->getHint() == Primitive::HintFrame)
		delete prim;

	return new_id;
}

void GLprimitivemanager::uncachePrimitive(Primitive *prim) {
	SCOPE_LOCK;		// this function will called from main thread

	int id = prim->getCachedId();

	if (!id)
		return;

	if (isFrameHandle(id))
		return;

	if (m_numWaitForDelete == MAX_PRIMITIVES)
		Errorf("GLprimitivemanager::uncachePrimitive: overflowed");

	m_waitForDelete[m_numWaitForDelete++] = uint_t(id)-1;
	//g_statistic->decValue(stat_staticPrims);
	stat_staticPrims.dec();
}

void GLprimitivemanager::endFrame() {
	SCOPE_LOCK;		// Lock WaitForDelete

	// clear frame primitive
	for (int i = 0; i < m_numFramePrims; i++) {
		delete m_framePrims[i];
	}
	m_numFramePrims = 0;

	// clear wait for delete
	for (int i = 0; i < m_numWaitForDelete; i++) {
		delete m_staticPrims[m_waitForDelete[i]];

		// add to free link
		m_staticPrims[m_waitForDelete[i]] = (GLgeometry*)m_freePrimLink;
		m_freePrimLink = &m_staticPrims[m_waitForDelete[i]];
	}
	m_numWaitForDelete = 0;

	m_framenum++;
}

GLprimitive *GLprimitivemanager::getPrimitive(int handle) {
	if (handle == -1) {
		return nullptr;
	}

	size_t h = (size_t)handle;
	if (h & FRAME_FLAG)
		return m_framePrims[(h & INDEX_MASK)-1];
	else
		return m_staticPrims[(h & INDEX_MASK)-1];
}

void GLprimitivemanager::findStaticFreeSlot(int &handle) {
	if (m_freePrimLink == NULL)
		Errorf("GLprimitivemanager::findStaticFreeSlot: no free primitive slot");

	// pop a free slot
	GLprimitive** ptr = (GLprimitive**)m_freePrimLink;
	intptr_t *link = (intptr_t*)m_freePrimLink;
	m_freePrimLink = (void*)(*link);

	handle = int((ptr - m_staticPrims) + 1);

	//g_statistic->incValue(stat_staticPrims);
	stat_staticPrims.inc();
}

void GLprimitivemanager::linkId(int id, GLprimitive *glprim) {
	if (id == 0)
		Errorf("GLprimitivemanager::linkId: error id");

	int index =(id & INDEX_MASK) - 1;
	AX_ASSERT(index < MAX_PRIMITIVES);

	if (isStatic(id)) {
		m_staticPrims[index] = glprim;
	} else {
		m_framePrims[index] = glprim;
	}
}

GLprimitive *GLprimitivemanager::createPrim(Primitive *prim) {
	GLprimitive *glprim;

	if (prim->getType() == Primitive::TextType)
		glprim = new GLtext();
	else if (prim->getType() == Primitive::GroupType)
		glprim = new GLgroup();
	else if (prim->getType() == Primitive::ChunkType)
		glprim = new GLterrain();
	else if (prim->getType() == Primitive::ReferenceType)
		glprim = new GLref();
	else if (prim->getType() == Primitive::InstancingType)
		glprim = new GLinstance();
	else
		glprim = new GLgeometry();

	glprim->initialize(prim);

	return glprim;
}


AX_END_NAMESPACE
