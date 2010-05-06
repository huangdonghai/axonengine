#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

typedef Handle *phandle_t;

/*
class WrapHandle {
public:
	WrapHandle() : m_realHandle(0) {}
	WrapHandle(const Handle &h) : m_realHandle(&h) {}
	WrapHandle(const WrapHandle &rhs) { m_realHandle = rhs.m_realHandle; }
	~WrapHandle() {}

private:
	Handle *m_realHandle;
};
*/

class RenderApi
{
public:
	// new interface
	static void (*createTexture2D)(phandle_t h, TexFormat format, int width, int height, int flags);
	static void (*uploadTexture)(phandle_t h, int level, void *pixels, TexFormat format);
	static void (*uploadSubTexture)(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
	static void (*generateMipmap)(phandle_t h);
	static void (*deleteTexture2D)(phandle_t h);

	static void (*createVertexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
	static void (*uploadVertexBuffer)(phandle_t h, int datasize, void *p);
	static void (*deleteVertexBuffer)(phandle_t h);

	static void (*createIndexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
	static void (*uploadIndexBuffer)(phandle_t h, int datasize, void *p);
	static void (*deleteIndexBuffer)(phandle_t h);

	static void (*findShader)(phandle_t h, const FixedString & name, const ShaderMacro &sm);
	static void (*setShader)(phandle_t shader, Technique tech);
	static void (*setVsConst)(const FixedString &name, int count, float *value);
	static void (*setPsConst)(const FixedString &name, int count, float *value);
	static void (*setPass)(int pass);

	static void (*setVertices)(phandle_t vb, VertexType vt, int vertcount);
	static void (*setInstanceVertices)(phandle_t vb, VertexType vt, int vertcount, Handle inb, int incount);
	static void (*setIndices)(phandle_t ib);

//	static void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	static void (*dipUp)();
};

class ApiWrap
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
	void createTexture2D(phandle_t result, TexFormat format, int width, int height, int flags = 0);
	void uploadTexture(phandle_t h, int level, void *pixels, TexFormat format);
	void uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap(phandle_t h);
	void deleteTexture2D(phandle_t h);

	void createVertexBuffer(phandle_t result, int datasize, Primitive::Hint hint);
	void *lockVertexBuffer(phandle_t h);
	void unlockVertexBuffer(phandle_t h);
	void deleteVertexBuffer(phandle_t h);

	void createIndexBuffer(phandle_t result, int datasize, Primitive::Hint hint);
	void *lockIndexBuffer(phandle_t hib);
	void unlockIndexBuffer(phandle_t hib);
	void deleteIndexBuffer(phandle_t hib);

	int setShader(Handle shader, Technique tech);
	void setPass(int pass);

	void setVertices(phandle_t vb, VertexType vt, int vertcount);
	void setInstanceVertices(phandle_t vb, VertexType vt, int vertcount, Handle inb, int incount);
	void setIndices(phandle_t ib);

	//	virtual void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	void dipUp();

	//
	// high level
	//
	void drawPrimitive();

protected:
	void *allocHunk(int size);

private:
	enum { HUNK_SIZE = 4 * 1024 * 1024, MAX_COMMANDS = 64 * 1024 };
	byte_t m_hunk[HUNK_SIZE];
	Command *m_cmds[MAX_COMMANDS];

	volatile int m_hunkPos;
	volatile int m_readPos, m_writePos;
};

AX_END_NAMESPACE

#endif // AX_RENDERINTERFACE_H
