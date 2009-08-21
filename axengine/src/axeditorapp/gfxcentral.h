/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef GFXCENTRAL_H
#define GFXCENTRAL_H

#include <QWidget>
#include "ui_gfxcentral.h"
#include "gfxtrackedit.h"

#include "private.h"

class GfxEditor;
class EditorFrame;

class GfxCentral : public QWidget
{
	Q_OBJECT

public:
	GfxCentral(QWidget *parent = 0);
	~GfxCentral();

	void toggleTrackEdit();

private:
	Ui::GfxCentralClass ui;
	GfxTrackEdit* m_trackEdit;
	GfxEditor* m_editor;
	EditorFrame* m_viewFrame;
	GfxContext* m_context;

private slots:
	void on_toggleTrackEdit_clicked();
};

#endif // GFXCENTRAL_H
