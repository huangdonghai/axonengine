#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

class MaterialBackend;
class GeometryPB;

class RenderApi
{
public:
	// new interface
	virtual Handle createTexture2D(TexFormat format, int width, int height, int flags = 0) = 0;
	virtual void uploadTexture(Handle htex, int level, void *pixels, TexFormat format = TexFormat::AUTO) = 0;
	virtual void uploadSubTexture(Handle htex, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO) = 0;
	virtual void generateMipmap(Handle htex) = 0;
	virtual void deleteTexture2D(Handle htex) = 0;

	virtual Handle createVertexBuffer(int datasize, Primitive2::Hint hint) = 0;
	virtual void *lockVertexBuffer(Handle hvb) = 0;
	virtual void unlockVertexBuffer(Handle hvb) = 0;
	virtual void deleteVertexBuffer(Handle hvb) = 0;

	virtual Handle createInstanceBuffer(int datasize, Primitive2::Hint hint) = 0;
	virtual void *lockInstanceBuffer(Handle hbuf) = 0;
	virtual void unlockInstanceBuffer(Handle hbuf) = 0;
	virtual void deleteInstanceBuffer(Handle hbuf) = 0;

	virtual Handle createIndexBuffer(int datasize, Primitive2::Hint hint) = 0;
	virtual void *lockIndexBuffer(Handle hib) = 0;
	virtual void unlockIndexBuffer(Handle hib) = 0;
	virtual void deleteIndexBuffer(Handle hib) = 0;

	virtual Handle findShader(MaterialBackend *mtl, GeometryPB *prim) = 0;
	virtual int setShader(Handle shader, Technique tech) = 0;
	virtual void setPass(int pass) = 0;

	virtual void setVertices(Handle vb, VertexType vt, int vertcount) = 0;
	virtual void setInstanceVertices(Handle vb, VertexType vt, int vertcount, Handle inb, int incount) = 0;
	virtual void setIndices(Handle ib) = 0;

//	virtual void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	virtual void dipUp() = 0;
};

typedef Handle * hptr_t;
typedef const Handle * hcptr_t;
class ApiWrapper
{
public:
	// new interface
	void createTexture2D(hptr_t result, TexFormat format, int width, int height, int flags = 0);
	void uploadTexture(hcptr_t h, int level, void *pixels, TexFormat format = TexFormat::AUTO);
	void uploadSubTexture(hcptr_t h, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	void generateMipmap(hcptr_t h);
	void deleteTexture2D(hcptr_t h);

	void createVertexBuffer(hcptr_t &result, int datasize, Primitive2::Hint hint);
	void *lockVertexBuffer(hcptr_t h);
	void unlockVertexBuffer(hcptr_t h);
	void deleteVertexBuffer(hcptr_t h);

	void createInstanceBuffer(Handle &result, int datasize, Primitive2::Hint hint);
	void *lockInstanceBuffer(hcptr_t h);
	void unlockInstanceBuffer(hcptr_t h);
	void deleteInstanceBuffer(hcptr_t h);

	void createIndexBuffer(Handle &result, int datasize, Primitive2::Hint hint);
	void *lockIndexBuffer(hcptr_t hib);
	void unlockIndexBuffer(hcptr_t hib);
	void deleteIndexBuffer(hcptr_t hib);

	void setShader(MaterialBackend *mtl, GeometryPB *prim);
	int setShader(Handle shader, Technique tech);
	void setPass(int pass);

	void setVertices(Handle vb, VertexType vt, int vertcount);
	void setInstanceVertices(Handle vb, VertexType vt, int vertcount, Handle inb, int incount);
	void setIndices(Handle ib);

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
