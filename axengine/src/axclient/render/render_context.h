#ifndef AX_RENDER_CONTEXT_H
#define AX_RENDER_CONTEXT_H

AX_BEGIN_NAMESPACE

#define AX_SU(a,b) g_renderContext->setUniform(ConstBuffer::a, b);
#define AX_ST(a,b) g_apiWrap->setGlobalTexture(GlobalTextureId::a, b)

struct MeshUP {
	MeshUP();
	~MeshUP();

	void init(int num_vertices, int num_indices);

	static bool setupScreenQuad(MeshUP*& mesh, const Rect &rect);
	static bool setupHexahedron(MeshUP*& mesh, Vector3 vertes[8]);
	static bool setupBoundingBox(MeshUP*& mesh, const BoundingBox &bbox);

	int m_numVertices;
	Vector3 *m_vertices;
	int m_numIndices;
	ushort_t *m_indices;
};


class RenderContext
{
public:
	enum {
		NUM_TONEMAP_TEXTURES = 4,
		NUM_BLOOM_TEXTURES = 3,
	};
	RenderContext();
	~RenderContext();

	void issueFrame(RenderFrame *rq);

	void drawUP(const void *vb, VertexType vt, int vertcount, const void *ib, ElementType et, int indicescount, Material *mat, Technique tech);
	void draw(VertexObject *vert, InstanceObject *inst, IndexObject *index, Material *mat, Technique tech);
	bool isReflecting() const { return m_isReflecting; }

	template <class Q>
	void setUniform(ConstBuffers::Item name, const Q &q)
	{
		//g_apiWrap->setShaderConst(name, sizeof(Q), &q);
		g_constBuffers.setField(name, sizeof(Q), reinterpret_cast<const float *>(&q));
	}

protected:
	void checkBufferSize(int width, int height);
	void cacheScene(RenderScene *scene);
	void cacheFrame(RenderFrame *queue);

	void drawScene(RenderScene *scene, const RenderClearer &clearer);
	void setupScene(RenderScene *scene, const RenderClearer *clearer, RenderCamera *camera);
	//	void unsetScene(QueuedScene *scene, const RenderClearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
	void drawPrimitive(Primitive *prim);
	void drawInteraction(Interaction *ia);

	void drawMeshUP(Material *material, MeshUP *mesh);

	void drawGlobalLight(RenderScene *scene, RenderLight *light);
	void drawLocalLight(RenderScene *scene, RenderLight *light);

	void drawPass_GeoFill(RenderScene *scene);
	void drawPass_Overlay(RenderScene *scene);
	void drawPass_Composite(RenderScene *scene);
	void drawPass_ShadowGen(RenderScene *scene);
	void drawPass_Lights(RenderScene *scene);
	void drawPass_Postprocess(RenderScene *scene);

	void drawScene_World(RenderScene *scene, const RenderClearer &clearer);
	void drawScene_WorldSub(RenderScene *scene);
	void drawScene_Noworld(RenderScene *scene, const RenderClearer &clearer);

	void issueVisQuery();
	void issueShadowQuery();

	void setMaterial(Material *mat);
	void setConstBuffers();

private:
	// init
	Material *m_defaultMat;
	Material *m_mtrGlobalLight;
	RenderThread *m_renderThread;
	RenderTarget *m_bloomMap[NUM_BLOOM_TEXTURES];
	RenderTarget *m_toneMap[NUM_TONEMAP_TEXTURES];

	// runtime
	RenderTarget *m_curWindow;
	RenderTarget *m_curWorldRt;
	RenderScene *m_curWorldScene;
	Interaction *m_curInteraction;
	const RenderEntity *m_curEntity;
	bool m_isStatistic;
	bool m_isReflecting;
	Technique m_curTechnique;
	RenderTargetSet m_targetSet;

	bool m_forceWireframe;

	DepthStencilDesc m_depthStencilDesc;
	RasterizerDesc m_rasterizerDesc;
	BlendDesc m_blendDesc;

	DepthStencilDesc m_depthStencilDescLast;
	RasterizerDesc m_rasterizerDescLast;
	BlendDesc m_blendDescLast;

	MeshUP *m_hexahedron;
};

extern RenderContext *g_renderContext;

AX_END_NAMESPACE

#endif // AX_RENDER_CONTEXT_H
