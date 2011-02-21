#include "dx11_private.h"

extern "C" {;
extern int WINAPI D3DPERF_BeginEvent(DWORD col, LPCWSTR wszName);
extern int WINAPI D3DPERF_EndEvent( void );
}

AX_DX11_BEGIN_NAMESPACE

namespace {
	enum {
		VERTEX_UP_SIZE = 128 * 1024,
		INDEX_UP_SIZE = 64 * 1024,
	};
}

FastParams g_curParams1;
FastParams g_curParams2;
phandle_t g_curGlobalTextures[GlobalTextureId::MaxType];
SamplerDesc g_curGlobalTextureSamplerDescs[GlobalTextureId::MaxType];
FastTextureParams g_curMaterialTextures;
bool g_curInstanced = false;
VertexType g_curVertexType;

static DX11_Shader *s_curShader;
static Technique s_curTechnique;

static phandle_t s_tempVertexBuffer;
static phandle_t s_tempIndexBuffer;
static int s_curVertexBufferPos;
static int s_curIndexBufferPos;

static int s_curNumVertices;
static int s_curNumIndices;
static int s_curNumInstances;
static int s_curPrimitiveCount;
static Size s_curRenderTargetSize;

void dx11CreateTextureFromFileInMemory(phandle_t h, IoRequest *asioRequest)
{
	ID3D11Resource *texture;
	V(D3DX11CreateTextureFromMemory(g_device, asioRequest->fileData(), asioRequest->fileSize(), 0, 0, &texture, 0));
	DX11_Resource *resource = new DX11_Resource(DX11_Resource::kImmutableTexture, texture);
	*h = resource;
	delete asioRequest;
}

void dx11CreateTexture(phandle_t h, TexType type, TexFormat format, int width, int height, int depth, int flags)
{
	DXGI_FORMAT d3dformat = DX11_Driver::trTexFormat(format);
	AX_ASSURE(d3dformat != DXGI_FORMAT_UNKNOWN);

	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	UINT bindflags = D3D11_BIND_SHADER_RESOURCE;
	UINT miscflags = 0;
	UINT cpuAccessFlags = 0;
	int miplevels = 1;

	if (flags & Texture::RenderTarget) {
		if (format.isDepth()) {
			bindflags |= D3D11_BIND_DEPTH_STENCIL;
		} else {
			bindflags |= D3D11_BIND_RENDER_TARGET;
		}
	} else {
		usage = D3D11_USAGE_DYNAMIC;
	}

	if (flags & Texture::AutoGenMipmap) {
		bool m_hardwareGenMipmap = g_renderDriverInfo.autogenMipmapSupports[format];

		if (m_hardwareGenMipmap) {
			miscflags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		miplevels = 0;
	}

	if (type == TexType::CUBE)
		miscflags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Resource *d3dresource = 0;
	DX11_Resource *apiResource = 0;
	if (type == TexType::_2D || type == TexType::CUBE) {
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = miplevels;
		desc.ArraySize = 1;
		desc.Format = d3dformat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = usage;
		desc.BindFlags = bindflags;
		desc.CPUAccessFlags = cpuAccessFlags;
		desc.MiscFlags = miscflags;

		ID3D11Texture2D *texture2D;
		V(g_device->CreateTexture2D(&desc, 0, &texture2D));

		apiResource = new DX11_Resource(DX11_Resource::kDynamicTexture, texture2D);
	} else if (type == TexType::_3D) {
		D3D11_TEXTURE3D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.Depth = depth;
		desc.MipLevels = miplevels;
		desc.Format = d3dformat;
		desc.Usage = usage;
		desc.BindFlags = bindflags;
		desc.CPUAccessFlags = cpuAccessFlags;
		desc.MiscFlags = miscflags;

		ID3D11Texture3D *texture3D;
		V(g_device->CreateTexture3D(&desc, 0, &texture3D));
		apiResource = new DX11_Resource(DX11_Resource::kDynamicTexture, texture3D);
	}

	// TODO
}

void dx11UploadTexture(phandle_t h, const void *pixels, TexFormat format)
{

}

void dx11UploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format)
{
	DX11_Resource *apiResource = h->castTo<DX11_Resource *>();
	AX_ASSURE(apiResource->m_type == DX11_Resource::kDynamicTexture);
	AX_ASSURE(format == apiResource->m_dynamicTextureData->texFormat);

	D3D11_BOX box;
	box.left = rect.x;
	box.top = rect.y;
	box.right = rect.xMax();
	box.bottom = rect.yMax();
	box.back = 0;
	box.front = 1;
	UINT rowPitch = format.calculateDataSize(rect.width, 1);

	g_context->UpdateSubresource(apiResource->m_dx11Resource, 0, &box, pixels, rowPitch, 0);
}

void dx11GenerateMipmap(phandle_t h)
{
	// TODO
}

void dx11DeleteTexture(phandle_t h)
{
	DX11_Resource *apiResource = h->castTo<DX11_Resource *>();
	delete apiResource;
	delete h;
}

void dx11CreateVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint, const void *p)
{
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	if (hint == Primitive::HintStatic) {
		usage = D3D11_USAGE_IMMUTABLE;
		AX_ASSURE(p);
	}

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = usage;
	bufferDesc.ByteWidth = datasize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = p;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	ID3D11Buffer *buffer;
	g_device->CreateBuffer(&bufferDesc, &InitData, &buffer);

	DX11_Resource *apiRes = new DX11_Resource(DX11_Resource::kVertexBuffer, buffer);
	apiRes->m_isDynamic = (hint != Primitive::HintStatic);
	*h = apiRes;
}

void dx11UploadVertexBuffer(phandle_t h, int offset, int datasize, const void *p)
{
	DX11_Resource *apiRes = h->castTo<DX11_Resource *>();
	AX_ASSURE(apiRes->m_type == DX11_Resource::kVertexBuffer);

	D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
	if (offset != 0)
		mapType = D3D11_MAP_WRITE_NO_OVERWRITE;

	D3D11_MAPPED_SUBRESOURCE mapped;
	g_context->Map(apiRes->m_dx11Resource, 0, mapType, 0, &mapped);
	memcpy((byte_t *)mapped.pData + offset, p, datasize);
	g_context->Unmap(apiRes->m_dx11Resource, 0);
}

void dx11DeleteVertexBuffer(phandle_t h)
{
	DX11_Resource *apiResource = h->castTo<DX11_Resource *>();
	delete apiResource;
	delete h;
}

void dx11CreateIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint, const void *p)
{
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	if (hint == Primitive::HintStatic) {
		usage = D3D11_USAGE_IMMUTABLE;
		AX_ASSURE(p);
	}

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = usage;
	bufferDesc.ByteWidth = datasize;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = p;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	ID3D11Buffer *buffer;
	g_device->CreateBuffer(&bufferDesc, &InitData, &buffer);

	DX11_Resource *apiRes = new DX11_Resource(DX11_Resource::kIndexBuffer, buffer);
	apiRes->m_isDynamic = (hint != Primitive::HintStatic);
	*h = apiRes;
}

void dx11UploadIndexBuffer(phandle_t h, int offset, int datasize, const void *p)
{
	DX11_Resource *apiRes = h->castTo<DX11_Resource *>();
	AX_ASSURE(apiRes->m_type == DX11_Resource::kIndexBuffer);

	D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD;
	if (offset != 0)
		mapType = D3D11_MAP_WRITE_NO_OVERWRITE;

	D3D11_MAPPED_SUBRESOURCE mapped;
	g_context->Map(apiRes->m_dx11Resource, 0, mapType, 0, &mapped);
	memcpy((byte *)mapped.pData + offset, p, datasize);
	g_context->Unmap(apiRes->m_dx11Resource, 0);
}

void dx11DeleteIndexBuffer(phandle_t h)
{
	DX11_Resource *apiResource = h->castTo<DX11_Resource *>();
	delete apiResource;
	delete h;
}

void dx11CreateWindowTarget(phandle_t h, Handle hwnd, int width, int height)
{
	DX11_Window *window = new DX11_Window(hwnd, width, height);
	DX11_Resource *resource = new DX11_Resource(DX11_Resource::kWindow, window);
	*h = resource;
}

void dx11UpdateWindowTarget(phandle_t h, Handle newHwnd, int width, int height)
{
	DX11_Resource *resource = h->castTo<DX11_Resource *>();
	AX_ASSERT(resource->m_type == DX11_Resource::kWindow);
	DX11_Window *window = resource->m_window;

	window->update(newHwnd, width, height);
}

void dx11DeleteWindowTarget(phandle_t h)
{
	DX11_Resource *resource = h->castTo<DX11_Resource *>();
	AX_ASSERT(resource->m_type == DX11_Resource::kWindow);
	delete resource;
	delete h;
}

void dx11CreateQuery(phandle_t &h)
{
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_OCCLUSION;
	desc.MiscFlags = 0;
	ID3D11Query *query;
	g_device->CreateQuery(&desc, &query);
	DX11_Resource *apiRes = new DX11_Resource(DX11_Resource::kOcclusionQuery, query);
	*h = apiRes;
}

void dx11DeleteQuery(phandle_t h)
{
	DX11_Resource *resource = h->castTo<DX11_Resource *>();
	AX_ASSERT(resource->m_type == DX11_Resource::kOcclusionQuery);
	delete resource;
	delete h;
}

void dx11BeginPerfEvent(const char *pixname)
{
	D3DPERF_BeginEvent(0, u2w(pixname).c_str());
}
void dx11EndPerfEvent()
{
	D3DPERF_EndEvent();
}

static inline ID3D11DepthStencilView *getDSV(phandle_t h, int slice)
{
	if (!h) return 0;
	DX11_Resource *apiRes = h->castTo<DX11_Resource *>();
	AX_ASSERT(apiRes->m_type == DX11_Resource::kDynamicTexture);
	AX_ASSERT(apiRes->m_dynamicTextureData->texFormat.isDepth());
	return apiRes->m_dynamicTextureData->m_depthStencilViews[slice];
}

static inline ID3D11RenderTargetView *getRTV(phandle_t h, int slice)
{
	if (!h) return 0;
	DX11_Resource *apiRes = h->castTo<DX11_Resource *>();
	AX_ASSERT(apiRes->m_type == DX11_Resource::kDynamicTexture);
	AX_ASSERT(apiRes->m_dynamicTextureData->texFormat.isColor());
	return apiRes->m_dynamicTextureData->m_renderTargetViews[slice];
}

void dx11SetTargetSet(phandle_t targetSet[RenderTargetSet::MaxTarget], int slices[RenderTargetSet::MaxTarget])
{
	ID3D11DepthStencilView *dsv = getDSV(targetSet[0], slices[0]);
	ID3D11RenderTargetView *rtv[RenderTargetSet::MaxColorTarget];

	int numView = 0;
	for (int i = 0; i < RenderTargetSet::MaxColorTarget; i++) {
		rtv[i] = getRTV(targetSet[i+1], slices[i+1]);
		if (!rtv[i])
			break;
		numView++;
	}

	g_context->OMSetRenderTargets(numView, rtv, dsv);
}

void dx11SetViewport(const Rect &rect, const Vector2 & depthRange)
{
	D3D11_VIEWPORT d3dviewport;
	d3dviewport.TopLeftX = rect.x;
	if (rect.y < 0)
		d3dviewport.TopLeftY = s_curRenderTargetSize.height + rect.y - rect.height;
	else
		d3dviewport.TopLeftY = rect.y;

	d3dviewport.Width = rect.width;
	d3dviewport.Height = rect.height;
	d3dviewport.MinDepth = depthRange.x;
	d3dviewport.MaxDepth = depthRange.y;

	g_context->RSSetViewports(1, &d3dviewport);
}

void dx11SetShader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm, Technique tech)
{
	s_curShader = g_shaderManager->findShader(name, gm, mm);
	s_curTechnique = tech;
}

void dx11SetConstBuffer(ConstBuffers::Type type, int size, const void *data)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	g_context->Map(g_d3dConstBuffers[type], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, data, size);
	g_context->Unmap(g_d3dConstBuffers[type], 0);
}

void dx11SetParameters(const FastParams *params1, const FastParams *params2)
{
	if (params1)
		g_curParams1 = *params1;
	else
		g_curParams1.clear();

	if (params2)
		g_curParams2 = *params2;
	else
		g_curParams2.clear();
}

void dx11SetVertices(phandle_t vb, VertexType vt, int offset, int vert_count)
{
	DX11_Resource *resource = vb->castTo<DX11_Resource *>();
	AX_ASSERT(resource->m_type == DX11_Resource::kVertexBuffer);

	g_curInstanced = false;
	g_curVertexType = vt;
	s_curNumVertices = vert_count;

	UINT stride = vt.stride();
	UINT uoffset = offset;
	g_context->IASetVertexBuffers(0, 1, &resource->m_vertexBuffer, &stride, &uoffset);
}

void dx11SetInstanceVertices(phandle_t vb, VertexType vt, int offset, int vert_count, phandle_t inb, int inoffset, int incount)
{
	DX11_Resource *resV = vb->castTo<DX11_Resource *>();
	AX_ASSERT(resV->m_type == DX11_Resource::kVertexBuffer);
	DX11_Resource *resI = inb->castTo<DX11_Resource *>();
	AX_ASSERT(resI->m_type == DX11_Resource::kVertexBuffer);

	g_curInstanced = true;
	g_curVertexType = vt;
	s_curNumVertices = vert_count;
	s_curNumInstances = incount;

	ID3D11Buffer *buffers[2] = {resV->m_vertexBuffer, resI->m_vertexBuffer};
	UINT strides[2] = { vt.stride(), 64 };
	UINT offsets[2] = { offset, inoffset };

	g_context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
}

static D3D11_PRIMITIVE_TOPOLOGY trElementType(ElementType et)
{
	switch (et) {
	case ElementType_PointList: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case ElementType_LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case ElementType_TriList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case ElementType_TriStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default: AX_WRONGPLACE; return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

void dx11SetIndices(phandle_t ib, ElementType et, int offset, int indicescount)
{
	DX11_Resource *resource = ib->castTo<DX11_Resource *>();
	AX_ASSERT(resource->m_type == DX11_Resource::kIndexBuffer);

	s_curNumIndices = indicescount;

	g_context->IASetIndexBuffer(resource->m_indexBuffer, DXGI_FORMAT_R16_UINT, offset);
	g_context->IASetPrimitiveTopology(trElementType(et));
}

void dx11SetVerticesUP(const void *vb, VertexType vt, int vertcount)
{
	int dataSize = vt.stride() * vertcount;
	int offset = s_curVertexBufferPos;
	if (offset + dataSize > VERTEX_UP_SIZE)
		offset = 0;

	dx11UploadVertexBuffer(s_tempVertexBuffer, offset, dataSize, vb);
	dx11SetVertices(s_tempVertexBuffer, vt, offset, vertcount);
}

void dx11SetIndicesUP(const void *ib, ElementType et, int indicescount)
{
	int data_size = indicescount * sizeof(ushort_t);
	int offset = s_curIndexBufferPos;
	if (offset + data_size > INDEX_UP_SIZE)
		offset = 0;

	dx11UploadIndexBuffer(s_tempIndexBuffer, offset, data_size, ib);
	dx11SetIndices(s_tempIndexBuffer, et, offset, indicescount);
}

void dx11SetGlobalTexture(GlobalTextureId id, phandle_t h, const SamplerDesc &samplerState)
{
	g_curGlobalTextures[id] = h;
	g_curGlobalTextureSamplerDescs[id] = samplerState;
}

void dx11SetMaterialTexture(const FastTextureParams *textures)
{
	g_curMaterialTextures = *textures;
}

void dx11SetRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd)
{
	g_stateManager->setDepthStencilState(dsd);
	g_stateManager->setRasterizerState(rd);
	g_stateManager->setBlendState(bd);
}

static void setupBoundingBoxIndices()
{
	static ushort_t indices[] = {
		0, 2, 1, 1, 2, 3,
		2, 6, 3, 3, 6, 7,
		6, 4, 7, 7, 4, 5,
		4, 0, 5, 5, 0, 1,
		1, 3, 5, 5, 3, 7,
		0, 4, 2, 2, 4, 6
	};

	dx11SetIndicesUP(indices, ElementType_TriList, ArraySize(indices));
}

static void setupBoundingBoxVertices(const BoundingBox &bbox)
{
	static Vector3 verts[8];
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				verts[i*4+j*2+k].x = i == 0 ? bbox.min.x : bbox.max.x; 
				verts[i*4+j*2+k].y = j == 0 ? bbox.min.y : bbox.max.y; 
				verts[i*4+j*2+k].z = k == 0 ? bbox.min.z : bbox.max.z; 
			}
		}
	}
	dx11SetVerticesUP(verts, VertexType::kChunk, ArraySize(verts));
}

static std::list<Query *> s_activeQueries;

void dx11IssueQueries(int n, Query *queries[])
{
	setupBoundingBoxIndices();

	s_curShader->begin(s_curTechnique);
	s_curShader->beginPass(0);

	for (int i = 0; i < n; i++) {
		Query *query = queries[i];
		DX11_Resource *resource = query->m_handle->castTo<DX11_Resource *>();
		// setup vertice
		setupBoundingBoxVertices(queries[i]->m_bbox);

		g_context->Begin(resource->m_query);
		// draw
		g_context->DrawIndexed(s_curNumIndices, 0, 0);

		g_context->End(resource->m_query);

		s_activeQueries.push_back(query);
	}

	s_curShader->endPass();
	s_curShader->end();
}

void dx11Draw()
{
	UINT npass = s_curShader->begin(s_curTechnique);
	for (int i = 0; i < npass; i++) {
		s_curShader->beginPass(i);

		if (!g_curInstanced) {
			g_context->DrawIndexed(s_curNumIndices, 0, 0);
		} else {
			g_context->DrawIndexedInstanced(s_curNumIndices, s_curNumInstances, 0, 0, 0);
		}

		s_curShader->endPass();
	}
	s_curShader->end();
}

void dx11Clear(const RenderClearer &clearer)
{
	//dx11_context->ClearDepthStencilView();
}

void dx11Present(phandle_t window)
{
	AX_ASSERT(window);
	DX11_Resource *resrouce = window->castTo<DX11_Resource *>();
	AX_ASSERT(resrouce->m_type == DX11_Resource::kWindow);
	resrouce->m_window->present();
	//dx11CheckQueryResult();
}

void dx11AssignRenderApi()
{
	RenderApi::createTextureFromFileInMemory = &dx11CreateTextureFromFileInMemory;
	RenderApi::createTexture = &dx11CreateTexture;
	RenderApi::uploadSubTexture = &dx11UploadSubTexture;
	RenderApi::generateMipmap = &dx11GenerateMipmap;
	RenderApi::deleteTexture = &dx11DeleteTexture;

	RenderApi::createVertexBuffer = &dx11CreateVertexBuffer;
	RenderApi::uploadVertexBuffer = &dx11UploadVertexBuffer;
	RenderApi::deleteVertexBuffer = &dx11DeleteVertexBuffer;

	RenderApi::createIndexBuffer = &dx11CreateIndexBuffer;
	RenderApi::uploadIndexBuffer = &dx11UploadIndexBuffer;
	RenderApi::deleteIndexBuffer = &dx11DeleteIndexBuffer;

	RenderApi::createWindowTarget = &dx11CreateWindowTarget;
	RenderApi::updateWindowTarget = &dx11UpdateWindowTarget;
	RenderApi::deleteWindowTarget = &dx11DeleteWindowTarget;

	RenderApi::createQuery = &dx11CreateQuery;
	RenderApi::issueQueries = &dx11IssueQueries;
	RenderApi::deleteQuery = &dx11DeleteQuery;

	RenderApi::beginPerfEvent = &dx11BeginPerfEvent;
	RenderApi::endPerfEvent = &dx11EndPerfEvent;

	RenderApi::setTargetSet = &dx11SetTargetSet;

	RenderApi::setViewport = &dx11SetViewport;

	RenderApi::setShader = &dx11SetShader;
	RenderApi::setConstBuffer = &dx11SetConstBuffer;
	RenderApi::setParameters = &dx11SetParameters;

	RenderApi::setVertices = &dx11SetVertices;
	RenderApi::setInstanceVertices = &dx11SetInstanceVertices;
	RenderApi::setIndices = &dx11SetIndices;

	RenderApi::setVerticesUP = &dx11SetVerticesUP;
	RenderApi::setIndicesUP = &dx11SetIndicesUP;

	RenderApi::setGlobalTexture = &dx11SetGlobalTexture;
	RenderApi::setMaterialTexture = &dx11SetMaterialTexture;

	RenderApi::setRenderState = &dx11SetRenderState;
	RenderApi::draw = &dx11Draw;

	RenderApi::clear = &dx11Clear;

	RenderApi::present = &dx11Present;
}

void dx11InitApi()
{
	// initialize temp buffer
	s_tempVertexBuffer = new Handle;
	s_tempIndexBuffer = new Handle;
	dx11CreateVertexBuffer(s_tempVertexBuffer, VERTEX_UP_SIZE, Primitive::HintDynamic, 0);
	dx11CreateIndexBuffer(s_tempIndexBuffer, INDEX_UP_SIZE, Primitive::HintDynamic, 0);
	s_curVertexBufferPos = 0;
	s_curIndexBufferPos = 0;

	// initialize const buffer
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	desc.ByteWidth = g_constBuffers.getBuffer(0)->getByteSize();
	g_device->CreateBuffer(&desc, 0, &g_d3dConstBuffers[0]);

	desc.ByteWidth = g_constBuffers.getBuffer(1)->getByteSize();
	g_device->CreateBuffer(&desc, 0, &g_d3dConstBuffers[1]);

	desc.ByteWidth = PRIMITIVECONST_COUNT * 16;
	g_device->CreateBuffer(&desc, 0, &g_d3dConstBuffers[2]);

	dx11AssignRenderApi();
}

AX_DX11_END_NAMESPACE
