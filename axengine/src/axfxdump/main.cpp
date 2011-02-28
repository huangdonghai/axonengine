#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <Cg/cg.h>    /* Can't include this?  Is Cg Toolkit installed? */
#include <Cg/cgGL.h>  /* 3D API specific Cg runtime API for OpenGL */


static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

std::string formatShader(const std::string &str)
{
	std::string result;
	result += "\n            ";            
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] != '\n') {
			result.push_back(str[i]);
		} else {
			result += "\n            ";            
		}
	}
	return result;
}

int main(int argc, char **argv)
{
	std::vector<const char*> args;
	const char *filename = 0;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == 'D') {
			args.push_back(argv[i]);
		} else {
			filename = argv[i];
		}
	}
	args.push_back(0);

	if (!filename) {
		printf("usage: axfxdump -Ddefine filename\n");
		return EXIT_FAILURE;
	}

	std::wstring ws = L"axfxdump";
	WNDCLASSEXW wcex;	
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HINSTANCE)::GetModuleHandle(NULL);
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)WHITE_BRUSH;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = ws.c_str();
	wcex.hIconSm = 0;

	if (!::RegisterClassExW(&wcex)) {
		printf("cann't create OpenGL window");
		return EXIT_FAILURE;
	}

	DWORD dwStyle,dwExStyle;
	dwExStyle = 0;		
	dwStyle = WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;

	HWND hwnd = ::CreateWindowExW(dwExStyle, ws.c_str(), ws.c_str(), dwStyle,
		0, 0, 640,480, NULL, NULL, (HINSTANCE)GetModuleHandleW(NULL), NULL);

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

	hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		printf("GLEW is not initialized!");
		return false;
	}

	//Checking GL version
	const char *GLVersionString = (const char *)glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	CGcontext cgcontext = cgCreateContext();
	cgGLRegisterStates(cgcontext);

	CGeffect effect = cgCreateEffectFromFile(cgcontext, filename, &args[0]);

	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR) {
		if (error == CG_COMPILER_ERROR) {
			printf("%s:\n%s", string, cgGetLastListing(cgcontext));
		} else {
			printf("Cg Error: %s", string);
		}
		return EXIT_FAILURE;
	}

	// compiled success, dump
	CGtechnique technique = cgGetFirstTechnique(effect);
	while (technique) {
		// Do something with each technique
		const char * techname = cgGetTechniqueName(technique);
		CGpass pass = cgGetFirstPass(technique);

		printf("technique %s {\n", techname);
		while (pass) {
			const char *passname = cgGetPassName(pass);
			printf("    pass %s {\n", passname);
			CGprogram vs = cgGetPassProgram(pass, CG_VERTEX_DOMAIN);
			CGprogram ps = cgGetPassProgram(pass, CG_FRAGMENT_DOMAIN);
			std::string vs_str = cgGetProgramString(vs, CG_COMPILED_PROGRAM);
			std::string ps_str = cgGetProgramString(ps, CG_COMPILED_PROGRAM);
			printf("        VertexShader = {\n%s\n        }\n", formatShader(vs_str).c_str());
			printf("        PixelShader = {\n%s\n        }\n", formatShader(ps_str).c_str());
			printf("    }\n");
			pass = cgGetNextPass(pass);
		}

		printf("}\n");
		technique = cgGetNextTechnique(technique);
	}

	return EXIT_SUCCESS;
}
