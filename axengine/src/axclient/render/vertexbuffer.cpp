#include "../private.h"

AX_BEGIN_NAMESPACE

VertexObject::VertexObject()
{
	m_localHandle = 0;
	m_h = &m_localHandle;
	m_hint = Primitive::HintStatic;
	m_vt = VertexType::kMesh;
	m_offset = 0;		// byte offset
	m_dataSize = 0;
	m_count = 0;
	m_chained = false;
}

VertexObject::~VertexObject()
{
	clear();
}

void VertexObject::init(const void *p, int count, Primitive::Hint hint, VertexType vt)
{
	clear();

	m_count = count;
	m_hint = hint;
	m_vt = vt;
	m_dataSize = count * vt.stride();

	if (hint != Primitive::HintStatic) {
		DynamicBuf db = g_bufferManager->allocVb(m_dataSize);
		m_h = db.phandle;
		m_offset = db.offset;

		memcpy(db.writePtr, p, m_dataSize);

		m_chained = true;

		return;
	} else {
		g_apiWrap->createVertexBuffer(m_h, m_dataSize, m_hint, p);

		stat_numVertexBuffers.inc();
		stat_vertexBufferMemory.add(m_dataSize);
	}
}

void VertexObject::clear()
{
	if (m_hint == Primitive::HintStatic && m_dataSize != 0) {
		g_apiWrap->deleteVertexBuffer(m_h);
		stat_numVertexBuffers.dec();
		stat_vertexBufferMemory.sub(m_dataSize);
	}

	m_h = &m_localHandle;
	m_hint = Primitive::HintStatic;
	m_vt = VertexType::kMesh;
	m_offset = 0;		// byte offset
	m_dataSize = 0;
	m_count = 0;
	m_chained = false;
}


InstanceObject::InstanceObject()
{
	m_h = 0;
	m_count = 0;
	m_offset = 0;
}

InstanceObject::~InstanceObject()
{

}

void InstanceObject::init(int numInstances, const InstancePrim::Param* params)
{
	m_count = numInstances;

	DynamicBuf db = g_bufferManager->allocInstance(m_count*64);
	Vector4 *pdata = (Vector4*)db.writePtr;
	for (int i = 0; i < m_count; i++) {
		const InstancePrim::Param &param = params[i];
		pdata[0] = param.worldMatrix.getRow(0);
		pdata[1] = param.worldMatrix.getRow(1);
		pdata[2] = param.worldMatrix.getRow(2);
		pdata[3] = param.userDefined;
		pdata += 4;
	}

	m_h = db.phandle;
	m_offset = db.offset;
}


IndexObject::IndexObject()
{
	m_h = &m_localHandle;
	m_hint = Primitive::HintStatic;
	m_elementType = ElementType_PointList;
	m_dataSize = 0;
	m_count = 0;
	m_activeCount = 0;
	m_offset = 0;		// index offset, not byte offset
	m_chained = false;
	m_localHandle = 0;
}

IndexObject::~IndexObject()
{
	clear();
}

void IndexObject::init(const ushort_t *p, int count, Primitive::Hint hint, ElementType et, int activeCount /*= 0*/)
{
	clear();

	m_hint = hint;
	m_count = count;
	m_elementType = et;
	m_activeCount = activeCount;
	if (!m_activeCount)
		m_activeCount = count;

	m_dataSize = m_activeCount * sizeof(ushort_t);

	if (m_hint != Primitive::HintStatic) {

		DynamicBuf db = g_bufferManager->allocIb(m_dataSize);
		m_h = db.phandle;
		m_offset = db.offset / sizeof(ushort_t);

		ushort_t *dst = (ushort_t*)db.writePtr;
		memcpy(dst, p, m_dataSize);
		m_chained = true;
	} else {
		m_offset = 0;

		g_apiWrap->createIndexBuffer(m_h, m_dataSize, m_hint, p);

		stat_numIndexBuffers.inc();
		stat_indexBufferMemory.add(m_dataSize);
	}
}

void IndexObject::clear()
{
	if (m_hint == Primitive::HintStatic && m_dataSize != 0) {
		g_apiWrap->deleteIndexBuffer(m_h);
		stat_numIndexBuffers.dec();
		stat_indexBufferMemory.sub(m_dataSize);
	}

	m_h = &m_localHandle;
	m_hint = Primitive::HintStatic;
	m_elementType = ElementType_PointList;
	m_dataSize = 0;
	m_count = 0;
	m_activeCount = 0;
	m_offset = 0;		// index offset, not byte offset
	m_chained = false;
}


BufferManager::BufferManager()
{

}

BufferManager::~BufferManager()
{

}

void BufferManager::beginAlloc()
{
	m_vertexBufferChain.beginFrame();
	m_instanceBufferChain.beginFrame();
	m_indexBufferChain.beginFrame();
}

DynamicBuf BufferManager::allocVb(int datasize)
{
	return m_vertexBufferChain.frameAlloc(datasize);
}

DynamicBuf BufferManager::allocInstance(int datasize)
{
	return m_instanceBufferChain.frameAlloc(datasize);
}

DynamicBuf BufferManager::allocIb(int datasize)
{
	return m_indexBufferChain.frameAlloc(datasize);
}

void BufferManager::endAlloc()
{
	m_vertexBufferChain.endFrame();
	m_instanceBufferChain.endFrame();
	m_indexBufferChain.endFrame();
}

AX_END_NAMESPACE
