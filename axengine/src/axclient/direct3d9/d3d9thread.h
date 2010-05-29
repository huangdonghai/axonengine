/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_D3D9THREAD_H
#define AX_D3D9THREAD_H

AX_BEGIN_NAMESPACE

struct D3D9clearer {
	Rgba color;
	float depth;
	int stencil;
	bool isClearColor : 1;
	bool isClearDepth : 1;
	bool isClearStencil : 1;

	D3D9clearer() : color(Rgba::Black), depth(1.0f), stencil(0), isClearColor(false), isClearDepth(false), isClearStencil(false) {}

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

class D3D9Thread : public Thread {
public:

	friend class D3D9Postprocess;

	D3D9Thread();
	~D3D9Thread();
	void runFrame(bool isInThread);

	// implement thread run
	virtual void doRun();		// work entry

protected:
	void beginFrame();
	void drawScene(RenderScene *scene, const D3D9clearer &clearer);
	void setupScene(RenderScene *scene, const D3D9clearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
	void unsetScene(RenderScene *scene, const D3D9clearer *clearer = 0, RenderTarget *target = 0, RenderCamera *camera = 0);
	void drawPrimitive(int prim_id);
	void drawInteraction(Interaction *ia);
	void endFrame();

	void drawGlobalLight(RenderScene *scene, QueuedLight *light);
	void drawLocalLight(RenderScene *scene, QueuedLight *light);

	void drawPass_zfill(RenderScene *scene);
	void drawPass_overlay(RenderScene *scene);
	void drawPass_composite(RenderScene *scene);
	void drawPass_shadowGen(RenderScene *scene);
	void drawPass_lights(RenderScene *scene);
	void drawPass_postprocess(RenderScene *scene);

	void drawScene_world(RenderScene *scene, const D3D9clearer &clearer);
	void drawScene_worldSub(RenderScene *scene);
	void drawScene_noworld(RenderScene *scene, const D3D9clearer &clearer);

	void issueVisQuery();
	void issueShadowQuery();

	void syncFrame();
	void cacheSceneRes(RenderScene *scene);

	void bindTarget(RenderTarget *target);

private:
	int m_frameId;
	bool m_isStatistic;
};

AX_END_NAMESPACE

#endif // end guardian

