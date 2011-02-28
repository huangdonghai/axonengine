#ifndef AX_GL_PRIVATE_H
#define AX_GL_PRIVATE_H

#include "../private.h"
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/wglew.h>
#include <Cg/cg.h>    /* Can't include this?  Is Cg Toolkit installed? */
#include <Cg/cgGL.h>  /* 3D API specific Cg runtime API for OpenGL */

#include "gl_shader.h"
#include "gl_window.h"
#include "gl_driver.h"
#include "gl_state.h"
#include "gl_api.h"

AX_BEGIN_NAMESPACE

extern GL_Driver *gl_driver;
extern GL_Window *gl_window;
extern HGLRC gl_context; // OpenGL Rendering Context
extern CGcontext cg_context;

extern GL_ShaderManager *gl_shaderManager;
extern GL_StateManager *gl_stateManager;

AX_END_NAMESPACE

#endif // AX_GL_PRIVATE_H