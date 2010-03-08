/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

AX_BEGIN_NAMESPACE

D3DVERTEXELEMENT9 s_veGeneric[] = {
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12,	D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,	0},
	{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	{0, 32, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	{0, 44, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	6},
	{0, 56, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 7},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 s_veDebug[] = {
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,	0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 s_veBlend[] = {
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12,	D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,	0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 s_veChunk[] = {
	{0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	D3DDECL_END()
};

D3DVERTEXELEMENT9 s_veInstance[] = {
	{1, 0,	D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	2},
	{1, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
	{1, 32, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4},
	{1, 48, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5},
	D3DDECL_END()
};

int veoGeneric[] = { 0, 12, 20, 24, 32, 44, 56, -1 };
int veoDebug[] = { 0, 12, -1 };
int veoBlend[] = { 0, 12, 20, -1 };
int veoChunk[] = { 0, -1 };

struct VeInfo {
	D3DVERTEXELEMENT9 *ve;
	int *offset;
} s_veInfos[] = {
	{ s_veGeneric, veoGeneric },
	{ s_veBlend, veoBlend },
	{ s_veDebug, veoDebug },
	{ s_veChunk, veoChunk },
};

static UINT s_strides[] = {
	sizeof(MeshVertex), sizeof(BlendVertex), sizeof(DebugVertex), sizeof(ChunkVertex)
};

static DWORD s_lockNewFrame = D3DLOCK_DISCARD/* | D3DLOCK_NOOVERWRITE*/;
static DWORD s_lockAppend = D3DLOCK_NOOVERWRITE;

IDirect3DVertexBuffer9 *CreateVertexBufferPage(int pagesize) {
	HRESULT hr;
	IDirect3DVertexBuffer9 *vb;

	V(d3d9Device->CreateVertexBuffer(pagesize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vb, 0));
	return vb;
}

IDirect3DIndexBuffer9 *CreateIndexBufferPage(int pagesize) {
	HRESULT hr;
	IDirect3DIndexBuffer9 *ib;

	V(d3d9Device->CreateIndexBuffer(pagesize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ib, 0));
	return ib;
}


//--------------------------------------------------------------------------
// class D3D9vertexobject
//--------------------------------------------------------------------------

D3D9vertexobject::D3D9vertexobject() {
	m_object = nullptr;
	m_dataSize = 0;
	m_declaration = nullptr;
	m_offset = 0;
	m_count = 0;
	m_inStack = false;
}

D3D9vertexobject::~D3D9vertexobject() {
	resetData();
}

void D3D9vertexobject::setData(const void *p, int count, Primitive::Hint primhint, VertexType vt) {
	HRESULT hr;

	m_count = count;
	m_vt = vt;
	int size = m_count * s_strides[vt];

	if (primhint != Primitive::HintStatic) {
		// alloc from pages
		DynVb dv = d3d9VertexBufferManager->allocVb(size);
		m_object = dv.vb;
		m_dataSize = size;
		m_offset = dv.offset;
		memcpy(dv.writePtr, p, size);

		createDeclaration();

		m_inStack = true;

		return;
	}

	resetData();

	V(d3d9Device->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_object, NULL));
#if 0
	g_statistic->incValue(stat_numVertexBuffers);
	g_statistic->addValue(stat_vertexBufferMemory, size);
#else
	stat_numVertexBuffers.inc();
	stat_vertexBufferMemory.add(size);
#endif

	void *dst;
	V(m_object->Lock(0, size, &dst, 0));
	memcpy(dst, p, size);
	V(m_object->Unlock());

	m_dataSize = size;

	if (!m_declaration) {
		createDeclaration();
	}
}

void D3D9vertexobject::resetData() {
	if (!m_object || m_inStack) {
		return;
	}

#if 0
	g_statistic->decValue(stat_numVertexBuffers);
	g_statistic->subValue(stat_vertexBufferMemory, m_dataSize);
#else
	stat_numVertexBuffers.dec();
	stat_vertexBufferMemory.sub(m_dataSize);
#endif
	SAFE_RELEASE(m_object);
	m_object = 0;
	m_dataSize = 0;
}

void D3D9vertexobject::bind() {
	HRESULT hr;

	V(d3d9Device->SetStreamSource(0, m_object, m_offset, s_strides[m_vt]));
	d3d9StateManager->setVertexDeclaration(m_declaration->getObject());
}


void D3D9vertexobject::bindInstanced(D3D9instancedbuffer *instancedBuffer)
{
	HRESULT hr;

	V(d3d9Device->SetStreamSource(0, m_object, m_offset, s_strides[m_vt]));
	V(d3d9Device->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | instancedBuffer->m_count));

	V(d3d9Device->SetStreamSource(1, instancedBuffer->m_object, instancedBuffer->m_offset, 64));
	V(d3d9Device->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul));
	d3d9StateManager->setVertexDeclaration(m_declaration->getObjectInstanced());
}

void D3D9vertexobject::unbindInstanced()
{
	HRESULT hr;
	V(d3d9Device->SetStreamSourceFreq(0, 1));
	V(d3d9Device->SetStreamSourceFreq(1, 1));
}

void D3D9vertexobject::createDeclaration()
{
	m_declaration = d3d9VertexBufferManager->getVertDecl(m_vt);
}

//--------------------------------------------------------------------------
// class D3D9instancedbuffer
//--------------------------------------------------------------------------

D3D9instancedbuffer::D3D9instancedbuffer()
{
	m_object = 0;
	m_offset = 0;
	m_count = 0;
}

D3D9instancedbuffer::~D3D9instancedbuffer()
{

}

void D3D9instancedbuffer::setData( const InstancePrim::ParamSeq &params )
{
	m_count = (int)params.size();

	DynVb dynvb = d3d9VertexBufferManager->allocInstance(m_count*64);
	Vector4 *pdata = (Vector4*)dynvb.writePtr;
	for (int i = 0; i < m_count; i++) {
		const InstancePrim::Param &param = params[i];
		pdata[0] = param.worldMatrix.getRow(0);
		pdata[1] = param.worldMatrix.getRow(1);
		pdata[2] = param.worldMatrix.getRow(2);
		pdata[3] = param.userDefined;
		pdata += 4;
	}

	m_object = dynvb.vb;
	m_offset = dynvb.offset;
}

//--------------------------------------------------------------------------
// class D3D9indexobject
//--------------------------------------------------------------------------

D3D9indexobject::D3D9indexobject() {
	m_object = 0;
	m_dataSize = 0;
	m_activeCount = 0;
	m_inStack = false;
}

D3D9indexobject::~D3D9indexobject() {
	resetData();
}

void D3D9indexobject::setData(const ushort_t *p, int count, Primitive::Hint hint, int activeCount) {
	if (hint != Primitive::HintStatic) {
		m_count = count;
		m_activeCount = activeCount;
		if (!m_activeCount) {
			m_activeCount = count;
		}
		m_dataSize = m_activeCount * sizeof(ushort_t);
		DynIb di = d3d9VertexBufferManager->allocIb(m_dataSize);
		m_object = di.ib;
		m_offset = di.offset / sizeof(ushort_t);

		ushort_t *dst = (ushort_t*)di.writePtr;
		memcpy(dst, p, m_dataSize);
		m_inStack = true;

		return;
	}

	resetData();

	m_count = count;
	m_activeCount = activeCount;
	if (!m_activeCount) {
		m_activeCount = count;
	}
	m_dataSize = count * sizeof(ushort_t);

	m_offset = 0;

	HRESULT hr;

	V(d3d9Device->CreateIndexBuffer(m_dataSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_object, NULL));
#if 0
	g_statistic->incValue(stat_numIndexBuffers);
	g_statistic->addValue(stat_indexBufferMemory, m_dataSize);
#else
	stat_numIndexBuffers.inc();
	stat_indexBufferMemory.add(m_dataSize);
#endif
	void *dst;
	V(m_object->Lock(0, m_dataSize, &dst, 0));

	memcpy(dst, p, m_dataSize);
	V(m_object->Unlock());
}

void D3D9indexobject::resetData() {
	if (m_object == 0 || m_inStack)
		return;

	m_object->Release();

#if 0
	g_statistic->decValue(stat_numIndexBuffers);
	g_statistic->subValue(stat_indexBufferMemory, m_dataSize);
#else
	stat_numIndexBuffers.dec();
	stat_indexBufferMemory.sub(m_dataSize);
#endif

	m_object = 0;
	m_dataSize = 0;
}

void D3D9indexobject::bind() {
	d3d9Device->SetIndices(m_object);
}

bool D3D9indexobject::haveData() const {
	return m_dataSize > 0;
}

int D3D9indexobject::getActiveCount() const {
	return m_activeCount;
}

void D3D9indexobject::setActiveCount(int val) {
	m_activeCount = val;
}

void D3D9indexobject::drawElements(D3DPRIMITIVETYPE mode, int numverts) {
	HRESULT hr;
	V(d3d9Device->SetIndices(m_object));
	V(d3d9Device->DrawIndexedPrimitive(mode, 0, 0, numverts, m_offset, calcNumElements(mode, m_activeCount)));
}

int D3D9indexobject::calcNumElements(D3DPRIMITIVETYPE mode, int numindexes)
{
	switch (mode) {
	case D3DPT_POINTLIST: return numindexes;
	case D3DPT_LINELIST: return numindexes / 2;
	case D3DPT_LINESTRIP: return numindexes - 1;
	case D3DPT_TRIANGLELIST: return numindexes / 3;
	case D3DPT_TRIANGLESTRIP: return numindexes - 2;
	case D3DPT_TRIANGLEFAN: return numindexes - 2;
	}

	Errorf("not support element mode %d", mode);
	return 0;
}

//--------------------------------------------------------------------------
// class D3D9vertdecl
//--------------------------------------------------------------------------

D3D9vertdecl::D3D9vertdecl(D3D9vertexbuffermanager *manager, D3D9vertexobject::VertexType vt, int offset)
{
	m_manager = manager;
	m_vt = vt;
	m_offset = offset;

	VeInfo &veinfo = s_veInfos[vt];

	// apply offset
	int c = 0;
	Sequence<D3DVERTEXELEMENT9> veiseq;

	for (D3DVERTEXELEMENT9 *ve = &veinfo.ve[0]; ve->Stream != 0xff; ve++, c++) {
		ve->Offset = offset + veinfo.offset[c];
		veiseq.push_back(*ve);
	}

	// add instanced vert elem to veInstanced
	for (size_t i = 0; i < ArraySize(s_veInstance); i++) {
		veiseq.push_back(s_veInstance[i]);
	}

	HRESULT hr;
	V(d3d9Device->CreateVertexDeclaration(veinfo.ve, &m_d3dObject));
	V(d3d9Device->CreateVertexDeclaration(&veiseq[0], &m_d3dObjectInstanced));
}

D3D9vertdecl::~D3D9vertdecl()
{
	SAFE_RELEASE(d3d9Device);
	m_manager->removeVertDecl(this);
}

//--------------------------------------------------------------------------
// class D3D9vertexbuffermanager
//--------------------------------------------------------------------------

D3D9vertexbuffermanager::D3D9vertexbuffermanager()
{
#if 0
	m_vbIndex = 0;
	m_ibIndex = 0;
	m_vbOffset = 0;
	m_ibOffset = 0;
	m_vbPointer = 0;
	m_ibPointer = 0;
	m_isWritting = false;
#endif

	init();
}

D3D9vertexbuffermanager::~D3D9vertexbuffermanager()
{
	shutdown();
}

void D3D9vertexbuffermanager::onDeviceLost()
{

}

void D3D9vertexbuffermanager::onReset()
{

}

void D3D9vertexbuffermanager::nextFrame()
{

}

void D3D9vertexbuffermanager::beginAlloc()
{
//		HRESULT hr;
//		V(d3d9Device->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul));
	m_vertexBufferChain.beginFrame();
//		m_instanceBufferChain.beginFrame();
	m_indexBufferChain.beginFrame();
}

DynVb D3D9vertexbuffermanager::allocVb(int datasize)
{
	VertexBufferChain::Result ret = m_vertexBufferChain.frameAlloc(datasize);
	DynVb result;
	result.vb = ret.buffer;
	result.offset = ret.offset;
	result.writePtr = ret.writePtr;
	return result;
}

DynVb D3D9vertexbuffermanager::allocInstance( int datasize )
{
	VertexBufferChain::Result ret = m_vertexBufferChain.frameAlloc(datasize);
	DynVb result;
	result.vb = ret.buffer;
	result.offset = ret.offset;
	result.writePtr = ret.writePtr;
	return result;
}

DynIb D3D9vertexbuffermanager::allocIb(int datasize)
{
	IndexBufferChain::Result ret = m_indexBufferChain.frameAlloc(datasize);
	DynIb result;
	result.ib = ret.buffer;
	result.offset = ret.offset;
	result.writePtr = ret.writePtr;
	return result;
}

void D3D9vertexbuffermanager::endAlloc()
{
	m_vertexBufferChain.endFrame();
//		m_instanceBufferChain.endFrame();
	m_indexBufferChain.endFrame();
}

void D3D9vertexbuffermanager::init()
{
	for (int i = 0; i < D3D9vertexobject::VERTEXTYPE_NUMBER; i++) {
		vertDecls[i] = allocVertDecl((D3D9vertexobject::VertexType)i, 0);
	}
}

void D3D9vertexbuffermanager::shutdown()
{
	// TODO: free all buffers
}

D3D9vertdecl *D3D9vertexbuffermanager::allocVertDecl(D3D9vertexobject::VertexType vt, int offset)
{
	D3D9vertdecl*& result = m_vertDeclPool[vt][offset];

	if (result) {
		result->addref();
		return result;
	}

	result = new D3D9vertdecl(this,vt,offset);
	return result;
}

void D3D9vertexbuffermanager::removeVertDecl(D3D9vertdecl *vd)
{
	VertDeclPool &decls = m_vertDeclPool[vd->m_vt];
	decls.erase(vd->m_offset);
}

DynVb D3D9vertexbuffermanager::appendVb( int datasize )
{
	VertexBufferChain::Result ret = m_vertexBufferChain.appendAlloc(datasize);
	DynVb result;
	result.vb = ret.buffer;
	result.offset = ret.offset;
	result.writePtr = ret.writePtr;
	return result;
}

DynVb D3D9vertexbuffermanager::appendInstance( int datasize )
{
	VertexBufferChain::Result ret = m_vertexBufferChain.appendAlloc(datasize);
	DynVb result;
	result.vb = ret.buffer;
	result.offset = ret.offset;
	result.writePtr = ret.writePtr;
	return result;
}

DynIb D3D9vertexbuffermanager::appendIb( int datasize )
{
	IndexBufferChain::Result ret = m_indexBufferChain.appendAlloc(datasize);
	DynIb result;
	result.ib = ret.buffer;
	result.offset = ret.offset;
	result.writePtr = ret.writePtr;
	return result;
}

void D3D9vertexbuffermanager::endAppendVb()
{
	m_vertexBufferChain.endAppend();
}

void D3D9vertexbuffermanager::endAppendInstance()
{
	m_vertexBufferChain.endAppend();
}

void D3D9vertexbuffermanager::endAppendIb()
{
	m_indexBufferChain.endAppend();
}

void D3D9vertexbuffermanager::reportStatices() const
{
#if 0
	g_statistic->setValue(stat_dynamicVBsize, m_vertexBufferChain.getSize());
	g_statistic->setValue(stat_usedVBsize, m_vertexBufferChain.getUsed());
	g_statistic->setValue(stat_instanceBufSize, m_instanceBufferChain.getSize());
	g_statistic->setValue(stat_usedInstanceBufSize, m_instanceBufferChain.getUsed());
	g_statistic->setValue(stat_dynamicIBsize, m_indexBufferChain.getSize());
	g_statistic->setValue(stat_usedIBsize, m_indexBufferChain.getUsed());
#else
	stat_dynamicVBsize.setInt(m_vertexBufferChain.getSize());
	stat_usedVBsize.setInt(m_vertexBufferChain.getUsed());
	stat_instanceBufSize.setInt(m_instanceBufferChain.getSize());
	stat_usedInstanceBufSize.setInt(m_instanceBufferChain.getUsed());
	stat_dynamicIBsize.setInt(m_indexBufferChain.getSize());
	stat_usedIBsize.setInt(m_indexBufferChain.getUsed());
#endif
}


AX_END_NAMESPACE

