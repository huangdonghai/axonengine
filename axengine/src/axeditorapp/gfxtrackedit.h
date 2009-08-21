/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef GFXTRACKEDIT_H
#define GFXTRACKEDIT_H

#include <QMainWindow>
#include "ui_gfxtrackedit.h"

class GfxCentral;

class GfxTrackEdit : public QMainWindow
{
	Q_OBJECT

public:
	GfxTrackEdit(QWidget *parent = 0);
	~GfxTrackEdit();

private:
	Ui::GfxTrackEditClass ui;

	GfxCentral *m_central;

private slots:
	void on_actionToggle_triggered();
};

#endif // GFXTRACKEDIT_H
