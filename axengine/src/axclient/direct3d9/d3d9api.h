#ifndef AX_D3D9API_H
#define AX_D3D9API_H

AX_BEGIN_NAMESPACE

bool CheckIfSupportHardwareMipmapGeneration(D3DFORMAT d3dformat, DWORD d3dusage);

// module private

// render api
void dx9CreateTexture2D(phandle_t h, TexFormat format, int width, int height, int flags);
void dx9UploadTexture(phandle_t h, int level, const void *pixels, TexFormat format);
void dx9UploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
void dx9GenerateMipmap(phandle_t h);
void dx9DeleteTexture2D(phandle_t h);

void dx9CreateVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void dx9UploadVertexBuffer(phandle_t h, int datasize, const void *p);
void dx9DeleteVertexBuffer(phandle_t h);

void dx9CreateIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void dx9UploadIndexBuffer(phandle_t h, int datasize, const void *p);
void dx9DeleteIndexBuffer(phandle_t h);

AX_END_NAMESPACE

#endif // AX_D3D9API_H
