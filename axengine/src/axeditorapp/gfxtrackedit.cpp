/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "gfxtrackedit.h"
#include "gfxcentral.h"

#include "private.h"

GfxTrackEdit::GfxTrackEdit(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_central = qobject_cast<GfxCentral*>(parent);
	assert(m_central);
}

GfxTrackEdit::~GfxTrackEdit()
{

}


void GfxTrackEdit::on_actionToggle_triggered()
{
	m_central->toggleTrackEdit();
}