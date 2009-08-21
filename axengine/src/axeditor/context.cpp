/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

namespace Axon { namespace Editor {

	Context::Context()
	{
		m_tooltype = Tool::None;
		m_tool = (nullptr);
		m_maxId = (0);

		TypeZeroArray(m_toolFactories);
		TypeZeroArray(m_actionFactories);

		m_isDirty = false;
		m_isLoading = false;

		m_state = 0;

		m_numViews = 0;
	}

	Context::~Context()
	{
	}

	int Context::generateActorId() {
		return ++m_maxId;
	}

	void Context::addActor(Actor* actor) {
		int id = actor->getId();
		ActorDict::iterator it = m_actorDict.find(id);

		if (it != m_actorDict.end() && it->second) {
			Errorf("Context::addActor: duplicated id");
		}

		m_actorDict[id] = actor;
	}

	void Context::removeActor(Actor* actor) {
		int id = actor->getId();
		ActorDict::iterator it = m_actorDict.find(id);

		if (it == m_actorDict.end()) {
			Errorf("Context::removeActor: can't find actor");
		}

		it->second = 0;
	}

	Actor* Context::findActor(int id) {
		ActorDict::iterator it = m_actorDict.find(id);

		if (it == m_actorDict.end() || it->second == nullptr) {
			Errorf("Context::findActor: can't find actor for id '%d'", id);
			return nullptr;
		}

		return it->second;
	}

	Action* Context::createAction(int type) {
		if (m_actionFactories[type] == nullptr)
			return nullptr;

		return m_actionFactories[type]->create(this);
	}

	Tool* Context::createTool(int type) {
		if (m_toolFactories[type] == nullptr)
			return nullptr;

		return m_toolFactories[type]->create(this);
	}

	void Context::doAction(int a) {
		Action* action = createAction(a);
		if (!action)
			return;

		action->doIt();

		if (action->isUndoable())
			addHistory(action);
		else
			delete action;
	}

	void Context::doTool(int tool) {
		SafeDelete(m_tool);

		m_tool = createTool(tool);
		if (m_tool) {
			m_tool->setView(m_activeView);
		}

		m_tooltype = tool;

		notify(ToolChanged);
	}

	History* Context::setSelectionHistoried( const ActorList& elist )
	{
		ActorList oldlist = m_selections;

		m_selections.setSelected(false);
		m_selections.clear();

		m_selections = elist;
		m_selections.unique();
		m_selections.setSelected(true);

		notify(SelectionChanged);

		return new SelectHis(this, oldlist, m_selections);
	}

	void Context::setSelection(const ActorList& elist, bool undoable) {
		ActorList oldlist = m_selections;

		m_selections.setSelected(false);
		m_selections.clear();

		m_selections = elist;
		m_selections.unique();
		m_selections.setSelected(true);

		if (undoable) {
			SelectHis* his = new SelectHis(this, oldlist, m_selections);
			addHistory(his);
		}

		notify(SelectionChanged);
	}

	void Context::addSelection( const ActorList& elist, bool undoable/*=true*/ )
	{
		ActorList newlist = m_selections;
		ActorList rhs = elist;

		newlist.merge(rhs);

		setSelection(newlist, undoable);
	}

	void Context::setSelection(Actor* actor, bool undoable) {
		ActorList oldlist = m_selections;

		m_selections.setSelected(false);
		m_selections.clear();

		m_selections.push_back(actor);
		actor->setSelected(true);

		if (undoable) {
			SelectHis* his = new SelectHis(this, oldlist, m_selections);
			addHistory(his);
		}

		notify(SelectionChanged);
	}

	void Context::selectNone(bool undoable) {
		ActorList oldlist = m_selections;

		m_selections.setSelected(false);
		m_selections.clear();

		if (undoable) {
			SelectHis* his = new SelectHis(this, oldlist, m_selections);
			addHistory(his);
		}

		notify(SelectionChanged);
	}

	void Context::selectAll(bool undoable) {
		ActorList oldlist = m_selections;

		m_selections.setSelected(false);
		m_selections.clear();

		ActorDict::iterator it;

		for (it = m_actorDict.begin(); it != m_actorDict.end(); ++it) {
			Actor* actor = it->second;

			if (!actor) continue;

			if (!actor->isSelected()) {
				actor->setSelected(true);
				m_selections.push_back(actor);
			}
		}

		if (undoable) {
			SelectHis* his = new SelectHis(this, oldlist, m_selections);
			addHistory(his);
		}

		notify(SelectionChanged);
	}

	void Context::selectInvert(bool undoable) {
		ActorDict::iterator it;

		ActorList newsel;
		for (it = m_actorDict.begin(); it != m_actorDict.end(); ++it) {
			Actor* actor = it->second;

			if (!actor) continue;

			if (!actor->isSelected()) {
				actor->setSelected(true);
				newsel.push_back(actor);
			}
		}

		setSelection(newsel, undoable);
	}

	void Context::addHistory(Action* his) {
		m_isDirty = true;
		m_editorHistory.addHistory(his);

		notify(HistoryChanged);
	}

	void Context::undo() {
		m_editorHistory.undo(1);
		notify(HistoryChanged);
	}

	void Context::redo() {
		m_editorHistory.redo(1);
		notify(HistoryChanged);
	}

	HistoryManager* Context::getHistory() {
		return &m_editorHistory;
	}

	View* Context::getView(int index) {
		AX_ASSERT(index >= 0 && index < View::MaxView);

		return m_indexedViews[index];
	}


}} // namespace Axon::Context


