/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// class UndoAction, undo action
	//--------------------------------------------------------------------------

	UndoAction::UndoAction(Context *context) : Action(context) {}
	UndoAction::~UndoAction() {}

	void UndoAction::doIt() {
		m_context->undo();
	}


	//--------------------------------------------------------------------------
	// class RedoAction, redo action
	//--------------------------------------------------------------------------

	RedoAction::RedoAction(Context *context) : Action(context) {}
	RedoAction::~RedoAction() {}

	void RedoAction::doIt() {
		m_context->redo();
	}

	//--------------------------------------------------------------------------
	// class DeleteAction, undo action
	//--------------------------------------------------------------------------

	DeleteAction::DeleteAction(Context *context) : Action(context) {
		m_isFirst = true;
	}

	DeleteAction::~DeleteAction() {}

	void DeleteAction::doIt() {
		if (m_isFirst) {
			m_actorlist = m_context->getSelection();
			if (m_actorlist.empty())
				return;

			m_context->selectNone();

			m_isFirst = false;
		}

		AgentList::const_iterator it = m_actorlist.begin();
		for (; it != m_actorlist.end(); ++it) {
			(*it)->setDeleted(true);
		}
	}

	void DeleteAction::undo() {
		AgentList::const_iterator it = m_actorlist.begin();
		for (; it != m_actorlist.end(); ++it) {
			(*it)->setDeleted(false);
		}
	}

	int DeleteAction::getMemoryUsed() {
		return sizeof(UndeleteHis) + sizeof(ActionList);
	}

	//--------------------------------------------------------------------------
	// class CloneAction, undo action
	//--------------------------------------------------------------------------

	CloneAction::CloneAction(Context *context) : Action(context) {
		m_isFirst = true;
		m_selectionHis = nullptr;
	}

	CloneAction::~CloneAction() {
		SafeDelete(m_selectionHis);
	}

	void CloneAction::doIt() {
		if (m_isFirst) {
			m_actorlist = m_context->getSelection().clone();
			if (m_actorlist.empty())
				return;

			m_selectionHis = m_context->setSelectionHistoried(m_actorlist);

			m_isFirst = false;

			return;
		}

		AgentList::const_iterator it = m_actorlist.begin();
		for (; it != m_actorlist.end(); ++it) {
			(*it)->setDeleted(false);
		}

		m_selectionHis->doIt();
	}

	void CloneAction::undo() {
		m_selectionHis->undo();

		AgentList::const_iterator it = m_actorlist.begin();
		for (; it != m_actorlist.end(); ++it) {
			(*it)->setDeleted(true);
		}
	}

	int CloneAction::getMemoryUsed() {
		return sizeof(UndeleteHis) + sizeof(ActionList);
	}

AX_END_NAMESPACE

