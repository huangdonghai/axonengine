#ifndef AX_RENDER_THREAD_H
#define AX_RENDER_THREAD_H

AX_BEGIN_NAMESPACE

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


class RenderThread : public Thread
{
public:
	void runFrame(bool isInThread);

	// implement thread run
	virtual void doRun();		// work entry

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

	void syncFrame();
	void cacheSceneRes(QueuedScene *scene);

	void bindTarget(RenderTarget *target);

private:
};

AX_END_NAMESPACE

#endif // AX_RENDER_THREAD_H
