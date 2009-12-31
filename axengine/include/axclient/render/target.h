/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_TARGET_H
#define AX_RENDER_TARGET_H

AX_BEGIN_NAMESPACE

	class AX_API RenderTarget {
	public:
		enum Type {
			kWindow, kTexture
		};

		enum AllocHint {
			Free,				// internal use, not for user
			PermanentAlloc,		// permanent allocation, user should free it
			TemporalAlloc,		// temporal, be freed immediately after used this
			PooledAlloc,
		};

		enum SuggestFormat {
			LDR_COLOR,
			MDR_COLOR,
			HDR_COLOR,
			SHADOW_MAP,
		};

		enum {
			MAX_COLOR_ATTACHMENT = 3
		};

		RenderTarget();
		virtual ~RenderTarget() = 0;;

		virtual Type getType() = 0;
		virtual Rect getRect() = 0;
		virtual void bind() = 0;
		virtual void unbind() = 0;
		virtual bool isWindow() { return false;}
		virtual bool isTexture() { return false;}
		virtual bool isColorFormat() { return false;}
		virtual bool isDepthFormat() { return false;}
		virtual bool isStencilFormat() { return false;}

		// for render texture target
		virtual void attachDepth(RenderTarget *depth) {}
		virtual RenderTarget *getDepthAttached() const { return nullptr; }

		virtual void attachColor(int index, RenderTarget *c) {}
		virtual void detachColor(int index) {}
		virtual void detachAllColor() {}
		virtual RenderTarget *getColorAttached(int index) const { return 0; }

		virtual Texture *getTexture() { return nullptr; }

		// since qt maybe change window id, so we need this function
		virtual void setWindowHandle(handle_t newId) {}
		virtual handle_t getWindowHandle() { return nullptr; }

		// for pooled target
		void allocReal();
		void freeReal();

	public:
		bool m_realAllocated;
	};

	class RenderWorld;
	class RenderEntity;
	class Primitive;
	struct QueuedScene;

	class AX_API ReflectionTarget {
	public:
		ReflectionTarget(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height);
		~ReflectionTarget();

		void update(QueuedScene *qscene);

	public:
		RenderWorld *m_world;
		RenderEntity *m_actor;
		Primitive *m_prim;
		RenderCamera m_camera;
		RenderTarget *m_target;
		int m_updateFrame;
	};

	class AX_API TargetManager {
	public:
		TargetManager();
		virtual ~TargetManager() = 0;

		virtual RenderTarget *allocTarget(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat) = 0;
		virtual void freeTarget(RenderTarget *target) = 0;	// only permanent target need be free
		virtual bool isFormatSupport(TexFormat format) = 0;
		virtual TexFormat getSuggestFormat(RenderTarget::SuggestFormat sf) = 0;

		RenderTarget *allocShadowMap(int width, int height);
		void freeShadowMap(RenderTarget *target);

		ReflectionTarget *findReflection(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height);

	protected:
		friend class RenderTarget;
		void allocReal(RenderTarget *target);
		void freeReal(RenderTarget *target);

	protected:
		List<RenderTarget*> m_realAllocTargets;
		List<RenderTarget*> m_freeRealTargets;

		List<ReflectionTarget*> m_reflectionTargets;
	};

AX_END_NAMESPACE

#endif // end guardian

