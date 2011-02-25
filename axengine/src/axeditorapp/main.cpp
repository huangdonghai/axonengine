/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "workbench.h"
#include "application.h"

/**
  Change our current directory to be the same as the directory containing the .exe.
  Visual C++ starts us in the project directory by default, and the config file
  is expected to be in the current directory.
 */
static void setCurrentDir()
{
    TCHAR filename[_MAX_PATH];
    TCHAR pathname[_MAX_PATH];
    LPTSTR file_part;

    GetModuleFileName(NULL, filename, sizeof(filename));
    GetFullPathName(filename, ArraySize(pathname), pathname, &file_part);
    if(file_part) *file_part = 0; // Truncate filename portion
    SetCurrentDirectory(pathname);
}

void profileHash()
{
	int loop = 1000000;

	std::map<int, int> intMap;
	Dict<int, int> intDict;
	for (int i = 0; i < loop; i++) {
		intMap[i] = rand();
		intDict[i] = rand();
	}

	double t0 = OsUtil::seconds();
	for (int i = 0; i < loop; i++) {
		intMap.find(i);
	}
	double t1 = OsUtil::seconds();
	for (int i = 0; i < loop; i++) {
		intDict.find(i);
	}
	double t2 = OsUtil::seconds();
	double timeMap = t1 - t0;
	double timeHash = t2 - t1;
	Printf("timeMap: %f; timeHash: %f\n", timeMap, timeHash);
}

int main(int argc, char *argv[])
{
	setCurrentDir();

	Q_INIT_RESOURCE(editor);

	g_settings = new QSettings("Ax", "EditorMain");

	g_app = new Application(argc, argv);
	g_globalData = new GlobalData(g_app);

	g_globalData->style = g_settings->value("style", "").toString();
	g_app->setStyle(g_globalData->style);

	g_globalData->language = g_settings->value("language", "").toString();
	QTranslator translator;
	if (!g_globalData->language.isEmpty()) {
		translator.load(":/translations/editor_zh.qm");
		g_app->installTranslator(&translator);
	}

//	app->setFont(gGlobalData->defaultFont);

	QSplashScreen *splash = new QSplashScreen;
	splash->setPixmap(QPixmap(":/images/splash.png"));
	splash->show();

	Qt::Alignment align = Qt::AlignLeft | Qt::AlignBottom;

	// initialize some lib
	splash->showMessage(QObject::tr("Initializing axCore module..."), align, Qt::black);
	axCoreInit(argc, argv);
	splash->showMessage(QObject::tr("Initializing axClient module..."), align, Qt::black);
	axClientInit();
	splash->showMessage(QObject::tr("Initializing axLogic module..."), align, Qt::black);
	axLogicInit();
	splash->showMessage(QObject::tr("Initializing axEditor module..."), align, Qt::black);
	axEditorInit();

	//profileHash();

	splash->showMessage(QObject::tr("Initializing axEditor..."), align, Qt::black);

    g_workbench = new Workbench();
    bool v = g_app->connect(g_app, SIGNAL(lastWindowClosed()), g_app, SLOT(quit()));
	AX_ASSERT(v);
	v = g_app->connect(g_app, SIGNAL(aboutToQuit()), g_app, SLOT(myQuit()));
	AX_ASSERT(v);

	splash->finish(g_workbench);
	SafeDelete(splash);

#if 0
    g_app->exec();
#else
	g_system->registerTickable(System::TickEvent, g_app);
	g_system->run();
	g_system->removeTickable(System::TickEvent, g_app);
#endif
	SafeDelete(g_workbench);

	axEditorQuit();
	axLogicQuit();
	axClientQuit();
	axCoreQuit();

	SafeDelete(g_globalData);
	SafeDelete(g_app);
	SafeDelete(g_settings);

	return 0;
}
