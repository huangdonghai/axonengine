/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_D3D9RENDERTARGET_H
#define AX_D3D9RENDERTARGET_H

AX_BEGIN_NAMESPACE

#if 0
//--------------------------------------------------------------------------
// class D3D9Target
//--------------------------------------------------------------------------

class D3D9Target : public RenderTarget
{
public:
	friend class D3D9TargetManager;

	D3D9Target(int width, int height, TexFormat format, bool pooled = false);
	virtual ~D3D9Target();

	// implement ITarget
	virtual Rect getRect();
	virtual Type getType() { return kTexture; }
	virtual void bind();
	virtual void unbind();
	virtual Texture *getTexture();
	virtual bool isTexture() { return true;}
	virtual bool isColorFormat() { return m_d3d9texture->getFormat().isColor(); }
	virtual bool isDepthFormat() { return m_d3d9texture->getFormat().isDepth(); }
	virtual bool isStencilFormat() { return m_d3d9texture->getFormat().isStencil(); }

	virtual void attachDepth(RenderTarget *depth);
	virtual RenderTarget *getDepthAttached() const { return m_depthTarget; }

	virtual void attachColor(int index, RenderTarget *c);
	virtual void detachColor(int index);
	virtual void detachAllColor();
	virtual RenderTarget *getColorAttached(int index) const;

	TexFormat getFormat() const { return m_format; }
	D3D9Texture *getTextureDX();

	void setHint(AllocHint hint, int frame);
	AllocHint getHint() const { return m_storeHint; }
	void copyFramebuffer(const Rect &r);

	// for pooled target
	void allocRealTarget();
	void freeRealTarget();
	bool isPooled() const { return m_isPooled; }
	bool alreadyAllocatedRealTarget() const { return m_realTarget != 0; }

protected:
	IDirect3DSurface9 *getSurface();
	void releaseSurface();

private:
	AllocHint m_storeHint;
	TexFormat m_format;
	TexturePtr m_texture;
	D3D9Texture *m_d3d9texture;
	IDirect3DSurface9 *m_surface;
	int m_width, m_height;

	int m_boundIndex;
	int m_lastFrameUsed;

	D3D9Target *m_depthTarget;
	D3D9Target *m_colorAttached[MAX_COLOR_ATTACHMENT];

	bool m_isNullColor;
	D3D9Target *m_nullColor;

	bool m_isPooled;
	D3D9Target *m_realTarget;	// for pooled target
};


//--------------------------------------------------------------------------
// class D3D9TargetManager
//--------------------------------------------------------------------------

class D3D9TargetManager : public RenderTargetManager {
public:
	D3D9TargetManager();
	virtual ~D3D9TargetManager();

	// implement TargetManager
	virtual RenderTarget *allocTarget(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat);
	virtual void freeTarget(RenderTarget *target);	// only permanent target need be free
	virtual bool isFormatSupport(TexFormat format);
	virtual TexFormat getSuggestFormat(RenderTarget::SuggestFormat sf);

	// for internal use
	TexFormat getNullTargetFormat();
	void syncFrame();
	D3D9Target *allocTargetDX(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat);

	IDirect3DSurface9 *getDepthStencil(int width, int height);

	void onDeviceLost();
	void onReset();

protected:
	void checkFormats();

private:
	typedef Sequence<D3D9Target*>	D3D9targetseq;
	typedef Dict<int,Dict<int,D3D9targetseq> > D3D9targetpool;
	D3D9targetpool m_targetPool;
	int m_curFrame;
	bool m_formatSupports[TexFormat::MAX_NUMBER];

	// we use only one zbuffer for all window
	IDirect3DSurface9 *m_depthStencilSurface;
	int m_dsWidth;
	int m_dsHeight;
};

#endif

AX_END_NAMESPACE

#endif // end guardian