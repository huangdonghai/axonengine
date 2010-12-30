#ifndef AX_RENDER_CONTEXT_H
#define AX_RENDER_CONTEXT_H

AX_BEGIN_NAMESPACE

class RenderContext
{
public:
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
	int m_bufferWidth;
	int m_bufferHeight;
	RenderTarget *m_frameWindow;
	RenderTarget *m_depthBuffer;
	RenderTarget *m_geoBuffer;
	RenderTarget *m_lightBuffer; // reuse as copied SceneColor
	RenderTarget *m_sceneBuffer;
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

#endif // AX_RENDER_CONTEXT_H
