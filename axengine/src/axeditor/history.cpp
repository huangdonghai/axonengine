/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

namespace Axon { namespace Editor {
	//--------------------------------------------------------------------------
	// class HistoryManager
	//--------------------------------------------------------------------------

	HistoryManager::HistoryManager() {
	}

	HistoryManager::~HistoryManager() {
	}

	void HistoryManager::addHistory(Action* his)
	{
		m_undoList.push_front(ActionPtr(his));
		m_redoList.clear();
	}

	void HistoryManager::undo(int count) {
		for (int i = 0; i < count; i++) {
			if (m_undoList.size() == 0)
				break;

			ActionPtr action = m_undoList.front();
			m_undoList.pop_front();

			action->undo();

			m_redoList.push_front(action);
		}
	}

	void HistoryManager::redo(int count) {
		for (int i = 0; i < count; i++) {
			if (m_redoList.size() == 0)
				break;

			ActionPtr action = m_redoList.front();
			m_redoList.pop_front();

			action->doIt();

			m_undoList.push_front(action);
		}
	}

	void HistoryManager::clear() {
		m_undoList.clear();
		m_redoList.clear();
	}

	//--------------------------------------------------------------------------
	// class TransformHis
	//--------------------------------------------------------------------------

	TransformHis::TransformHis(Context* context, const String& msg, int actorId, const AffineMat& oldmat, const AffineMat& newmat)
		: History(context, msg)
	{
		m_actorId = actorId;
		m_oldMat = oldmat;
		m_newMat = newmat;
	}

	TransformHis::~TransformHis() {}

	void TransformHis::doIt() {
		Actor* actor = m_context->findActor(m_actorId);

		if (!actor) {
			return;
		}

		actor->setMatrix(m_newMat);

		m_context->notify(Context::ActorTransformed);
	}

	void TransformHis::undo() {
		Actor* actor = m_context->findActor(m_actorId);

		if (!actor) {
			return;
		}

		actor->setMatrix(m_oldMat);
		m_context->notify(Context::ActorTransformed);
	}

	bool TransformHis::isUndoable() {
		return true;
	}
	String TransformHis::getName() {
		return "TransformHis";
	}

	int TransformHis::getMemoryUsed() {
		return sizeof(*this);
	}

	//--------------------------------------------------------------------------
	// class GroupHis
	//--------------------------------------------------------------------------

	GroupHis::GroupHis(Context* context, const String& msg)
		: History(context, msg)
	{}

	GroupHis::~GroupHis() {}

	void GroupHis::doIt() {
		ActionList::const_iterator it = m_actionList.begin();

		for (; it != m_actionList.end(); ++it) {
			(*it)->doIt();
		}
	}

	void GroupHis::undo() {
		ActionList::const_iterator it = m_actionList.begin();

		for (; it != m_actionList.end(); ++it) {
			(*it)->undo();
		}
	}

	bool GroupHis::isUndoable() {
		return true;
	}

	String GroupHis::getName() {
		return "GroupHis";
	}

	int GroupHis::getMemoryUsed() {
		ActionList::const_iterator it = m_actionList.begin();

		int used = 0;
		for (; it != m_actionList.end(); ++it) {
			used += (*it)->getMemoryUsed();
		}

		return used + sizeof(*this);
	}

	void GroupHis::append(Action* his) {
		AX_ASSERT(his->isUndoable());

		m_actionList.push_back(his);
	}

	//--------------------------------------------------------------------------
	// class DeleteHis
	//--------------------------------------------------------------------------

	DeleteHis::DeleteHis(Context* context, const String& msg, const ActorList& actorlist)
		: History(context, msg)
		, m_actorList(actorlist)
	{}

	void DeleteHis::doIt() {
		ActorList::const_iterator it = m_actorList.begin();
		for (; it != m_actorList.end(); ++it) {
			(*it)->setDeleted(true);
		}
	}

	void DeleteHis::undo() {
		ActorList::const_iterator it = m_actorList.begin();
		for (; it != m_actorList.end(); ++it) {
			(*it)->setDeleted(false);
		}
	}

	int DeleteHis::getMemoryUsed() {
		return sizeof(DeleteHis) + sizeof(ActionList);
	}

	//--------------------------------------------------------------------------
	// class UndeleteHis
	//--------------------------------------------------------------------------

	UndeleteHis::UndeleteHis(Context* ctx, const String& msg, const ActorList& actorlist)
		: History(ctx, msg)
		, m_actorList(actorlist)
	{}

	void UndeleteHis::doIt() {
		ActorList::const_iterator it = m_actorList.begin();
		for (; it != m_actorList.end(); ++it) {
			(*it)->setDeleted(false);
		}
	}

	void UndeleteHis::undo() {
		ActorList::const_iterator it = m_actorList.begin();
		for (; it != m_actorList.end(); ++it) {
			(*it)->setDeleted(true);
		}
	}

	int UndeleteHis::getMemoryUsed() {
		return sizeof(UndeleteHis) + sizeof(ActionList);
	}

	//--------------------------------------------------------------------------
	// class PropertyEditHis
	//--------------------------------------------------------------------------

	PropertyEditHis::PropertyEditHis(Context* ctx, Actor* actor, const String& propname, const Variant& oldvalue, const Variant& newvalue) 
		: History(ctx, "Property Edited")
		, m_actor(actor)
		, m_propName(propname)
		, m_oldValue(oldvalue)
		, m_newValue(newvalue)
	{}

	PropertyEditHis::~PropertyEditHis() {

	}

	void PropertyEditHis::doIt() {
		m_actor->setProperty(m_propName, m_newValue);
		m_context->notify(Context::SelectionChanged);
	}

	void PropertyEditHis::undo() {
		m_actor->setProperty(m_propName, m_oldValue);
		m_context->notify(Context::SelectionChanged);
	}

	int PropertyEditHis::getMemoryUsed() {
		return s2i(sizeof(*this) + m_propName.size());
	}


	//--------------------------------------------------------------------------
	// class SelectHis
	//--------------------------------------------------------------------------

	SelectHis::SelectHis(Context* ctx, const ActorList& oldlist, const ActorList& newlist)
		: History(ctx, "Selection")
		, m_oldlist(oldlist)
		, m_newlist(newlist)
	{}

	SelectHis::~SelectHis()
	{}

	void SelectHis::doIt()
	{
		m_context->setSelection(m_newlist, false);
	}

	void SelectHis::undo()
	{
		m_context->setSelection(m_oldlist, false);
	}

	int SelectHis::getMemoryUsed()
	{
		return sizeof(this) + sizeof(m_newlist) + sizeof(m_oldlist);
	}

}} // namespace Axon::Editor

