/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITORAPP_ACTORPANEL_H
#define AX_EDITORAPP_ACTORPANEL_H

#include "ui_actorpanel.h"

class ActorPanel : public QWidget, public IObserver
{
    Q_OBJECT

public:
    ActorPanel(QWidget *parent = 0);
    ~ActorPanel();

	// IObserver
	virtual void doNotify(IObservable *subject, int arg );

private:
    Ui::ActorPanelClass ui;

private slots:
	void onPropertyChanged(IProperty *property);
	void on_editScript_clicked();
	void on_objectColor_colorChanged(const QColor &color);
};

#endif // AX_EDITORAPP_ACTORPANEL_H
