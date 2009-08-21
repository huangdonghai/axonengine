/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_MAP_CONTEXT_H
#define AX_EDITOR_MAP_CONTEXT_H

namespace Axon { namespace Editor { namespace MapEdit {

	class Layer {
	public:
	private:
		typedef DictSet<int> IdSet;
		int m_id;
		IdSet m_actors;

	};

	struct Bookmark 
	{
		int id;
		String name;
		AffineMat viewMatrix;
	};

	class AX_API MapContext : public Editor::Context {
	public:
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

		MapContext();
		~MapContext();

#if 0
		int generateActorId();
		void addActor(Actor* a);
		void removeActor(Actor* a);
		Actor* findActor(int id);

		Action* createAction(Action::Type type);
		Tool* createTool(Tool::Type type);
#endif
		void reset();

		String getTitle() const;
		String getFilename() const;
		bool createNew();
		bool load(const String& filename);
		bool save();
		bool saveAs(const String& filename);
#if 0
		bool isDirty();
		bool isLoading() const { return m_isLoading; }
#endif
		// view process
		void setActiveView(Editor::View* view) { m_activeView = view; }
		Editor::View* getActiveView() const { return m_activeView; }

		Vector3 getViewPos();
#if 0
		void doAction(Action::Type action);
		void doTool(Tool::Type tool);
		Tool* getTool() const { return m_tool; }
		Tool::Type getToolType() const { return m_tooltype; }

		// history
		void addHistory(Action* his);
		void undo();
		void redo();
		HistoryManager* getHistory();

		// selection
		const ActorList& getSelection() { return m_selections; }
		History* setSelectionHistoried(const ActorList& elist);
		void setSelection(const ActorList& elist, bool undoable=true);
		void setSelection(Actor* actor, bool undoable=true);
		void selectNone(bool undoable=true);
		void selectAll(bool undoable=true);
		void selectInvert(bool undoable=true);
		void addSelection(const ActorList& elist, bool undoable=true);
#endif

		MapTerrain* createTerrain(int tiles, int tilemeters);
		MapTerrain* getTerrain();
		void setTerrainMaterialDef(Map::MaterialDef* matdef);

		GameWorld* getGameWorld() const { return m_gameWorld; }
		void runGame();

		// present
		void doRender(const Render::Camera& camera, bool world = false);
		void doSelect(const Render::Camera& camera, int part);

		// bookmarks --timlly add
		void addBookmark(const AffineMat& viewMatrix, const String& name = "", int id = -1);
		void addBookmark(const Bookmark &bookmark);
		void deleteBookmark(const String& name);
		void deleteBookmark(int index);

		int getNumBookmark();
		Bookmark* getBookmark(const String& name);
		Bookmark* getBookmark(int index);

		void clearAllBookmarks();

		// properties
		void setActorProperty(const String& propName, const Variant& value);

		// map state
		MapState* getMapState() const { return m_mapState; }
		void setMapState(MapState* val) { m_mapState = val; }

	protected:
		void writeToFile(File* f);
		void readActor(const TiXmlElement* node);

		// save/load the helper info in the editor. --timlly add
		void saveEditorInfo(const String& filename);
		bool loadEditorInfo(const String& filename);

		// save/load the bookmark info. --timlly add
		void saveBookmarkInfo(File* file, int indent);
		void loadBookmarkInfo(const TiXmlElement* elem);

	private:
#if 0
		ActorDict m_actorDict;
		int m_maxId;

		ActionFactory* m_actionFactories[Action::MaxType];
		ToolFactory* m_toolFactories[Tool::MaxType];
#endif

	private:
		GameWorld* m_gameWorld;
		String m_title;
		String m_filename;
		MapTerrain* m_terrain;
		Game::TerrainFixed* m_terrainFixed;

		// views
		PerspectiveView* m_perspectiveView;
		TopView* m_topView;
		FrontView* m_frontView;
		LeftView* m_leftView;

#if 0
		Editor::View* m_indexedViews[Editor::View::NumView];
		Editor::View* m_activeView;

		// tool
		Tool::Type m_tooltype;
		Tool* m_tool;

		// selection
		ActorList m_selections;

		// history
		HistoryManager m_editorHistory;

		// dirty flag
		bool m_isDirty;
		bool m_isLoading;
#endif
		// map state
		MapState* m_mapState;

		// bookmarks  --timlly add
		Sequence<Bookmark>	m_bookmarks;
		int m_bookmarkIndex;
	};

	inline String MapContext::getTitle() const {
		if (m_filename.empty())
			return m_title;
		return m_filename;
	}

	inline String MapContext::getFilename() const {
		return m_filename;
	}

#if 0
	inline renderWorld* Context::getRenderWorld() {
		return m_renderWorld;
	}
#endif

	inline MapTerrain* MapContext::getTerrain() {
		return m_terrain;
	}

}}} // namespace Axon::Editor::MapEdit

#endif // end guardian

