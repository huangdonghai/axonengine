/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_ACTION_H
#define AX_EDITOR_ACTION_H

namespace Axon { namespace Editor {
	//--------------------------------------------------------------------------
	// class Action
	//--------------------------------------------------------------------------

	class AX_API Action {
	public:
		// editor action
		enum Type {
			None, Delete, Redo, Undo, Clone,
			UserDefined = 128,
			MaxType = 256
		};

		Action(Context* context) : m_context(context) {}
		virtual ~Action() {}

		virtual void doIt() = 0;
		virtual void undo() { /* do nothing default */}
		virtual bool isUndoable() { return false; }
		virtual String getName() { return "Action"; }
		virtual String getMessage() { return getName(); }
		virtual void setMessage(const String& msg) { /* do nothing default */}
		virtual int getMemoryUsed() { return 0; }

	protected:
		Context* m_context;
	};

	//--------------------------------------------------------------------------
	// class ActionFactory
	//--------------------------------------------------------------------------

	class ActionFactory {
	public:
		virtual Action* create(Context* context) = 0;
	};

	template< class T >
	class ActionFactory_ : public ActionFactory {
		virtual Action* create(Context* context) {
			return new T(context);
		}
	};

	//--------------------------------------------------------------------------
	// class UndoAction, undo action
	//--------------------------------------------------------------------------

	class UndoAction : public Action {
	public:
		UndoAction(Context* context);
		virtual ~UndoAction();

		virtual void doIt();
	};

	//--------------------------------------------------------------------------
	// class RedoAction, redo action
	//--------------------------------------------------------------------------

	class RedoAction : public Action {
	public:
		RedoAction(Context* context);
		virtual ~RedoAction();

		virtual void doIt();
	};

	//--------------------------------------------------------------------------
	// class DeleteAction, undo action
	//--------------------------------------------------------------------------

	class DeleteAction : public Action {
	public:
		DeleteAction(Context* context);
		virtual ~DeleteAction();

		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable() { return true; }
		virtual String getName() { return "Delete"; }
		virtual String getMessage() { return getName(); }
		virtual int getMemoryUsed();

	private:
		bool m_isFirst;
		ActorList m_actorlist;
	};

	//--------------------------------------------------------------------------
	// class CloneAction, clone action
	//--------------------------------------------------------------------------

	class History;
	class CloneAction : public Action {
	public:
		CloneAction(Context* context);
		virtual ~CloneAction();

		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable() { return true; }
		virtual String getName() { return "Clone"; }
		virtual String getMessage() { return getName(); }
		virtual int getMemoryUsed();

	private:
		bool m_isFirst;
		ActorList m_actorlist;
		History* m_selectionHis;
	};
}} // namespace Axon::Editor

#endif

