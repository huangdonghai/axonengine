#ifndef AX_DX9_API_H
#define AX_DX9_API_H

AX_BEGIN_NAMESPACE

struct DX9_Resource {
	enum Type {
		kRenderTarget, kVertexBuffer, kIndexBuffer, kWindow
	};

	Type type;
	IUnknown *obj;

	union {
		// for texture use
		struct {
			int width, height;
		};
		// for vertex and index buffer
		struct {
			bool isDynamic;
		};
	};
};

bool trTexFormat(TexFormat texformat, D3DFORMAT &d3dformat);

void dx9AssignRenderApi();
void dx9InitState();

AX_END_NAMESPACE

#endif // AX_D3D9API_H
