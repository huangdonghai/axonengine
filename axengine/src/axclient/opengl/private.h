/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_GL2_H
#define AX_RENDER_GL2_H


#include "../private.h"

#include "gl.h"
#include "glext.h"

#ifdef _WIN32
//#	include <GL/wgl.h>
#	include "wglext.h"
#endif

#ifdef __linux__
#	include <GL/glx.h>
#	include "./opengl/glxext.h"
#endif

#include <Cg/cg.h>     /* Cg Core API: Can't include this?  Is Cg Toolkit installed! */
#include <Cg/cgGL.h>   /* Cg OpenGL API (part of Cg Toolkit) */


using namespace Axon;

#include "gltexture.h"
#include "glshader.h"
#include "glframebuffer.h"
#include "glvertexbuffer.h"
#include "glrender.h"
#include "glwindow.h"
#include "glprimitive.h"
#include "glthread.h"
#include "gldriver.h"
#include "glpostprocess.h"


namespace Axon {

	enum VertexAttribBindLocation {
		VABL_tangent	= 1,
		VABL_binormal	= 4,
		VABL_lightmapTc	= 7,
	};

	// some global object
	extern GLwindow* glInternalWindow;
	extern HGLRC					glContext;
	extern GLdriver* glDriver;
	extern IRenderDriver::Info* glDriverInfo;
	extern GLthread* glThread;
	extern CGcontext				glCgContext;
	extern GLprimitivemanager* glPrimitiveManager;
	extern GLframebuffermanager* glFramebufferManager;
	extern GLshadermanager* glShaderManager;
	extern GLpostprocess* glPostprocess;

	extern Material* glNullMat;
	extern Interaction* gCurInteraction;

	extern GLfontrender* glFontRender;

} // namespace Axon


#endif // AX_RENDER_GL2_H
