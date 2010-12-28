#ifndef AX_RENDERINTERFACE_H
#define AX_RENDERINTERFACE_H

AX_BEGIN_NAMESPACE

class RenderApi
{
public:
	// resource management
	static void (*createTextureFromFileInMemory)(phandle_t h, AsioRequest *asioRequest);
	static void (*createTexture2D)(phandle_t h, TexFormat format, int width, int height, int flags);
	static void (*uploadTexture)(phandle_t h, const void *pixels, TexFormat format, IEventHandler *uploadedEventSendTo);
	static void (*uploadSubTexture)(phandle_t h, const Rect &rect, const void *pixels, TexFormat format, IEventHandler *uploadedEventSendTo);
	static void (*generateMipmap)(phandle_t h);
	static void (*deleteTexture2D)(phandle_t h);

	static void (*createVertexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
	static void (*uploadVertexBuffer)(phandle_t h, int datasize, const void *p, IEventHandler *uploadedEventSendTo);
	static void (*deleteVertexBuffer)(phandle_t h);

	static void (*createIndexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
	static void (*uploadIndexBuffer)(phandle_t h, int datasize, const void *p, IEventHandler *uploadedEventSendTo);
	static void (*deleteIndexBuffer)(phandle_t h);

	static void (*createWindowTarget)(phandle_t h, Handle hwnd, int width, int height);
	static void (*updateWindowTarget)(phandle_t h, Handle newHwnd, int width, int height);
	static void (*deleteWindowTarget)(phandle_t h);
#if 0
	static void (*createSamplerState)(phandle_t h, const SamplerDesc &samplerState);
	static void (*deleteSamplerState)(phandle_t h);

	static void (*createBlendState)(phandle_t h, const BlendDesc &src);
	static void (*deleteBlendState)(phandle_t h);

	static void (*createDepthStencilState)(phandle_t h, const DepthStencilDesc &src);
	static void (*deleteDepthStencilState)(phandle_t h);

	static void (*createRasterizerState)(phandle_t h, const RasterizerDesc &src);
	static void (*deleteRasterizerState)(phandle_t h);
#endif
	static void (*setRenderTarget)(int index, phandle_t h);
	static void (*setDepthStencil)(phandle_t h);

	static void (*setViewport)(const Rect &rect, const Vector2 & depthRange);
	static void (*setScissorRect)(const Rect &scissorRect);

	static void (*setShader)(const FixedString &name, const ShaderMacro &sm, Technique tech);
	static void (*setConstBuffer)(ConstBuffers::Type type, int size, const float *data);
	static void (*setParameters)(const FastParams *params1, const FastParams *params2);

	static void (*setVertices)(phandle_t vb, VertexType vt, int offset);
	static void (*setInstanceVertices)(phandle_t vb, VertexType vt, int offset, phandle_t inb, int inoffset, int incount);
	static void (*setIndices)(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount);

	static void (*setVerticesUP)(const void *vb, VertexType vt, int vertcount);
	static void (*setIndicesUP)(const void *ib, ElementType et, int indicescount);

	static void (*setGlobalTexture)(GlobalTextureId id, phandle_t h, const SamplerDesc &samplerState);
	static void (*setMaterialTexture)(phandle_t texs[], SamplerDesc states[]);

	static void (*setRenderState)(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd);

//	static void dip(ElementType et, int offset, int vertcount, int indices_count) = 0;
	static void (*draw)();

	// actions
	static void (*clear)(const RenderClearer &clearer);
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
public:
	ApiWrap();
	~ApiWrap();

	// api wrapper interface
	void createTextureFromFileInMemory(phandle_t &h, AsioRequest *asioRequest);
	void createTexture2D(phandle_t &h, TexFormat format, int width, int height, int flags = 0);
	void uploadTexture(phandle_t h, void *pixels, TexFormat format, IEventHandler *eventHandler);
	void uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format, IEventHandler *eventHandler);
	void generateMipmap(phandle_t h);
	void deleteTexture2D(phandle_t h);

	void createVertexBuffer(phandle_t &h, int datasize, Primitive::Hint hint);
	void uploadVertexBuffer(phandle_t h, int datasize, const void *p, IEventHandler *eventHandler);
	void deleteVertexBuffer(phandle_t h);

	void createIndexBuffer(phandle_t &h, int datasize, Primitive::Hint hint);
	void uploadIndexBuffer(phandle_t h, int datasize, const void *p, IEventHandler *eventHandler);
	void deleteIndexBuffer(phandle_t h);

	void createWindowTarget(phandle_t &h, Handle hwnd, int width, int height);
	void updateWindowTarget(phandle_t h, Handle newWndId, int width, int height);
	void deleteWindowTarget(phandle_t h);

	void createQuery(phandle_t &h);
	void issueQuery(phandle_t h, AsyncQuery *asioQuery);
	void deleteQuery(phandle_t h);
#if 0
	void createSamplerState(phandle_t &h, const SamplerDesc &desc);
	void deleteSamplerState(phandle_t h);

	void createBlendState(phandle_t &h, const BlendDesc &desc);
	void deleteBlendState(phandle_t h);

	void createDepthStencilState(phandle_t &h, const DepthStencilDesc &desc);
	void deleteDepthStencilState(phandle_t h);

	void createRasterizerState(phandle_t &h, const RasterizerDesc &desc);
	void deleteRasterizerState(phandle_t h);
#endif
	void setRenderTarget(int index, phandle_t h);
	void setDepthStencil(phandle_t h);

	void setViewport(const Rect &rect, const Vector2 & depthRange);
	void setScissorRect(const Rect &scissorRect);

	void setShader(const FixedString & name, const ShaderMacro &sm, Technique tech);
	void setConstBuffer(ConstBuffers::Type type, int size, const float *data);
	void setParameters(const FastParams *params1, const FastParams *param2);

	void setGlobalTexture(GlobalTextureId gt, Texture *tex);
	void setMaterialTexture(Texture * const tex[]);

	void setVertices(phandle_t vb, VertexType vt, int offset);
	void setVerticesInstanced(phandle_t vb, VertexType vt, int offset, phandle_t inb, int inoffset, int incount);
	void setIndices(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount);

	void setVerticesUP(const void *vb, VertexType vt, int vertcount);
	void setIndicesUP(const void *ib, ElementType et, int indicescount);

	void draw();

	void clear(const RenderClearer &clearer);

	void issueDeletions() {}
	byte_t *allocRingBuf(int size);

	// wait all commands executed
	void finish()
	{
		while (m_cmdWritePos != m_cmdReadPos) {
			OsUtil::sleep(0);
		}
	}

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

	ApiCommand *fetchCommand()
	{
		if (isEmpty())
			return 0;
		return m_ringCommand[m_cmdReadPos];
	}

	void popCommand()
	{
		ApiCommand *cmd = m_ringCommand[m_cmdReadPos];
		int bufpos = cmd->m_bufPos;
		cmd->~ApiCommand();
		m_bufReadPos = bufpos;
		m_cmdReadPos = (m_cmdReadPos + 1) % MAX_COMMANDS;
	}

	void pushCommand(ApiCommand *cmd)
	{
		cmd->m_bufPos = m_bufWritePos;
		m_ringCommand[m_cmdWritePos] = cmd;
		m_cmdWritePos = (m_cmdWritePos + 1) % MAX_COMMANDS;
	}

protected:
	phandle_t allocHandle();
	void freeHandle(phandle_t h);

	typedef void (*delete_func_t)(phandle_t);
	void addObjectDeletion(delete_func_t func, phandle_t h);

	void waitToPos(int pos);

	bool isFull() const { return normalizeRange(m_cmdReadPos - m_cmdWritePos) == 1; }
	bool isEmpty() const { return m_cmdReadPos == m_cmdWritePos; }
	static int normalizeRange(int a) { return (a + MAX_COMMANDS) % MAX_COMMANDS; }

private:
	enum {
		RING_BUFFER_SIZE = 4 * 1024 * 1024,
		MAX_COMMANDS = 64 * 1024,
		MAX_DELETE_COMMANDS = 8 * 1024,
		MAX_POS = 0x70000000
	};

	struct ObjectDeletion {
		delete_func_t func;
		phandle_t handle;
	};

	byte_t m_ringBuffer[RING_BUFFER_SIZE];
	ApiCommand *m_ringCommand[MAX_COMMANDS];

	volatile int m_bufReadPos, m_bufWritePos;
	volatile int m_cmdReadPos, m_cmdWritePos;

	int m_numObjectDeletions;
	ObjectDeletion m_objectDeletions[MAX_DELETE_COMMANDS];
};

class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	void issueFrame(RenderFrame *rq);

	void draw(VertexObject *vert, InstanceObject *inst, IndexObject *index, Material *mat, Technique tech);

protected:
	void cacheScene(RenderScene *scene);
	void cacheFrame(RenderFrame *queue);

	void beginFrame();
	void drawScene(RenderScene *scene, const RenderClearer &clearer);
	void setupScene(RenderScene *scene, const RenderClearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
//	void unsetScene(QueuedScene *scene, const RenderClearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
	void drawPrimitive(Primitive *prim);
	void drawInteraction(Interaction *ia);
	void endFrame();

	void drawGlobalLight(RenderScene *scene, RenderLight *light);
	void drawLocalLight(RenderScene *scene, RenderLight *light);

	void drawPass_gfill(RenderScene *scene);
	void drawPass_overlay(RenderScene *scene);
	void drawPass_composite(RenderScene *scene);
	void drawPass_shadowGen(RenderScene *scene);
	void drawPass_lights(RenderScene *scene);
	void drawPass_postprocess(RenderScene *scene);

	void drawScene_world(RenderScene *scene, const RenderClearer &clearer);
	void drawScene_worldSub(RenderScene *scene);
	void drawScene_noworld(RenderScene *scene, const RenderClearer &clearer);

	void issueVisQuery();
	void issueShadowQuery();

	void setMaterialUniforms(Material *mat);

	template <class Q>
	void setUniform(ConstBuffers::Item name, const Q &q)
	{
//		g_apiWrap->setShaderConst(name, sizeof(Q), &q);
	}

private:
	// init
	Material *m_defaultMat;
	RenderThread *m_renderThread;

	// runtime
	RenderTarget *m_frameWindow;
	RenderTarget *m_gbuffer;
	RenderTarget *m_lbuffer;
	RenderTarget *m_worldRt;
	RenderScene *m_worldScene;
	Interaction *m_ia;
	const RenderEntity *m_entity;
	bool m_isStatistic;
	Technique m_technique;

	ShaderMacro m_shaderMacro;
	RenderStateId m_renderStateId;
	bool m_forceWireframe;

	DepthStencilDesc m_depthStencilDesc;
	RasterizerDesc m_rasterizerDesc;
	BlendDesc m_blendDesc;

	DepthStencilDesc m_depthStencilDescLast;
	RasterizerDesc m_rasterizerDescLast;
	BlendDesc m_blendDescLast;
};

extern RenderContext *g_renderContext;

AX_END_NAMESPACE

#endif // AX_RENDERINTERFACE_H
