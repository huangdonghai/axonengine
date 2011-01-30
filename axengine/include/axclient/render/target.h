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

class AX_API RenderTarget : public Texture
{
	friend class RenderContext;

public:
	RenderTarget(TexFormat format, const Size &size);
	RenderTarget(TexType texType, TexFormat format, int width, int height, int depth);
	RenderTarget(Handle hwnd, const std::string &debugname, const Size &size);
	~RenderTarget();

	Size size() const { if (isTexture()) return Texture::size(); else return m_size; }
	bool isWindow() const { return m_isWindow; }
	bool isTexture() const { return !m_isWindow; }
	void setSlice(int slice) { m_slice = slice; }
	int slice() const { return m_slice; }

	RenderTarget *cloneSlice(int slice) const;

	// since qt maybe change window id, so we need this function
	void updateWindowInfo(Handle newId, const Size &size);
	Handle getWindowHandle() { return m_wndId; }

	phandle_t getPHandle()
	{
		if (isTexture()) return Texture::getPHandle();
		else return m_window;
	}

protected:
	RenderTarget(const RenderTarget &rhs);

private:
	bool m_isWindow; // is window, or texture
	phandle_t m_window;

	// for texture target
	int m_slice;

	// for window
	Handle m_wndId;
	Size m_size;
	std::string m_name;

	RenderTarget *m_rtDepth; // depth stencil
	RenderTarget *m_rt0; // hdr color accum
	RenderTarget *m_rt1; // normal.xyz, specular power
	RenderTarget *m_rt2; // diffuse.xyz, specular color
	RenderTarget *m_rt3; // motion.xy, ssao, skyAO
};

struct RenderTargetSet {
	enum {
		MaxColorTarget = 4,
		MaxTarget = MaxColorTarget + 1
	};

	RenderTarget *m_depthTarget;
	RenderTarget *m_colorTargets[MaxColorTarget];

	RenderTarget *getFirstUsed() const
	{
		if (m_depthTarget) return m_depthTarget;
		for (int i = 0; i < MaxColorTarget; i++)
			if (m_colorTargets[i]) return m_colorTargets[i];
		return 0;
	}
};

class RenderWorld;
class RenderEntity;
class Primitive;
struct RenderScene;

class AX_API ReflectionMap {
public:
	ReflectionMap(RenderWorld *world, RenderEntity *actor, Primitive *prim, const Size &size);
	~ReflectionMap();

	void update(RenderScene *qscene);

public:
	RenderWorld *m_world;
	RenderEntity *m_actor;
	Primitive *m_prim;
	RenderCamera m_camera;
	RenderTarget *m_target;
	int m_updateFrame;
};

#if 0
class AX_API RenderTargetManager {
public:
	RenderTargetManager();
	virtual ~RenderTargetManager() = 0;

	virtual RenderTarget *allocTarget(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat) = 0;
	virtual void freeTarget(RenderTarget *target) = 0;	// only permanent target need be free
	virtual bool isFormatSupport(TexFormat format) = 0;
	virtual TexFormat getSuggestFormat(RenderTarget::SuggestFormat sf) = 0;

	ShadowMap *allocShadowMap(int width, int height);
	void freeShadowMap(ShadowMap *target);

	ReflectionMap *findReflection(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height);

protected:
	friend class RenderTarget;
	void allocReal(RenderTarget *target);
	void freeReal(RenderTarget *target);

protected:
	std::list<RenderTarget*> m_realAllocTargets;
	std::list<RenderTarget*> m_freeRealTargets;

	std::list<ReflectionMap*> m_reflectionTargets;
};
#endif

AX_END_NAMESPACE

#endif // end guardian

