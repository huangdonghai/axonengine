/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXTOOLSPANEL_H
#define CXTOOLSPANEL_H

#include "private.h"
#include "ui_toolspanel.h"

class ToolsPanel : public QWidget
{
    Q_OBJECT

public:
    ToolsPanel(QWidget *parent = 0);
    ~ToolsPanel();

private:
    Ui::ToolsPanelClass ui;
};

#endif // CXTOOLSPANEL_H
