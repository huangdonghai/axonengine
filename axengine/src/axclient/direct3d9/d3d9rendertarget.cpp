/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

AX_BEGIN_NAMESPACE

#if 0
//--------------------------------------------------------------------------
// class D3D9Target
//--------------------------------------------------------------------------

D3D9Target::D3D9Target(int width, int height, TexFormat format, bool pooled)
{
	m_boundIndex = -1;
	m_depthTarget = nullptr;
	TypeZeroArray(m_colorAttached);

	m_storeHint = RenderTarget::Free;

	m_texture = 0;
	m_d3d9texture = 0;
	m_lastFrameUsed = 0;

	m_width = width;
	m_height = height;
	m_format = format;

	m_surface = 0;

	// create asset
	m_realTarget = 0;
	if (!pooled) {
		m_isPooled = false;
		String texname;
		StringUtil::sprintf(texname, "_render_target_%d_%d_%d", m_width, m_height, g_system->generateId());
#if 0
		m_texture = dynamic_cast<D3D9texture*>(g_assetManager->createEmptyAsset(Texture::AssetType));
		m_texture->initialize(format, width, height, Texture::IF_RenderTarget);
		g_assetManager->addAsset(Asset::kTexture, texname, m_texture);
#else
		m_texture = new Texture(texname, format, m_width, m_height, Texture::IF_RenderTarget);
		m_d3d9texture = static_cast<D3D9Texture*>(m_texture.get());
#endif
	} else {
		m_texture = 0;
		m_isPooled = true;
	}

	m_isNullColor = false;
}

D3D9Target::~D3D9Target()
{
	SAFE_RELEASE(m_surface);
}

Rect D3D9Target::getRect()
{
	return Rect(0,0,m_width,m_height);
}

void D3D9Target::bind()
{
	HRESULT hr;

	if (m_format.isDepth()) {
		V(d3d9StateManager->setDepthStencilSurface(getSurface(), m_format));
		releaseSurface();

		// TODO: set dummy color target
		TexFormat format = d3d9TargetManager->getNullTargetFormat();
		m_nullColor = d3d9TargetManager->allocTargetDX(RenderTarget::TemporalAlloc, m_width, m_height, format);
		V(dx9_device->SetRenderTarget(0, m_nullColor->getSurface()));
		m_nullColor->releaseSurface();

		m_isNullColor = true;
	} else {
		V(dx9_device->SetRenderTarget(0, getSurface()));
		releaseSurface();

		if (m_colorAttached[0]) {
			dx9_device->SetRenderTarget(1, m_colorAttached[0]->getSurface());
			m_colorAttached[0]->releaseSurface();
		} else {
			dx9_device->SetRenderTarget(1, 0);
		}

		IDirect3DSurface9 *ds = 0;
		if (m_depthTarget) {
			ds = m_depthTarget->getSurface();
			V(d3d9StateManager->setDepthStencilSurface(ds, TexFormat::D24S8));
			m_depthTarget->releaseSurface();
		} else {
			ds = d3d9TargetManager->getDepthStencil(m_width, m_height);
			V(d3d9StateManager->setDepthStencilSurface(ds, TexFormat::D24S8));
		}

		m_isNullColor = false;
	}
}

void D3D9Target::unbind()
{
#if 0
	if (m_isNullColor) {
		d3d9TargetManager->freeTarget(m_nullColor);
	}
	releaseSurface();

	if (m_depthTarget) {
		m_depthTarget->releaseSurface();
	}
#endif
}

void D3D9Target::attachDepth(RenderTarget *depth)
{
	AX_ASSERT(depth->isTexture());
	AX_ASSERT(depth->isDepthFormat());
	D3D9Target *gldepth = dynamic_cast<D3D9Target*>(depth);
	m_depthTarget = gldepth;
}

void D3D9Target::attachColor(int index, RenderTarget *c)
{
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	m_colorAttached[index] = (D3D9Target*)c;
}

void D3D9Target::detachColor(int index)
{
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	m_colorAttached[index] = nullptr;
}

void D3D9Target::detachAllColor()
{
	TypeZeroArray(m_colorAttached);
}

RenderTarget *D3D9Target::getColorAttached(int index) const
{
	AX_ASSERT(index < MAX_COLOR_ATTACHMENT);
	return m_colorAttached[index];
}

void D3D9Target::setHint(AllocHint hint, int frame) {
	m_storeHint = hint;

	if (hint != Free) {
		m_lastFrameUsed = frame;
	}
}

void D3D9Target::copyFramebuffer(const Rect &r)
{
	getSurface();

	IDirect3DSurface9 *surface = 0;

	if (d3d9BoundTarget->isWindow()) {
		surface = ((DX9_Window*)d3d9BoundTarget)->getSurface();
	} else {
		surface = ((D3D9Target*)d3d9BoundTarget)->getSurface();
	}

	RECT d3drect;

	d3drect.left = 0;
	d3drect.top = 0;
	d3drect.right = r.width;
	d3drect.bottom = r.height;

	HRESULT hr;
	V(dx9_device->StretchRect(surface, &d3drect, m_surface, &d3drect, D3DTEXF_NONE));

	releaseSurface();
}

void D3D9Target::allocRealTarget()
{
	if (m_realTarget)
		return;

	m_realTarget = d3d9TargetManager->allocTargetDX(RenderTarget::PermanentAlloc, m_width, m_height, m_format);
}

void D3D9Target::freeRealTarget()
{
	if (!m_realTarget)
		Errorf("free real target error: not even allocated");

	d3d9TargetManager->freeTarget(m_realTarget);
	m_realTarget = 0;
}

IDirect3DSurface9 *D3D9Target::getSurface()
{
	if (!m_isPooled) {
		if (!m_surface) {
			HRESULT hr;
			V(m_d3d9texture->getObject()->GetSurfaceLevel(0, &m_surface));
		}
		return m_surface;
	}

	if (!m_realTarget) {
		Errorf("can't bind a pooled target when it not alloc a real target");
		return 0;
	}

	return m_realTarget->getSurface();
}

Texture *D3D9Target::getTexture()
{
	if (!m_isPooled)
		return m_d3d9texture;

	if (!m_realTarget) {
		Errorf("need alloc a real target for pooled target");
		return 0;
	}

	return m_realTarget->getTexture();
}

void D3D9Target::releaseSurface()
{
	if (!m_isPooled) {
		SAFE_RELEASE(m_surface);
		return;
	}

	if (!m_realTarget) {
		return;
	}

	m_realTarget->releaseSurface();
}

D3D9Texture *D3D9Target::getTextureDX()
{
	return static_cast<D3D9Texture*>(getTexture());
}

//--------------------------------------------------------------------------
// class D3D9TargetManager
//--------------------------------------------------------------------------

D3D9TargetManager::D3D9TargetManager()
{
	m_curFrame = 0;
	m_depthStencilSurface = nullptr;
	m_dsWidth = 0;
	m_dsHeight = 0;

	checkFormats();

	D3DFORMAT d3dformat = (D3DFORMAT) MAKEFOURCC('N','V','D','B');

	// check depth bounds test
	if (dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, d3dformat) == S_OK)
	{
		d3d9NVDB = true;
	}

	d3dformat = (D3DFORMAT) MAKEFOURCC('N','U','L','L');

	// check depth bounds test
	if (dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, d3dformat) == S_OK)
	{
		d3d9NULL = true;
	}
}

D3D9TargetManager::~D3D9TargetManager()
{

}

RenderTarget *D3D9TargetManager::allocTarget(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat)
{
	return allocTargetDX(hint, width, height, texformat);
}

void D3D9TargetManager::freeTarget(RenderTarget *target)
{
	D3D9_SCOPELOCK;

	if (target->isWindow()) {
		Errorf("can't free window target");
	}
	D3D9Target *dxtarget = (D3D9Target*)target;
	dxtarget->setHint(RenderTarget::Free, m_curFrame);

	// if is pooled, free it's real target also
	if (!dxtarget->isPooled())
		return;

	dxtarget = dxtarget->m_realTarget;
	if (!dxtarget)
		return;
	freeTarget(dxtarget);
}

bool D3D9TargetManager::isFormatSupport(TexFormat format)
{
	return m_formatSupports[format];
}

void D3D9TargetManager::syncFrame()
{
	m_curFrame ++;

	{ // free real target first
		List<RenderTarget*>::iterator it = m_freeRealTargets.begin();
		while (it != m_freeRealTargets.end()) {
			D3D9Target *target = static_cast<D3D9Target*>(*it);
			target->freeRealTarget();
			++it;
		}
		m_freeRealTargets.clear();
	}

	{ // alloc real target
		List<RenderTarget*>::iterator it = m_realAllocTargets.begin();

		while (it != m_realAllocTargets.end()) {
			D3D9Target *target = static_cast<D3D9Target*>(*it);
			target->allocRealTarget();
			++it;
		}

		m_realAllocTargets.clear();
	}

	D3D9targetpool::iterator it = m_targetPool.begin();

	for (; it != m_targetPool.end(); ++it) {
		Dict<int,D3D9targetseq>& h2 = it->second;

		Dict<int,D3D9targetseq>::iterator it2 = h2.begin();
		for (; it2 != h2.end(); ++it2) {
			D3D9targetseq &ts = it2->second;
			D3D9targetseq::iterator it3 = ts.begin();

			while (it3 != ts.end()) {
				D3D9Target *t = *it3;
#if 0
				if (t->getHint() == Target::PermanentAlloc || t->getHint() == Target::PooledAlloc) {
					++it3;
					continue;
				}
#else
				if (t->getHint() != RenderTarget::Free) {
					++it3;
					continue;
				}
#endif
				if (m_curFrame - t->m_lastFrameUsed > 5) {
					it3 = ts.erase(it3);
					delete t;
					continue;
				}

				t->setHint(RenderTarget::Free, m_curFrame);
				++it3;
			}
		}
	}
}

void D3D9TargetManager::checkFormats()
{
	TypeZeroArray(m_formatSupports);

	for (int i = TexFormat::AUTO+1; i < TexFormat::MAX_NUMBER; i++) {
		TexFormat format = i;
		D3DFORMAT d3dformat;
		if (!trTexFormat(i, d3dformat)) {
			continue;
		}

		HRESULT hr;
		if (format.isDepth() || format.isStencil()) {
			hr = dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				D3DFMT_X8R8G8B8,
				D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE,
				d3dformat);
		} else {
			hr = dx9_api->CheckDeviceFormat(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				D3DFMT_X8R8G8B8,
				D3DUSAGE_RENDERTARGET,
				D3DRTYPE_SURFACE,
				d3dformat);
		}

		if (SUCCEEDED(hr)) {
			m_formatSupports[i] = true;
		}
	}
}

D3D9Target *D3D9TargetManager::allocTargetDX(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat)
{
	D3D9_SCOPELOCK;

	if (!isFormatSupport(texformat)) {
		Errorf("can't support format %s", texformat.toString());
		return nullptr;
	}

	D3D9targetseq &targets = m_targetPool[width][height];

	D3D9Target *result = nullptr;
	AX_FOREACH(D3D9Target *target, targets) {
		if (target->getHint() != RenderTarget::Free && target->getHint() != RenderTarget::TemporalAlloc) {
			continue;
		}

		if (target->getFormat() != texformat) {
			continue;
		}

		if (hint == RenderTarget::PooledAlloc) {
			if (!target->isPooled())
				continue;
		} else {
			if (target->isPooled())
				continue;
		}

		result = target;
		break;
	}

	if (!result) {
		result = new D3D9Target(width, height, texformat, hint == RenderTarget::PooledAlloc);
		targets.push_back(result);
	}

	result->setHint(hint, m_curFrame);
	return result;
}

TexFormat D3D9TargetManager::getSuggestFormat(RenderTarget::SuggestFormat sf)
{
	switch (sf) {
	case RenderTarget::LDR_COLOR:
		return TexFormat::BGRA8;
	case RenderTarget::MDR_COLOR:
		return TexFormat::RGBA16F;
	case RenderTarget::HDR_COLOR:
		return TexFormat::RGBA16F;
	case RenderTarget::SHADOW_MAP:
		return TexFormat::D16;
		if (r_shadowFormat.getInteger() == 0) {
			if (m_formatSupports[TexFormat::DF16]) {
				return TexFormat::DF16;
			}
			return TexFormat::D16;
		}

		if (m_formatSupports[TexFormat::DF24]) {
//				return TexFormat::DF24;
		}
		return TexFormat::D24S8;
	}

	return 0;
}

TexFormat D3D9TargetManager::getNullTargetFormat()
{
	if (m_formatSupports[TexFormat::NULLTARGET]) {
		return TexFormat::NULLTARGET;
	}

	if (m_formatSupports[TexFormat::R5G6B5]) {
		return TexFormat::R5G6B5;
	}

	return getSuggestFormat(RenderTarget::LDR_COLOR);
}

void D3D9TargetManager::onDeviceLost()
{

}

void D3D9TargetManager::onReset()
{

}

IDirect3DSurface9 *D3D9TargetManager::getDepthStencil(int width, int height)
{
	if (m_depthStencilSurface && m_dsWidth >= width && m_dsHeight >= height) {
		return m_depthStencilSurface;
	}

#if 0
	static D3D9target *ds = 0;
	d3d9TargetManager->freeTarget(ds);
	ds = d3d9TargetManager->allocTargetDX(Target::PermanentAlloc, width, height, TexFormat::D24S8);
	m_depthStencilSurface = ds->getSurface();
#endif
	SAFE_RELEASE(m_depthStencilSurface);
	HRESULT hr;
	V(dx9_device->CreateDepthStencilSurface(width, height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, & m_depthStencilSurface, 0));
	m_dsWidth = width;
	m_dsHeight = height;

	return m_depthStencilSurface;
}


#endif

AX_END_NAMESPACE

