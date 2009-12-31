/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "gfxeditor.h"

GfxEditor::GfxEditor(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_gfxContext = new GfxContext();

	m_central = new GfxCentral(this);
	setCentralWidget(m_central);

	createActions();
}

GfxEditor::~GfxEditor()
{

}

void GfxEditor::createActions()
{
	typedef Action editorAction;
	typedef GfxTool editorTool;

	m_editorTools = new QActionGroup(this);
	m_editorTools->setExclusive(true);
	connect(m_editorTools, SIGNAL(triggered(QAction*)), this, SLOT(onEditorToolTriggered(QAction*)));

	m_editorActions = new QActionGroup(this);
	m_editorActions->setExclusive(false);
	connect(m_editorActions, SIGNAL(triggered(QAction*)), this, SLOT(onEditorActionTriggered(QAction*)));

	addEditorAction(editorAction::Delete, ui.actionDelete);

	addEditorAction(editorAction::Undo, ui.actionEditUndo);
	addEditorAction(editorAction::Redo, ui.actionEditRedo);
	addEditorAction(editorAction::Clone, ui.actionClone);

	addEditorTool(editorTool::Select, ui.actionObjectSelect);
	addEditorTool(editorTool::Move, ui.actionObjectMove);
	addEditorTool(editorTool::Rotate, ui.actionObjectRotate);
	addEditorTool(editorTool::Scale, ui.actionObjectScale);

	addEditorTool(editorTool::CreateParticle, ui.actionAddParticle);
	addEditorTool(editorTool::CreateRibbon, ui.actionAddRibbon);
}

void GfxEditor::onEditorToolTriggered(QAction *action)
{
	int type = Tool::None;
	if (action) {
		type = action->data().toInt();
	}

	m_gfxContext->doTool(type);
}

void GfxEditor::onEditorActionTriggered(QAction *action)
{
	int type = action->data().toInt();

	g_mapContext->doAction(type);
}

void GfxEditor::addEditorTool(int toolType, QAction *action)
{
	action->setCheckable(true);
	action->setData(toolType);
	m_editorTools->addAction(action);
}

void GfxEditor::addEditorAction(int actionType, QAction *action)
{
	action->setCheckable(false);
	action->setData(actionType);
	m_editorActions->addAction(action);
}
