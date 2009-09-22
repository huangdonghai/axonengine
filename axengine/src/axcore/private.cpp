/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

#include <windows.h>
#include "BugTrap.h"

template< typename SignatureT >
class tclass {};

template< typename T, typename Rt >
class tclass< Rt (T::*)() > {};

namespace {
	using namespace Axon;

	void SetupExceptionHandler() {
		// required since VS 2005 SP1
		BT_InstallSehFilter();

		// Setup exception handler
		BT_SetAppName(L"Axon Engine");
//		BT_SetSupportEMail(L"bugs@axonengine.com");
		BT_SetFlags(BTF_DETAILEDMODE/*BTF_EDITMAIL| | BTF_ATTACHREPORT | BTF_SCREENCAPTURE*/);
		BT_SetSupportURL(L"http://www.axonengine.com");

		// = BugTrapServer ===========================================
//		BT_SetSupportServer(L"axon.ddns.info", 9999);
#if 0
		// Add custom log file using default name
		g_bugTraceLog = BT_OpenLogFile(NULL, BTLF_TEXT);
		BT_SetLogSizeInEntries(g_bugTraceLog, 100);
		BT_SetLogFlags(g_bugTraceLog, BTLF_SHOWTIMESTAMP);
		BT_SetLogEchoMode(g_bugTraceLog, BTLE_STDERR | BTLE_DBGOUT);
		BT_ClearLog(g_bugTraceLog);

		PCTSTR pszLogFileName = BT_GetLogFileName(g_bugTraceLog);
		BT_AddLogFile(pszLogFileName);
#endif
	}
}

namespace Axon {

	FileSystem* g_fileSystem;
	CmdSystem* g_cmdSystem;
	CvarSystem* g_cvarSystem;
	ConfigFile* g_systemConfig;
	System* g_system;
	ClassFactory* g_classFactory;
	ScriptSystem* g_scriptSystem;
	Statistic* g_statistic;
	Simd* g_simd;

	FILE* g_logFile;

	// global cvar
#define AX_CVARDECL(name, defaultstring, flags) \
	Cvar* name;
#include <axcore/cvardecls.h>
#undef AX_CVARDECL

	void terminateHandler() {
		MessageBoxW(NULL, L"terminateHandler", L"Error", MB_ICONERROR | MB_OK | MB_TASKMODAL);
		int* p = 0;
		*p = 0;
	}

	void axCoreInit(int argc, char* argv[]) {
		static bool _isInitialized = false;

		SetupExceptionHandler();
//		set_terminate(&terminateHandler);
//		BT_SetTerminate(); // set_terminate() must be called from every thread

		if (_isInitialized) {
			Errorf(_("axCoreInit: have initialized"));
			return;
		}
		// create log file
		g_logFile = ::fopen("log.txt", "w");

		// init math
		Math::initialize();
		Simd::initialize();
		//	testmem();

		g_statistic = new Statistic;
		g_statistic->initialize();

		g_system = new System;
		g_system->initialize();

		g_fileSystem = new FileSystem;
		g_fileSystem->initialize();

		g_scriptSystem = new ScriptSystem();
		g_scriptSystem->initialize();

		g_systemConfig = new ConfigFile;
		g_systemConfig->load("system.cfg");
		g_systemConfig->load("user.cfg");

		g_cmdSystem = new CmdSystem;
		g_cmdSystem->initialize();

		g_cvarSystem = new CvarSystem;
		g_cvarSystem->initialize();

		g_cmdSystem->execCmdLine(argc, argv);

		g_classFactory = new ClassFactory();
		g_classFactory->initialize();

#define AX_CVARDECL(name, defaultstring, flags) \
	name = g_cvarSystem->createCvar(#name, defaultstring, flags);
#include <axcore/cvardecls.h>
#undef AX_CVARDECL


		MaterialDecl::initManager();
#if 0
		for (int i = 0; i < 1000; i++) {
			ulonglong_t t = Microseconds();
			Printf("%I64d ", t);
		}
#endif
		_isInitialized = true;
	}

	void axCoreQuit() {
		MaterialDecl::finalizeManager();

		g_classFactory->finalize();
		SafeDelete(g_classFactory);

		g_cvarSystem->finalize();
		SafeDelete(g_cvarSystem);

		g_cmdSystem->finalize();
		SafeDelete(g_cmdSystem);

		g_systemConfig->saveTo("user.cfg");
		SafeDelete(g_systemConfig);

		g_scriptSystem->finalize();
		SafeDelete(g_scriptSystem);

		g_fileSystem->finalize();
		SafeDelete(g_fileSystem);

		g_system->finalize();
		SafeDelete(g_system);

		g_statistic->finalize();
		SafeDelete(g_statistic);

		// close log file
		if (g_logFile) {
			::fclose(g_logFile);
		}
	}

} // namespace Axon
