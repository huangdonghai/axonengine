#include "dx11_private.h"

AX_BEGIN_NAMESPACE

void dx11CreateTextureFromFileInMemory(phandle_t h, IoRequest *asioRequest)
{
	ID3D11Resource *texture;
	V(D3DX11CreateTextureFromMemory(dx11_device, asioRequest->fileData(), asioRequest->fileSize(), 0, 0, &texture, 0));
	DX11_Resource *resource = new DX11_Resource(DX11_Resource::kTexture, texture);
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

		apiResource = new DX11_Resource(DX11_Resource::kRenderTarget, texture2D);
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
		apiResource = new DX11_Resource(DX11_Resource::kRenderTarget, texture3D);
	}

	// TODO
}

void dx11UploadTexture(phandle_t h, const void *pixels, TexFormat format)
{

}

void dx11UploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
void dx11GenerateMipmap(phandle_t h);
void dx11DeleteTexture(phandle_t h);

void dx11CreateVertexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void dx11UploadVertexBuffer(phandle_t h, int datasize, const void *p);
void dx11DeleteVertexBuffer(phandle_t h);

void dx11CreateIndexBuffer(phandle_t h, int datasize, Primitive::Hint hint);
void dx11UploadIndexBuffer(phandle_t h, int datasize, const void *p);
void dx11DeleteIndexBuffer(phandle_t h);

void dx11CreateWindowTarget(phandle_t h, Handle hwnd, int width, int height);
void dx11UpdateWindowTarget(phandle_t h, Handle newHwnd, int width, int height);
void dx11DeleteWindowTarget(phandle_t h);

void dx11CreateQuery(phandle_t &h);
void dx11IssueQueries(int n, Query *queries[]);
void dx11DeleteQuery(phandle_t h);

void dx11BeginPerfEvent(const char *pixname);
void dx11EndPerfEvent();

void dx11SetTargetSet(phandle_t targetSet[RenderTargetSet::MaxTarget], int slices[RenderTargetSet::MaxTarget]);

void dx11SetViewport(const Rect &rect, const Vector2 & depthRange);
void dx11SetScissorRect(const Rect &scissorRect);

void dx11SetShader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm, Technique tech);
void dx11SetConstBuffer(ConstBuffers::Type type, int size, const void *data);
void dx11SetParameters(const FastParams *params1, const FastParams *params2);

void dx11SetVertices(phandle_t vb, VertexType vt, int offset);
void dx11SetInstanceVertices(phandle_t vb, VertexType vt, int offset, phandle_t inb, int inoffset, int incount);
void dx11SetIndices(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount);

void dx11SetVerticesUP(const void *vb, VertexType vt, int vertcount);
void dx11SetIndicesUP(const void *ib, ElementType et, int indicescount);

void dx11SetGlobalTexture(GlobalTextureId id, phandle_t h, const SamplerDesc &samplerState);
void dx11SetMaterialTexture(const FastTextureParams *textures);

void dx11SetRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd);

void dx11Draw();
void dx11Clear(const RenderClearer &clearer);

void dx11Present(phandle_t window);

AX_END_NAMESPACE
