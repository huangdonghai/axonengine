#include "dx11_private.h"

extern "C" {;
extern int WINAPI D3DPERF_BeginEvent(DWORD col, LPCWSTR wszName);
extern int WINAPI D3DPERF_EndEvent( void );
}

AX_BEGIN_NAMESPACE

enum {
	Size_VerticeUP = 64 * 1024,
	Size_IndiceUP = 64 * 1024,
};

FastParams dx11_curParams1;
FastParams dx11_curParams2;
phandle_t dx11_curGlobalTextures[GlobalTextureId::MaxType];
SamplerDesc dx11_curGlobalTextureSamplerDescs[GlobalTextureId::MaxType];
FastTextureParams dx11_curMaterialTextures;

static DX11_Shader *s_curShader;
static Technique s_curTechnique;

static int s_curNumVertices;
static int s_curNumIndices;
static int s_curNumInstances;
static int s_curStartIndex;
static int s_curPrimitiveCount;
static byte_t *s_curVerticeBufferUP[Size_VerticeUP];
static bool s_curVerticeUP = false;
static bool s_curInstanced = false;
static VertexType s_curVertexType;
static int s_curVertexOffset;
static byte_t *s_curIndiceBufferUP[Size_IndiceUP];
static bool s_curIndiceUP = false;
static Size s_curRenderTargetSize;

void dx11CreateTextureFromFileInMemory(phandle_t h, IoRequest *asioRequest)
{
	ID3D11Resource *texture;
	V(D3DX11CreateTextureFromMemory(dx11_device, asioRequest->fileData(), asioRequest->fileSize(), 0, 0, &texture, 0));
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
		bindflags |= D3D11_BIND_RENDER_TARGET;
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
		desc.SampleDesc.Quality = 1;
		desc.Usage = usage;
		desc.BindFlags = bindflags;
		desc.CPUAccessFlags = cpuAccessFlags;
		desc.MiscFlags = miscflags;

		ID3D11Texture2D *texture2D;
		V(dx11_device->CreateTexture2D(&desc, 0, &texture2D));

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
		V(dx11_device->CreateTexture3D(&desc, 0, &texture3D));
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

	dx11_context->UpdateSubresource(apiResource->m_dx11Resource, 0, &box, pixels, rowPitch, 0);
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
	dx11_device->CreateBuffer(&bufferDesc, &InitData, &buffer);

	DX11_Resource *apiRes = new DX11_Resource(DX11_Resource::kVertexBuffer, buffer);
	apiRes->m_isDynamic = (hint != Primitive::HintStatic);
	*h = apiRes;
}

void dx11UploadVertexBuffer(phandle_t h, int datasize, const void *p)
{
	DX11_Resource *apiRes = h->castTo<DX11_Resource *>();
	AX_ASSURE(apiRes->m_type == DX11_Resource::kVertexBuffer);
	D3D11_MAPPED_SUBRESOURCE mapped;
	dx11_context->Map(apiRes->m_dx11Resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, p, datasize);
	dx11_context->Unmap(apiRes->m_dx11Resource, 0);
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
	dx11_device->CreateBuffer(&bufferDesc, &InitData, &buffer);

	DX11_Resource *apiRes = new DX11_Resource(DX11_Resource::kIndexBuffer, buffer);
	apiRes->m_isDynamic = (hint != Primitive::HintStatic);
	*h = apiRes;
}

void dx11UploadIndexBuffer(phandle_t h, int datasize, const void *p)
{
	DX11_Resource *apiRes = h->castTo<DX11_Resource *>();
	AX_ASSURE(apiRes->m_type == DX11_Resource::kIndexBuffer);
	D3D11_MAPPED_SUBRESOURCE mapped;
	dx11_context->Map(apiRes->m_dx11Resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, p, datasize);
	dx11_context->Unmap(apiRes->m_dx11Resource, 0);
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
	dx11_device->CreateQuery(&desc, &query);
	DX11_Resource *apiRes = new DX11_Resource(DX11_Resource::kOcclusionQuery, query);
	*h = apiRes;
}

void dx11IssueQueries(int n, Query *queries[])
{

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

	dx11_context->OMSetRenderTargets(numView, rtv, dsv);
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

	dx11_context->RSSetViewports(1, &d3dviewport);
}

void dx11SetScissorRect(const Rect &scissorRect)
{
}

void dx11SetShader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm, Technique tech)
{
	s_curShader = dx11_shaderManager->findShader(name, gm, mm);
	s_curTechnique = tech;
}

void dx11SetConstBuffer(ConstBuffers::Type type, int size, const void *data)
{

}

void dx11SetParameters(const FastParams *params1, const FastParams *params2)
{
	if (params1)
		dx11_curParams1 = *params1;
	else
		dx11_curParams1.clear();

	if (params2)
		dx11_curParams2 = *params2;
	else
		dx11_curParams2.clear();
}

void dx11SetVertices(phandle_t vb, VertexType vt, int offset)
{
	DX11_Resource *resource = vb->castTo<DX11_Resource *>();
	AX_ASSERT(resource->m_type == DX11_Resource::kVertexBuffer);

	s_curVerticeUP = false;
	s_curInstanced = false;
	s_curVertexType = vt;

	UINT stride = vt.stride();
	UINT uoffset = offset;
	dx11_context->IASetVertexBuffers(0, 1, &resource->m_vertexBuffer, &stride, &uoffset);
}

void dx11SetInstanceVertices(phandle_t vb, VertexType vt, int offset, phandle_t inb, int inoffset, int incount)
{
	DX11_Resource *resV = vb->castTo<DX11_Resource *>();
	AX_ASSERT(resV->m_type == DX11_Resource::kVertexBuffer);
	DX11_Resource *resI = inb->castTo<DX11_Resource *>();
	AX_ASSERT(resI->m_type == DX11_Resource::kVertexBuffer);

	s_curVerticeUP = false;
	s_curInstanced = true;
	s_curVertexType = vt;
	s_curNumInstances = incount;

	ID3D11Buffer *buffers[2] = {resV->m_vertexBuffer, resI->m_vertexBuffer};
	UINT strides[2] = { vt.stride(), 64 };
	UINT offsets[2] = { offset, inoffset };

	dx11_context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
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

void dx11SetIndices(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount)
{
	DX11_Resource *resource = ib->castTo<DX11_Resource *>();
	AX_ASSERT(resource->m_type == DX11_Resource::kIndexBuffer);

	s_curIndiceUP = false;
	s_curNumVertices = vertcount;
	s_curStartIndex = offset;
	s_curNumIndices = indicescount;

	dx11_context->IASetIndexBuffer(resource->m_indexBuffer, DXGI_FORMAT_R16_UINT, offset);
	dx11_context->IASetPrimitiveTopology(trElementType(et));
}

void dx11SetVerticesUP(const void *vb, VertexType vt, int vertcount)
{}

void dx11SetIndicesUP(const void *ib, ElementType et, int indicescount)
{}

void dx11SetGlobalTexture(GlobalTextureId id, phandle_t h, const SamplerDesc &samplerState)
{
	dx11_curGlobalTextures[id] = h;
	dx11_curGlobalTextureSamplerDescs[id] = samplerState;
}

void dx11SetMaterialTexture(const FastTextureParams *textures)
{
	dx11_curMaterialTextures = *textures;
}

void dx11SetRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd)
{
	dx11_stateManager->setDepthStencilState(dsd);
	dx11_stateManager->setRasterizerState(rd);
	dx11_stateManager->setBlendState(bd);
}

void dx11Draw()
{
	UINT npass = s_curShader->begin(s_curTechnique);
	for (int i = 0; i < npass; i++) {
		s_curShader->beginPass(i);

		if (s_curIndiceUP && s_curVerticeUP) {
			//V(dx11_device->DrawIndexedPrimitiveUP(s_curPrimitiveType, 0, s_curNumVertices, s_curPrimitiveCount, s_curIndiceBufferUP, D3DFMT_INDEX16, s_curVerticeBufferUP, s_curVertexType.stride()));
		} else if (!s_curVerticeUP && !s_curIndiceUP) {
			if (!s_curInstanced) {
				dx11_context->DrawIndexed(s_curNumIndices, 0, 0);
			} else {
				dx11_context->DrawIndexedInstanced(s_curNumIndices, s_curNumInstances, 0, 0, 0);
			}
		} else {
			AX_WRONGPLACE;
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
	RenderApi::setScissorRect = &dx11SetScissorRect;

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

AX_END_NAMESPACE
