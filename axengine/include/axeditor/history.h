/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_HISTORY_H
#define AX_EDITOR_HISTORY_H

AX_BEGIN_NAMESPACE

typedef std::list<Action*> ActionList;
typedef shared_ptr<Action> ActionPtr;
typedef std::list<ActionPtr> HistoryList;

class AX_API History : public Action {
public:
	History(Context *context) : Action(context) {}
	History(Context *context, const std::string &msg) : Action(context), m_message(msg) {}
	virtual ~History() {}

	virtual void doIt() = 0;
	virtual void undo() { /* do nothing default */}
	virtual bool isUndoable() { return true; }
	virtual std::string getName() { return "History"; }
	virtual std::string getMessage() { return m_message; }
	virtual void setMessage(const std::string &msg) { m_message = msg; }
	virtual int getMemoryUsed() { return 0; }

protected:
	std::string m_message;
};

//--------------------------------------------------------------------------
// class TransformHis
//--------------------------------------------------------------------------

class TransformHis : public History {
public:
	TransformHis(Context *context, const std::string &msg, int actorId, const Matrix &oldmat, const Matrix &newmat);
	virtual ~TransformHis();

	virtual void doIt();
	virtual void undo();
	virtual bool isUndoable();
	virtual std::string getName();
	virtual int getMemoryUsed();

private:
	int m_actorId;
	Matrix m_oldMat;
	Matrix m_newMat;
};

//--------------------------------------------------------------------------
// class GroupHis
//--------------------------------------------------------------------------

class GroupHis : public History {
public:
	GroupHis(Context *context, const std::string &msg);
	virtual ~GroupHis();

	virtual void doIt();
	virtual void undo();
	virtual bool isUndoable();
	virtual std::string getName();
	virtual int getMemoryUsed();

	void append(Action *his);
	bool isEmpty() const { return m_actionList.empty(); }

private:
	HistoryList m_actionList;
};


//--------------------------------------------------------------------------
// class DeleteHis
//--------------------------------------------------------------------------

class DeleteHis : public History {
public:
	DeleteHis(Context *context, const std::string &msg, const AgentList &actorlist);
	virtual ~DeleteHis() {}

	virtual void doIt();
	virtual void undo();
	virtual std::string getName() { return "DeleteHis"; }
	virtual int getMemoryUsed();

private:
	AgentList m_actorList;
};

//--------------------------------------------------------------------------
// class UndeleteHis
//--------------------------------------------------------------------------

class UndeleteHis : public History {
public:
	UndeleteHis(Context *context, const std::string &msg, const AgentList &actorlist);
	virtual ~UndeleteHis() {}

	virtual void doIt();
	virtual void undo();
	virtual std::string getName() { return "UndeleteHis"; }
	virtual int getMemoryUsed();

private:
	AgentList m_actorList;
};

//--------------------------------------------------------------------------
// class PropertyEditHis
//--------------------------------------------------------------------------

class PropertyEditHis : public History {
public:
	PropertyEditHis(Context *context, Agent *actor, const std::string &propname, const Variant &oldvalue, const Variant &newvalue);
	virtual ~PropertyEditHis();

	virtual void doIt();
	virtual void undo();
	virtual std::string getName() { return "PropertyEditHis"; }
	virtual bool isUndoable() { return true; }
	virtual int getMemoryUsed();

private:
	Agent *m_actor;
	const std::string m_propName;
	const Variant m_oldValue;
	const Variant m_newValue;
};

//--------------------------------------------------------------------------
// class SelectHis
//--------------------------------------------------------------------------

class SelectHis : public History {
public:
	SelectHis(Context *context, const AgentList &oldlist, const AgentList &newlist);
	virtual ~SelectHis();

	virtual void doIt();
	virtual void undo();
	virtual std::string getName() { return "SelectHis"; }
	virtual bool isUndoable() { return true; }
	virtual int getMemoryUsed();

private:
	AgentList m_oldlist;
	AgentList m_newlist;
};

//--------------------------------------------------------------------------
// class HistoryManager
//--------------------------------------------------------------------------

class AX_API HistoryManager {
public:
	HistoryManager();
	~HistoryManager();

	void addHistory(Action *his);
	void undo(int count);
	void redo(int count);

	// stacked
	void begin(Context *ctx, const std::string &msg);
	void end();

	void clear();

	HistoryList getUndoList() { return m_undoList; }
	HistoryList getRedoList() { return m_redoList; }

private:
	HistoryList m_undoList;
	HistoryList m_redoList;

	typedef std::list<GroupHis*> GroupHisList;
	GroupHisList m_stacked;
};

AX_END_NAMESPACE

#endif

