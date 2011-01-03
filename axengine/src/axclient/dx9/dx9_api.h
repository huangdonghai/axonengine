#ifndef AX_D3D9API_H
#define AX_D3D9API_H

AX_BEGIN_NAMESPACE

bool trTexFormat(TexFormat texformat, D3DFORMAT &d3dformat);

// module private

// render api

#if 0
void dx9CreateTextureFromFileInMemory(phandle_t h, AsioRequest *asioRequest);
void dx9CreateTexture2D(phandle_t h, TexFormat format, int width, int height, int flags);
void dx9UploadTexture(phandle_t h, const void *pixels, TexFormat format, IEventHandler *eventHandler);
void dx9UploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format, IEventHandler *eventHandler);
void dx9GenerateMipmap(phandle_t h);
void dx9DeleteTexture2D(phandle_t h);

void dx9CreateVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void dx9UploadVertexBuffer(phandle_t h, int datasize, const void *p, IEventHandler *eventHandler);
void dx9DeleteVertexBuffer(phandle_t h);

void dx9CreateIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void dx9UploadIndexBuffer(phandle_t h, int datasize, const void *p, IEventHandler *eventHandler);
void dx9DeleteIndexBuffer(phandle_t h);

void dx9CreateWindowTarget(phandle_t h, Handle hwnd, int width, int height);
void dx9UpdateWindowTarget(phandle_t h, Handle newHwnd, int width, int height);
void dx9DeleteWindowTarget(phandle_t h);

void dx9CreateSamplerState(phandle_t h, const SamplerDesc &samplerState);
void dx9DeleteSamplerState(phandle_t h);

void dx9CreateBlendState(phandle_t h, const BlendDesc &src);
void dx9DeleteBlendState(phandle_t h);

void dx9CreateDepthStencilState(phandle_t h, const DepthStencilDesc &src);
void dx9DeleteDepthStencilState(phandle_t h);

void dx9CreateRasterizerState(phandle_t h, const RasterizerDesc &src);
void dx9DeleteRasterizerState(phandle_t h);
#endif

void dx9AssignRenderApi();

AX_END_NAMESPACE

#endif // AX_D3D9API_H
