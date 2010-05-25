/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE


//------------------------------------------------------------------------------
//	class GLwindow
//
//	wrap OpenGL state operation
//------------------------------------------------------------------------------

GLwindow::GLwindow(const String &name)
	: m_hdc(NULL)
	, m_name(name)
{
	// create context
	m_wndId = GLdriver::createGLWindow(m_name).to<HWND>();
#if 0
	initGLRC();
#endif
}

GLwindow::GLwindow(Handle wndId, const String &name) : m_wndId(wndId.to<HWND>()), m_name(name) {
#if 0
	initGLRC();
	initialize();
	unbind();
	glInternalWindow->bind();
#endif
	m_hdc = ::GetDC((HWND)m_wndId);

	int num_pfds = ::DescribePixelFormat(m_hdc, 0, 0, NULL);
	BYTE desiredColorBits, desiredDepthBits, desiredStencilBits;

	desiredColorBits = 32;
	desiredDepthBits = 24;
	desiredStencilBits = 8;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		desiredColorBits,	// color bits
		0,0,0,0,0,0,		// r, g, b bits & shift
		0,0,				// alpha bits & shift
		0,0,0,0,0,		// accum
		desiredDepthBits,	// depth bits
		desiredStencilBits,	// stencil bits
		0,					// aux buffer
		PFD_MAIN_PLANE,		// 
		0,
		0,0,0
	};

	m_pixelFormat = ::ChoosePixelFormat(m_hdc, &pfd);

	if (0 == m_pixelFormat)
		return;

	// create res context
	if (!::SetPixelFormat(m_hdc, m_pixelFormat, &pfd)) {
		DWORD err = ::GetLastError();
		return;
	}

	m_pfd = pfd;
}

GLwindow::~GLwindow()
{}


void GLwindow::initGLRC() {
	m_hdc = ::GetDC((HWND)m_wndId);

	int num_pfds = ::DescribePixelFormat(m_hdc, 0, 0, NULL);
	BYTE desiredColorBits, desiredDepthBits, desiredStencilBits;

	desiredColorBits = 32;
	desiredDepthBits = 24;
	desiredStencilBits = 8;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		desiredColorBits,	// color bits
		0,0,0,0,0,0,		// r, g, b bits & shift
		0,0,				// alpha bits & shift
		0,0,0,0,0,		// accum
		desiredDepthBits,	// depth bits
		desiredStencilBits,	// stencil bits
		0,					// aux buffer
		PFD_MAIN_PLANE,		// 
		0,
		0,0,0
	};

	m_pixelFormat = ::ChoosePixelFormat(m_hdc, &pfd);
	DescribePixelFormat(m_hdc, m_pixelFormat, pfd.nSize, &pfd);

	if (0 == m_pixelFormat)
		return;

	// create res context
	if (!::SetPixelFormat(m_hdc, m_pixelFormat, &pfd)) {
		DWORD err = ::GetLastError();
		return;
	}

	m_pfd = pfd;

	if ((glContext = wglCreateContext(m_hdc)) == NULL) {
		DWORD err = ::GetLastError();
		return;
	}

	// make resource context as current
	if (!wglMakeCurrent(m_hdc, glContext)) {
		DWORD err = ::GetLastError();
		return;
	}

#if 0
	if (glInternalWindow)
		shareList(glInternalWindow);
#endif
	if (glClampColorARB) {
		glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
		glClampColorARB(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
		glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
	}

	glDepthFunc(GL_LEQUAL);
}

Rect GLwindow::getRect() {
	RECT r;

	::GetClientRect(m_wndId, &r);

	return Rect(r.left, r.top, r.right, r.bottom);
}


void GLwindow::initialize() {
	// find extensions
	glActiveTexture(GL_TEXTURE0);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// set pixel pack alignment to 1, MUST SET FOR TEXTURE UPLOAD!!!!
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void GLwindow::shareList(const GLwindow *other) {
#if 0
	// share resources between resource context and render context
	if (!wglShareLists(m_glContext, other->m_glContext)) {
		DWORD err = ::GetLastError();
		Errorf("GLwindow::shareList: error");
		return;
	}
	if (!wglShareLists(other->m_glContext, m_glContext)) {
		DWORD err = ::GetLastError();
		Errorf("GLwindow::shareList: error");
		return;
	}
#endif
}

void GLwindow::bind() {
#if 0
	// check wireframe and fill mode
	if (r_wireframe->isModified()) {
		if (r_wireframe->getInteger())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		r_wireframe->clearModifiedFlag();
	}
#endif
	if (m_hdc == 0) {
		m_hdc = ::GetDC(m_wndId);
	}

	// make as current
	if (!wglMakeCurrent(m_hdc, glContext)) {
		DWORD err = ::GetLastError();
		Errorf("GLwindow::bind: wglMakeCurrent error");
	}

	GLrender::checkErrors();
}

void GLwindow::unbind() {
	if (!wglMakeCurrent(NULL, NULL)) {
		Errorf("GLwindow::unbind: error");
	}

	if (!::ReleaseDC(m_wndId, m_hdc)) {
		Errorf("GLwindow::unbind: error");
	}

	m_hdc = nullptr;
}

void GLwindow::swapBuffers() {
	if (r_vsync.isModified()) {
		r_vsync.clearModifiedFlag();

		if (SUPPORT_WGL_EXT_swap_control) {
			wglSwapIntervalEXT(r_vsync.getBool());
		}
	}

	if (!::SwapBuffers(m_hdc)) {
		Errorf("GLwindow::swapBuffers: swapBuffers error");
	}
}
#if 0
void
GLwindow::getSize(Int &width, Int &height) {
	RECT r;

	::GetClientRect(m_wndId, &r);

	width = r.right - r.left;
	height = r.bottom - r.top;
}

Point
GLwindow::getClientSize() {
	return gDriver->getWindowClientSize(m_wndId);
}
#endif
void GLwindow::finalize() {
}

AX_END_NAMESPACE

