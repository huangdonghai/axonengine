#ifndef AX_RENDER_CONTEXT_H
#define AX_RENDER_CONTEXT_H

AX_BEGIN_NAMESPACE

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

	void draw(VertexObject *vert, InstanceObject *inst, IndexObject *index, Material *mat, Technique tech);

protected:
	void checkBufferSize(int width, int height);
	void cacheScene(RenderScene *scene);
	void cacheFrame(RenderFrame *queue);

	void beginFrame();
	void drawScene(RenderScene *scene, const RenderClearer &clearer);
	void setupScene(RenderScene *scene, const RenderClearer *clearer, RenderCamera *camera);
	//	void unsetScene(QueuedScene *scene, const RenderClearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
	void drawPrimitive(Primitive *prim);
	void drawInteraction(Interaction *ia);
	void endFrame();

	void drawGlobalLight(RenderScene *scene, RenderLight *light);
	void drawLocalLight(RenderScene *scene, RenderLight *light);

	void drawPass_geoFill(RenderScene *scene);
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
		g_constBuffers.setField(name, sizeof(Q), reinterpret_cast<const float *>(&q));
	}

private:
	// init
	Material *m_defaultMat;
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

	ShaderMacro m_shaderMacro;
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

#endif // AX_RENDER_CONTEXT_H
