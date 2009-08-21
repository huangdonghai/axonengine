/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_TARGET_H
#define AX_RENDER_TARGET_H

namespace Axon { namespace Render {

	class AX_API Target {
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

		Target();
		virtual ~Target() = 0;;

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
		virtual void attachDepth(Target* depth) {}
		virtual Target* getDepthAttached() const { return nullptr; }

		virtual void attachColor(int index, Target* c) {}
		virtual void detachColor(int index) {}
		virtual void detachAllColor() {}
		virtual Target* getColorAttached(int index) const { return 0; }

		virtual Texture* getTexture() { return nullptr; }

		// since qt maybe change window id, so we need this function
		virtual void setWindowHandle(handle_t newId) {}
		virtual handle_t getWindowHandle() { return nullptr; }

		// for pooled target
		void allocReal();
		void freeReal();

	public:
		bool m_realAllocated;
	};

	class World;
	class Entity;
	class Primitive;
	struct QueuedScene;

	class AX_API ReflectionTarget {
	public:
		ReflectionTarget(World* world, Entity* actor, Primitive* prim, int width, int height);
		~ReflectionTarget();

		void update(QueuedScene* qscene);

	public:
		World* m_world;
		Entity* m_actor;
		Primitive* m_prim;
		Camera m_camera;
		Target* m_target;
		int m_updateFrame;
	};

	class AX_API TargetManager {
	public:
		TargetManager();
		virtual ~TargetManager() = 0;

		virtual Target* allocTarget(Target::AllocHint hint, int width, int height, TexFormat texformat) = 0;
		virtual void freeTarget(Target* target) = 0;	// only permanent target need be free
		virtual bool isFormatSupport(TexFormat format) = 0;
		virtual TexFormat getSuggestFormat(Target::SuggestFormat sf) = 0;

		Target* allocShadowMap(int width, int height);
		void freeShadowMap(Target* target);

		ReflectionTarget* findReflection(World* world, Entity* actor, Primitive* prim, int width, int height);

	protected:
		friend class Target;
		void allocReal(Target* target);
		void freeReal(Target* target);

	protected:
		List<Target*> m_realAllocTargets;
		List<Target*> m_freeRealTargets;

		List<ReflectionTarget*> m_reflectionTargets;
	};

}} // namespace Axon::Render

#endif // end guardian

