/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXCONSOLEDLG_H
#define CXCONSOLEDLG_H

#include "private.h"
#include "ui_consoledlg.h"

class ConsoleDlg : public QWidget, public ILogHandler
{
    Q_OBJECT

public:
    ConsoleDlg(QWidget *parent = 0);
    ~ConsoleDlg();

	void addMessage(QString &msg);

	// implement ILogHandler
//	virtual void Print(const Wchar *text);
	virtual void print(const std::string &str);
	virtual void print(const char *text);

private slots:
	void on_message_textChanged();
	void on_input_returnPressed();
	bool eventFilter(QObject * watched, QEvent * event);
	void setInput(QString &str);

private:
    Ui::ConsoleDlgClass ui;
	QVector< QString > m_inputHistory;
	QVector< QString >::iterator m_currentHistory;
	QString m_currentInput;
};

#endif // CXCONSOLEDLG_H
