/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "private.h"

Application* g_app;
Workbench* g_workbench;
QSettings* g_settings;


//------------------------------------------------------------------------------
// global data
//------------------------------------------------------------------------------
GlobalData* gGlobalData;

GlobalData::GlobalData(QApplication* m_app)
	: cursorHand(QPixmap(":/images/hand.png"))
	, cursorViewPan(QPixmap(":/cursors/view_pan.png"))
	, cursorViewZoom(QPixmap(":/cursors/view_zoom.png"))
	, cursorViewRotate(QPixmap(":/cursors/view_rotate.png"))
//	, defaultFont(QObject::tr("Tahoma"))
//	, defaultFont(QObject::tr("MS UI Gothic"), 9)
	, defaultFont(QObject::tr("MS Shell Dlg"), 9)
{
}


GlobalData::~GlobalData() {
}


