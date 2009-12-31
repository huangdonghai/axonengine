/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include <QMainWindow>
#include "ui_scripteditor.h"

class ScriptEditor : public QMainWindow
{
	Q_OBJECT

public:
	ScriptEditor(QWidget *parent = 0);
	~ScriptEditor();

	void openFile(const QString &filename);

private:
	Ui::ScriptEditorClass ui;

private slots:
	void on_fileTree_itemDoubleClicked(QTreeWidgetItem*,int);
};

#endif // SCRIPTEDITOR_H
