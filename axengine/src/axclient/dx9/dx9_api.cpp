#include "dx9_private.h"

AX_BEGIN_NAMESPACE

FastParams dx9_curParams1;
FastParams dx9_curParams2;
phandle_t dx9_curGlobalTextures[GlobalTextureId::MaxType];
SamplerDesc dx9_curGlobalTextureSamplerDescs[GlobalTextureId::MaxType];
FastTextureParams dx9_curMaterialTextures;

enum {
	Size_VerticeUP = 64 * 1024,
	Size_IndiceUP = 64 * 1024,
};
static DX9_Shader *s_curShader;
static Technique s_curTechnique;
static ConstBuffers s_curConstBuffer;
static D3DPRIMITIVETYPE s_curPrimitiveType;
static int s_curNumVertices;
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

inline bool trTexFormat(TexFormat texformat, D3DFORMAT &d3dformat)
{
	d3dformat = D3DFMT_UNKNOWN;

	switch (texformat) {
	case TexFormat::AUTO: return false;
	case TexFormat::NULLTARGET: d3dformat = (D3DFORMAT)MAKEFOURCC('N','U','L','L'); break;
	case TexFormat::R5G6B5: d3dformat = D3DFMT_R5G6B5; break;
	case TexFormat::RGB10A2: d3dformat = D3DFMT_A2R10G10B10; break;
	case TexFormat::RG16: d3dformat = D3DFMT_G16R16; break;
	case TexFormat::L8: d3dformat = D3DFMT_L8; break;
	case TexFormat::LA8: d3dformat = D3DFMT_A8L8; break;
	case TexFormat::A8: d3dformat = D3DFMT_A8; break;
	case TexFormat::BGR8: d3dformat = D3DFMT_R8G8B8; break;
	case TexFormat::BGRA8: d3dformat = D3DFMT_A8R8G8B8; break;
	case TexFormat::BGRX8: d3dformat = D3DFMT_X8R8G8B8; break;
	case TexFormat::DXT1: d3dformat = D3DFMT_DXT1; break;
	case TexFormat::DXT3: d3dformat = D3DFMT_DXT3; break;
	case TexFormat::DXT5: d3dformat = D3DFMT_DXT5; break;
	case TexFormat::L16: d3dformat = D3DFMT_L16; break;
	case TexFormat::R16F: d3dformat = D3DFMT_R16F; break;
	case TexFormat::RG16F: d3dformat = D3DFMT_G16R16F; break;
	case TexFormat::RGB16F: break;
	case TexFormat::RGBA16F: d3dformat = D3DFMT_A16B16G16R16F; break;
	case TexFormat::R32F: d3dformat = D3DFMT_R32F; break;
	case TexFormat::RG32F: d3dformat = D3DFMT_G32R32F; break;
	case TexFormat::RGB32F: break;
	case TexFormat::RGBA32F: d3dformat = D3DFMT_A32B32G32R32F; break;
	case TexFormat::D16: d3dformat = D3DFMT_D16; break;
	case TexFormat::D24: d3dformat = D3DFMT_D24X8; break;
	case TexFormat::D32: d3dformat = D3DFMT_D32; break;
	case TexFormat::D24S8: d3dformat = D3DFMT_D24S8; break;
	case TexFormat::DF16: d3dformat = (D3DFORMAT)MAKEFOURCC('D','F','1','6'); break;
	case TexFormat::DF24: d3dformat = (D3DFORMAT)MAKEFOURCC('D','F','2','4'); break;
	case TexFormat::RAWZ: d3dformat = (D3DFORMAT)MAKEFOURCC('R','A','W','Z'); break;
	case TexFormat::INTZ: d3dformat = (D3DFORMAT)MAKEFOURCC('I','N','T','Z'); break;
	default: AX_WRONGPLACE;
	}

	return d3dformat != D3DFMT_UNKNOWN;
}

inline D3DPRIMITIVETYPE trElementType(ElementType type)
{
	switch (type) {
	case ElementType_PointList: return D3DPT_POINTLIST;
	case ElementType_LineList: return D3DPT_LINELIST;
	case ElementType_TriList: return D3DPT_TRIANGLELIST;
	case ElementType_TriStrip: return D3DPT_TRIANGLESTRIP;
	default: AX_WRONGPLACE; return D3DPT_TRIANGLELIST;
	}
}

static inline int sCalcNumElements(D3DPRIMITIVETYPE mode, int numindexes)
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



static void dx9CreateTextureFromFileInMemory(phandle_t h, IoRequest *asioRequest)
{
	IDirect3DTexture9 *texture;
	V(D3DXCreateTextureFromFileInMemory(dx9_device, asioRequest->fileData(), asioRequest->fileSize(), &texture));
	DX9_Resource *resource = new DX9_Resource(DX9_Resource::kTexture, texture);
	*h = resource;
	delete asioRequest;
}

static void dx9CreateTexture(phandle_t h, TexType textype, TexFormat format, int width, int height, int depth, int flags)
{
	bool mipmap = false;
	D3DFORMAT d3dformat;
	trTexFormat(format, d3dformat);

	if (d3dformat == D3DFMT_UNKNOWN) {
		Errorf("Direct3D don't support texture format '%s'", format.toString());
	}

	D3DPOOL d3dpool = D3DPOOL_MANAGED;
	DWORD d3dusage = 0;

	if (flags & Texture::RenderTarget) {
		d3dpool = D3DPOOL_DEFAULT;
		if (format.isDepth()) {
			d3dusage = D3DUSAGE_DEPTHSTENCIL;
		} else {
			d3dusage = D3DUSAGE_RENDERTARGET;
		}
	}

	if (flags & Texture::AutoGenMipmap) {
		d3dusage |= D3DUSAGE_AUTOGENMIPMAP;

		bool m_hardwareGenMipmap = g_renderDriverInfo.autogenMipmapSupports[format];

		if (!m_hardwareGenMipmap) {
			d3dusage &= ~D3DUSAGE_AUTOGENMIPMAP;
		}
	}

	IDirect3DBaseTexture9 *texture = 0;
	IDirect3DTexture9 *tex2D = 0;
	IDirect3DVolumeTexture9 *tex3D = 0;
	IDirect3DCubeTexture9 *texCube = 0;

	int m_videoMemoryUsed = format.calculateDataSize(width, height);

	if (textype == TexType::_2D) {
		V(dx9_device->CreateTexture(width, height, 1, d3dusage, d3dformat, d3dpool, &tex2D, 0));
		texture = tex2D;
	} else if (textype == TexType::_3D) {
		V(dx9_device->CreateVolumeTexture(width, height, depth, 1, d3dusage, d3dformat, d3dpool, &tex3D, 0));
		texture = tex3D;
	} else if (textype == TexType::CUBE) {
		V(dx9_device->CreateCubeTexture(width, 1, d3dusage, d3dformat, d3dpool, &texCube, 0));
		texture = texCube;
	} else {
		AX_WRONGPLACE;
	}

	DX9_Resource *resource = new DX9_Resource(DX9_Resource::kTexture, texture);
	if (tex2D) {
		V(tex2D->GetSurfaceLevel(0, &resource->m_level0));
	}
	resource->texType = textype;
	resource->width = width; resource->height = height; resource->depth = depth;
	*h = resource;

	stat_textureMemory.add(m_videoMemoryUsed);
}

static void sUploadTexture(phandle_t h, int level, const void *pixels, TexFormat format)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kTexture);
	IDirect3DTexture9 *obj = resource->m_texture;

	LPDIRECT3DSURFACE9 surface;

	V(obj->GetSurfaceLevel(level, &surface));
	D3DSURFACE_DESC surfdesc;
	obj->GetLevelDesc(0, &surfdesc);

	D3DFORMAT d3dformat = D3DFMT_UNKNOWN;
	if (format == TexFormat::AUTO) {
		d3dformat = surfdesc.Format;
	} else {
		trTexFormat(format, d3dformat);
	}

	if (d3dformat == D3DFMT_UNKNOWN)
		return;

	RECT srcrect;
	srcrect.left = 0; srcrect.top = 0; srcrect.right = surfdesc.Width; srcrect.bottom = surfdesc.Height;
	UINT srcpitch = format.calculateDataSize(surfdesc.Width, 1);

	V(D3DXLoadSurfaceFromMemory(surface, 0, 0, pixels, d3dformat, srcpitch, 0, &srcrect, 0, 0));
	SAFE_RELEASE(surface);
}

static void dx9UploadTexture(phandle_t h, const void *pixels, TexFormat format)
{
	sUploadTexture(h, 0, pixels, format);
}

static void dx9UploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format)
{
	AX_ASSERT(h && *h);
	AX_ASSERT(format);
	AX_ASSERT(!rect.isEmpty());

	D3DFORMAT d3dformat;
	trTexFormat(format, d3dformat);

	if (d3dformat == D3DFMT_UNKNOWN) {
		return;
	}

	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kTexture);
	IDirect3DTexture9 *obj = resource->m_texture;

	IDirect3DSurface9 *surface;
	V(obj->GetSurfaceLevel(0, &surface));

	RECT d3drect;
	d3drect.left = rect.x; d3drect.top = rect.y; d3drect.right = rect.xMax(); d3drect.bottom = rect.yMax();

	RECT d3dsrcrect;
	d3dsrcrect.left = 0; d3dsrcrect.top = 0; d3dsrcrect.right = rect.width; d3dsrcrect.bottom = rect.height;

	UINT srcpitch = format.calculateDataSize(rect.width, 1);
	DWORD d3dfilter = D3DX_FILTER_NONE;

	V(D3DXLoadSurfaceFromMemory(surface, 0, &d3drect, pixels, d3dformat, srcpitch, 0, &d3dsrcrect, d3dfilter, 0));

	SAFE_RELEASE(surface);
}

static void dx9DeleteTexture(phandle_t h)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kTexture);

	delete resource;
	delete h;
}

static void dx9GenerateMipmap(phandle_t h)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kTexture);
	IDirect3DTexture9 *obj = resource->m_texture;

	IDirect3DSurface9 *surface;

	V(obj->GetSurfaceLevel(0, &surface));
	D3DSURFACE_DESC surfdesc;
	obj->GetLevelDesc(0, &surfdesc);
	bool hardwaremipmap = g_renderDriverInfo.autogenMipmapSupports[surfdesc.Format];

	if (hardwaremipmap) {
		obj->GenerateMipSubLevels();
		return;
	}

	// software convert.
	// convert to BGRA8 first, then use hardware mipmap generation, then convert back
	if (surfdesc.Format < D3DFMT_DXT1 || surfdesc.Format > D3DFMT_DXT5) {
		// currently, only support dxtc
		return;
	}

	D3DLOCKED_RECT lockedRect;
	V(obj->LockRect(0, &lockedRect, 0, 0));
	V(obj->UnlockRect(0));

	Handle dum;
	dx9CreateTexture(&dum, TexType::_2D, TexFormat::BGRA8, surfdesc.Width, surfdesc.Height, 1, Texture::AutoGenMipmap);
	// TODO: sUploadTexture(&dum, 0, lockedRect.pBits, surfdesc.Format);
	AX_ASSERT(0);

	LPDIRECT3DTEXTURE9 dummyobj = dum.castTo<LPDIRECT3DTEXTURE9>();

	V(dummyobj->LockRect(0, &lockedRect, 0, 0));
	Image image;
	image.initImage(TexFormat::BGRA8, surfdesc.Width, surfdesc.Height);
	image.setData(0, lockedRect.pBits, image.getDataSize(0));
	V(dummyobj->UnlockRect(0));

	image.generateMipmaps();

	int width = surfdesc.Width;
	int height = surfdesc.Height;
	for (DWORD i = 0; i < obj->GetLevelCount(); i++) {
		if (i >= (DWORD)image.getNumMipmapLevels()) {
			break;
		}
		sUploadTexture(h, i, image.getData(i), TexFormat::BGR8);

		width >>= 1;
		height >>= 1;
		if (width < 1) width = 1;
		if (height < 1) height = 1;
	}

	dx9DeleteTexture(&dum);
}

static void dx9CreateVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{
	DWORD d3dusage = D3DUSAGE_WRITEONLY;
	D3DPOOL d3dpool = D3DPOOL_DEFAULT;

	if (hint != Primitive::HintStatic) {
		d3dusage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	}

	IDirect3DVertexBuffer9 *vb;
	V(dx9_device->CreateVertexBuffer(datasize, d3dusage, 0, d3dpool, &vb, 0));
	DX9_Resource *resource = new DX9_Resource(DX9_Resource::kVertexBuffer, vb);
	resource->m_isDynamic = hint != Primitive::HintStatic;
	*h = resource;

	stat_numVertexBuffers.inc();
	stat_vertexBufferMemory.add(datasize);
}

static void dx9UploadVertexBuffer(phandle_t h, int datasize, const void *p)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type = DX9_Resource::kVertexBuffer);
	IDirect3DVertexBuffer9 *obj = resource->m_vertexBuffer;

	DWORD flag = 0;
	if (resource->m_isDynamic)
		flag = D3DLOCK_DISCARD;

	void *dst = 0;
	V(obj->Lock(0, datasize, &dst, flag));
	memcpy(dst, p, datasize);
	V(obj->Unlock());
}

static void dx9DeleteVertexBuffer(phandle_t h)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type = DX9_Resource::kVertexBuffer);

	delete resource;
	delete h;
}

static void dx9CreateIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{
	DWORD d3dusage = D3DUSAGE_WRITEONLY;
	D3DPOOL d3dpool = D3DPOOL_DEFAULT;

	if (hint != Primitive::HintStatic) {
		d3dusage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	}

	IDirect3DIndexBuffer9 *ib;
	V(dx9_device->CreateIndexBuffer(datasize, d3dusage, D3DFMT_INDEX16, d3dpool, &ib, 0));
	DX9_Resource *resource = new DX9_Resource(DX9_Resource::kIndexBuffer, ib);
	resource->m_isDynamic = hint != Primitive::HintStatic;
	*h = resource;

	stat_numIndexBuffers.inc();
	stat_indexBufferMemory.add(datasize);
}

static void dx9UploadIndexBuffer(phandle_t h, int datasize, const void *p)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kIndexBuffer);
	IDirect3DIndexBuffer9 *obj = resource->m_indexBuffer;

	DWORD flag = 0;
	if (resource->m_isDynamic)
		flag = D3DLOCK_DISCARD;

	void *dst = 0;
	V(obj->Lock(0, datasize, &dst, flag));
	memcpy(dst, p, datasize);
	V(obj->Unlock());
}

static void dx9DeleteIndexBuffer(phandle_t h)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kIndexBuffer);
	delete resource;
	delete h;
}

static void dx9CreateWindowTarget(phandle_t h, Handle hwnd, int width, int height)
{
	DX9_Window *window = new DX9_Window(hwnd, width, height);
	DX9_Resource *resource = new DX9_Resource(DX9_Resource::kWindow, window);
	*h = resource;
}

static void dx9UpdateWindowTarget(phandle_t h, Handle newHwnd, int width, int height)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kWindow);
	DX9_Window *window = resource->m_window;

	window->update(newHwnd, width, height);
}

static void dx9DeleteWindowTarget(phandle_t h)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kWindow);
	delete resource;
	delete h;
}

static void dx9CreateQuery(phandle_t &h)
{
	IDirect3DQuery9 *query = 0;
	V(dx9_device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &query));
	DX9_Resource *resource = new DX9_Resource(DX9_Resource::kOcclusionQuery, query);
	*h = resource;
}

static void dx9SetShader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm, Technique tech);
static void dx9SetRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd);

static void dx9IssueQueries(int n, Query *queries[])
{
	return;

	dx9SetShader("_query", GlobalMacro(), MaterialMacro(), Technique::Main);
	dx9_stateManager->setVertexDeclaration(dx9_vertexDeclarations[VertexType::kChunk]);
	//dx9SetRenderState();

	s_curShader->begin(s_curTechnique);
	s_curShader->beginPass(0);

	// setup vertice

	// draw
	V(dx9_device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, s_curIndiceBufferUP, D3DFMT_INDEX16, s_curVerticeBufferUP, sizeof(ChunkVertex)));

}

static void dx9DeleteQuery(phandle_t h)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kOcclusionQuery);
	delete resource;
	delete h;
}

static void dx9BeginPix(const char *pixname)
{
	D3DPERF_BeginEvent(D3DCOLOR_RGBA(0,0,0,255), u2w(pixname).c_str());
}

static void dx9EndPix()
{
	D3DPERF_EndEvent();
}

static bool surfaceSizeIsSet = false;
inline static IDirect3DSurface9 *getSurface(phandle_t h, int slice, bool setSize)
{
	if (!h || !*h) return 0;

	DX9_Resource *resource = h->castTo<DX9_Resource *>();

	if (resource->m_type == DX9_Resource::kTexture) {
		if (setSize) {
			s_curRenderTargetSize.set(resource->width, resource->height);
			surfaceSizeIsSet = true;
		}
		return resource->getSliceSurface(slice);
	} else {
		if (setSize) {
			s_curRenderTargetSize = resource->m_window->getSize();
			surfaceSizeIsSet = true;
		}
		return resource->m_window->getSurface();
	}
}

static void dx9SetTargetSet(phandle_t targetSet[RenderTargetSet::MaxTarget], int slices[RenderTargetSet::MaxTarget])
{
	AX_ASSURE(targetSet[0] || targetSet[1]);

	surfaceSizeIsSet = false;
	IDirect3DSurface9 *surface = getSurface(targetSet[0], 0, true);
	if (surface) {
		V(dx9_device->SetDepthStencilSurface(surface));
		SAFE_RELEASE(surface);
	}

	if (!targetSet[1]) {
		AX_ASSURE(surfaceSizeIsSet)
		surface = dx9_driver->getNullTarget(s_curRenderTargetSize);
		V(dx9_device->SetRenderTarget(0, surface))
	} else {
		surface = getSurface(targetSet[1], slices[1], true);
		AX_ASSURE(surface);
		V(dx9_device->SetRenderTarget(0, surface));
		SAFE_RELEASE(surface);
	}

	for (int i = 1; i < RenderTargetSet::MaxColorTarget; i++) {
		surface = getSurface(targetSet[i+1], slices[i+1], false);
		V(dx9_device->SetRenderTarget(i, surface));
		SAFE_RELEASE(surface);
	}

	// if no depth surface, we assign a default
	if (!targetSet[0]) {
		surface = dx9_driver->getDepthStencil(s_curRenderTargetSize);
		V(dx9_device->SetDepthStencilSurface(surface));
	}
}

static void dx9SetViewport(const Rect &rect, const Vector2 & depthRange)
{
	D3DVIEWPORT9 d3dviewport;
#if 1
	d3dviewport.X = rect.x;
	if (rect.y < 0)
		d3dviewport.Y = s_curRenderTargetSize.height + rect.y - rect.height;
	else
		d3dviewport.Y = rect.y;

	d3dviewport.Width = rect.width;
	d3dviewport.Height = rect.height;
#else
	d3dviewport.X = d3dviewport.Y = 0;
	d3dviewport.Width = d3dviewport.Height = 512;
#endif
	d3dviewport.MinZ = depthRange.x;
	d3dviewport.MaxZ = depthRange.y;

	V(dx9_device->SetViewport(&d3dviewport));
}

static void dx9SetScissorRect(const Rect &rect)
{
	RECT d3dRect;
	d3dRect.left = rect.x;

	if (rect.y < 0)
		d3dRect.top = s_curRenderTargetSize.height + rect.y - rect.height;
	else
		d3dRect.top = rect.y;

	d3dRect.right = d3dRect.left + rect.width;
	d3dRect.bottom = d3dRect.top + rect.height;
	V(dx9_device->SetScissorRect(&d3dRect));
}


static void dx9SetShader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm, Technique tech)
{
	s_curShader = dx9_shaderManager->findShader(name, gm, mm);
	s_curTechnique = tech;
}

static void dx9SetConstBuffer(ConstBuffers::Type type, int size, const void *data)
{
	s_curConstBuffer.setData(type, size, data);

	int nreg = (size + 15) / 16;
	if (type == ConstBuffer::SceneConst) {
		dx9_device->SetVertexShaderConstantF(SCENECONST_REG, reinterpret_cast<const float *>(data), nreg);
		dx9_device->SetPixelShaderConstantF(SCENECONST_REG, reinterpret_cast<const float *>(data), nreg);
	} else if (type == ConstBuffer::InteractionConst) {
		dx9_device->SetVertexShaderConstantF(INTERACTIONCONST_REG, reinterpret_cast<const float *>(data), nreg);
		dx9_device->SetPixelShaderConstantF(INTERACTIONCONST_REG, reinterpret_cast<const float *>(data), nreg);
	} else {
		AX_WRONGPLACE;
	}
}

static void dx9SetParameters(const FastParams *params1, const FastParams *params2)
{
	if (params1)
		dx9_curParams1 = *params1;
	else
		dx9_curParams1.clear();

	if (params2)
		dx9_curParams2 = *params2;
	else
		dx9_curParams2.clear();
}

static void dx9SetVertices(phandle_t vb, VertexType vt, int offset)
{
	DX9_Resource *resource = vb->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kVertexBuffer);

	s_curVerticeUP = false;
	V(dx9_device->SetStreamSource(0, resource->m_vertexBuffer, offset, vt.stride()));
	dx9_stateManager->setVertexDeclaration(dx9_vertexDeclarations[vt]);

	if (s_curInstanced) {
		V(dx9_device->SetStreamSourceFreq(0, 1));
		V(dx9_device->SetStreamSourceFreq(1, 1));
		s_curInstanced = false;
	}
}

static void dx9SetInstanceVertices(phandle_t vb, VertexType vt, int offset, phandle_t inb, int inoffset, int incount)
{
	DX9_Resource *resV = vb->castTo<DX9_Resource *>();
	AX_ASSERT(resV->m_type == DX9_Resource::kVertexBuffer);
	DX9_Resource *resI = inb->castTo<DX9_Resource *>();
	AX_ASSERT(resI->m_type == DX9_Resource::kVertexBuffer);

	s_curVerticeUP = false;
	s_curInstanced = true;
	V(dx9_device->SetStreamSource(0, resV->m_vertexBuffer, offset, vt.stride()));
	V(dx9_device->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | incount));

	V(dx9_device->SetStreamSource(1, resI->m_vertexBuffer, inoffset, 64));
	V(dx9_device->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul));
	dx9_stateManager->setVertexDeclaration(dx9_vertexDeclarationsInstanced[vt]);
}

static void dx9SetIndices(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount)
{
	DX9_Resource *resource = ib->castTo<DX9_Resource *>();
	AX_ASSERT(resource->m_type == DX9_Resource::kIndexBuffer);

	s_curIndiceUP = false;
	V(dx9_device->SetIndices(resource->m_indexBuffer));
	s_curPrimitiveType = trElementType(et);
	s_curNumVertices = vertcount;
	s_curStartIndex = offset;
	s_curPrimitiveCount = sCalcNumElements(s_curPrimitiveType, indicescount);
}

static void dx9SetVerticesUP(const void *vb, VertexType vt, int vertcount)
{
	int datasize = vt.calcSize(vertcount);
	AX_ASSERT(datasize <= Size_VerticeUP);
	memcpy(s_curVerticeBufferUP, vb, datasize);
	s_curVerticeUP = true;
	s_curVertexType = vt;
	s_curNumVertices = vertcount;
	dx9_stateManager->setVertexDeclaration(dx9_vertexDeclarations[vt]);
}

static void dx9SetIndicesUP(const void *ib, ElementType et, int indicescount)
{
	int datasize = indicescount * sizeof(ushort_t);
	AX_ASSERT(datasize <= Size_IndiceUP);
	memcpy(s_curIndiceBufferUP, ib, datasize);
	s_curIndiceUP = true;
	s_curPrimitiveType = trElementType(et);
	s_curPrimitiveCount = sCalcNumElements(s_curPrimitiveType, indicescount);
}

static void dx9SetGlobalTexture(GlobalTextureId id, phandle_t h, const SamplerDesc &desc)
{
	dx9_curGlobalTextures[id] = h;
	dx9_curGlobalTextureSamplerDescs[id] = desc;
}

static void dx9SetMaterialTexture(const FastTextureParams *textures)
{
	dx9_curMaterialTextures = *textures;
}

static void dx9SetRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd)
{
	dx9_stateManager->setDepthStencilState(dsd);
	dx9_stateManager->setRasterizerState(rd);
	dx9_stateManager->setBlendState(bd);
}


//	static vOid dip(ElementType et, int offset, int vertcount, int indices_count) = 0;
static void dx9Draw()
{
	UINT npass = s_curShader->begin(s_curTechnique);
	for (int i = 0; i < npass; i++) {
		s_curShader->beginPass(i);

		if (s_curIndiceUP && s_curVerticeUP) {
			V(dx9_device->DrawIndexedPrimitiveUP(s_curPrimitiveType, 0, s_curNumVertices, s_curPrimitiveCount, s_curIndiceBufferUP, D3DFMT_INDEX16, s_curVerticeBufferUP, s_curVertexType.stride()));
		} else if (!s_curVerticeUP && !s_curIndiceUP) {
			HRESULT hr = dx9_device->DrawIndexedPrimitive(s_curPrimitiveType, 0, 0, s_curNumVertices, s_curStartIndex, s_curPrimitiveCount);
			const char *err = D3DErrorString(hr);
		} else {
			AX_WRONGPLACE;
		}

		s_curShader->endPass();
	}
	s_curShader->end();
}


// actions
static void dx9Clear(const RenderClearer &clearer)
{
	DWORD d3dclear = 0;

	if (clearer.isClearDepth) {
		d3dclear |= D3DCLEAR_ZBUFFER;
	}

	if (clearer.isClearStencil) {
		d3dclear |= D3DCLEAR_STENCIL;
	}

	if (clearer.isClearColor) {
		d3dclear |= D3DCLEAR_TARGET;
	}

	if (!d3dclear) {
		return;
	}

	D3DCOLOR d3dcolor = D3DCOLOR_RGBA(clearer.color.r,clearer.color.g,clearer.color.b,clearer.color.a);

	dx9_device->Clear(0, 0, d3dclear, d3dcolor, clearer.depth, clearer.stencil);
}

static void dx9Present(phandle_t window)
{
	AX_ASSERT(window);
	DX9_Resource *resrouce = window->castTo<DX9_Resource *>();
	AX_ASSERT(resrouce->m_type == DX9_Resource::kWindow);
	dx9_device->EndScene();
	resrouce->m_window->present();
	dx9_device->BeginScene();
}

void dx9AssignRenderApi()
{
	RenderApi::createTextureFromFileInMemory = &dx9CreateTextureFromFileInMemory;
	RenderApi::createTexture = &dx9CreateTexture;
	RenderApi::uploadTexture = &dx9UploadTexture;
	RenderApi::uploadSubTexture = &dx9UploadSubTexture;
	RenderApi::generateMipmap = &dx9GenerateMipmap;
	RenderApi::deleteTexture = &dx9DeleteTexture;

	RenderApi::createVertexBuffer = &dx9CreateVertexBuffer;
	RenderApi::uploadVertexBuffer = &dx9UploadVertexBuffer;
	RenderApi::deleteVertexBuffer = &dx9DeleteVertexBuffer;

	RenderApi::createIndexBuffer = &dx9CreateIndexBuffer;
	RenderApi::uploadIndexBuffer = &dx9UploadIndexBuffer;
	RenderApi::deleteIndexBuffer = &dx9DeleteIndexBuffer;

	RenderApi::createWindowTarget = &dx9CreateWindowTarget;
	RenderApi::updateWindowTarget = &dx9UpdateWindowTarget;
	RenderApi::deleteWindowTarget = &dx9DeleteWindowTarget;

	RenderApi::createQuery = &dx9CreateQuery;
	RenderApi::issueQueries = &dx9IssueQueries;
	RenderApi::deleteQuery = &dx9DeleteQuery;

	RenderApi::beginPerfEvent = &dx9BeginPix;
	RenderApi::endPerfEvent = &dx9EndPix;

	RenderApi::setTargetSet = &dx9SetTargetSet;

	RenderApi::setViewport = &dx9SetViewport;
	RenderApi::setScissorRect = &dx9SetScissorRect;

	RenderApi::setShader = &dx9SetShader;
	RenderApi::setConstBuffer = &dx9SetConstBuffer;
	RenderApi::setParameters = &dx9SetParameters;

	RenderApi::setVertices = &dx9SetVertices;
	RenderApi::setInstanceVertices = &dx9SetInstanceVertices;
	RenderApi::setIndices = &dx9SetIndices;

	RenderApi::setVerticesUP = &dx9SetVerticesUP;
	RenderApi::setIndicesUP = &dx9SetIndicesUP;

	RenderApi::setGlobalTexture = &dx9SetGlobalTexture;
	RenderApi::setMaterialTexture = &dx9SetMaterialTexture;

	RenderApi::setRenderState = &dx9SetRenderState;
	RenderApi::draw = &dx9Draw;

	RenderApi::clear = &dx9Clear;

	RenderApi::present = &dx9Present;
}

void dx9InitState()
{
	dx9SetRenderState(DepthStencilDesc(), RasterizerDesc(), BlendDesc());
}

AX_END_NAMESPACE
