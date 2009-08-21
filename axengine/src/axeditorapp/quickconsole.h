/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXQUICKCONSOLE_H
#define CXQUICKCONSOLE_H

#include "private.h"
#include "ui_quickconsole.h"
#include "consoledlg.h"

class QuickConsole : public QWidget, public ILogHandler
{
    Q_OBJECT

public:
    QuickConsole(QWidget *parent = 0);
    ~QuickConsole();

	// implement ILogHandler
#if 0
	virtual void Print(const Wchar* text);
#endif
	virtual void print(const String& str);
	virtual void print(const char* text);

private:
	void setInput(QString& str);

private slots:
	void on_toolButton_clicked();
	void on_input_returnPressed();
	bool eventFilter (QObject * watched, QEvent * event);

private:
	Ui::QuickConsoleClass ui;
	QVector<QString> inputHistory;
	QVector<QString>::iterator currentHistory;
	QString currentInput;
	ConsoleDlg* mConsoleDlg;
};

#endif // CXQUICKCONSOLE_H
