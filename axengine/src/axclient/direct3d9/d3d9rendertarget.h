/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_D3D9RENDERTARGET_H
#define AX_D3D9RENDERTARGET_H

namespace Axon { namespace Render {

	//--------------------------------------------------------------------------
	// class D3D9target
	//--------------------------------------------------------------------------

	class D3D9target : public Target
	{
	public:
		friend class D3D9targetmanager;

		D3D9target(int width, int height, TexFormat format, bool pooled = false);
		virtual ~D3D9target();

		// implement ITarget
		virtual Rect getRect();
		virtual Type getType() { return kTexture; }
		virtual void bind();
		virtual void unbind();
		virtual Texture* getTexture();
		virtual bool isTexture() { return true;}
		virtual bool isColorFormat() { return m_texture->getFormat().isColor(); }
		virtual bool isDepthFormat() { return m_texture->getFormat().isDepth(); }
		virtual bool isStencilFormat() { return m_texture->getFormat().isStencil(); }

		virtual void attachDepth(Target* depth);
		virtual Target* getDepthAttached() const { return m_depthTarget; }

		virtual void attachColor(int index, Target* c);
		virtual void detachColor(int index);
		virtual void detachAllColor();
		virtual Target* getColorAttached(int index) const;

		TexFormat getFormat() const { return m_format; }
		D3D9texture* getTextureDX();

		void setHint(AllocHint hint, int frame);
		AllocHint getHint() const { return m_storeHint; }
		void copyFramebuffer(const Rect& r);

		// for pooled target
		void allocRealTarget();
		void freeRealTarget();
		bool isPooled() const { return m_isPooled; }
		bool alreadyAllocatedRealTarget() const { return m_realTarget != 0; }

	protected:
		IDirect3DSurface9* getSurface();
		void releaseSurface();

	private:
		AllocHint m_storeHint;
		TexFormat m_format;
		D3D9texture* m_texture;
		IDirect3DSurface9* m_surface;
		int m_width, m_height;

		int m_boundIndex;
		int m_lastFrameUsed;

		D3D9target* m_depthTarget;
		D3D9target* m_colorAttached[MAX_COLOR_ATTACHMENT];

		bool m_isNullColor;
		D3D9target* m_nullColor;

		bool m_isPooled;
		D3D9target* m_realTarget;	// for pooled target
	};


	//--------------------------------------------------------------------------
	// class D3D9targetmanager
	//--------------------------------------------------------------------------

	class D3D9targetmanager : public TargetManager {
	public:
		D3D9targetmanager();
		virtual ~D3D9targetmanager();

		// implement TargetManager
		virtual Target* allocTarget(Target::AllocHint hint, int width, int height, TexFormat texformat);
		virtual void freeTarget(Target* target);	// only permanent target need be free
		virtual bool isFormatSupport(TexFormat format);
		virtual TexFormat getSuggestFormat(Target::SuggestFormat sf);

		// for internal use
		TexFormat getNullTargetFormat();
		void syncFrame();
		D3D9target* allocTargetDX(Target::AllocHint hint, int width, int height, TexFormat texformat);

		IDirect3DSurface9* getDepthStencil(int width, int height);

		void onDeviceLost();
		void onReset();

	protected:
		void checkFormats();

	private:
		typedef Sequence<D3D9target*>	D3D9targetseq;
		typedef Dict<int,Dict<int,D3D9targetseq> > D3D9targetpool;
		D3D9targetpool m_targetPool;
		int m_curFrame;
		bool m_formatSupports[TexFormat::MAX_NUMBER];

		// we use only one zbuffer for all window
		IDirect3DSurface9* m_depthStencilSurface;
		int m_dsWidth;
		int m_dsHeight;
	};

}} // namespace Axon::Render

#endif // end guardian