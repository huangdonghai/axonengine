/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "gfxcentral.h"
#include "gfxeditor.h"
#include "editorframe.h"

GfxCentral::GfxCentral(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_trackEdit = new GfxTrackEdit(this);
	ui.trackViewC->layout()->addWidget(m_trackEdit);

	m_editor = qobject_cast<GfxEditor*>(parent);
	AX_ASSERT(m_editor);

	m_context = m_editor->getContext();
	m_viewFrame = new EditorFrame(this, m_context->getView(0));

	ui.viewFrame->layout()->addWidget(m_viewFrame);

	ui.trackViewC->hide();
}

GfxCentral::~GfxCentral()
{

}


void GfxCentral::on_toggleTrackEdit_clicked()
{
	toggleTrackEdit();
}

void GfxCentral::toggleTrackEdit()
{
	if (ui.trackViewC->isVisible()) {
		ui.trackViewC->hide();
		ui.miniTrack->show();
	} else {
		ui.trackViewC->show();
		ui.miniTrack->hide();
	}
}
