/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GLDRIVER_H
#define AX_GLDRIVER_H

namespace Axon { namespace Render {

		// declare ogl functions
	#define GL_EXT(name) extern bool SUPPORT##name;
	#define GL_FNC(ext,ret,func,parms)  extern ret (STDCALL *func)parms;
	#include "glfunc.h"
	#undef GL_EXT
	#undef GL_FNC

	const int SELECT_BUFFER_SIZE = 4096;

	#define BUFFER_OFFSET(i)		((byte_t*)NULL + (i))
	#define BUFFER_OFFSET2(p, i)	((byte_t*)p + (i))


	class GLdriver : public IDriver, public ICmdHandler {
	public:
		AX_DECLARE_FACTORY();
		AX_DECLARE_COMMAND_HANDLER(GLdriver);

		GLdriver();
		~GLdriver();

	private:
		bool findExtension(const char* extname);
		void findFunction(void*& func, const char* name, const char* extname, bool extsupport);
		void loadMatrix(GLenum mode, const Matrix4& m);

	public:
		// implement Render::IDriver
		virtual void initialize();
		virtual void finalize();
		virtual void postInit();			// after render system is initilized, call this
		void findFunctions(bool allowExt);

		virtual bool isHDRRendering();
		virtual bool isInRenderingThread() { return false; }

		// resource management
		virtual Target* createWindowTarget(handle_t wndId, const String& name);
#if 0
		virtual Target* getColorTarget(int width, int height);
		virtual Target* getDepthTarget(int width, int height);
#endif
		// primitive
		virtual int cachePrimitive(Primitive* prim);
		virtual void uncachePrimitive(Primitive* prim);

		// caps
		virtual const Info* getDriverInfo();
		virtual uint_t getBackendCaps();

		// before runFrame, do some pre frame things
		virtual void preFrame();

		// render command
		virtual void runFrame();

		// new selcection
		virtual void beginSelect(const Camera& view);
		virtual void loadSelectId(int id);
		virtual void testActor(Entity* re);
		virtual void testPrimitive(Primitive* prim);
		virtual SelectRecordSeq endSelect();

		// occlusion query
		virtual int genQuery();
		virtual void deleteQuery(int id);
		virtual int getQueryResult( int id ){ return 0; }

	public:
		static handle_t createGLWindow(const String& wnd_name);
		
	protected:
		// console command
		void dumpTex_f(const CmdArgs& param);

	private:
		bool m_initialized;
		bool m_foundExts;
		handle_t m_glLibHandle;
		uint_t m_time;				// in millisecond
		Vector3 m_viewOrg;
		Matrix3 m_axisInverse;

		// select buffer 
		bool m_isSelectMode;
		GLuint m_selectBuffer[SELECT_BUFFER_SIZE];

		// queryid
		int m_queryId;
	};

}} // namespace Axon::Render

#endif // AX_GLDRIVER_H
