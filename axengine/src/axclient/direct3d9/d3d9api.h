#ifndef AX_D3D9API_H
#define AX_D3D9API_H

AX_BEGIN_NAMESPACE

class D3D9_Api : public RenderApi
{
public:
	virtual Handle createTexture2D(TexFormat format, int width, int height, int flags = 0);
	virtual void uploadTexture(Handle htex, int level, void *pixels, TexFormat format = TexFormat::AUTO);
	virtual void uploadSubTexture(Handle htex, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	virtual void generateMipmap(Handle htex);
	virtual void deleteTexture2D(Handle htex);

	virtual Handle createVertexBuffer(int datasize, Primitive2::Hint hint);
	virtual void *lockVertexBuffer(Handle hvb);
	virtual void unlockVertexBuffer(Handle hvb);
	virtual void deleteVertexBuffer(Handle hvb);

	virtual Handle createInstanceBuffer(int datasize, Primitive2::Hint hint);
	virtual void *lockInstanceBuffer(Handle hbuf);
	virtual void unlockInstanceBuffer(Handle hbuf);
	virtual void deleteInstanceBuffer(Handle hbuf);

	virtual Handle createIndexBuffer(int datasize, Primitive2::Hint hint);
	virtual void *lockIndexBuffer(Handle hib);
	virtual void unlockIndexBuffer(Handle hib);
	virtual void deleteIndexBuffer(Handle hib);

	virtual Handle findShader(MaterialBackend *mtl, GeometryPB *prim);
	virtual int setCurrentTechnique(Handle shader, Technique tech);
	virtual void setCurrentPass(int pass);

	virtual void drawIndexedPrimitive();
	virtual void drawIndexedPrimitiveUP();
};

AX_END_NAMESPACE

#endif // AX_D3D9API_H
