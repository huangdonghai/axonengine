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

class AX_API RenderTarget
{
public:
	enum Type {
		kWindow, kTexture
	};

	enum {
		MAX_COLOR_ATTACHMENT = 3
	};

	RenderTarget(int width, int height, TexFormat format);
	RenderTarget(Handle hwnd, const std::string &debugname);
	~RenderTarget();

	Type getType() const { return m_type; }
	Rect getRect() const;
	bool isWindow() const { return m_type == kWindow; }
	bool isTexture() const { return m_type == kTexture; }
	bool isColorFormat() const { return m_format.isColor(); }
	bool isDepthFormat() const { return m_format.isDepth(); }
	bool isStencilFormat() const { return m_format.isStencil(); }

	// for render texture target
	void attachDepth(RenderTarget *depth);
	RenderTarget *getDepthAttached() const { return m_depthTarget; }

	Texture *getTexture() { if (isTexture()) return m_texture; else return 0; }

	void attachColor(int index, RenderTarget *c);
	void detachColor(int index);
	void detachAllColor();
	RenderTarget *getColorAttached(int index) const;

	// since qt maybe change window id, so we need this function
	void setWindowHandle(Handle newId);
	Handle getWindowHandle() { return m_wndId; }

	RenderTarget *getGeoBuffer() const { return m_geoBuffer; }
	RenderTarget *getLightBuffer() const { return m_lightBuffer; }

	phandle_t getPHandle()
	{
		if (isTexture()) return m_texture->getPHandle();
		else return m_window;
	}

private:
	Type m_type;
	phandle_t m_window;
	Texture *m_texture;

	TexFormat m_format;
	int m_width, m_height;

	int m_boundIndex;

	RenderTarget *m_depthTarget;
	RenderTarget *m_colorAttached[MAX_COLOR_ATTACHMENT];

	// for window
	Handle m_wndId;
	std::string m_name;

	RenderTarget *m_geoBuffer;
	RenderTarget *m_lightBuffer;
};

class RenderWorld;
class RenderEntity;
class Primitive;
struct RenderScene;

class AX_API ReflectionMap {
public:
	ReflectionMap(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height);
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

class AX_API ShadowMap {
public:
	ShadowMap(int width, int height);
	~ShadowMap();

	void allocReal();
	void freeReal();

public:
	int m_width, m_height;
	RenderTarget *m_renderTarget;
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

