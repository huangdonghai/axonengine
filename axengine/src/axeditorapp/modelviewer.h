/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef MODELVIEWER_H
#define MODELVIEWER_H

#include <QMainWindow>
#include "ui_modelviewer.h"

class ModelViewer : public QMainWindow
{
	Q_OBJECT

public:
	ModelViewer(QWidget *parent = 0);
	~ModelViewer();

private:
	Ui::ModelViewerClass ui;

	HavokModel* m_renderModel;

private slots:
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem*,int);
	void on_modelType_activated(int);
};

#endif // MODELVIEWER_H
