/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef TERRAINGEN_H
#define TERRAINGEN_H

#include <QMainWindow>
#include "ui_terraingen.h"

class terraingen : public QMainWindow
{
	Q_OBJECT

public:
	terraingen(QWidget *parent = 0);
	~terraingen();

private:
	Ui::terraingenClass ui;
};

#endif // TERRAINGEN_H
