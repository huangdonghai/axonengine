/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GLFRAMEBUFFER_H
#define AX_GLFRAMEBUFFER_H

AX_BEGIN_NAMESPACE

	class GLframebuffer;

	//--------------------------------------------------------------------------
	// class GLtarget
	//--------------------------------------------------------------------------

	class GLtarget : public RenderTarget {
	public:
		friend class GLframebuffer;

		GLtarget(GLframebuffer *fb, int width, int height, TexFormat format, bool asrenderbuffer = false);
		virtual ~GLtarget();

		// implement ITarget
		virtual Rect getRect();
		virtual Type getType();
		virtual void bind();
		virtual void unbind();
		virtual Texture *getTexture() { return m_texture; }
		virtual bool isTexture() { return true;}
		virtual bool isColorFormat() { return m_texture->getFormat().isColor(); }
		virtual bool isDepthFormat() { return m_texture->getFormat().isDepth(); }
		virtual bool isStencilFormat() { return m_texture->getFormat().isStencil(); }

		virtual void attachDepth(RenderTarget *depth);
		virtual RenderTarget *getDepthAttached() const { return m_depthTarget; }

		virtual void attachColor(int index, RenderTarget *c);
		virtual void detachColor(int index);
		virtual void detachAllColor();
		virtual RenderTarget *getColorAttached(int index) const;

		GLframebuffer *getFramebuffer() const { return m_framebuffer; }
		GLtexture *getTextureGL() { return m_texture; }

		void setHint(AllocHint hint, int frame);
		AllocHint getHint() const { return m_storeHint; }

		void attachDepth(GLtarget *depth);
		GLtarget *getDepthAttachedGL() const { return m_depthTarget; }
		GLtarget *getColorAttachedGL(int index) const;

	private:
		AllocHint m_storeHint;
		bool m_isRenderBuffer;
		GLframebuffer *m_framebuffer;
		GLtexture *m_texture;
		GLenum m_renderBuffer;
		int m_width;
		int m_height;
		int m_lastFrameUsed;

		int m_boundIndex;

		GLtarget *m_depthTarget;
		GLtarget *colorAttached[MAX_COLOR_ATTACHMENT];
	};

	//--------------------------------------------------------------------------
	// class GLframebuffer
	//--------------------------------------------------------------------------

	class GLframebuffer {
	public:
		enum { MAX_DRAW_BUFFERS = 4 };

		GLframebuffer(int width, int height);
		~GLframebuffer();

		GLtarget *allocTarget(RenderTarget::AllocHint hint, TexFormat texformat);
		void freeTarget(RenderTarget *target);	// only permanent target need be free

		void newFrame(int frame);
		void endFrame();

		uint_t getObject();

		void bind(GLtarget *target);
		void unbind();

		GLtexture *getBoundColor() const;
		GLtexture *getBoundDepth() const;

		void blitColor(GLtexture *tex);
		void blitDepth(GLtexture *tex);

		Rect getRect() const { return Rect(0,0,m_width,m_height); }

	protected:
		void checkStatus(GLenum target);
		bool checkColorFormat(TexFormat format);

		GLenum bindColor(GLtarget *target);
		void bindDepth(GLtarget *target);
		void bindStencil(GLtarget *target);

	private:
		uint_t m_object;
		uint_t m_object2;

		GLtarget *m_boundColor[MAX_DRAW_BUFFERS];
		GLtarget *m_boundDepth;
		GLtarget *m_boundStencil;
		GLtarget *m_boundTarget;

		GLsizei m_width, m_height;

#if 0
		bool m_dirty;
#endif
		// render to texture's target
		typedef Sequence<GLtarget*>	GLtargetseq;
		GLtargetseq m_targetpool;
	};


	//--------------------------------------------------------------------------
	// class GLframebuffermanager
	//--------------------------------------------------------------------------

	class GLframebuffermanager : public TargetManager {
	public:
		GLframebuffermanager();
		~GLframebuffermanager();

		// implement TargetManager
		virtual RenderTarget *allocTarget(RenderTarget::AllocHint hint, int width, int height, TexFormat texformat);
		virtual void freeTarget(RenderTarget *target);	// only permanent target need be free
		virtual bool isFormatSupport(TexFormat format);
		virtual TexFormat getSuggestFormat(RenderTarget::SuggestFormat sf);

		void initialize();
		void finalize();

		void preFrame();
		GLframebuffer *getFramebuffer(int width, int height);
#if 0
		GLtarget *getColorTarget(int width, int height);
		GLtarget *getDepthTarget(int width, int height);
#endif
		void endFrame();

		int getFrame() const { return m_curFrame; }

	protected:
		bool checkFormat(TexFormat format);
		void checkFormats();

	private:
		typedef Dict< int, Dict<int, GLframebuffer*> > GLframebufferDict;

		int m_curFrame;
		GLframebufferDict m_framebuffers;
		bool m_formatSupports[TexFormat::MAX_NUMBER];
	};

AX_END_NAMESPACE

#endif // end guardian

