/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

namespace Axon {

	AX_BEGIN_CLASS_MAP(axopengl)
		AX_CLASS_ENTRY("driver", GLdriver)
	AX_END_CLASS_MAP()

	GLwindow* glInternalWindow;
	HGLRC glContext;
	GLdriver* glDriver;
	IDriver::Info* glDriverInfo;
	GLthread* glThread;
	GLprimitivemanager* glPrimitiveManager;
	GLframebuffermanager* glFramebufferManager;
	GLshadermanager* glShaderManager;
	CGcontext glCgContext;
	Material* glNullMat;
	GLpostprocess* glPostprocess;

	Interaction* gCurInteraction;

	GLfontrender* glFontRender;


} // namespace Axon

