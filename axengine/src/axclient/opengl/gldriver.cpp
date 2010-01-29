/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE

	AX_IMPLEMENT_FACTORY(GLdriver)

	// declare OpenGL functions' pointer
	#define GL_EXT(name) bool SUPPORT##name = 0;
	#define GL_FNC(ext,ret,func,parms)  ret (STDCALL *func)parms;
	#include "glfunc.h"
	#undef GL_EXT
	#undef GL_FNC

	#if !defined _WIN32
	#	define OPENGL_LIB_NAME "libGL.so"
	#else
	#	define OPENGL_LIB_NAME "opengl32"
	#endif // !defined _WIN32


	//------------------------------------------------------------------------------
	//	class GLdriver
	//
	//	OpenGL driver
	//------------------------------------------------------------------------------

	GLdriver::GLdriver()
		: m_initialized(false)
		, m_glLibHandle(0)
		, m_isSelectMode(false)
		, m_foundExts(false)
	{
		m_queryId = 0;
	}

	GLdriver::~GLdriver() {
	}

	bool GLdriver::findExtension(const char *extname) {
		bool found = strstr(glDriverInfo->extension.c_str(), extname) != NULL;

	#ifdef OS_WIN
		if (!found) {
			if (!strcmp("WGL_EXT_extensions_string", extname))
				return true;
		}

		if (!found && wglGetExtensionsStringEXT) {
			found = strstr(wglGetExtensionsStringEXT(), extname) != NULL;
		}
	#endif

#if 0
		if (found)
			Debugf("GLdriver::FindExtension: OpenGL support: %s\n", extname);
		else
			Debugf("GLdriver::FindExtension: OpenGL don't support: %s\n", extname);
#endif
		return found;
	}

	void GLdriver::findFunction(void*& func, const char *name, const char *extname, bool extsupport) {
		if (!extsupport)
			return;

		if (!func)
			func = OsUtil::getProcEntry(m_glLibHandle, name);
		
	#ifdef _WIN32
		if (!func && wglGetProcAddress)
			func = wglGetProcAddress(name);
	#endif
		
		if (!func) {
			Debugf("GLdriver::FindFunction: Cann't find proc address '%s' for extension '%s'\n", name, extname);
		}
	}

	void GLdriver::findFunctions(bool allowExt) {
		if (allowExt && !m_foundExts) {
	#ifdef _WIN32
			glDriverInfo->extension += " WGL_EXT_extensions_string ";
			findFunction((void*&)wglGetExtensionsStringEXT, "wglGetExtensionsStringEXT", "WGL_EXT_extensions_string", true);
			if (wglGetExtensionsStringEXT) {
				glDriverInfo->extension += wglGetExtensionsStringEXT();
			}
	#endif
		}

	#define GL_EXT(name) if (allowExt && !SUPPORT##name) SUPPORT##name = findExtension((#name)+1);
	#define GL_FNC(ext,ret,func,parms) findFunction(*(void**)&func, #func, #ext, SUPPORT##ext);
	#include "glfunc.h"
	#undef GL_EXT
	#undef GL_FNC
	}

	void GLdriver::initialize() {
		if (m_initialized)
			return;

		// create some object
		g_renderDriver = this;

		Printf("..Initializing GLdriver...\n");
		m_glLibHandle = OsUtil::loadSysDll(OPENGL_LIB_NAME);

		if (!m_glLibHandle) {
			Errorf("GLdriver::Initialize: Cannot load OpenGL library %s", OPENGL_LIB_NAME);
			return;
		}

		// HACK
		SUPPORT_GL = true;

		findFunctions(false);

		glInternalWindow = new GLwindow("glrendertarget");
		glInternalWindow->initGLRC();

		GLrender::checkErrors();

		// init device info
		Info *glDriverInfo = new Info();
		glDriverInfo = glDriverInfo;
		glDriverInfo->driverType = Info::OpenGL;
		glDriverInfo->highestQualitySupport = ShaderQuality::Low;
		glDriverInfo->vendor = (const char*)glGetString(GL_VENDOR);
		glDriverInfo->renderer = (const char*)glGetString(GL_RENDERER);
		glDriverInfo->version = (const char*)glGetString(GL_VERSION);
		glDriverInfo->extension = (const char*)glGetString(GL_EXTENSIONS);

		GLrender::checkErrors();

		uint_t ver =(glDriverInfo->version[0] - '0') << 8;
		ver += glDriverInfo->version[2] - '0';

		// HACK
		if (ver >= 0x0102)
			SUPPORT_GL_1_2 = true;

		if (ver >= 0x0103)
			SUPPORT_GL_1_3 = true;

		if (ver >= 0x0104)
			SUPPORT_GL_1_4 = true;

		if (ver >= 0x0105)
			SUPPORT_GL_1_5 = true;

		if (ver >= 0x0200)
			SUPPORT_GL_2_0 = true;

		if (!SUPPORT_GL_2_0)
			Errorf("GLdriver::initialize: we need OpenGL 2.0 support!");

		// set default state, must after find extension functions
		findFunctions(true);
		glInternalWindow->initialize();
		gFrameWindow = glInternalWindow;

		GLrender::checkErrors();


		// check some must have extensions support
		glDriverInfo->caps = 0;

		if (SUPPORT_GL_EXT_texture_compression_s3tc) {
			glDriverInfo->caps |= Info::DXT;
		}

		if (SUPPORT_GL_EXT_framebuffer_object && SUPPORT_GL_EXT_packed_depth_stencil && SUPPORT_GL_EXT_framebuffer_blit && SUPPORT_GL_EXT_framebuffer_multisample) {
			glDriverInfo->highestQualitySupport = ShaderQuality::Middle;
		}

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glDriverInfo->maxTextureSize);

		glDriverInfo->max3DTextureSize = 0;
		glDriverInfo->maxCubeMapTextureSize = 0;
		glDriverInfo->maxTextureUnits = 1;
		glDriverInfo->maxTextureCoords = 0;
		glDriverInfo->maxTextureImageUnits = 0;

		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &glDriverInfo->max3DTextureSize);
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &glDriverInfo->maxCubeMapTextureSize);
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &glDriverInfo->maxTextureUnits);

		if (glDriverInfo->maxTextureUnits < 2) {
			Errorf("GLdriver::initialize: the OpenGL hardware only have 1 TMU, but expect 2 TMUs.");
		}

		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &glDriverInfo->maxTextureCoords);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &glDriverInfo->maxTextureImageUnits);

		int maxva = 0;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &maxva);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxva);


		GLrender::checkErrors();

		glPrimitiveManager = new GLprimitivemanager();
		glPrimitiveManager->initialize();

		glFramebufferManager = new GLframebuffermanager();
		glFramebufferManager->initialize();

		g_targetManager = glFramebufferManager;

		glReadBuffer(GL_BACK);

		GLrender::checkErrors();
		// init Cg something
		glCgContext = cgCreateContext();

		GLrender::checkErrors();
		GLrender::checkForCgError();

		cgGLSetDebugMode(CG_FALSE);

		cgSetParameterSettingMode(glCgContext, CG_DEFERRED_PARAMETER_SETTING);
//		cgSetParameterSettingMode(glCgContext, CG_IMMEDIATE_PARAMETER_SETTING);
		cgGLRegisterStates(glCgContext);
		cgGLSetManageTextureParameters(glCgContext, CG_FALSE);
		cgSetLockingPolicy(CG_NO_LOCKS_POLICY);
#if 0
		cgGLSetOptimalOptions(CG_PROFILE_ARBVP1);
		cgGLSetOptimalOptions(CG_PROFILE_ARBFP1);
		cgGLSetOptimalOptions(CG_PROFILE_GLSLV);
		cgGLSetOptimalOptions(CG_PROFILE_GLSLF);
		cgGLSetOptimalOptions(CG_PROFILE_VP30);
		cgGLSetOptimalOptions(CG_PROFILE_FP30);
		cgGLSetOptimalOptions(CG_PROFILE_VP40);
		cgGLSetOptimalOptions(CG_PROFILE_FP40);
		cgSetAutoCompile(glCgContext, CG_COMPILE_LAZY);
#endif

		GLrender::checkErrors();
		GLrender::checkForCgError();

#if 0
		CGstate state = cgGetFirstState(glCgContext);
		while (state) {
			const char *n = cgGetStateName(state);
			Printf("%s ", n);
			state = cgGetNextState(state);
		}
#endif

		glShaderManager = new GLshadermanager();
		g_shaderManager = glShaderManager;
		GLtexture::initFactory();

		m_initialized = true;

		g_cmdSystem->registerHandler(this);

		glThread = new GLthread();
		glThread->initialize();
		glThread->startThread();

		Printf("..Initialized GLdriver\n");

		return;
	}

	void GLdriver::finalize() {
		Printf("..Finalizing GLdriver...\n");

		g_cmdSystem->registerHandler(this);

		GLtexture::finalizeFactory();
		g_shaderManager = 0;
		SafeDelete(glShaderManager);

		glFramebufferManager->finalize();
		SafeDelete(glFramebufferManager);

		glPrimitiveManager->finalize();
		SafeDelete(glPrimitiveManager);

		glInternalWindow->finalize();
		SafeDelete(glInternalWindow);

		Printf("..Finalized GLdriver\n");
	}

	const IRenderDriver::Info *GLdriver::getDriverInfo() {
		return glDriverInfo;
	}

	uint_t GLdriver::getBackendCaps() {
		return 0;
	}

	void GLdriver::loadMatrix(GLenum mode, const Matrix4 &m) {
		glMatrixMode(mode);
		glLoadMatrixf(m.c_ptr());
	}


	static double select_time;
	static Matrix4 select_viewmatrix;
	void GLdriver::beginSelect(const RenderCamera &view) {
		select_time = OsUtil::getTime();

		loadMatrix(GL_PROJECTION, view.getProjMatrix());
		loadMatrix(GL_MODELVIEW, view.getViewMatrix());

		select_viewmatrix = view.getViewMatrix();

		//
		glSelectBuffer(SELECT_BUFFER_SIZE, m_selectBuffer);

		GLint r = glRenderMode(GL_SELECT);

		m_isSelectMode = true;

		glInitNames();
		glPushName(-1);
	}

	void GLdriver::loadSelectId(int id) {
		glLoadName(id);
	}

	void GLdriver::testActor(RenderEntity *re) {
#if 0
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf(re->getModelMatrix());
#else
		loadMatrix(GL_MODELVIEW, select_viewmatrix * re->getModelMatrix());
#endif
		Primitives prims = re->getHitTestPrims();

		Primitives::iterator it;

		for (it = prims.begin(); it != prims.end(); ++it) {
			Primitive *prim = *it;

			testPrimitive(prim);
		}

#if 0
		glPopMatrix();
#else
		loadMatrix(GL_MODELVIEW, select_viewmatrix);
#endif
	}

	static inline void testMesh(MeshPrim *mesh) {
		GLrender::bindVertexBuffer(VertexType::kVertex, 0, (uintptr_t)mesh->getVertexesPointer());
		GLrender::checkErrors();

		int count = mesh->getActivedIndexes();
		if (!count) {
			count = mesh->getNumIndexes();
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, mesh->getIndexPointer());
	}

	static inline void testLine(LinePrim *line) {
		GLrender::bindVertexBuffer(VertexType::kDebug, 0, (uintptr_t)line->getVertexesPointer());

		int count = line->getActivedIndexes();
		if (!count) {
			count = line->getNumIndexes();
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, line->getIndexPointer());
	}

	void GLdriver::testPrimitive(Primitive *prim) {
		int id = prim->getCachedId();

		if (id <= 0) {
			MeshPrim *mesh = dynamic_cast<MeshPrim*>(prim);

			if (mesh) {
				testMesh(mesh);
				return;
			}

			LinePrim *line = dynamic_cast<LinePrim*>(prim);

			if (line) {
				testLine(line);
				return;
			}
			return;
		}

		GLprimitive *glprim = glPrimitiveManager->getPrimitive(id);

		if (!glprim)
			return;

		GLgeometry *glgeo = dynamic_cast<GLgeometry*>(glprim);
		if (!glgeo)
			return;

		glgeo->bindVertexBuffer();
		glgeo->drawElements();
	}

	struct GLHitRecord {
		GLuint numNames;
		GLuint minz;
		GLuint maxz;
		GLuint name[1];
		static GLfloat mappingz(GLuint v) {
			double r = (double)v / (double)0xffffffff;
			return r * 2.0f - 1.0f;
		}
	};

	HitRecords GLdriver::endSelect() {
		// unbind vertex buffer and index buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		HitRecords selectBuffer;

		GLint recordCount = glRenderMode(GL_RENDER);
		m_isSelectMode = false;
		if (!recordCount)
			return selectBuffer;

		GLHitRecord *p = (GLHitRecord*)m_selectBuffer;
		GLHitRecord *end = (GLHitRecord*)m_selectBuffer + recordCount;

		int hitCount = 0;
		while (p != end) {
			AX_ASSERT(p<end);

			HitRecord r;
			r.name = p->name[0];
			r.minz = GLHitRecord::mappingz(p->minz);
			r.maxz = GLHitRecord::mappingz(p->maxz);
			selectBuffer.push_back(r);

			// next
			AX_ASSERT(p->numNames==1);
			p++;
			hitCount++;
		}

		GLrender::checkErrors();

		select_time = OsUtil::getTime() - select_time;

		Printf("select time: %f\n", select_time);

		return selectBuffer;
	}

	bool GLdriver::isHDRRendering() {
		return r_hdr->getInteger() && glDriverInfo->caps & Info::HDR;
	}

	RenderTarget *GLdriver::createWindowTarget(handle_t wndId, const String &name) {
		GLwindow *state = new GLwindow(wndId, name);
		AX_ASSERT(state);
		return state;
	}

	// console command
	AX_BEGIN_COMMAND_MAP(GLdriver)
		AX_COMMAND_ENTRY("dumpTex",	dumpTex_f)
	AX_END_COMMAND_MAP()


	static const String getImageFilename(const char *ext) {
		String filename;
		char *pattern;
		int i;

		/* find a file name */
		pattern = "images/img%i%i%i%i.%s";
		for (i = 0; i<=9999; i++) {
			StringUtil::sprintf(filename, pattern, i/1000, (i / 100) % 10, (i / 10) % 10, i%10, ext);
			if (g_fileSystem->readFile(filename, NULL) <= 0)
				break;
		}
		if (i==10000) {
			Printf("getImageFilename: Couldn't create a file\n");
			return String();
		}

		return filename;
	}

	void GLdriver::dumpTex_f(const CmdArgs &params) {
		if (params.tokened.size() != 2) {
			Printf("Usage: writeTexture <tex_name>\n");
			return;
		}
		const String &imagename = params.tokened[1];

		TexturePtr tex = Texture::load(imagename);

		if (!tex) {
			Printf("Cann't found texture '%s'\n", imagename.c_str());
			return;
		}

		int width, height, internal_format;

		GLtexture *gltex = dynamic_cast<GLtexture*>(tex.get());
		AX_ASSERT(gltex);

#if 0
		gInternalWindow->bindTexture(0, gltex);
#else
		glBindTexture(GL_TEXTURE_2D, gltex->getObject());
#endif
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);

		byte_t *pixels = NULL;
		if (gltex->getFormat().isFloat()) {
			pixels = new byte_t[width*height*8];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_HALF_FLOAT_ARB, pixels);
			String filename = getImageFilename("exr");
#if 0
			if (!filename.empty()) {
				Image::WriteEXR(filename, pixels, width, height);
				Printf(L"Have wrote texture to file '%s'\n", filename.c_str());
			}
#endif
		} else if (gltex->getFormat().isDepth()) {
			pixels = new byte_t[width*height*4];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, pixels);
			String filename = getImageFilename("tga");

			if (!filename.empty()) {
				Image::writeTGA(filename, pixels, width, height);
				Printf("Have wrote texture to file '%s'\n", filename.c_str());
			}
		} else if (gltex->getFormat() == TexFormat::D24S8) {
			pixels = new byte_t[width*height*4];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8_EXT, pixels);
			String filename = getImageFilename("tga");

			if (!filename.empty()) {
				Image::writeTGA(filename, pixels, width, height);
				Printf("Have wrote texture to file '%s'\n", filename.c_str());
			}
		} else {
			pixels = new byte_t[width*height*4];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
			String filename = getImageFilename("tga");

			if (!filename.empty()) {
				Image::writeTGA(filename, pixels, width, height);
				Printf("Have wrote texture to file '%s'\n", filename.c_str());
			}
		}

		SafeDeleteArray(pixels);

		GLrender::checkErrors();
	}

	void GLdriver::postInit()
	{
		glPostprocess = new GLpostprocess();
		glFontRender = new GLfontrender();
		glFontRender->initialize();
	}

	int GLdriver::genQuery() {
		return m_queryId += 2;
	}

	void GLdriver::deleteQuery(int id) {
		GLuint objs[] = { id, id+1 };
		glDeleteQueries(2, objs);
	}

AX_END_NAMESPACE


