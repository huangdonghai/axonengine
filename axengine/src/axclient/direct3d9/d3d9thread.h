/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_D3D9THREAD_H
#define AX_D3D9THREAD_H

namespace Axon { namespace Render {

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

	class D3D9thread : public Thread {
	public:

		friend class D3D9postprocess;

		D3D9thread();
		~D3D9thread();
		void runFrame(bool isInThread);

		// implement thread run
		virtual void doRun();		// work entry

	protected:
		void beginFrame();
		void drawScene(QueuedScene* scene, const D3D9clearer& clearer);
		void setupScene(QueuedScene* scene, const D3D9clearer* clearer = nullptr, Target* target = nullptr, Camera* camera = nullptr);
		void unsetScene(QueuedScene* scene, const D3D9clearer* clearer = nullptr, Target* target = nullptr, Camera* camera = nullptr);
		void drawPrimitive(int prim_id);
		void drawInteraction(Interaction* ia);
		void endFrame();

		void drawGlobalLight(QueuedScene* scene, QueuedLight* light);
		void drawLocalLight(QueuedScene* scene, QueuedLight* light);

		void drawPass_zfill(QueuedScene* scene);
		void drawPass_overlay(QueuedScene* scene);
		void drawPass_composite(QueuedScene* scene);
		void drawPass_shadowGen(QueuedScene* scene);
		void drawPass_lights(QueuedScene* scene);
		void drawPass_postprocess(QueuedScene* scene);

		void drawScene_world(QueuedScene* scene, const D3D9clearer& clearer);
		void drawScene_worldSub(QueuedScene* scene);
		void drawScene_noworld(QueuedScene* scene, const D3D9clearer& clearer);

		void issueVisQuery();
		void issueShadowQuery();

		void syncFrame();
		void cacheSceneRes(QueuedScene* scene);

		void bindTarget(Target* target);

	private:
		int m_frameId;
		bool m_isStatistic;
	};

}} // namespace Axon::Render

#endif // end guardian

