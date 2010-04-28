#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

class MaterialBackend;
class GeometryPB;

typedef Handle * HandlePtr;
typedef const Handle * HandleCptr;

class RenderApi
{
public:
	// new interface
	static void (*createTexture2D)(HandlePtr h, TexFormat format, int width, int height, int flags);
	static void (*uploadTexture)(HandleCptr h, int level, void *pixels, TexFormat format);
	static void (*uploadSubTexture)(HandleCptr h, const Rect &rect, const void *pixels, TexFormat format);
	static void (*generateMipmap)(HandleCptr h);
	static void (*deleteTexture2D)(HandleCptr h);

	static void (*createVertexBuffer)(HandlePtr h, int datasize, Primitive2::Hint hint);
	static void (*uploadVertexBuffer)(HandleCptr h, int datasize, void *p);
	static void (*deleteVertexBuffer)(HandleCptr h);

	static void (*createIndexBuffer)(HandlePtr h, int datasize, Primitive2::Hint hint);
	static void (*uploadIndexBuffer)(HandleCptr h, int datasize, void *p);
	static void (*deleteIndexBuffer)(HandleCptr h);

	static void (*findShader)(HandlePtr h, MaterialBackend *mtl, GeometryPB *prim);
	static void (*setShader)(HandleCptr shader, Technique tech);
	static void (*setPass)(int pass);

	static void (*setVertices)(HandlePtr vb, VertexType vt, int vertcount);
	static void (*setInstanceVertices)(HandlePtr vb, VertexType vt, int vertcount, Handle inb, int incount);
	static void (*setIndices)(HandlePtr ib);

//	static void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	static void (*dipUp)();
};

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
	void createTexture2D(HandlePtr result, TexFormat format, int width, int height, int flags = 0);
	void uploadTexture(HandleCptr h, int level, void *pixels, TexFormat format);
	void uploadSubTexture(HandleCptr h, const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap(HandleCptr h);
	void deleteTexture2D(HandleCptr h);

	void createVertexBuffer(HandleCptr result, int datasize, Primitive2::Hint hint);
	void *lockVertexBuffer(HandleCptr h);
	void unlockVertexBuffer(HandleCptr h);
	void deleteVertexBuffer(HandleCptr h);

	void createIndexBuffer(HandlePtr result, int datasize, Primitive2::Hint hint);
	void *lockIndexBuffer(HandleCptr hib);
	void unlockIndexBuffer(HandleCptr hib);
	void deleteIndexBuffer(HandleCptr hib);

	void setShader(MaterialBackend *mtl, GeometryPB *prim);
	int setShader(Handle shader, Technique tech);
	void setPass(int pass);

	void setVertices(HandleCptr vb, VertexType vt, int vertcount);
	void setInstanceVertices(HandleCptr vb, VertexType vt, int vertcount, Handle inb, int incount);
	void setIndices(HandleCptr ib);

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
