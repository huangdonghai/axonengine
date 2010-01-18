/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

AX_BEGIN_NAMESPACE

template< class T >
class MapToolFactory_ : public ToolFactory {
	virtual Tool *create(Context *context) {
		return new T(static_cast<MapContext*>(context));
	}
};

//--------------------------------------------------------------------------
// class MapContext
//--------------------------------------------------------------------------

MapContext::MapContext() {
	m_mapState = new MapState();
	setState(m_mapState);

	m_terrain = (nullptr);
	m_bookmarkIndex = 0;
	
	m_toolFactories[Tool::Select]		= new ToolFactory_<SelectTool>;
	m_toolFactories[Tool::Move]			= new ToolFactory_<MoveTool>;
	m_toolFactories[Tool::Rotate]		= new ToolFactory_<RotateTool>;
	m_toolFactories[Tool::Scale]		= new ToolFactory_<ScaleTool>;
	m_toolFactories[MapTool::TerrainRaise]	= new MapToolFactory_<TerrainRaiseTool>;
	m_toolFactories[MapTool::TerrainLower]	= new MapToolFactory_<TerrainLowerTool>;
	m_toolFactories[MapTool::TerrainLevel]	= new MapToolFactory_<TerrainFlatTool>;
	m_toolFactories[MapTool::TerrainSmooth]	= new MapToolFactory_<TerrainSmoothTool>;
	m_toolFactories[MapTool::TerrainGrab]	= new MapToolFactory_<TerrainGrabTool>;
	m_toolFactories[MapTool::TerrainPaint]	= new MapToolFactory_<TerrainPaintTool>;
	m_toolFactories[MapTool::TerrainErase]	= new MapToolFactory_<TerrainEraseTool>;
	m_toolFactories[MapTool::CreateStatic]	= new MapToolFactory_<CreateStaticTool>;
	m_toolFactories[MapTool::CreateEntity]	= new MapToolFactory_<CreateEntityTool>;
#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	m_toolFactories[MapTool::CreateTree]	= new MapToolFactory_<CreateTreeTool>;
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

	m_actionFactories[Action::Delete]	= new ActionFactory_<DeleteAction>;
	m_actionFactories[Action::Undo]		= new ActionFactory_<UndoAction>;
	m_actionFactories[Action::Redo]		= new ActionFactory_<RedoAction>;
	m_actionFactories[Action::Clone]	= new ActionFactory_<CloneAction>;

	m_perspectiveView = new PerspectiveView(this);
	m_topView = new TopView(this);
	m_frontView = new FrontView(this);
	m_leftView = new LeftView(this);

	m_indexedViews[MapView::Perspective] = m_perspectiveView;
	m_indexedViews[MapView::Top] = m_topView;
	m_indexedViews[MapView::Front] = m_frontView;
	m_indexedViews[MapView::Left] = m_leftView;

	m_numViews = MapView::Number;

	m_activeView = m_perspectiveView;

	m_gameWorld = new GameWorld();
}

MapContext::~MapContext() {
	reset();

	SafeDelete(m_gameWorld);
	SafeDelete(m_perspectiveView);

	setState(0);
	SafeDelete(m_mapState);
}

void MapContext::reset() {
	SafeDelete(m_tool);
	m_selections.clear();
	m_historyManager.clear();

	if (m_terrain) {
		SafeDelete(m_terrainFixed);
		SafeDelete(m_terrain);
	}

	// release actors
	AgentDict::iterator it = m_agentDict.begin();
	for (; it != m_agentDict.end(); ++it) {
		SafeDelete(it->second);
	}

	m_agentDict.clear();
	m_isDirty = false;

	m_maxId = 0;

	m_title = "untitled";
	m_filename.clear();

	g_gameSystem->reset();
	m_gameWorld->reset();

	notify(EverythingChanged);
}

bool MapContext::createNew() {
	reset();
	return true;
}

bool MapContext::load(const String &filename) {
	m_isLoading = true;

	reset();

	m_perspectiveView->doUpdate();

	void *filebuf;
	size_t filesize = g_fileSystem->readFile(filename, &filebuf);

	if (!filesize || !filebuf)
		return false;

	TiXmlDocument doc;

	doc.Parse((const char*)filebuf, NULL, TIXML_ENCODING_UTF8);
	g_fileSystem->freeFile(filebuf);

	if (doc.Error()) {
		Errorf(_("%s: error parse %s in line %d - %s")
			, __func__
			, filename.c_str()
			, doc.ErrorRow()
			, doc.ErrorDesc());
		return false;
	}

	const TiXmlElement *root = doc.FirstChildElement("map");
	const TiXmlAttribute *attr = NULL;

	if (!root)
		return false;

	String map_name = PathUtil::removeExt(filename);

	int progress = 0;
	int numActors = 0;
	int actorRead = 0;

	root->Attribute("numActors", &numActors);

	g_system->beginProgress("Loading Map...");

	g_system->showProgress(progress += 5, "Loading XML...");

	const TiXmlElement *elem;
	for (elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
		const String &value = elem->ValueTStr();

		if (value == "envDef") {
			MapEnvDef *ed = m_gameWorld->getEnvironment();
			if (ed) {
				ed->readProperties(elem);
				m_gameWorld->updateEnvdef();
			}

		} else if (value == "terrain") {
			m_terrain = new MapTerrain;
			g_system->showProgress(progress += 5, "Loading kTerrain...");
			m_terrainFixed = new TerrainFixed(m_terrain);
			m_terrain->initFromXml(map_name, elem);
			m_gameWorld->addObject(m_terrainFixed);
//				gEditorActiveAreaMgr->setTerrain(m_terrain);
		} else if (value == "actor") {
			progress = 10 + (float)actorRead / numActors * 90;
			char msg[200];
			StringUtil::snprintf(msg, ArraySize(msg), "Loading actor #%d...", actorRead++);
			g_system->showProgress(progress, msg);
			readActor(elem);
		} else if (value == "area") {
//				gEditorActiveAreaMgr->initFromXml(map_name,elem);
		
		} else {
			Debugf("don't know element\n");
		}
	}

	m_filename = filename;

	loadEditorInfo(filename);	// load editor info. --timlly add

	g_system->endProgress();

	m_gameWorld->updateEnvdef();

	notify(EverythingChanged);

	m_isLoading = false;

	return true;
}

bool MapContext::save() {
	std::auto_ptr<File> file(g_fileSystem->openFileWrite(m_filename));
	if (!file.get())
		return false;

	writeToFile(file.get());

	saveEditorInfo(m_filename);	// save editor info --timlly add.

	return true;
}

bool MapContext::saveAs(const String &filename) {
	std::auto_ptr<File> file(g_fileSystem->openFileWrite(filename));
	if (!file.get())
		return false;

	writeToFile(file.get());

	saveEditorInfo(filename);	// save editor info --timlly add.

	m_filename = filename;

	return true;
}

void MapContext::writeToFile(File *f) {
	int numActors = s2i(m_agentDict.size());

	f->printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	f->printf("<map haveTerrain=\"%d\" numActors=\"%d\">\n", m_terrain ? 1 : 0, numActors);

	// write env
	MapEnvDef *ed = m_gameWorld->getEnvironment();
	if (ed) {
		ed->writeToFile(f, 1);
	}

	// write terrain
	if (m_terrain) {
		m_terrain->writeXml(f, 1);
//			gEditorActiveAreaMgr->writeXml(f,1);
	}

	// write actors
	AgentDict::iterator it = m_agentDict.begin();
	for (; it != m_agentDict.end(); ++it) {
		MapAgent *actor = (MapAgent*)it->second;

		if (!actor) continue;

		if (actor->isDeleted()) {
			continue;
		}
		actor->writeXml(f, 1);
	}
	f->printf("</map>\n");

	m_isDirty = false;
}

#if 0
bool MapContext::isDirty() {
	return m_isDirty;
}
#endif

Vector3 MapContext::getViewPos() {
	return m_perspectiveView->getEyeMatrix().origin;
}

MapTerrain *MapContext::createTerrain(int tiles, int tilemeters) {
	if (m_terrain) {
		Errorf("%s: map already has a terrain object");
		return nullptr;
	}

	m_terrain = new MapTerrain();
	m_terrainFixed = new TerrainFixed(m_terrain);
	m_terrain->init(tiles, tilemeters);

	m_gameWorld->addObject(m_terrainFixed);

	return m_terrain;
}

void MapContext::setTerrainMaterialDef(MapMaterialDef *matdef)
{
	if (m_terrain) {
		MapMaterialDef *oldmatdef = m_terrain->getMaterialDef()->clone();
		m_terrain->setMaterialDef(matdef);

		TerrainMaterialDefHis *his = new TerrainMaterialDefHis(oldmatdef, matdef->clone(), m_terrain);
		addHistory(his);

		notify(MapContext::TerrainMaterialEdited);
	}
}

void MapContext::doRender(const RenderCamera &camera, bool world) {
	if (world) {
		m_gameWorld->drawScene(camera);
	}

	if (g_gameSystem->isRunning()) {
		return;
	}

	g_renderSystem->beginScene(camera);
	// present static
	AgentDict::iterator it = m_agentDict.begin();

	for (; it != m_agentDict.end(); ++it) {
		if (it->second) it->second->drawHelper();
	}

	if (m_tool) {
		m_tool->doRender(camera);
	}

	g_renderSystem->endScene();
}

void MapContext::doHitTest(const RenderCamera &camera, int part) {
	// select terrain
	if (part & SelectPart::kTerrain) {
		if (m_terrain)
			m_terrain->doSelect(camera);
	}

	if (part - SelectPart::kTerrain == 0)
		return;

	// present static
	AgentDict::iterator it = m_agentDict.begin();

	for (; it != m_agentDict.end(); ++it) {
		MapAgent *actor = (MapAgent*)it->second;
		if (actor && !actor->isDeleted())
			actor->doHitTest();
	}
}

void MapContext::runGame() {
	g_gameSystem->startRunning();
}

#if 0
void MapContext::startGaming() {
	ActorDict::iterator it = m_actorDict.begin();
	for (; it != m_actorDict.end(); ++it) {
		Actor *actor = it->second;
		if (actor->getType() != Actor::kEntity) {
			continue;
		}

		Entity *ent = dynamic_cast<Entity*>(actor);
		AX_ASSERT(ent);
		ent->backup();
	}
}

void MapContext::stopGaming() {
	ActorDict::iterator it = m_actorDict.begin();
	for (; it != m_actorDict.end(); ++it) {
		Actor *actor = it->second;
		if (actor->getType() != Actor::kEntity) {
			continue;
		}

		Entity *ent = dynamic_cast<Entity*>(actor);
		AX_ASSERT(ent);
		ent->restore();
	}
}
#endif

void MapContext::readActor(const TiXmlElement *node) {
	const char *typestr = node->Attribute("type");
	if (!typestr) {
		return;
	}

	int id;
	Matrix matrix;
	Rgb color;

	const char *v = node->Attribute("id", &id);
	if (!v) {
		return;
	}

	v = node->Attribute("matrix");
	if (!v) {
		return;
	}

	matrix.fromString(v);

	v = node->Attribute("color");
	if (!v)
		return;

	color.fromString(v);

	const TiXmlElement *childnode = node->FirstChildElement("node");

	if (!childnode) {
		return;
	}

	const char *childtypename = childnode->Attribute("className");

	if (!childtypename) {
		return;
	}

	MapAgent::Type type = MapAgent::stringToType(typestr);

	MapAgent *actor = nullptr;

	switch (type) {
	case MapAgent::kNone:
		return;
	case MapAgent::kStatic:
		actor = new MapStatic();
		break;
	case MapAgent::kSpeedTree:
#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
		actor = new MapSpeedTree();
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40
		break;
	case MapAgent::kBrush:
		break;
	case MapAgent::kEntity:
		actor = new MapActor(childtypename);
		break;
	}

	if (!actor) {
		return;
	}

	GameObject *gamenode =  actor->getGameObject();
	gamenode->readXml(childnode);

	actor->setMatrix(matrix);
	actor->setColor(color);
	actor->setId(id);

	actor->addToContext();

	// update some info
	m_maxId = std::max(id, m_maxId);
	g_scriptSystem->updateNameIndex(gamenode->get_objectName());
}

void MapContext::addBookmark(const Matrix &viewMatrix, const String &name, int id)
{
	Bookmark bookmark;

	if (id == -1)
	{
		bookmark.id = m_bookmarkIndex;
	}
	else
	{
		bookmark.id = id;
	}
	
	if (name == "")
	{
		char ch[100];

		_itoa(bookmark.id, ch, 10);

		bookmark.name = String("bookmark ") + ch;
	}
	else
	{
		bookmark.name = name;
	}
	
	bookmark.viewMatrix = viewMatrix;

	addBookmark(bookmark);
}

void MapContext::addBookmark(const Bookmark &bookmark)
{
	m_bookmarks.push_back(bookmark);

	++ m_bookmarkIndex;
}

void MapContext::deleteBookmark(const String &name)
{
	Sequence<Bookmark>::iterator itr;

	for (itr=m_bookmarks.begin(); itr!=m_bookmarks.end(); ++itr)
	{
		if ((*itr).name == name)
		{
			m_bookmarks.erase(itr);

			break;
		}
	}
}

void MapContext::deleteBookmark(int index)
{
	AX_ASSERT(index < (int) m_bookmarks.size() && index >= 0);

	m_bookmarks.erase(m_bookmarks.begin() + index);
}

int MapContext::getNumBookmark()
{
	return (int) m_bookmarks.size();
}

Bookmark *MapContext::getBookmark(const String &name)
{
	Sequence<Bookmark>::iterator itr;

	for (itr=m_bookmarks.begin(); itr!=m_bookmarks.end(); ++itr)
	{
		if ((*itr).name == name)
		{
			return &(*itr);
		}
	}

	return NULL;
}

Bookmark *MapContext::getBookmark(int index)
{
	//AX_ASSERT(index < (int) m_bookmarks.size());

	if (index < (int) m_bookmarks.size() && index >= 0)
	{
		return &m_bookmarks[index];
	}
	else
	{
		return NULL;
	}
}

void MapContext::clearAllBookmarks()
{
	m_bookmarks.clear();
	m_bookmarkIndex = 0;
}

void MapContext::saveEditorInfo(const String &filename)
{
	String filepath = PathUtil::removeExt(filename) + "_editor.info";

	File *file = g_fileSystem->openFileWrite(filepath);
	if (!file)
	{
		return ;
	}

	file->printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	file->printf("<EditorInfo>\n");

	saveBookmarkInfo(file, 1);

	file->printf("</EditorInfo>\n");

	file->forceFlush();
	file->close();
}

bool MapContext::loadEditorInfo(const String &filename)
{
	String filepath = PathUtil::removeExt(filename) + "_editor.info";

	void *filebuf;
	size_t filesize = g_fileSystem->readFile(filepath, &filebuf);

	if (!filesize || !filebuf)
	{
		return false;
	}

	TiXmlDocument doc;

	doc.Parse((const char*)filebuf, NULL, TIXML_ENCODING_UTF8);
	g_fileSystem->freeFile(filebuf);

	if (doc.Error()) 
	{
		Errorf(_("%s: error parse %s in line %d - %s")
			, __func__
			, filename.c_str()
			, doc.ErrorRow()
			, doc.ErrorDesc());

		return false;
	}

	const TiXmlElement *root = doc.FirstChildElement("EditorInfo");
	const TiXmlAttribute *attr = NULL;

	if (!root)
	{
		return false;
	}

	//gSystem->beginProgress("Loading Editor Info...");

	const TiXmlElement *elem;
	for (elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) 
	{
		const String &value = elem->ValueTStr();

		if (value == "bookmark") 
		{
			loadBookmarkInfo(elem);
		}
		else 
		{
			Debugf("don't know element\n");
		}
	}

	//gSystem->endProgress();

	return true;
}

void MapContext::saveBookmarkInfo(File *file, int indent)
{
#define INDENT if (indent) file->printf("%s", ind.c_str());
	String ind(indent, '\t');		//String ind(indent*2, ' ');

	INDENT; file->printf("<bookmark num=\"%d\">\n", m_bookmarks.size());

	for (size_t i=0; i<m_bookmarks.size(); ++i)
	{
		INDENT; file->printf("\t<bookmarknode\n");

		INDENT; file->printf("\t\tid=\"%d\"\n", i);
		INDENT; file->printf("\t\tname=\"%s\"\n", m_bookmarks[i].name.c_str());
		INDENT; file->printf("\t\tviewmatrix=\"%s\"\n", m_bookmarks[i].viewMatrix.toString().c_str());

		INDENT; file->printf("\t/>\n");
	}
	
	INDENT; file->printf("</bookmark>\n");

#undef INDENT
}

void MapContext::loadBookmarkInfo(const TiXmlElement *elem)
{
	clearAllBookmarks();

	const TiXmlElement *bookmarkNode;
	Bookmark bookmark;
	const char *v(NULL);

	for (bookmarkNode = elem->FirstChildElement(); bookmarkNode; bookmarkNode = bookmarkNode->NextSiblingElement()) 
	{
		v = bookmarkNode->Attribute("id", &bookmark.id);
		if (!v)
		{
			return ;
		}

		v = bookmarkNode->Attribute("name");
		if (!v)
		{
			return ;
		}
		bookmark.name = v;

		v = bookmarkNode->Attribute("viewmatrix");
		if (!v)
		{
			return ;
		}
		bookmark.viewMatrix.fromString(v);

		addBookmark(bookmark);
	}
}

void MapContext::setActorProperty(const String &propName, const Variant &value)
{
	m_selections.setNodeProperty(propName,value);
}

AX_END_NAMESPACE
