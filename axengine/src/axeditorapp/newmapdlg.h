/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXNEWMAPDLG_H
#define CXNEWMAPDLG_H

#include "ui_newmapdlg.h"

class NewMapDlg : public QDialog
{
    Q_OBJECT

public:
    NewMapDlg(QWidget *parent = 0);
    ~NewMapDlg();

private slots:
	void on_okButton_clicked();
	void OnSetDefaultValue();
	void OnParamsChanged();
	void OnAccept();

private:
    Ui::NewMapDlgClass ui;
};

#endif // CXNEWMAPDLG_H
