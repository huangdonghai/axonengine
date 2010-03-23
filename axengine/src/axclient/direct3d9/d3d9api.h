#ifndef AX_D3D9API_H
#define AX_D3D9API_H

AX_BEGIN_NAMESPACE

class D3D9_Api : public RenderApi
{
public:
	virtual handle_t createTexture2D(TexFormat format, int width, int height, int flags = 0);
	virtual void uploadTexture(handle_t htex, int level, void *pixels, TexFormat format = TexFormat::AUTO);
	virtual void uploadSubTexture(handle_t htex, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	virtual void generateMipmap(handle_t htex);
	virtual void deleteTexture2D(handle_t htex);

	virtual handle_t createVertexBuffer(size_t datasize, Primitive2::Hint hint);
	virtual void *lockVertexBuffer(handle_t hvb);
	virtual void unlockVertexBuffer(handle_t hvb);
	virtual void deleteVertexBuffer(handle_t hvb);

	virtual handle_t createIndexBuffer(size_t datasize, Primitive2::Hint hint);
	virtual void *lockIndexBuffer(handle_t hib);
	virtual void unlockIndexBuffer(handle_t hib);
	virtual void deleteIndexBuffer(handle_t hib);

	virtual handle_t createShader(const String &name);
	//	virtual int setCurrentTechnique(handle_t shader, Technique tech, MaterialBr *mtr);
	virtual void setCurrentPass(int pass);

	virtual void drawIndexedPrimitive();
	virtual void drawIndexedPrimitiveUP();
};

AX_END_NAMESPACE

#endif // AX_D3D9API_H
