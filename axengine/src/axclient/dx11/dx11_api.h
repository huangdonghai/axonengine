#ifndef AX_DX11_API_H
#define AX_DX11_API_H

AX_BEGIN_NAMESPACE

struct DX11_Resource {
	enum Type {
		kTexture, kRenderTarget, kVertexBuffer, kIndexBuffer, kWindow, kOcclusionQuery
	};

	DX11_Resource(Type type, IUnknown *obj) : m_type(type), m_obj(obj), m_renderTargetData(0)
	{}

	~DX11_Resource()
	{
		SafeDelete(m_renderTargetData);
		SAFE_RELEASE(m_obj);
	}

	Type m_type;

	// resource
	union {
		IUnknown *m_obj;
		ID3D11Resource *m_dx11Resource;
		ID3D11Texture2D *m_texture2D;
		ID3D11Texture3D *m_texture3D;
		ID3D11Buffer *m_vertexBuffer;
		ID3D11Buffer *m_indexBuffer;
		ID3D11Query *m_query;
		DX11_Window *m_window;
	};

	struct DynamicTextureData {
		TexType texType;
		int width, height, depth;

		// render target view
		std::vector<ID3D11DepthStencilView *> m_depthStencilViews;
		std::vector<ID3D11RenderTargetView *> m_renderTargetViews;
	};

	DynamicTextureData *m_renderTargetData;

	// shader resource view
	ID3D11ShaderResourceView *m_shaderResourceView;
	bool m_isDynamic;
};

AX_END_NAMESPACE

#endif // AX_DX11_API_H
