#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

class RenderApi
{
public:
	// resource management
	static void (*createTextureFromFileInMemory)(phandle_t h, IoRequest *asioRequest);
	static void (*createTexture)(phandle_t h, TexType type, TexFormat format, int width, int height, int depth, int flags);
	static void (*uploadSubTexture)(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
	static void (*generateMipmap)(phandle_t h);
	static void (*deleteTexture)(phandle_t h);

	static void (*createVertexBuffer)(phandle_t h, int datasize, Primitive::Hint hint, const void *p);
	static void (*uploadVertexBuffer)(phandle_t h, int offset, int datasize, const void *p);
	static void (*deleteVertexBuffer)(phandle_t h);

	static void (*createIndexBuffer)(phandle_t h, int datasize, Primitive::Hint hint, const void *p);
	static void (*uploadIndexBuffer)(phandle_t h, int offset, int datasize, const void *p);
	static void (*deleteIndexBuffer)(phandle_t h);

	static void (*createWindowTarget)(phandle_t h, Handle hwnd, int width, int height);
	static void (*updateWindowTarget)(phandle_t h, Handle newHwnd, int width, int height);
	static void (*deleteWindowTarget)(phandle_t h);

	static void (*createQuery)(phandle_t &h);
	static void (*issueQueries)(int n, Query *queries[]);
	static void (*deleteQuery)(phandle_t h);

	static void (*beginPerfEvent)(const char *pixname);
	static void (*endPerfEvent)();

	static void (*setTargetSet)(phandle_t targetSet[RenderTargetSet::MaxTarget], int slices[RenderTargetSet::MaxTarget]);

	static void (*setViewport)(const Rect &rect, const Vector2 & depthRange);

	static void (*setShader)(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm, Technique tech);
	static void (*setConstBuffer)(ConstBuffers::Type type, int size, const void *data);
	static void (*setParameters)(const FastParams *params1, const FastParams *params2);

	static void (*setVertices)(phandle_t vb, VertexType vt, int offset, int vertcount);
	static void (*setInstanceVertices)(phandle_t vb, VertexType vt, int offset, int vertcount, phandle_t inb, int inoffset, int incount);
	static void (*setIndices)(phandle_t ib, ElementType et, int offset, int indicescount);

	static void (*setVerticesUP)(const void *vb, VertexType vt, int vertcount);
	static void (*setIndicesUP)(const void *ib, ElementType et, int indicescount);

	static void (*setGlobalTexture)(GlobalTextureId id, phandle_t h, const SamplerDesc &samplerState);
	static void (*setMaterialTexture)(const FastTextureParams *textures);

	static void (*setRenderState)(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd);

	static void (*draw)();

	// actions
	static void (*clear)(const RenderClearer &clearer);

	// present to eye
	static void (*present)(phandle_t window);
};

class ApiCommand
{
public:
	ApiCommand() {}
	virtual ~ApiCommand() {}
	virtual void exec() = 0;
	int m_bufPos;
};


class ApiWrap
{
	friend class RenderContext;

public:
	enum {
		RING_BUFFER_SIZE = 4 * 1024 * 1024,
		MAX_COMMANDS = 64 * 1024,
		MAX_DELETE_COMMANDS = 8 * 1024,
		MAX_POS = 0x70000000
	};

	ApiWrap();
	~ApiWrap();

	// api wrapper interface
	void createTextureFromFileInMemory(phandle_t &h, IoRequest *asioRequest);
	void createTexture(phandle_t &h, TexType type, TexFormat format, int width, int height, int depth, int flags);
	void uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap(phandle_t h);
	void deleteTexture(phandle_t h);

	void createVertexBuffer(phandle_t &h, int datasize, Primitive::Hint hint, const void *p);
	void uploadVertexBuffer(phandle_t h, int offset, int datasize, const void *p);
	void deleteVertexBuffer(phandle_t h);

	void createIndexBuffer(phandle_t &h, int datasize, Primitive::Hint hint, const void *p);
	void uploadIndexBuffer(phandle_t h, int offset, int datasize, const void *p);
	void deleteIndexBuffer(phandle_t h);

	void createWindowTarget(phandle_t &h, Handle hwnd, int width, int height);
	void updateWindowTarget(phandle_t h, Handle newWndId, int width, int height);
	void deleteWindowTarget(phandle_t h);

	void createQuery(phandle_t &h);
	void issueQueries(int n, Query *queries[]);
	void deleteQuery(phandle_t h);

	void beginPerfEvent(const char *pixname);
	void endPerfEvent();

	void setTargetSet(const RenderTargetSet &target_set);

	void setViewport(const Rect &rect, const Vector2 & depthRange);

	void setShader(const FixedString & name, const MaterialMacro &mm, Technique tech);
	void setConstBuffer(ConstBuffers::Type type, int size, const void *data);
	void setParameters(const FastParams *params1, const FastParams *param2);

	void setGlobalTexture(GlobalTextureId gt, Texture *tex);
	void setMaterialTexture(const FastTextureParams *textures);

	void setVertices(phandle_t vb, VertexType vt, int offset, int vertcount);
	void setVerticesInstanced(phandle_t vb, VertexType vt, int offset, int vertcount, phandle_t inb, int inoffset, int incount);
	void setIndices(phandle_t ib, ElementType et, int offset, int indicescount);

	void setVerticesUP(const void *vb, VertexType vt, int vertcount);
	void setIndicesUP(const void *ib, ElementType et, int indicescount);

	void setRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd);

	void draw();

	void clear(const RenderClearer &clearer);
	void present(RenderTarget *window);

	void issueDeletions();
	byte_t *allocRingBuf(int size);

	// wait all commands executed
	void finish();

	// called in render thread, return number commands executed
	int runCommands();

	template <class Q>
	Q *allocType(int n=1)
	{
		Q *result = reinterpret_cast<Q *>(allocRingBuf(n * sizeof(Q)));
		return result;
	}

	// alloca a command from ring buffer
	template <class Q>
	Q *allocCommand()
	{
		while (isFull())
			OsUtil::sleep(0);

		int size = sizeof(Q);
		byte_t *pbuf = allocRingBuf(size);
		Q *ptr = reinterpret_cast<Q *>(pbuf);

		return ptr;
	}

	void pushCommand(ApiCommand *cmd);
	ApiCommand *fetchCommand();
	void popCommand();

protected:
	phandle_t newHandle();
	void deleteHandle(phandle_t h);

	typedef void (*delete_func_t)(phandle_t);
	void addObjectDeletion(delete_func_t func, phandle_t h);

	size_t calcStop(size_t size);
	size_t calcPos(size_t size);
	void waitToPos(size_t pos);

	bool isFull() const;
	bool isEmpty() const { return m_cmdReadPos == m_cmdWritePos; }

private:

	struct ObjectDeletion {
		delete_func_t func;
		phandle_t handle;
	};

	byte_t m_ringBuffer[RING_BUFFER_SIZE];
	ApiCommand *m_ringCommand[MAX_COMMANDS];

	volatile size_t m_bufReadPos, m_bufWritePos;
	volatile size_t m_cmdReadPos, m_cmdWritePos;

	int m_numObjectDeletions;
	ObjectDeletion m_objectDeletions[MAX_DELETE_COMMANDS];
};

AX_END_NAMESPACE

#endif // AX_RENDERINTERFACE_H
