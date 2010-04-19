#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

class MaterialBackend;
class GeometryPB;

class RenderApi
{
public:
	// new interface
	static Handle (*createTexture2D)(TexFormat format, int width, int height, int flags);
	static void (*uploadTexture)(Handle htex, int level, void *pixels, TexFormat format);
	static void (*uploadSubTexture)(Handle htex, const Rect &rect, const void *pixels, TexFormat format);
	static void (*generateMipmap)(Handle htex);
	static void (*deleteTexture2D)(Handle htex);

	static Handle (*createVertexBuffer)(int datasize, Primitive2::Hint hint);
	static void *(*lockVertexBuffer)(Handle hvb);
	static void (*unlockVertexBuffer)(Handle hvb);
	static void (*deleteVertexBuffer)(Handle hvb);

	static Handle (*createInstanceBuffer)(int datasize, Primitive2::Hint hint);
	static void *(*lockInstanceBuffer)(Handle hbuf);
	static void (*unlockInstanceBuffer)(Handle hbuf);
	static void (*deleteInstanceBuffer)(Handle hbuf);

	static Handle (*createIndexBuffer)(int datasize, Primitive2::Hint hint);
	static void *(*lockIndexBuffer)(Handle hib);
	static void (*unlockIndexBuffer)(Handle hib);
	static void (*deleteIndexBuffer)(Handle hib);

	static Handle (*findShader)(MaterialBackend *mtl, GeometryPB *prim);
	static int (*setShader)(Handle shader, Technique tech);
	static void (*setPass)(int pass);

	static void (*setVertices)(Handle vb, VertexType vt, int vertcount);
	static void (*setInstanceVertices)(Handle vb, VertexType vt, int vertcount, Handle inb, int incount);
	static void (*setIndices)(Handle ib);

//	static void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	static void (*dipUp)();
};

typedef Handle * hptr_t;
typedef const Handle * hcptr_t;
class ApiWrapper
{
public:
	class Command
	{
	public:
		Command();
		virtual ~Command() {}
		virtual void exec() = 0;
		int hunkMark;
	};

	// new interface
	void createTexture2D(hptr_t result, TexFormat format, int width, int height, int flags = 0);
	void uploadTexture(hcptr_t h, int level, void *pixels, TexFormat format = TexFormat::AUTO);
	void uploadSubTexture(hcptr_t h, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	void generateMipmap(hcptr_t h);
	void deleteTexture2D(hcptr_t h);

	void createVertexBuffer(hcptr_t result, int datasize, Primitive2::Hint hint);
	void *lockVertexBuffer(hcptr_t h);
	void unlockVertexBuffer(hcptr_t h);
	void deleteVertexBuffer(hcptr_t h);

	void createInstanceBuffer(hptr_t result, int datasize, Primitive2::Hint hint);
	void *lockInstanceBuffer(hcptr_t h);
	void unlockInstanceBuffer(hcptr_t h);
	void deleteInstanceBuffer(hcptr_t h);

	void createIndexBuffer(hptr_t result, int datasize, Primitive2::Hint hint);
	void *lockIndexBuffer(hcptr_t hib);
	void unlockIndexBuffer(hcptr_t hib);
	void deleteIndexBuffer(hcptr_t hib);

	void setShader(MaterialBackend *mtl, GeometryPB *prim);
	int setShader(Handle shader, Technique tech);
	void setPass(int pass);

	void setVertices(hcptr_t vb, VertexType vt, int vertcount);
	void setInstanceVertices(hcptr_t vb, VertexType vt, int vertcount, Handle inb, int incount);
	void setIndices(hcptr_t ib);

	//	virtual void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	void dipUp();

	//
	// high level
	//
	void drawPrimitive();

protected:


protected:
	void *allocHunk(int size);

private:
private:
	enum { HUNK_SIZE = 4 * 1024 * 1024, MAX_COMMANDS = 64 * 1024 };
	byte_t m_hunk[HUNK_SIZE];
	Command *m_cmds[MAX_COMMANDS];

	volatile int m_hunkPos;
	volatile int m_readPos, m_writePos;
};

AX_END_NAMESPACE

#endif // AX_RENDERINTERFACE_H
