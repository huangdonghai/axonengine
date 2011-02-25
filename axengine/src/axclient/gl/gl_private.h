#ifndef AX_GL_PRIVATE_H
#define AX_GL_PRIVATE_H

#include "../private.h"
#define GLEW_STATIC
extern "C" {
	#include <GL/glew.h>
	#include <GL/wglew.h>
};

#include "gl_shader.h"
#include "gl_window.h"
#include "gl_driver.h"
#include "gl_state.h"
#include "gl_api.h"

AX_BEGIN_NAMESPACE

extern GL_Driver *gl_driver;
extern GL_Window *gl_window;
extern HGLRC gl_context; // OpenGL Rendering Context

AX_END_NAMESPACE

#endif // AX_GL_PRIVATE_H