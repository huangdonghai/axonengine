/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "QuickConsole.h"

QuickConsole::QuickConsole(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	ui.input->installEventFilter(this);
	ui.message->setBackgroundRole(QPalette::Background);
	QFont f = ui.message->font();
	f.setStyleStrategy(QFont::PreferBitmap);
	ui.message->setFont(f);
	g_coreSystem->registerLog(this);

	mConsoleDlg = new ConsoleDlg(NULL);
	currentHistory = inputHistory.end();
}

QuickConsole::~QuickConsole()
{
	g_coreSystem->removeLog(this);
	delete mConsoleDlg;
}

// implement iLogHandler
void QuickConsole::print(const char *text) {
	// we should parse text format escape
	char line[1024];

	const char *f = text;
	char *t = line;

	while (1) {
		if (*f == FORMAT_ESCAPE) {
			f++;
			if (*f == FORMAT_ESCAPE) {
				*t = FORMAT_ESCAPE;
				t++;
			} else {
				char ch = *f;
				Rgba color = Rgba::ColorTable[C_ColorIndex(ch)];
				ui.message->setTextColor(QColor(color.r*.75f, color.g*.75f, color.b*.75f, color.a));
			}
			f++;

			continue;
		}

		if (*f == '\n') {
			f++;
			*t = 0;
			QString str;
			str = u2q(line);
			t = line;
			ui.message->clear();
			ui.message->append(str);
			ui.message->setTextColor(QColor(255, 255, 255));

			continue;
		}

		if (*f == 0) {
			*t = 0;

			if (*line == 0)
				break;

			QString str;
			str = u2q(line);
			ui.message->clear();
			ui.message->append(str);
			ui.message->setTextColor(QColor(255, 255, 255));

			break;
		}

		*t++ = *f++;

		if (t - line >= ArraySize(line) - 2) {
			*t = 0;
			QString str;
			str = u2q(line);
			t = line;
			ui.message->clear();
			ui.message->append(str);
			ui.message->setTextColor(QColor(255, 255, 255));

			continue;
		}
	}
	ui.message->verticalScrollBar()->setValue(ui.message->verticalScrollBar()->maximum());
}

void QuickConsole::print(const std::string &str) {
	print(str.c_str());
}

#if 0
void
QuickConsole::Print(const char *text) {
	SCOPE_CONVERT;

	Print(M2W(text));
}
#endif

void QuickConsole::on_input_returnPressed()
{
	// get text form input than clear input
	QString text = ui.input->text();
	ui.input->clear();

	// add history and set current history
	inputHistory.append(text);
	currentHistory = inputHistory.end();

	// send command
	QString otext = "]" + text;
	ui.message->append(otext);

	g_cmdSystem->executeString(q2u(text));
}

void QuickConsole::setInput(QString &str)
{
	ui.input->clear();
	ui.input->insert(str);
}

bool QuickConsole::eventFilter (QObject * watched, QEvent * event)
{
	if (watched == ui.input && event->type() == QEvent::KeyPress) {
		QKeyEvent *e = (QKeyEvent*)event;
		if (e->key() == Qt::Key_Up) {

			// backup current input if need
			if (currentHistory == inputHistory.end()) {
				currentInput = ui.input->text();
			}

			// prev history
			if (currentHistory != inputHistory.begin()) {
				--currentHistory;
				setInput(*currentHistory);
			}

		} else if (e->key() == Qt::Key_Down) {
			// next history
			if (currentHistory == inputHistory.end() - 1) {
				currentHistory++;
				setInput(currentInput);
			} else if (currentHistory == inputHistory.end()) {
				//setCurrentInput(currentInput);
			} else {
				currentHistory++;
				setInput(*currentHistory);
			}
		}
	}
	return QWidget::eventFilter(watched, event);
}


void QuickConsole::on_toolButton_clicked()
{
	mConsoleDlg->setVisible(true);
}