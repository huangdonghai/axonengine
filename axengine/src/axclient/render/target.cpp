/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

RenderTarget::RenderTarget(int width, int height, TexFormat format)
{
	m_type = kTexture;
	m_boundIndex = -1;
	m_depthTarget = nullptr;
	TypeZeroArray(m_colorAttached);

	m_width = width;
	m_height = height;
	m_format = format;

	m_gbuffer = 0;
	m_lightBuffer = 0;

	String texname;
	StringUtil::sprintf(texname, "_render_target_%d_%d_%d", m_width, m_height, g_system->generateId());

	g_apiWrap->createTexture2D(&m_window, format, width, height, Texture::IF_RenderTarget);
}

RenderTarget::RenderTarget(Handle hwnd, const String &debugname)
{
	m_type = kWindow;
	m_wndId = hwnd;
	m_name = debugname;

	m_width = -1;
	m_height = -1;
	m_format = TexFormat::AUTO;

	m_gbuffer = 0;
	m_lightBuffer = 0;

	setWindowHandle(hwnd);
}

RenderTarget::~RenderTarget()
{
	if (isTexture()) {
		g_apiWrap->deleteTexture2D(&m_window);
	} else {
		g_apiWrap->deleteWindowTarget(&m_window);
		SafeDelete(m_gbuffer);
		SafeDelete(m_lightBuffer);
	}
}

Rect RenderTarget::getRect() const
{
	return Rect(0, 0, m_width, m_height);
}

void RenderTarget::attachDepth(RenderTarget *depth)
{
	AX_ASSERT(depth->isTexture());
	AX_ASSERT(depth->isDepthFormat());
	m_depthTarget = depth;
}

void RenderTarget::attachColor(int index, RenderTarget *c)
{
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	m_colorAttached[index] = c;
}

void RenderTarget::detachColor(int index)
{
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	m_colorAttached[index] = 0;
}

void RenderTarget::detachAllColor()
{
	TypeZeroArray(m_colorAttached);
}

RenderTarget *RenderTarget::getColorAttached(int index) const
{
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	return m_colorAttached[index];
}

void RenderTarget::setWindowHandle(Handle newId)
{
	Rect rect = RenderSystem::getWindowRect(newId);

	if (rect.width == m_width || rect.height == m_height || newId == m_wndId)
		return;

	if (m_width == -1) {
		g_apiWrap->createWindowTarget(&m_window, newId, rect.width, rect.height);
	} else {
		g_apiWrap->updateWindowTarget(&m_window, newId, rect.width, rect.height);
	}

	SafeDelete(m_gbuffer);
	SafeDelete(m_lightBuffer);
	m_gbuffer = new RenderTarget(rect.width, rect.height, TexFormat::RGBA16F);
	m_lightBuffer = new RenderTarget(rect.width, rect.height, TexFormat::BGRA8);

	m_width = rect.width;
	m_height = rect.height;
	m_wndId = newId;
}

ReflectionMap::ReflectionMap(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height)
{
	m_world = world;
	m_actor = actor;
	m_prim = prim;
	m_updateFrame = -1;
	m_target = new RenderTarget(width, height, TexFormat::BGRA8);
}

ReflectionMap::~ReflectionMap()
{
	delete m_target;
}

void ReflectionMap::update(QueuedScene *qscene)
{
	bool needUpdate = false;

#if 0
	if (!m_target->m_realAllocated) {
		m_target->allocReal();
		needUpdate = true;
	}
#endif
	if (m_updateFrame == -1) {
		needUpdate = true;
	}

	if (m_world->getVisFrameId() - m_updateFrame > 4) {
		needUpdate = true;
	}

	if (!needUpdate)
		return;

	QueuedScene *subscene = qscene->addSubScene();
	if (!subscene)
		return;

	subscene->sceneType = QueuedScene::Reflection;
	subscene->camera = qscene->camera.createMirrorCamera(Plane(0, 0, 1, 0));
	subscene->camera.setTarget(m_target);
	subscene->camera.setViewRect(Rect(0, 0, 512, 512));

	m_updateFrame = m_world->getVisFrameId();
}

#if 0
RenderTargetManager::~RenderTargetManager()
{}

RenderTargetManager::RenderTargetManager()
{
}

void RenderTargetManager::allocReal(RenderTarget *target)
{
	m_realAllocTargets.push_back(target);
}

void RenderTargetManager::freeReal(RenderTarget *target)
{
	m_freeRealTargets.push_back(target);
}

ReflectionMap *RenderTargetManager::findReflection(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height)
{
	List<ReflectionMap*>::iterator it = m_reflectionTargets.begin();

	ReflectionMap *reflTarget = 0;
	for (; it != m_reflectionTargets.end(); ++it) {
		ReflectionMap *curTarget = *it;

		if (curTarget->m_world == world && curTarget->m_actor == actor && curTarget->m_prim == prim) {
			reflTarget = curTarget;
			break;
		}
	}

	if (!reflTarget) {
		reflTarget = new ReflectionMap(world, actor, prim, width, height);
		m_reflectionTargets.push_back(reflTarget);
	}

	return reflTarget;
}
#endif


ShadowMap::ShadowMap( int width, int height )
{
	m_width = width; m_height = height;
	m_renderTarget = 0;
}

ShadowMap::~ShadowMap()
{
	freeReal();
}

void ShadowMap::allocReal()
{
	
}

void ShadowMap::freeReal()
{

}

AX_END_NAMESPACE

