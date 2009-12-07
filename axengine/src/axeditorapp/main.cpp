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

int main(int argc, char *argv[])
{
	setCurrentDir();

	Q_INIT_RESOURCE(editor);

	g_settings = new QSettings("Ax", "EditorMain");

	g_app = new Application(argc, argv);
	gGlobalData = new GlobalData(g_app);

	gGlobalData->style = g_settings->value("style", "").toString();
	g_app->setStyle(gGlobalData->style);

	gGlobalData->language = g_settings->value("language", "").toString();
	QTranslator translator;
	if (!gGlobalData->language.isEmpty()) {
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

	splash->showMessage(QObject::tr("Initializing axEditor..."), align, Qt::black);

    g_workbench = new Workbench();
	//ParticleEditor* editor = new ParticleEditor();
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
	//SafeDelete(editor);

	axEditorQuit();
	axLogicQuit();
	axClientQuit();
	axCoreQuit();

	SafeDelete(gGlobalData);
	SafeDelete(g_app);
	SafeDelete(g_settings);

	return 0;
}
