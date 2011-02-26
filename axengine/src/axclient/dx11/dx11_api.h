#ifndef AX_DX11_API_H
#define AX_DX11_API_H

AX_DX11_BEGIN_NAMESPACE

extern FastParams dx11_curParams1;
extern FastParams dx11_curParams2;
extern phandle_t dx11_curGlobalTextures[GlobalTextureId::MaxType];
extern SamplerDesc dx11_curGlobalTextureSamplerDescs[GlobalTextureId::MaxType];
extern FastTextureParams dx11_curMaterialTextures;
extern bool dx11_curInstanced;
extern VertexType dx11_curVertexType;

struct DX11_Resource {
	enum Type {
		kImmutableTexture, kDynamicTexture, kVertexBuffer, kIndexBuffer, kWindow, kOcclusionQuery
	};

	DX11_Resource(Type type, IUnknown *obj) : m_type(type), m_obj(obj), m_dynamicTextureData(0)
	{
		if (type == kDynamicTexture)
			m_dynamicTextureData = new DynamicTextureData();
	}

	~DX11_Resource()
	{
		SafeDelete(m_dynamicTextureData);
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
		TexFormat texFormat;
		int width, height, depth;

		// render target view
		std::vector<ID3D11DepthStencilView *> m_depthStencilViews;
		std::vector<ID3D11RenderTargetView *> m_renderTargetViews;
	};

	DynamicTextureData *m_dynamicTextureData;

	// shader resource view
	ID3D11ShaderResourceView *m_shaderResourceView;
	bool m_isDynamic;
};

void dx11InitApi();

AX_DX11_END_NAMESPACE

#endif // AX_DX11_API_H
