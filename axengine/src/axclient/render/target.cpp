/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

RenderTarget::RenderTarget(TexFormat format, const Size &size)
{
	m_type = kTexture;
	m_boundIndex = -1;

	m_size = size;
	m_format = format;

	m_rtDepth = m_rt0 = m_rt1 = m_rt2 = m_rt3 = 0;

	std::string texname;
	StringUtil::sprintf(texname, "_render_target_%d_%d_%d", m_size.width, m_size.height, g_system->generateId());

	m_texture = new Texture(texname, format, m_size, Texture::RenderTarget);
}

RenderTarget::RenderTarget(Handle hwnd, const std::string &debugname, const Size &size)
{
	m_type = kWindow;
	m_window = 0;
	m_wndId = hwnd;
	m_name = debugname;

	m_size = size;
	m_format = TexFormat::AUTO;

	m_rtDepth = m_rt0 = m_rt1 = m_rt2 = m_rt3 = 0;

	updateWindowInfo(hwnd, size);
}

RenderTarget::~RenderTarget()
{
	if (isTexture()) {
		g_apiWrap->deleteTexture2D(m_window);
	} else {
		g_apiWrap->deleteWindowTarget(m_window);
		SafeDelete(m_rtDepth);
		SafeDelete(m_rt0);
		SafeDelete(m_rt1);
		SafeDelete(m_rt2);
		SafeDelete(m_rt3);
	}
}

void RenderTarget::updateWindowInfo(Handle newId, const Size &size)
{
	if (!m_window) {
		g_apiWrap->createWindowTarget(m_window, newId, size.width, size.height);
	} else {
		g_apiWrap->updateWindowTarget(m_window, newId, size.width, size.height);
	}

	if (m_size == size)
		return;

	m_size = size;

	SafeDelete(m_rtDepth);
	SafeDelete(m_rt0);
	SafeDelete(m_rt1);
	SafeDelete(m_rt2);
	SafeDelete(m_rt3);

	m_rtDepth = new RenderTarget(g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_DepthStencilTexture], m_size);
	m_rt0 = new RenderTarget(g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_HdrSceneColor], m_size);
	m_rt1 = new RenderTarget(g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_NormalBuffer], m_size);
	m_rt2 = new RenderTarget(g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_SceneColor], m_size);
	m_rt3 = new RenderTarget(g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_SceneColor], m_size);
	m_wndId = newId;
}

ReflectionMap::ReflectionMap(RenderWorld *world, RenderEntity *actor, Primitive *prim, const Size &size)
{
	m_world = world;
	m_actor = actor;
	m_prim = prim;
	m_updateFrame = -1;
	m_target = new RenderTarget(g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_HdrSceneColor], size);
}

ReflectionMap::~ReflectionMap()
{
	delete m_target;
}

void ReflectionMap::update(RenderScene *qscene)
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

	RenderScene *subscene = qscene->addSubScene();
	if (!subscene)
		return;

	subscene->sceneType = RenderScene::Reflection;
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
	std::list<ReflectionMap*>::iterator it = m_reflectionTargets.begin();

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

