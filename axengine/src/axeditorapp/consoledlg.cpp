/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "consoledlg.h"

ConsoleDlg::ConsoleDlg(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	layout()->setMargin(0);

	ui.input->installEventFilter(this);

	ui.message->setBackgroundRole(QPalette::Background);
	QFont f = ui.message->font();
	f.setStyleStrategy(QFont::PreferBitmap);
	ui.message->setFont(f);

	g_system->registerLog(this);
}

ConsoleDlg::~ConsoleDlg()
{
	g_system->removeLog(this);
}

void ConsoleDlg::addMessage(QString &msg)
{
	ui.message->append(msg);
}

void ConsoleDlg::on_input_returnPressed()
{
	// get text form input than clear input
	QString text = ui.input->text();
	ui.input->clear();

	// add history and set current history
	m_inputHistory.append(text);
	m_currentHistory = m_inputHistory.end();

	// send command
	QString otext = "]" + text;
	addMessage(otext);

	g_cmdSystem->executeString(q2u(text));
}


void ConsoleDlg::setInput(QString &str)
{
	ui.input->clear();
	ui.input->insert(str);
}

bool ConsoleDlg::eventFilter (QObject * watched, QEvent * event)
{
	if (watched == ui.input && event->type() == QEvent::KeyPress) {
		QKeyEvent *e = (QKeyEvent*)event;
		if (e->key() == Qt::Key_Up) {

			// backup current input if need
			if (m_currentHistory == m_inputHistory.end()) {
				m_currentInput = ui.input->text();
			}

			// prev history
			if (m_currentHistory != m_inputHistory.begin()) {
				--m_currentHistory;
				setInput(*m_currentHistory);
			}

		} else if (e->key() == Qt::Key_Down) {
			// next history
			if (m_currentHistory == m_inputHistory.end() - 1) {
				m_currentHistory++;
				setInput(m_currentInput);
			} else if (m_currentHistory == m_inputHistory.end()) {
				//setCurrentInput(m_currentInput);
			} else {
				m_currentHistory++;
				setInput(*m_currentHistory);
			}
		}
	}
	return QWidget::eventFilter(watched, event);
}

void ConsoleDlg::print(const char *text) {
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
				ui.message->setTextColor(QColor(color.r*1.0f, color.g*1.0f, color.b*1.0f));
			}
			f++;

			continue;
		}

		if (*f == L'\n') {
			f++;
			*t = 0;
			QString str;
			str = u2q(line);
			addMessage(str);
			t = line;

			ui.message->setTextColor(QColor(255, 255, 255));

			continue;
		}

		if (*f == 0) {
			*t = 0;

			if (*line == 0)
				break;

			QString str;
			str = u2q(line);
			addMessage(str);

			ui.message->setTextColor(QColor(255, 255, 255));

			break;
		}

		*t++ = *f++;

		if (t - line >= ArraySize(line) - 2) {
			*t = 0;
			QString str;
			str = u2q(line);
			addMessage(str);
			t = line;

			ui.message->setTextColor(QColor(255, 255, 255));

			continue;
		}
	}
}

void ConsoleDlg::print(const String &text) {
	print(text.c_str());
}

#if 0
void ConsoleDlg::Print(const char *text) {
	SCOPE_CONVERT;

	Print(M2W(text));
}
#endif

void ConsoleDlg::on_message_textChanged()
{

}