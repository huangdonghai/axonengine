/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "creationpanel.h"
#include "workbench.h"
#include "filedialog.h"

CreationPanel::CreationPanel(QWidget *parent)
    : QWidget(parent)
{
	m_renderActor = nullptr;

	ui.setupUi(this);

	ui.rollup->initialize();

	ui.modelButton->setDefaultAction(g_workbench->ui.actionCreateStatic);
	ui.buildingButton->setDefaultAction(g_workbench->ui.actionCreateBrush);
	ui.entity->setDefaultAction(g_workbench->ui.actionCreateEntity);
	ui.createTree->setDefaultAction(g_workbench->ui.actionCreateTree);

	g_settings->beginGroup("CreationPanel");
	g_settings->endGroup();

	m_filename[ET_Model] = u2q("models/box.mesh"); // "models/box.mesh";

	g_mapContext->getMapState()->staticModelName = q2u(m_filename[ET_Model]);

	ui.selectModel->setRoot("models/");
	ui.selectModel->setFilter("*.mesh");
	ui.selectModel->refresh();

	ui.selectTree->setRoot("speedtrees/");
	ui.selectTree->setFilter("*.spt");
	ui.selectTree->refresh();

#if 0
	ui.selectEntity->setRoot("scripts/entities/");
	ui.selectEntity->setFilter("*.lua");
	ui.selectEntity->setShowExt(false);
	ui.selectEntity->refresh();
#else
	initEntityList();
#endif

#if 0
	PropertyCollection *collection = new PropertyCollection(QLatin1String("<root>"));
	IProperty *p;

	p = new SeparatorProperty("", "Object"); collection->addProperty(p);

	p = new IntProperty(0, "int"); collection->addProperty(p);

	p = new StringProperty("abc", "string"); collection->addProperty(p);

	p = new ColorProperty(QColor(255,255,255), "color"); collection->addProperty(p);

	p = new BoolProperty(true, "bool"); collection->addProperty(p);

	p = new DoubleProperty(1.0, "double"); collection->addProperty(p);

	p = new DateTimeProperty(QDateTime(), "datetime"); collection->addProperty(p);

	p = new UrlProperty(QUrl(), "url"); collection->addProperty(p);

	ui.propEditor->setInitialInput(collection);
	ui.propEditor->setProperHeight();
#endif
	g_mapContext->attachObserver(this);
}

CreationPanel::~CreationPanel()
{
	if (m_renderActor) {
		ui.preview->getRenderWorld()->removeEntity(m_renderActor);
		delete m_renderActor;
	}
	g_mapContext->detachObserver(this);
}

void CreationPanel::doNotify(IObservable *subject, int arg)
{
	if (subject != g_mapContext) {
		return;
	}

	bool showbasic = false;
	bool showmodel = false;

	int track = Context::SelectionChanged | Context::ToolChanged;

	if (!(arg&track)) {
		return;
	}

	ui.rollup->setAllPagesVisible(false);

	const AgentList &actorlist = g_mapContext->getSelection();

	if (!actorlist.containsOne()) {
		ui.objectName->setText(QString("%1 objects").arg(actorlist.size()));
	} else {
		Agent *actor = actorlist.back();
		GameObject *node = static_cast<MapActor*>(actor)->getGameObject();
		ui.objectName->setText(u2q(node->get_objectName()));
		ui.objectColor->setColor(actor->getColor());
	}

	if (actorlist.empty()) {
		showbasic = false;
	} else {
		showbasic = true;
	}

	int tooltype = g_mapContext->getToolType();

	if (tooltype == MapTool::CreateStatic) {
		ui.rollup->setPageVisible(0, true);
		ui.rollup->setPageVisible(3, true);
	} else if (tooltype == MapTool::CreateEntity) {
		ui.rollup->setPageVisible(0, true);
		ui.rollup->setPageVisible(1, true);
		ui.rollup->setPageVisible(3, true);
	} else if (tooltype == MapTool::CreateTree) {
		ui.rollup->setPageVisible(2, true);
		ui.rollup->setPageVisible(3, true);
	}

	ui.rollup->setPageVisible(4, showbasic);
	ui.rollup->setPageVisible(5, showbasic);
}


void CreationPanel::on_modelButton_clicked()
{

}

void CreationPanel::on_selectModel_itemDoubleClicked(QTreeWidgetItem *item,int)
{
	FileItem *f_item = dynamic_cast<FileItem*>(item);
	if (!f_item)
		return;

	const FileInfo &finfo = f_item->getFileInfo();

	if (f_item->isDirectory()) {
		return;
	}

	RenderWorld *world = ui.preview->getRenderWorld();
	if (m_renderActor) {
		world->removeEntity(m_renderActor);
		delete m_renderActor;
	}

	g_mapContext->getMapState()->staticModelName = finfo.fullpath;

	m_renderActor = new HavokModel(finfo.fullpath);
	world->addEntity(m_renderActor);

	ui.preview->setFocusActor(m_renderActor);
	ui.preview->update();

	// set model for selected actor
	const AgentList &actorlist = g_mapContext->getSelection();

	if (actorlist.containsOne()) {
		actorlist.setNodeProperty("model", finfo.fullpath);
	}
}

void CreationPanel::on_selectEntity_itemDoubleClicked(QTreeWidgetItem *item,int)
{
	if (!item)
		return;

	if (item->type()) {
		return;
	}

	g_mapContext->getMapState()->entityClass = "Game.";
	g_mapContext->getMapState()->entityClass += q2u(item->toolTip(0));
}



void CreationPanel::on_selectTree_itemDoubleClicked(QTreeWidgetItem *item,int)
{
#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	FileItem *f_item = dynamic_cast<FileItem*>(item);
	if (!f_item)
		return;

	const FileInfo &finfo = f_item->getFileInfo();

	if (f_item->isDirectory()) {
		return;
	}

	RenderWorld *world = ui.preview->getRenderWorld();
	if (m_renderActor) {
		world->removeEntity(m_renderActor);
		delete m_renderActor;
	}

	m_renderActor = new TreeEntity(finfo.fullpath);
	world->addEntity(m_renderActor);

	ui.preview->setFocusActor(m_renderActor);
	ui.preview->update();

	g_mapContext->getMapState()->treeFilename = finfo.fullpath;

	// set model for selected actor
	const AgentList &actorlist = g_mapContext->getSelection();

	if (actorlist.containsOne()) {
		actorlist.setNodeProperty("tree", finfo.fullpath);
	}
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40
}

static QTreeWidgetItem *findOrCreate(QTreeWidgetItem *item, const QString &fullname, const QString &name, bool isdir)
{
	QTreeWidgetItem *found = 0;

	for (int i=0; i<item->childCount(); i++) {
		if (item->child(i)->text(0) == name) {
			found = item->child(i);
			break;
		}
	}

	if (found) {
		return found;
	}

	found = new QTreeWidgetItem(item, isdir);

	found->setText(0, name);
	found->setToolTip(0, fullname);

	if (isdir) {
		found->setIcon(0, QIcon(":/images/dirclosed-16.png"));
	} else {
		found->setIcon(0, QIcon(":/fileicons/xfile_16.png"));
	}

	found->setHidden(false);

	return found;
}

void CreationPanel::initEntityList()
{
	StringSeq entlist;
	g_scriptSystem->getClassList("Game.", true, entlist);

	for (int i = 0; i < entlist.size(); i++) {
		QString entname = u2q(entlist[i]);

		int pos = 0;
		QTreeWidgetItem *item = ui.selectEntity->invisibleRootItem();

		while (1) {
			int curpos = entname.indexOf(QChar('.'), pos);
			if (curpos != -1) {
				item = findOrCreate(item, entname.left(curpos), entname.mid(pos, curpos-pos), true);
				pos = curpos+1;
				continue;
			}

			if (pos == 0) {
				// discard root entity type
				break;
			}

			item = findOrCreate(item, entname, entname.right(entname.size()-pos), false);
			break;
		}
	}

	int c = ui.selectEntity->topLevelItemCount();
}

void CreationPanel::on_objectColor_colorChanged( const QColor &color )
{
	const AgentList &actorlist = g_mapContext->getSelection();

	actorlist.setColor(q2x(color));
}