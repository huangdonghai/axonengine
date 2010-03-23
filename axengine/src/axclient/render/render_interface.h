#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

class RenderApi
{
public:
	// new interface
	virtual handle_t createTexture2D(TexFormat format, int width, int height, int flags = 0) = 0;
	virtual void uploadTexture(handle_t htex, int level, void *pixels, TexFormat format = TexFormat::AUTO) = 0;
	virtual void uploadSubTexture(handle_t htex, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO) = 0;
	virtual void generateMipmap(handle_t htex) = 0;
	virtual void deleteTexture2D(handle_t htex) = 0;

	virtual handle_t createVertexBuffer(size_t datasize, Primitive2::Hint hint) = 0;
	virtual void *lockVertexBuffer(handle_t hvb) = 0;
	virtual void unlockVertexBuffer(handle_t hvb) = 0;
	virtual void deleteVertexBuffer(handle_t hvb) = 0;
	virtual handle_t createIndexBuffer(size_t datasize, Primitive2::Hint hint) = 0;
	virtual void *lockIndexBuffer(handle_t hib) = 0;
	virtual void unlockIndexBuffer(handle_t hib) = 0;
	virtual void deleteIndexBuffer(handle_t hib) = 0;

	virtual handle_t createShader(const String &name) = 0;
	//	virtual int setCurrentTechnique(handle_t shader, Technique tech, MaterialBr *mtr) = 0;
	virtual void setCurrentPass(int pass) = 0;

	virtual void drawIndexedPrimitive() = 0;
	virtual void drawIndexedPrimitiveUP() = 0;
};

AX_END_NAMESPACE

#endif // AX_RENDERINTERFACE_H
