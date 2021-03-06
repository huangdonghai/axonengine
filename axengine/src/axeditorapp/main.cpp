/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "workbench.h"
#include "application.h"

void testfixstring() {
	FixedString fixed("abc");
	const char* str = fixed.c_str();
	FixedString fixed2("xyz");
	str = fixed2.c_str();

	fixed = "you mama";
	str = fixed.c_str();
}

int main(int argc, char *argv[]) {
	testfixstring();

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
	v = g_app->connect(g_app, SIGNAL(aboutToQuit()), g_app, SLOT(myquit()));
	AX_ASSERT(v);

	splash->finish(g_workbench);
	SafeDelete(splash);

#if 0
	Image image;
	bool v = image.loadFileByType("textures/default.dds", Image::DDS, Image::NoCompressed);
	Image* resized = image.resize(192, 192);

	resized->saveFile_tga("resized.tga");
	image.clear();
#endif

#if 1
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
