/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GLTHREAD_H
#define AX_GLTHREAD_H

AX_BEGIN_NAMESPACE

	extern GLwindow* gFrameWindow;	// cast from gFrameTarget
	extern GLtarget* gWorldTarget;
	extern GLtarget* gLinearDepth;
	extern GLframebuffer* gWorldFramebuffer;

	extern bool gIsReflecting;
	extern RenderTarget* gTarget;
	extern RenderCamera* gCamera;
	extern QueuedScene* gScene;
	extern GLframebuffer* gFramebuffer;
	extern const QueuedEntity* gActor;

#if 0
	struct VertexDefInfo {
		bool have;
		uint_t vboId;
		GLenum datatype;
		int components;
		int data_size;
		int stride;
		uint_t offset;			// also used for vbo buffer offset
	};
#endif

	enum ClearFlag {
		ClearColor = 1,
		ClearDepth = 2,
		ClearStencil = 4
	};

	typedef Flags_<ClearFlag>	ClearFlags;

	struct Clearer {
		Rgba color;
		float depth;
		int stencil;
		bool isClearColor : 1;
		bool isClearDepth : 1;
		bool isClearStencil : 1;

		Clearer() : color(Rgba::Black), depth(1.0f), stencil(0), isClearColor(false), isClearDepth(false), isClearStencil(false) {}

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

	class GLthread : public Thread {
	public:
		friend class GLpostprocess;

		enum DrawPass {
			Zfill, ShadowGen, ShadowMask, Lighting, Composite, Overlay, PostProcess
		};

		GLthread();
		~GLthread();

		void initialize();
		void finalize();
		void preFrame();
		void runFrame(bool isInThread);

		// implement thread run
		virtual void doRun();		// work entry

	protected:
		void beginFrame();
		void drawScene(QueuedScene* scene, const Clearer& clearer);
		void setupScene(QueuedScene* scene, const Clearer* clearer = nullptr, RenderTarget* target = nullptr, RenderCamera* camera = nullptr);
		void unsetScene(QueuedScene* scene, const Clearer* clearer = nullptr, RenderTarget* target = nullptr, RenderCamera* camera = nullptr);
		void drawPrimitive(int prim_id);
		void drawInteraction(Interaction* ia);
		void endFrame();

		void drawPass_zfill(QueuedScene* scene);
		void drawPass_overlay(QueuedScene* scene);
		void drawPass_composite(QueuedScene* scene);
		void drawPass_shadowGen(QueuedScene* scene);
#if 0
		void drawPass_shadowMasks(QueuedScene* scene);
		void drawPass_shadowMask(QueuedScene* scene, const Matrix4& matrix);
		void drawPass_shadowBlur(QueuedScene* scene);
#endif
		void drawPass_postprocess(QueuedScene* scene);

		void drawScene_world(QueuedScene* scene, const Clearer& clearer);
		void drawScene_worldSub(QueuedScene* scene);
		void drawScene_noworld(QueuedScene* scene, const Clearer& clearer);

		void cacheResource();
		void cacheSceneRes(QueuedScene* scene);

		void beginQuery(int id);
		void endQuery();

	private:
		bool m_initialized;
		bool m_threadRendering;
	};

AX_END_NAMESPACE

#endif // end guardian
