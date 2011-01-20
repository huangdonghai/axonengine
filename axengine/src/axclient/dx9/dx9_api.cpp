#include "dx9_private.h"

AX_BEGIN_NAMESPACE

FastParams s_curParams1;
FastParams s_curParams2;
phandle_t s_curGlobalTextures[GlobalTextureId::MaxType];
SamplerDesc s_curGlobalTextureSamplerDescs[GlobalTextureId::MaxType];
phandle_t s_curMaterialTextures[MaterialTextureId::MaxType];
SamplerDesc s_curMaterialTextureSamplerDescs[MaterialTextureId::MaxType];

static DX9_Shader *s_curShader;
static Technique s_curTechnique;
static ConstBuffers s_curConstBuffer;
static D3DPRIMITIVETYPE s_curPrimitiveType;
static int s_curNumVertices;
static int s_curStartIndex;
static int s_curPrimitiveCount;
static const void *s_curVerticeBufferUP;
static VertexType s_curVertexType;
static int s_curVertexOffset;
static const void *s_curIndiceBufferUP;
static DepthStencilDesc s_curDepthStencilDesc;
static RasterizerDesc s_curRasterizerDesc;
static BlendDesc s_curBlendDesc;
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



void dx9CreateTextureFromFileInMemory(phandle_t h, AsioRequest *asioRequest)
{
	V(D3DXCreateTextureFromFileInMemory(dx9_device, asioRequest->fileData(), asioRequest->fileSize(), (LPDIRECT3DTEXTURE9*)h));
	delete asioRequest;
}

void dx9CreateTexture2D(phandle_t h, TexFormat format, int width, int height, int flags)
{
	bool mipmap = false;
	D3DFORMAT d3dformat;
	trTexFormat(format, d3dformat);

	if (d3dformat == D3DFMT_UNKNOWN) {
		Errorf("Direct3D don't support texture format '%s'", format.toString());
	}

	D3DPOOL d3dpool = D3DPOOL_MANAGED;
	DWORD d3dusage = 0;

	if (flags & Texture::InitFlag_RenderTarget) {
		d3dpool = D3DPOOL_DEFAULT;
		if (format.isDepth()) {
			d3dusage = D3DUSAGE_DEPTHSTENCIL;
		} else {
			d3dusage = D3DUSAGE_RENDERTARGET;
		}
	}

	if (flags & Texture::InitFlag_AutoGenMipmap) {
		d3dusage |= D3DUSAGE_AUTOGENMIPMAP;

		bool m_hardwareGenMipmap = g_renderDriverInfo.autogenMipmapSupports[d3dformat];

		if (!m_hardwareGenMipmap) {
			d3dusage &= ~D3DUSAGE_AUTOGENMIPMAP;
		}
	}

	int m_videoMemoryUsed = format.calculateDataSize(width, height);

	V(dx9_device->CreateTexture(width, height, 1, d3dusage, d3dformat, d3dpool, (LPDIRECT3DTEXTURE9*)h, 0));

	stat_textureMemory.add(m_videoMemoryUsed);
}

static void sUploadTexture(phandle_t h, int level, const void *pixels, TexFormat format, IEventHandler *eventHandler)
{
	LPDIRECT3DTEXTURE9 obj = h->castTo<LPDIRECT3DTEXTURE9>();
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


void dx9UploadTexture(phandle_t h, const void *pixels, TexFormat format, IEventHandler *eventHandler)
{
	sUploadTexture(h, 0, pixels, format, eventHandler);
}

void dx9UploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format, IEventHandler *eventHandler)
{
	if (rect.isEmpty())
		return;

	if (!format)
		return;

	D3DFORMAT d3dformat;
	trTexFormat(format, d3dformat);

	if (d3dformat == D3DFMT_UNKNOWN) {
		return;
	}

	LPDIRECT3DSURFACE9 surface;
	LPDIRECT3DTEXTURE9 obj = h->castTo<LPDIRECT3DTEXTURE9>();
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

void dx9DeleteTexture2D(phandle_t h)
{
	LPDIRECT3DTEXTURE9 obj = h->castTo<LPDIRECT3DTEXTURE9>();

	SAFE_RELEASE(obj);

	delete h;
}

void dx9GenerateMipmap(phandle_t h)
{
	LPDIRECT3DTEXTURE9 obj = h->castTo<LPDIRECT3DTEXTURE9>();
	LPDIRECT3DSURFACE9 surface;

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
	dx9CreateTexture2D(&dum, TexFormat::BGRA8, surfdesc.Width, surfdesc.Height, Texture::InitFlag_AutoGenMipmap);
	sUploadTexture(&dum, 0, lockedRect.pBits, surfdesc.Format, 0);

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
		sUploadTexture(h, i, image.getData(i), TexFormat::BGR8, 0);

		width >>= 1;
		height >>= 1;
		if (width < 1) width = 1;
		if (height < 1) height = 1;
	}

	dx9DeleteTexture2D(&dum);
}

void dx9CreateVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{
	DWORD d3dusage = D3DUSAGE_WRITEONLY;
	D3DPOOL d3dpool = D3DPOOL_DEFAULT;

	if (hint != Primitive::HintStatic) {
		d3dusage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	}

	IDirect3DVertexBuffer9 *vb;
	V(dx9_device->CreateVertexBuffer(datasize, d3dusage, 0, d3dpool, &vb, 0));
	*h = vb;

	stat_numVertexBuffers.inc();
	stat_vertexBufferMemory.add(datasize);
}

void dx9UploadVertexBuffer(phandle_t h, int datasize, const void *p, IEventHandler *eventHandler)
{
	IDirect3DVertexBuffer9 *obj = h->castTo<IDirect3DVertexBuffer9 *>();

	DWORD flag = 0;
	D3DVERTEXBUFFER_DESC desc;
	V(obj->GetDesc(&desc));
	if (desc.Usage & D3DUSAGE_DYNAMIC)
		flag = D3DLOCK_DISCARD;

	void *dst = 0;
	V(obj->Lock(0, datasize, &dst, flag));
	memcpy(dst, p, datasize);
	V(obj->Unlock());
}

void dx9DeleteVertexBuffer(phandle_t h)
{
	IDirect3DVertexBuffer9 *obj = h->castTo<IDirect3DVertexBuffer9 *>();

	SAFE_RELEASE(obj);
}

void dx9CreateIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{
	DWORD d3dusage = D3DUSAGE_WRITEONLY;
	D3DPOOL d3dpool = D3DPOOL_DEFAULT;

	if (hint != Primitive::HintStatic) {
		d3dusage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	}

	V(dx9_device->CreateIndexBuffer(datasize, d3dusage, D3DFMT_INDEX16, d3dpool, (IDirect3DIndexBuffer9 **)(h), 0));

	stat_numIndexBuffers.inc();
	stat_indexBufferMemory.add(datasize);
}

void dx9UploadIndexBuffer(phandle_t h, int datasize, const void *p, IEventHandler *eventHandler)
{
	IDirect3DIndexBuffer9 *obj = h->castTo<IDirect3DIndexBuffer9 *>();

	DWORD flag = 0;
	D3DINDEXBUFFER_DESC desc;
	V(obj->GetDesc(&desc));
	if (desc.Usage & D3DUSAGE_DYNAMIC)
		flag = D3DLOCK_DISCARD;

	void *dst = 0;
	V(obj->Lock(0, datasize, &dst, flag));
	memcpy(dst, p, datasize);
	V(obj->Unlock());
}

void dx9DeleteIndexBuffer(phandle_t h)
{
	IDirect3DIndexBuffer9 *obj = h->castTo<IDirect3DIndexBuffer9 *>();
	SAFE_RELEASE(obj);
	delete h;
}

void dx9CreateWindowTarget(phandle_t h, Handle hwnd, int width, int height)
{
	DX9_Window *window = new DX9_Window(hwnd, width, height);
	*h = window;
}

void dx9UpdateWindowTarget(phandle_t h, Handle newHwnd, int width, int height)
{
	DX9_Window *window = h->castTo<DX9_Window *>();

	window->update(newHwnd, width, height);
}

void dx9DeleteWindowTarget(phandle_t h)
{
	DX9_Window *window = handle_cast<DX9_Window*>(*h);
	SAFE_RELEASE(window);
	delete h;
}

#if 0
void dx9CreateSamplerState( phandle_t h, const SamplerDesc &desc )
{
	*h = new DX9_SamplerState(desc);
}

void dx9DeleteSamplerState( phandle_t h )
{
	DX9_SamplerState *obj = handle_cast<DX9_SamplerState*>(*h);
	SAFE_RELEASE(obj);
}

void dx9CreateBlendState( phandle_t h, const BlendDesc &src )
{
	IDirect3DStateBlock9 *stateblock;
	dx9_device->BeginStateBlock();
	dx9_device->EndStateBlock(&stateblock);

	*h = stateblock;
}

void dx9DeleteBlendState( phandle_t h )
{
	IDirect3DStateBlock9 *stateblock = handle_cast<IDirect3DStateBlock9 *>(*h);
	SAFE_RELEASE(stateblock);
}

void dx9CreateDepthStencilState( phandle_t h, const DepthStencilDesc &src )
{
	IDirect3DStateBlock9 *stateblock;
	dx9_device->BeginStateBlock();
	dx9_device->EndStateBlock(&stateblock);

	*h = stateblock;
}

void dx9DeleteDepthStencilState(phandle_t h)
{
	IDirect3DStateBlock9 *stateblock = handle_cast<IDirect3DStateBlock9 *>(*h);
	SAFE_RELEASE(stateblock);
}

void dx9CreateRasterizerState(phandle_t h, const RasterizerDesc &src)
{
	IDirect3DStateBlock9 *stateblock;
	dx9_device->BeginStateBlock();
	dx9_device->EndStateBlock(&stateblock);

	*h = stateblock;
}

void dx9DeleteRasterizerState(phandle_t h)
{
	IDirect3DStateBlock9 *stateblock = handle_cast<IDirect3DStateBlock9 *>(*h);
	SAFE_RELEASE(stateblock);
}
#endif

inline static IDirect3DSurface9 *getSurface(phandle_t h)
{
	if (!h || !*h) return 0;

	IUnknown *unknown = h->castTo<IUnknown *>();

	if (!unknown)
		return 0;

	IDirect3DTexture9 *texture = 0;
	unknown->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&texture);

	IDirect3DSurface9 *surface = 0;
	if (texture) {
		texture->GetSurfaceLevel(0, &surface);
	} else {
		DX9_Window *window = h->castTo<DX9_Window *>();
		surface = window->getSurface();
	}

	return surface;
}

inline static void setRenderTargetSize(IDirect3DSurface9 *surface)
{
	AX_ASSERT(surface);
	D3DSURFACE_DESC desc;
	surface->GetDesc(&desc);
	s_curRenderTargetSize.set(desc.Width, desc.Height);
}

static void dx9SetTargetSet(phandle_t targetSet[RenderTargetSet::MaxTarget])
{
	AX_ASSURE(targetSet[0] || targetSet[1]);

	bool surfaceSizeIsSet = false;
	IDirect3DSurface9 *surface = getSurface(targetSet[0]);
	if (surface) {
		setRenderTargetSize(surface);
		surfaceSizeIsSet = true;
		V(dx9_device->SetDepthStencilSurface(surface));
		SAFE_RELEASE(surface);
	}

	for (int i = 0; i < RenderTargetSet::MaxColorTarget; i++) {
		surface = getSurface(targetSet[i+1]);
		V(dx9_device->SetRenderTarget(i, surface));
		if (surface && !surfaceSizeIsSet)
			setRenderTargetSize(surface);
		SAFE_RELEASE(surface);
	}

	// if no depth surface, we assign a default
	if (!targetSet[0]) {
		surface = dx9_driver->getDepthStencil(s_curRenderTargetSize);
		V(dx9_device->SetDepthStencilSurface(surface));
	}

	// if no render target in 0, we assign a null target, let dx9 happy
	if (!targetSet[1]) {
		surface = dx9_driver->getNullTarget(s_curRenderTargetSize);
		V(dx9_device->SetRenderTarget(0, surface))
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


static void dx9SetShader(const FixedString &name, const ShaderMacro &sm, Technique tech)
{
	s_curShader = dx9_shaderManager->findShader(name, sm);
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
		s_curParams1 = *params1;
	else
		s_curParams1.clear();

	if (params2)
		s_curParams2 = *params2;
	else
		s_curParams2.clear();
}

static void dx9SetVertices(phandle_t vb, VertexType vt, int offset)
{
	s_curVerticeBufferUP = 0;
	V(dx9_device->SetStreamSource(0, vb->castTo<IDirect3DVertexBuffer9 *>(), offset, vt.stride()));
	V(dx9_device->SetVertexDeclaration(dx9_vertexDeclarations[vt]));

	V(dx9_device->SetStreamSourceFreq(0, 1));
	V(dx9_device->SetStreamSourceFreq(1, 1));
}

static void dx9SetInstanceVertices(phandle_t vb, VertexType vt, int offset, phandle_t inb, int inoffset, int incount)
{
	s_curVerticeBufferUP = 0;
	V(dx9_device->SetStreamSource(0, vb->castTo<IDirect3DVertexBuffer9 *>(), offset, vt.stride()));
	V(dx9_device->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | incount));

	V(dx9_device->SetStreamSource(1, inb->castTo<IDirect3DVertexBuffer9 *>(), inoffset, incount));
	V(dx9_device->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul));
	V(dx9_device->SetVertexDeclaration(dx9_vertexDeclarationsInstanced[vt]));
}

static void dx9SetIndices(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount)
{
	s_curIndiceBufferUP = 0;
	V(dx9_device->SetIndices(ib->castTo<IDirect3DIndexBuffer9 *>()));
	s_curPrimitiveType = trElementType(et);
	s_curNumVertices = vertcount;
	s_curStartIndex = offset;
	s_curPrimitiveCount = sCalcNumElements(s_curPrimitiveType, indicescount);
}

static void dx9SetVerticesUP(const void *vb, VertexType vt, int vertcount)
{
	s_curVerticeBufferUP = vb;
	s_curVertexType = vt;
	s_curNumVertices = vertcount;
}

static void dx9SetIndicesUP(const void *ib, ElementType et, int indicescount)
{
	s_curIndiceBufferUP = ib;
	s_curPrimitiveType = trElementType(et);
	s_curPrimitiveCount = sCalcNumElements(s_curPrimitiveType, indicescount);
}

static void dx9SetGlobalTexture(GlobalTextureId id, phandle_t h, const SamplerDesc &desc)
{
	s_curGlobalTextures[id] = h;
	s_curGlobalTextureSamplerDescs[id] = desc;
}

static void dx9SetMaterialTexture(phandle_t texs[], SamplerDesc descs[])
{
	::memcpy(s_curMaterialTextures, texs, sizeof(phandle_t) * MaterialTextureId::MaxType);
	::memcpy(s_curMaterialTextureSamplerDescs, descs, sizeof(SamplerDesc) * MaterialTextureId::MaxType);
}

static void dx9SetRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd)
{
	if (dsd != s_curDepthStencilDesc) {
		s_curDepthStencilDesc = dsd;
		DX9_DepthStencilState::find(dsd)->apply();
	}

	if (rd != s_curRasterizerDesc) {
		s_curRasterizerDesc = rd;
		DX9_RasterizerState::find(rd)->apply();
	}

	if (bd != s_curBlendDesc) {
		s_curBlendDesc = bd;
		DX9_BlendState::find(bd)->apply();
	}
}


//	static vOid dip(ElementType et, int offset, int vertcount, int indices_count) = 0;
static void dx9Draw()
{
	UINT npass = s_curShader->begin(s_curTechnique);
	for (int i = 0; i < npass; i++) {
		s_curShader->beginPass(i);

		if (s_curIndiceBufferUP && s_curVerticeBufferUP) {
			V(dx9_device->DrawIndexedPrimitiveUP(s_curPrimitiveType, 0, s_curNumVertices, s_curPrimitiveCount, s_curIndiceBufferUP, D3DFMT_INDEX16, s_curVerticeBufferUP, s_curVertexType.stride()));
		} else if (!s_curVerticeBufferUP && !s_curIndiceBufferUP) {
			HRESULT hr = dx9_device->DrawIndexedPrimitive(s_curPrimitiveType, 0, 0, s_curNumVertices, s_curStartIndex, s_curPrimitiveCount);
			const char *err = D3DErrorString(hr);
		} else {
			AX_WRONGPLACE;
			Errorf("wrong");
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
	DX9_Window *dx9window = window->castTo<DX9_Window *>();
	dx9_device->EndScene();
	dx9window->present();
	dx9_device->BeginScene();
}

void dx9AssignRenderApi()
{
	RenderApi::createTexture2D = &dx9CreateTexture2D;
	RenderApi::uploadTexture = &dx9UploadTexture;
	RenderApi::uploadSubTexture = &dx9UploadSubTexture;
	RenderApi::generateMipmap = &dx9GenerateMipmap;
	RenderApi::deleteTexture2D = &dx9DeleteTexture2D;

	RenderApi::createVertexBuffer = &dx9CreateVertexBuffer;
	RenderApi::uploadVertexBuffer = &dx9UploadVertexBuffer;
	RenderApi::deleteVertexBuffer = &dx9DeleteVertexBuffer;

	RenderApi::createIndexBuffer = &dx9CreateIndexBuffer;
	RenderApi::uploadIndexBuffer = &dx9UploadIndexBuffer;
	RenderApi::deleteIndexBuffer = &dx9DeleteIndexBuffer;

	RenderApi::createWindowTarget = &dx9CreateWindowTarget;
	RenderApi::updateWindowTarget = &dx9UpdateWindowTarget;
	RenderApi::deleteWindowTarget = &dx9DeleteWindowTarget;
#if 0
	RenderApi::createSamplerState = &dx9CreateSamplerState;
	RenderApi::deleteSamplerState = &dx9DeleteSamplerState;

	RenderApi::createBlendState = &dx9CreateBlendState;
	RenderApi::deleteBlendState = &dx9DeleteBlendState;

	RenderApi::createDepthStencilState = &dx9CreateDepthStencilState;
	RenderApi::deleteDepthStencilState = &dx9DeleteDepthStencilState;

	RenderApi::createRasterizerState = &dx9CreateRasterizerState;
	RenderApi::deleteRasterizerState = &dx9DeleteRasterizerState;
#endif
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

	RenderApi::draw = &dx9Draw;

	RenderApi::clear = &dx9Clear;

	RenderApi::present = &dx9Present;
}

AX_END_NAMESPACE
