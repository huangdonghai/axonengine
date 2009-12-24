/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_CONTEXT_H
#define AX_EDITOR_CONTEXT_H

AX_BEGIN_NAMESPACE

	class AX_API Context : public IObservable {
	public:
		typedef Dict<int, Agent*> AgentDict;

		enum ObserverFlag {
			SelectionChanged = 1,
			HistoryChanged = 2,
			StatusChanged = 4,
			TerrainMaterialEdited = 8,
			ToolChanged = 0x10,
			EnvironmentChanged = 0x20,
			ActorTransformed = 0x40,
			EverythingChanged = 0xffffffff
		};

		Context();
		virtual ~Context();

		// present
		virtual void doRender(const RenderCamera& camera, bool world = false) = 0;
		virtual void doSelect(const RenderCamera& camera, int part) = 0;
		virtual MapTerrain *getTerrain() { return 0; }

		int generateActorId();
		void addActor(Agent *a);
		void removeActor(Agent *a);
		Agent *findActor(int id);
		const AgentDict& getActorDict() { return m_agentDict; }

		Action *createAction(int type);
		Tool *createTool(int type);

		void reset();

		String getTitle() const;
		String getFilename() const;
		bool createNew();
		bool load(const String& filename);
		bool save();
		bool saveAs(const String& filename);
		bool isDirty() const { return m_isDirty; }
		bool isLoading() const { return m_isLoading; }

		// view process
		int getNumViews() const { return m_numViews; }
		View *getView(int index);
		void setActiveView(View *view) { m_activeView = view; }
		View *getActiveView() const { return m_activeView; }

		Vector3 getViewPos();

		void doAction(int action);
		void doTool(int tool);
		Tool *getTool() const { return m_tool; }
		int getToolType() const { return m_tooltype; }

		// history
		void addHistory(Action *his);
		void beginHis(const String& msg) { m_historyManager.begin(this, msg); }
		void endHis() { m_historyManager.end(); }
		void undo();
		void redo();
		HistoryManager *getHistory();

		// selection
		const AgentList& getSelection() { return m_selections; }
		History *setSelectionHistoried(const AgentList& elist);
		void setSelection(const AgentList& elist, bool undoable=true);
		void setSelection(Agent *actor, bool undoable=true);
		void selectNone(bool undoable=true);
		void selectAll(bool undoable=true);
		void selectInvert(bool undoable=true);
		void addSelection(const AgentList& elist, bool undoable=true);

		// properties
		void setActorProperty(const String& propName, const Variant& value);

		// state
		State *getState() const { return m_state; }
		void setState(State *val) { m_state = val; }

	protected:
		int m_maxId;
		AgentDict m_agentDict;
		bool m_isDirty;
		bool m_isLoading;

		ActionFactory *m_actionFactories[Action::MaxType];
		ToolFactory *m_toolFactories[Tool::MaxType];

		int m_numViews;
		View *m_indexedViews[View::MaxView];
		View *m_activeView;

		// tool
		int m_tooltype;
		Tool *m_tool;

		// selection
		AgentList m_selections;

		// history
		HistoryManager m_historyManager;

		// state
		State *m_state;
	};

AX_END_NAMESPACE

#endif

