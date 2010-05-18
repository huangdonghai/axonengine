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

	static void (*setShader)(const FixedString & name, const ShaderMacro &sm, Technique tech);
	static void (*setVsConst)(const FixedString &name, int count, float *value);
	static void (*setPsConst)(const FixedString &name, int count, float *value);

	static void (*setVertices)(phandle_t vb, VertexType vt, int vertcount);
	static void (*setInstanceVertices)(phandle_t vb, VertexType vt, int vertcount, Handle inb, int incount);
	static void (*setIndices)(phandle_t ib);

//	static void dip(ElementType et, int offset, int vertcount, int indices_count) = 0;
	static void (*dipUp)();
};

struct RenderClearer {
	Rgba color;
	float depth;
	int stencil;
	bool isClearColor : 1;
	bool isClearDepth : 1;
	bool isClearStencil : 1;

	RenderClearer() : color(Rgba::Black), depth(1.0f), stencil(0), isClearColor(false), isClearDepth(false), isClearStencil(false) {}

	void clearDepth(bool enable, float ref = 1.0f) {
		isClearDepth = enable;
		depth = ref;
	}

	void clearColor(bool enable, Rgba ref = Rgba::Zero) {
		isClearColor = enable;
		color = ref;
	}

	void clearStencil(bool enable, int ref) {
		isClearStencil = enable;
		stencil = ref;
	}

	void doClear() const;
};


class ApiWrap
{
public:
	class Command
	{
	public:
		Command() {}
		virtual ~Command() {}
		virtual void exec() = 0;
		int hunkMark;
	};

	// api wrapper interface
	void createTexture2D(phandle_t result, TexFormat format, int width, int height, int flags = 0);
	void uploadTexture(phandle_t h, int level, void *pixels, TexFormat format);
	void uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap(phandle_t h);
	void deleteTexture2D(phandle_t h);

	void createVertexBuffer(phandle_t result, int datasize, Primitive::Hint hint);
	void uploadVertexBuffer(phandle_t h, int datasize, void *p);
	void deleteVertexBuffer(phandle_t h);

	void createIndexBuffer(phandle_t result, int datasize, Primitive::Hint hint);
	void uploadIndexBuffer(phandle_t h, int datasize, void *p);
	void deleteIndexBuffer(phandle_t h);

	int setShader(Handle shader, Technique tech);

	void setVertices(phandle_t vb, VertexType vt, int vertcount);
	void setInstanceVertices(phandle_t vb, VertexType vt, int vertcount, Handle inb, int incount);
	void setIndices(phandle_t ib);

	//	virtual void dip(ElementType et, int offset, int vertcount, int indicescount) = 0;
	void dipUp();

	//
	// high level
	//
	void drawPrimitive();

	void issueQueue(RenderQueue *rq);

protected:
	void beginFrame();
	void drawScene(QueuedScene *scene, const RenderClearer &clearer);
	void setupScene(QueuedScene *scene, const RenderClearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
	void unsetScene(QueuedScene *scene, const RenderClearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
	void drawPrimitive(int prim_id);
	void drawInteraction(Interaction *ia);
	void endFrame();

	void drawGlobalLight(QueuedScene *scene, QueuedLight *light);
	void drawLocalLight(QueuedScene *scene, QueuedLight *light);

	void drawPass_zfill(QueuedScene *scene);
	void drawPass_overlay(QueuedScene *scene);
	void drawPass_composite(QueuedScene *scene);
	void drawPass_shadowGen(QueuedScene *scene);
	void drawPass_lights(QueuedScene *scene);
	void drawPass_postprocess(QueuedScene *scene);

	void drawScene_world(QueuedScene *scene, const RenderClearer &clearer);
	void drawScene_worldSub(QueuedScene *scene);
	void drawScene_noworld(QueuedScene *scene, const RenderClearer &clearer);

	void issueVisQuery();
	void issueShadowQuery();

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
