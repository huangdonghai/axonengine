/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef GFXEDITOR_H
#define GFXEDITOR_H

#include <QMainWindow>
#include "ui_gfxeditor.h"

#include "gfxcentral.h"

#include "private.h"

class GfxEditor : public QMainWindow
{
	Q_OBJECT

public:
	GfxEditor(QWidget *parent = 0);
	~GfxEditor();

	GfxContext *getContext() const { return m_gfxContext; }

	// editor tools and editor actions
	void addEditorTool(int toolType, QAction *action);
	void addEditorAction(int actionType, QAction *action);

protected:
	void createActions();

private slots:
	void onEditorToolTriggered(QAction *action );
	void onEditorActionTriggered(QAction *action);

private:
	Ui::GfxEditorClass ui;
	GfxContext *m_gfxContext;
	GfxCentral *m_central;

	// editor tools and editor action
	QActionGroup *m_editorTools;
	QActionGroup *m_editorActions;

};

#endif // GFXEDITOR_H
