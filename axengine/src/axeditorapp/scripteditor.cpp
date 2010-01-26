/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "scripteditor.h"
#include "filedialog.h"

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

struct FileOpened {
	QString filename;
	QWidget *widget;
	QsciScintilla *textEditor;
};

typedef QVector<FileOpened>	FileOpenedSeq;
QsciLexerLua *lexer;

FileOpenedSeq fileOpendSeq;

ScriptEditor::ScriptEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.fileTree->setRoot("/scripts");
	ui.fileTree->setFilter("*.nut");
	ui.fileTree->refresh();

	lexer = new QsciLexerLua();
}

ScriptEditor::~ScriptEditor()
{

}

void ScriptEditor::openFile(const QString &filename) {
	char *buf;
	size_t filesize = g_fileSystem->readFile(q2u(filename), (void**)&buf);

	if (!filesize || !buf) {
		return;
	}

#if 0
	QFont f("Fixedsys",10);;
	f.setStyleStrategy(QFont::PreferBitmap);
#endif
	FileOpened fo;
	fo.filename = filename;
	fo.textEditor = new QsciScintilla;
	fo.textEditor->setTabIndents(true);
	fo.textEditor->setTabWidth(4);
	fo.textEditor->setIndentationsUseTabs(true);
	fo.textEditor->setIndentationWidth(4);
	fo.textEditor->setMarginLineNumbers(1, true);
	fo.textEditor->setMarginWidth(1, QString("12345"));
	fo.textEditor->setLexer(lexer);
	fo.textEditor->setText(buf);
#if 0
	fo.textEditor->setFont(f);
#endif
	fo.textEditor->setFolding(QsciScintilla::BoxedTreeFoldStyle);
	fo.textEditor->setCaretLineVisible(true);
	fo.textEditor->setEdgeColumn(80);
	fo.textEditor->setEdgeMode(QsciScintilla::EdgeLine);
	fo.textEditor->setIndentationGuides(true);

	ui.tabWidget->insertTab(0, fo.textEditor, filename);
}

void ScriptEditor::on_fileTree_itemDoubleClicked(QTreeWidgetItem *item,int) {
	FileItem *f_item = dynamic_cast<FileItem*>(item);
	if (!f_item)
		return;

	const FileInfo &finfo = f_item->getFileInfo();

	if (f_item->isDirectory()) {
		return;
	}

	openFile(u2q(finfo.fullpath));
}