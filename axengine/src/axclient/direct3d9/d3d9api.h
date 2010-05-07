#ifndef AX_D3D9API_H
#define AX_D3D9API_H

AX_BEGIN_NAMESPACE

void createTexture2D(phandle_t h, TexFormat format, int width, int height, int flags);
void uploadTexture(phandle_t h, int level, void *pixels, TexFormat format);
void uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
void generateMipmap(phandle_t h);
void deleteTexture2D(phandle_t h);

void createVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void uploadVertexBuffer(phandle_t h, int datasize, void *p);
void deleteVertexBuffer(phandle_t h);

void createIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void uploadIndexBuffer(phandle_t h, int datasize, void *p);
void deleteIndexBuffer(phandle_t h);

AX_END_NAMESPACE

#endif // AX_D3D9API_H
