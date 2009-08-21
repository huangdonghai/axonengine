/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef CXQUICKSTATUSINFOPANEL_H
#define CXQUICKSTATUSINFOPANEL_H

#include "private.h"
#include "ui_quickstatusinfopanel.h"

class QuickStatusInfoPanel : public QWidget
{
    Q_OBJECT

public:
    QuickStatusInfoPanel(QWidget *parent = 0);
    ~QuickStatusInfoPanel();

private:
    Ui::QuickStatusInfoPanelClass ui;
};

#endif // CXQUICKSTATUSINFOPANEL_H
