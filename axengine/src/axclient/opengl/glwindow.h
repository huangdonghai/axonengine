/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GLSTATE_H
#define AX_GLSTATE_H

AX_BEGIN_NAMESPACE

	class GLprimitive;

	class GLwindow : public RenderTarget {
	public:
		GLwindow(const String& name);
		GLwindow(handle_t wndId, const String& name);
		virtual ~GLwindow();

		// implement ITarget
		virtual Rect getRect();
		virtual Type getType() { return RenderTarget::kWindow; }
		virtual void bind();
		virtual void unbind();
		virtual bool isWindow() { return true;}


		// wrap OpenGL state function
		void initialize();
		void finalize();
		void shareList(const GLwindow* other);
		void swapBuffers();
#if 0
		void getSize(Int& width, Int& height);
		Point getClientSize();
#endif
		void initGLRC();

	protected:

	protected:
		// window, dc, gl context
		String m_name;					// just for easy debug

	#ifdef _WIN32
		HWND m_wndId;
#if 0
		HGLRC m_glContext;
#endif
		HDC m_hdc;
		int m_pixelFormat;
		PIXELFORMATDESCRIPTOR m_pfd;
	#endif
		
	};

AX_END_NAMESPACE


#endif // AX_GLSTATE_H
