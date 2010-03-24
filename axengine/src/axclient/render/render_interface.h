#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

class MaterialBackend;
class GeometryPB;

class RenderApi
{
public:
	// new interface
	virtual handle_t createTexture2D(TexFormat format, int width, int height, int flags = 0) = 0;
	virtual void uploadTexture(handle_t htex, int level, void *pixels, TexFormat format = TexFormat::AUTO) = 0;
	virtual void uploadSubTexture(handle_t htex, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO) = 0;
	virtual void generateMipmap(handle_t htex) = 0;
	virtual void deleteTexture2D(handle_t htex) = 0;

	virtual handle_t createVertexBuffer(int datasize, Primitive2::Hint hint) = 0;
	virtual void *lockVertexBuffer(handle_t hvb) = 0;
	virtual void unlockVertexBuffer(handle_t hvb) = 0;
	virtual void deleteVertexBuffer(handle_t hvb) = 0;

	virtual handle_t createInstanceBuffer(int datasize, Primitive2::Hint hint) = 0;
	virtual void *lockInstanceBuffer(handle_t hbuf) = 0;
	virtual void unlockInstanceBuffer(handle_t hbuf) = 0;
	virtual void deleteInstanceBuffer(handle_t hbuf) = 0;

	virtual handle_t createIndexBuffer(int datasize, Primitive2::Hint hint) = 0;
	virtual void *lockIndexBuffer(handle_t hib) = 0;
	virtual void unlockIndexBuffer(handle_t hib) = 0;
	virtual void deleteIndexBuffer(handle_t hib) = 0;

	virtual handle_t findShader(MaterialBackend *mtl, GeometryPB *prim) = 0;
	virtual int setShader(handle_t shader, Technique tech) = 0;
	virtual void setPass(int pass) = 0;

	virtual void setVertices(handle_t vb, VertexType vt, int vertcount) = 0;
	virtual void setInstanceVertices(handle_t vb, VertexType vt, int vertcount, handle_t inb, int incount) = 0;
	virtual void setIndices(handle_t ib) = 0;

//	virtual void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	virtual void dipUp() = 0;
};

AX_END_NAMESPACE

#endif // AX_RENDERINTERFACE_H
