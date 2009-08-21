/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef MATERIALEDITOR_H
#define MATERIALEDITOR_H

#include <QMainWindow>
#include "ui_materialeditor.h"

class MaterialEditor : public QMainWindow
{
	Q_OBJECT

public:
	MaterialEditor(QWidget *parent = 0);
	~MaterialEditor();

protected:
	void initWidgets();
	void createMaterial(const String& shadername);
	void initFromMaterial(const String& name);

private:
	Ui::MaterialEditorClass ui;

private slots:
	void on_actionRefresh_triggered();
	void on_matTree_itemActivated(QTreeWidgetItem*,int);

private:
	Sequence<Render::Material*> m_openedMat;
	Render::Material* m_curMat;
};

#endif // MATERIALEDITOR_H
