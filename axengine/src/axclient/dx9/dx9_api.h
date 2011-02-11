#ifndef AX_DX9_API_H
#define AX_DX9_API_H

AX_BEGIN_NAMESPACE

struct DX9_Resource {
	enum Type {
		kTexture, kRenderTarget, kVertexBuffer, kIndexBuffer, kWindow, kOcclusionQuery
	};

	DX9_Resource(Type type, IUnknown *obj) : m_type(type), m_obj(obj), m_level0(0)
	{}

	~DX9_Resource()
	{
		SAFE_RELEASE(m_level0);
		SAFE_RELEASE(m_obj);
	}

	Type m_type;

	union {
		IUnknown *m_obj;
		IDirect3DTexture9 *m_texture;
		IDirect3DVolumeTexture9 *m_texture3D;
		IDirect3DCubeTexture9 *m_textureCube;
		IDirect3DVertexBuffer9 *m_vertexBuffer;
		IDirect3DIndexBuffer9 *m_indexBuffer;
		IDirect3DQuery9 *m_query;
		DX9_Window *m_window;
	};

	IDirect3DSurface9 *getSliceSurface(int slice)
	{
		if (texType == TexType::_2D) {
			AX_ASSERT(slice == 0);
			m_level0->AddRef();
			return m_level0;
		} else if (texType == TexType::_3D) {
			// direct3d 9 can't render to a volume texture
			// but opengl and d3d10 can do this
			AX_WRONGPLACE;
		} else if (texType == TexType::CUBE) {
			AX_ASSERT(slice < CubeMapFace::MaxType);
			IDirect3DSurface9 *surface;
			V(m_textureCube->GetCubeMapSurface(D3DCUBEMAP_FACES(D3DCUBEMAP_FACE_POSITIVE_X+slice), 0, &surface));
			return surface;
		}
		AX_WRONGPLACE;
		return 0;
	}

	IDirect3DSurface9 *m_level0;
	TexType texType;
	int width, height, depth;
	bool m_isDynamic;
};

template <class Q>
Q CastDX9(phandle_t h)
{
	DX9_Resource *resource = h->castTo<DX9_Resource *>();
}

bool trTexFormat(TexFormat texformat, D3DFORMAT &d3dformat);

void dx9AssignRenderApi();
void dx9InitState();

AX_END_NAMESPACE

#endif // AX_D3D9API_H
