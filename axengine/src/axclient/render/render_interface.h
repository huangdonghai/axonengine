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

class ApiWrapper
{
public:
	// new interface
	void createTexture2D(handle_t &result, TexFormat format, int width, int height, int flags = 0);
	void uploadTexture(handle_t h, int level, void *pixels, TexFormat format = TexFormat::AUTO);
	void uploadSubTexture(handle_t h, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	void generateMipmap(handle_t h);
	void deleteTexture2D(handle_t h);

	void createVertexBuffer(handle_t &result, int datasize, Primitive2::Hint hint);
	void *lockVertexBuffer(handle_t h);
	void unlockVertexBuffer(handle_t h);
	void deleteVertexBuffer(handle_t h);

	void createInstanceBuffer(handle_t &result, int datasize, Primitive2::Hint hint);
	void *lockInstanceBuffer(handle_t h);
	void unlockInstanceBuffer(handle_t h);
	void deleteInstanceBuffer(handle_t h);

	void createIndexBuffer(handle_t &result, int datasize, Primitive2::Hint hint);
	void *lockIndexBuffer(handle_t hib);
	void unlockIndexBuffer(handle_t hib);
	void deleteIndexBuffer(handle_t hib);

	void setShader(MaterialBackend *mtl, GeometryPB *prim);
	int setShader(handle_t shader, Technique tech);
	void setPass(int pass);

	void setVertices(handle_t vb, VertexType vt, int vertcount);
	void setInstanceVertices(handle_t vb, VertexType vt, int vertcount, handle_t inb, int incount);
	void setIndices(handle_t ib);

	//	virtual void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	void dipUp();

	//
	// high level
	//
	void drawPrimitive();

protected:
	void *allocHunk(int size);

private:
	class Command
	{
	public:
		int hunkMark;
	};

private:
	enum { HUNK_SIZE = 4 * 1024 * 1024, MAX_COMMANDS = 64 * 1024 };
	byte_t m_hunk[HUNK_SIZE];
	Command *m_cmds[MAX_COMMANDS];

	volatile int m_hunkPos;
	volatile int m_readPos, m_writePos;
};

AX_END_NAMESPACE

#endif // AX_RENDERINTERFACE_H
