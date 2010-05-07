#include "d3d9private.h"

AX_BEGIN_NAMESPACE

void createTexture2D(phandle_t h, TexFormat format, int width, int height, int flags)
{
	D3DFORMAT d3dformat = D3DFMT_UNKNOWN;
	
	trTexFormat(format, d3dformat);

	if (d3dformat == D3DFMT_UNKNOWN) {
		Errorf("don't support format");
	}

	D3DPOOL d3dpool = D3DPOOL_MANAGED;
	DWORD d3dusage = 0;
}

void uploadTexture(phandle_t h, int level, void *pixels, TexFormat format)
{

}

void uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format)
{

}

void generateMipmap(phandle_t h)
{

}

void deleteTexture2D(phandle_t h)
{

}

void createVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{

}

void uploadVertexBuffer(phandle_t h, int datasize, void *p)
{

}

void deleteVertexBuffer(phandle_t h)
{

}

void createIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint)
{

}

void uploadIndexBuffer(phandle_t h, int datasize, void *p)
{

}

void deleteIndexBuffer(phandle_t h)
{

}

AX_END_NAMESPACE
