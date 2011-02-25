#include "gl_private.h"

AX_BEGIN_NAMESPACE

AX_IMPLEMENT_FACTORY(GL_Driver)

// console command
AX_BEGIN_COMMAND_MAP(GL_Driver)
AX_END_COMMAND_MAP()

GL_Driver::GL_Driver()
{

}

GL_Driver::~GL_Driver()
{

}

bool GL_Driver::initialize()
{
	gl_window = new GL_Window();

	HWND hwnd = gl_window->getHandle();
	HDC hdc = GetDC(hwnd);
	HGLRC hglrc = 0;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);

	if (!bResult) return false; 

	HGLRC tempContext = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		Printf("GLEW is not initialized!");
		return false;
	}

#if 0
	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1) {
		hglrc = wglCreateContextAttribsARB(hdc, 0, attribs);
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hdc, hglrc);
	} else {	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		hglrc = tempContext;
	}
#endif

	//Checking GL version
	const char *GLVersionString = (const char *)glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	hglrc = tempContext;
	if (!hglrc) return false;

	gl_context = hglrc;
	return true;
}


void GL_Driver::finalize()
{

}

const ShaderInfo * GL_Driver::findShaderInfo( const FixedString &key )
{
	return 0;
}

AX_END_NAMESPACE
