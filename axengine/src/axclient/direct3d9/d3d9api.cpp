#include "d3d9private.h"

AX_BEGIN_NAMESPACE

bool CheckIfSupportHardwareMipmapGeneration(D3DFORMAT d3dformat, DWORD d3dusage)
{
	if (d3d9Api->CheckDeviceFormat(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		D3DFMT_X8R8G8B8,
		d3dusage | D3DUSAGE_AUTOGENMIPMAP,
		D3DRTYPE_TEXTURE,
		d3dformat) == S_OK)
	{
		return true;
	}

	return false;
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

	if (flags & Texture::IF_RenderTarget) {
		d3dpool = D3DPOOL_DEFAULT;
		if (format.isDepth()) {
			d3dusage = D3DUSAGE_DEPTHSTENCIL;
		} else {
			d3dusage = D3DUSAGE_RENDERTARGET;
		}
	}

	if (flags & Texture::IF_AutoGenMipmap) {
		d3dusage |= D3DUSAGE_AUTOGENMIPMAP;

		bool m_hardwareGenMipmap = CheckIfSupportHardwareMipmapGeneration(d3dformat, d3dusage);

		if (!m_hardwareGenMipmap) {
			d3dusage &= ~D3DUSAGE_AUTOGENMIPMAP;
		}
	}

	int m_videoMemoryUsed = format.calculateDataSize(width, height);

	V(d3d9Device->CreateTexture(width, height, 1, d3dusage, d3dformat, d3dpool, (LPDIRECT3DTEXTURE9*)h, 0));

	stat_textureMemory.add(m_videoMemoryUsed);
}

void dx9UploadTexture(phandle_t h, int level, void *pixels, TexFormat format)
{
	LPDIRECT3DTEXTURE9 obj = h->to<LPDIRECT3DTEXTURE9>();
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

void dx9UploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format)
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
	LPDIRECT3DTEXTURE9 obj = h->to<LPDIRECT3DTEXTURE9>();
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

void dx9GenerateMipmap(phandle_t h)
{
	LPDIRECT3DTEXTURE9 obj = h->to<LPDIRECT3DTEXTURE9>();
	LPDIRECT3DSURFACE9 surface;

	V(obj->GetSurfaceLevel(0, &surface));
	D3DSURFACE_DESC surfdesc;
	obj->GetLevelDesc(0, &surfdesc);
	bool hardwaremipmap = CheckIfSupportHardwareMipmapGeneration(surfdesc.Format, surfdesc.Usage);

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
	dx9CreateTexture2D(&dum, TexFormat::BGRA8, surfdesc.Width, surfdesc.Height, Texture::IF_AutoGenMipmap);
	dx9UploadTexture(&dum, 0, lockedRect.pBits, surfdesc.Format);

	LPDIRECT3DTEXTURE9 dummyobj = dum.to<LPDIRECT3DTEXTURE9>();

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
		dx9UploadTexture(h, i, image.getData(i), TexFormat::BGR8);

		width >>= 1;
		height >>= 1;
		if (width < 1) width = 1;
		if (height < 1) height = 1;
	}

	dx9DeleteTexture2D(&dum);

}

void dx9DeleteTexture2D(phandle_t h)
{
	LPDIRECT3DTEXTURE9 obj = h->to<LPDIRECT3DTEXTURE9>();

	SAFE_RELEASE(obj);
}

void dx9CreateVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{
	DWORD d3dusage = D3DUSAGE_WRITEONLY;
	D3DPOOL d3dpool = D3DPOOL_DEFAULT;

	if (hint != Primitive::HintStatic) {
		d3dusage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	}

	V(d3d9Device->CreateVertexBuffer(datasize, d3dusage, 0, d3dpool, (IDirect3DVertexBuffer9 **)h, 0));

	stat_numVertexBuffers.inc();
	stat_vertexBufferMemory.add(datasize);
}

void dx9UploadVertexBuffer(phandle_t h, int datasize, void *p)
{
	IDirect3DVertexBuffer9 *obj = h->to<IDirect3DVertexBuffer9 *>();

	void *dst = 0;
	V(obj->Lock(0, datasize, &dst, D3DLOCK_DISCARD));
	memcpy(dst, p, datasize);
	V(obj->Unlock());
}

void dx9DeleteVertexBuffer(phandle_t h)
{
	IDirect3DVertexBuffer9 *obj = h->to<IDirect3DVertexBuffer9 *>();

	SAFE_RELEASE(obj);
}

void dx9CreateIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{
	DWORD d3dusage = D3DUSAGE_WRITEONLY;
	D3DPOOL d3dpool = D3DPOOL_DEFAULT;

	if (hint != Primitive::HintStatic) {
		d3dusage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	}

	V(d3d9Device->CreateIndexBuffer(datasize, d3dusage, D3DFMT_INDEX16, d3dpool, (IDirect3DIndexBuffer9 **)h, 0));

	stat_numIndexBuffers.inc();
	stat_indexBufferMemory.add(datasize);
}

void dx9UploadIndexBuffer(phandle_t h, int datasize, void *p)
{
	IDirect3DIndexBuffer9 *obj = h->to<IDirect3DIndexBuffer9 *>();

	void *dst = 0;
	V(obj->Lock(0, datasize, &dst, D3DLOCK_DISCARD));
	memcpy(dst, p, datasize);
	V(obj->Unlock());
}

void dx9DeleteIndexBuffer(phandle_t h)
{
	IDirect3DIndexBuffer9 *obj = h->to<IDirect3DIndexBuffer9 *>();

	SAFE_RELEASE(obj);
}

AX_END_NAMESPACE
