/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CREATIONPANEL_H
#define CREATIONPANEL_H

#include "private.h"
#include "ui_creationpanel.h"

class CreationPanel : public QWidget, public IObserver
{
    Q_OBJECT

public:
    CreationPanel(QWidget *parent = 0);
    ~CreationPanel();

	// IObserver
	virtual void doNotify(IObservable *subject, int arg);

protected:
	void initEntityList();

private:
	enum EntityType { ET_Model, ET_Building, ET_Max };
    Ui::CreationPanelClass ui;

	EntityType mEntityType;
	QString m_filename[ET_Max];
	RenderEntity *m_renderActor;

private slots:
	void on_selectTree_itemDoubleClicked(QTreeWidgetItem*,int);
	void on_selectEntity_itemDoubleClicked(QTreeWidgetItem*,int);
	void on_selectModel_itemDoubleClicked(QTreeWidgetItem*,int);
	void on_modelButton_clicked();
	void on_objectColor_colorChanged(const QColor &color);
};

#endif // CREATIONPANEL_H
