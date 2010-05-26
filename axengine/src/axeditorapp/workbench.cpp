/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "workbench.h"
#include "editorframe.h"
#include "newmapdlg.h"
#include "terrainmaterialdlg.h"
#include "filedialog.h"
#include "application.h"

enum { SideBar_MinSize = 256, SideBar_MaxSize = 256 };

//------------------------------------------------------------------------------
// class Workbench
//------------------------------------------------------------------------------
Workbench::Workbench(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
	, m_modelViewer(nullptr)
	, m_materialEditor(nullptr)
	, m_selectByNameDlg(nullptr)
	, m_scriptEditor(nullptr)
	, m_gfxEditor(nullptr)
{
	hide();
	g_workbench = this;

	ui.setupUi(this);

	setIconSize(QSize(24, 24));

	g_system->registerProgress(this);

	createActions();
	createToolBars();
	createStatusBar();
	createDockablePanel();
	createMenus();

	g_mapContext->attachObserver(this);

	m_workspace = new Workspace(this);
	setCentralWidget(m_workspace);

	ui.actionToggleFrameMaximun->setEnabled(true);

	readSettings();

	onViewChanged();
	updateTitle();

	m_timerUpdateStatus = startTimer(1000);

	//m_cameraDlg.setParent(m_sidePanel);

	connect(ui.viewpos, SIGNAL(mouseDoubleClicked(QPoint)),
		this, SLOT(onViewLabelDoubleClicked(QPoint)));

	show();
}

Workbench::~Workbench()
{
	this->killTimer(m_timerUpdateStatus);
	g_system->removeProgress(this);
	g_mapContext->detachObserver(this);
//	SafeDelete(m_uiEditor);
}

void Workbench::updateStatusInfo(const Vector3 &vieworg, const Vector3 &cursorpos) {
	QString pos;
	ui.viewpos->setText(tr("ViewPos:") + pos.sprintf("%.1f,%.1f,%.1f", vieworg.x, vieworg.y, vieworg.z));
}

void Workbench::updateTitle() {
	QString title;
	if (g_mapContext->isDirty()) {
		title = "*" + u2q(g_mapContext->getTitle()) + " - axon editor";
	} else {
		title = u2q(g_mapContext->getTitle()) + " - axon editor";
	}
	setWindowTitle(title);
}

void Workbench::doNotify(IObservable *subjest, int arg) {
	if (subjest != g_mapContext)
		return;

	if (arg & Context::SelectionChanged) {
		AgentList sel = g_mapContext->getSelection();
		ui.actionDelete->setEnabled(!sel.empty());
		ui.actionClone->setEnabled(!sel.empty());
	}

	if (arg & Context::HistoryChanged) {
		onHistoryChanged();
	}

	if (arg & Context::StatusChanged) {
		Vector3 viewpos = g_mapContext->getViewPos();
		Vector3 viedir = g_mapContext->getActiveView()->getCamera().getViewAxis()[0];

		QString pos;
		QString dir;
		ui.viewpos->setText(tr(" EYE: ") + pos.sprintf("%.1f, %.1f, %.1f ", viewpos.x, viewpos.y, viewpos.z)
			+ tr(" DIR: ") + dir.sprintf("%.1f, %.1f, %.1f ", viedir.x, viedir.y, viedir.z));
		if (m_cameraDlg.isVisible())
		{
			m_cameraDlg.refreshDlg(g_mapContext->getActiveView()->getCamera());
		}
	}

	updateTitle();
}

void Workbench::onViewLabelDoubleClicked(QPoint point)
{
	RenderCamera camera = g_mapContext->getActiveView()->getCamera();

	m_cameraDlg.refreshDlg(camera);
	m_cameraDlg.show();
}

void Workbench::setLanguage(const QString &lang) {
	g_settings->setValue("language", lang);
	QMessageBox::about(this, tr("Selected Language"),
		tr("Language selection will enable when program restart"));
}

void Workbench::setStyle(const QString &style) {
	g_app->setStyle(style);

	g_settings->setValue("style", style);
}

void Workbench::addEditorTool(int type, QAction *action) {
	action->setCheckable(true);
	action->setData(type);
	m_editorTools->addAction(action);
}

void Workbench::addEditorAction(int type, QAction *action) {
	action->setCheckable(false);
	action->setData(type);
	m_editorActions->addAction(action);
}

void Workbench::createActions() {
	typedef Action editorAction;
	typedef MapTool editorTool;

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

	addEditorTool(editorTool::CreateStatic, ui.actionCreateStatic);
	addEditorTool(editorTool::CreateBrush, ui.actionCreateBrush);
	addEditorTool(editorTool::CreateEntity, ui.actionCreateEntity);
	addEditorTool(editorTool::CreateTree, ui.actionCreateTree);
}

void Workbench::createMenus() {
	QMenu *uiMenu = new QMenu(tr("Toolbars"));
	uiMenu->addAction(m_sideBar->toggleViewAction());
	uiMenu->addAction(ui.mainToolBar->toggleViewAction());
	ui.menu_Setting->addMenu(uiMenu);

	// init language menu
	QActionGroup *langGroup = new QActionGroup(this);
	langGroup->addAction(ui.actionSimplifiedChinese);
	langGroup->addAction(ui.actionEnglish);
	connect(ui.actionSimplifiedChinese, SIGNAL(triggered(bool)), this, SLOT(onSimplifiedChinese(bool)));
	connect(ui.actionEnglish, SIGNAL(triggered(bool)), this, SLOT(onEnglish(bool)));

	if (g_globalData->language.isEmpty())
		ui.actionEnglish->setChecked(true);
	else
		ui.actionSimplifiedChinese->setChecked(true);

	// init style menu
	QActionGroup *styleGroup = new QActionGroup(this);
	styleGroup->addAction(ui.actionStyleWindows);
	styleGroup->addAction(ui.actionStyleWindowsXP);
	styleGroup->addAction(ui.actionStylePlastique);
	styleGroup->addAction(ui.actionStyleMacintosh);
	connect(ui.actionStyleWindows, SIGNAL(triggered(bool)), this, SLOT(onStyleWindows(bool)));
	connect(ui.actionStyleWindowsXP, SIGNAL(triggered(bool)), this, SLOT(onStyleWindowsXP(bool)));
	connect(ui.actionStylePlastique, SIGNAL(triggered(bool)), this, SLOT(onStylePlastique(bool)));
	connect(ui.actionStyleMacintosh, SIGNAL(triggered(bool)), this, SLOT(onStyleMacintosh(bool)));

	if (g_globalData->style == "windows")
		ui.actionStyleWindows->setChecked(true);
	else if (g_globalData->style == "windowsxp")
		ui.actionStyleWindowsXP->setChecked(true);
	else if (g_globalData->style == "plastique")
		ui.actionStylePlastique->setChecked(true);
	else if (g_globalData->style == "macintosh")
		ui.actionStyleMacintosh->setChecked(true);
}

void Workbench::createToolBars() {
	// undo button
	m_undoToolButton = new QToolButton(this);
	m_undoToolButton->setDefaultAction(ui.actionEditUndo);
	m_undoToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	m_undoToolButton->setMenu(new QMenu(m_undoToolButton));
	ui.mainToolBar->insertWidget(ui.actionObjectSelect, m_undoToolButton);

	// redo button
	m_redoToolButton = new QToolButton(this);
	m_redoToolButton->setDefaultAction(ui.actionEditRedo);
	m_redoToolButton->setPopupMode(QToolButton::MenuButtonPopup);
	m_redoToolButton->setMenu(new QMenu(m_redoToolButton));
	ui.mainToolBar->insertWidget(ui.actionObjectSelect, m_redoToolButton);

	ui.mainToolBar->insertSeparator(ui.actionObjectSelect);

	ui.mainToolBar->insertWidget(ui.actionObjectSelect, ui.selectableType);

	QActionGroup *gizmoCenterGroup = new QActionGroup(this);

	gizmoCenterGroup->addAction(ui.actionUsePivotCenter);
	gizmoCenterGroup->addAction(ui.actionUseSelectionCenter);
	gizmoCenterGroup->addAction(ui.actionUseTransformCenter);

	ui.mainToolBar->insertWidget(ui.actionUsePivotCenter, ui.transformSpace);

	g_mapContext->getMapState()->selectionPart = SelectPart::All;

	{ // setup snap to grid
		QWidget *widget = ui.mainToolBar->widgetForAction(ui.actionSnapToGrid);
		QToolButton *tb = qobject_cast<QToolButton*>(widget);

		AX_ASSERT(tb);

		tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		ui.actionSnapToGrid->setIconText(QString("%1").arg(g_mapContext->getMapState()->snapToGrid));

		QMenu *menu = new QMenu(tb);
		tb->setPopupMode(QToolButton::MenuButtonPopup);
		tb->setMenu(menu);

		QAction *action = 0;
		action = menu->addAction("1/64");	action->setData(1.0f / 64.0f);
		action = menu->addAction("1/32");	action->setData(1.0f / 32.0f);
		action = menu->addAction("1/16");	action->setData(1.0f / 16.0f);
		action = menu->addAction("1/8");	action->setData(1.0f / 8.0f);
		action = menu->addAction("1/4");	action->setData(1.0f / 4.0f);
		action = menu->addAction("1/2");	action->setData(1.0f / 2.0f);
		action = menu->addAction("1");	action->setData(1.0f);
		action = menu->addAction("2");	action->setData(2.0f);
		action = menu->addAction("4");	action->setData(4.0f);
		action = menu->addAction("8");	action->setData(8.0f);
		action = menu->addAction("16");	action->setData(16.0f);
		action = menu->addAction("32");	action->setData(32.0f);
		action = menu->addAction("64");	action->setData(64.0f);

		connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onSnapToGridChanged(QAction*)));
	}

	{	// setup snap to angle
		QWidget *widget = ui.mainToolBar->widgetForAction(ui.actionSnapToAngle);
		QToolButton *tb = qobject_cast<QToolButton*>(widget);

		AX_ASSERT(tb);

		tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		ui.actionSnapToAngle->setIconText(QString("%1").arg(g_mapContext->getMapState()->snapToAngle));

		QMenu *menu = new QMenu(tb);
		tb->setPopupMode(QToolButton::MenuButtonPopup);
		tb->setMenu(menu);

		menu->addAction("5");
		menu->addAction("10");
		menu->addAction("20");
		menu->addAction("30");
		menu->addAction("45");
		menu->addAction("90");
		menu->addAction("180");

		connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(onSnapToAngleChanged(QAction*)));
	}
}

void Workbench::createStatusBar() {
	m_statusBar = statusBar();
	m_statusBar->showMessage(tr("Ready"));

	m_statusBar->addPermanentWidget(ui.viewpos);
	m_statusBar->addPermanentWidget(ui.cpuUsage);
	m_statusBar->addPermanentWidget(ui.memoryUsage);
	m_statusBar->addPermanentWidget(ui.engineBuild);
	m_statusBar->addPermanentWidget(ui.renderDriver);

	QString build = AX_BUILD;

	ui.engineBuild->setText(build);

	const RenderDriverInfo *ri = g_renderSystem->getDriverInfo();
	if (ri->driverType == RenderDriverInfo::D3D) {
		ui.renderDriver->setText("DX9");
	} else {
		ui.renderDriver->setText("GL2");
	}

	ui.progress->setParent(nullptr);
	ui.progress->hide();
}

void Workbench::createDockablePanel() {
	// side panel
	m_sideBar = new QDockWidget(tr("Side Bar"), this);
	m_sideBar->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_sideBar->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	m_sideBar->setMinimumWidth(SideBar_MinSize);
//	m_sideBar->setMaximumWidth(SideBar_MaxSize);
	m_sidePanel = new SidePanel(this);
	m_sideBar->setWidget(m_sidePanel);

#if 0
	// console
	mConsoleBar = new QDockWidget(tr("Console") /*, this */);
	mConsoleBar->setAllowedAreas(Qt::BottomDockWidgetArea);
	mConsoleBar->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	mConsoleBar->setMinimumHeight(30);
	addDockWidget(Qt::BottomDockWidgetArea, mConsoleBar, Qt::Horizontal);
//	ConsoleFrame *consoleFrame = new ConsoleFrame(console);
//	console->setWidget(consoleFrame);
#endif
}

void Workbench::readSettings() {
	QPoint pos;
	QSize size;
	bool vis;
	bool floating;
	uint dockArea;                   

	// read mainWindow settings
	g_settings->beginGroup("mainWindow");
	pos = g_settings->value("pos", QPoint(0, 0)).toPoint();
	this->move(pos);
	bool maximized = g_settings->value("maximized", true).toBool();
	if (maximized) {
		showMaximized();
	} else {
		size = g_settings->value("size", QSize(800,600)).toSize();
		this->resize(size);
	}
	g_settings->endGroup();

	// read side panel
	g_settings->beginGroup("SidePanel");
	pos = g_settings->value("pos", QPoint(0, 0)).toPoint();
//	m_sideBar->move(pos);
	size = g_settings->value("size", QSize(SideBar_MinSize, 400)).toSize();
	m_sideBar->resize(size);
	dockArea = g_settings->value("dockArea",Qt::RightDockWidgetArea).toUInt();
	addDockWidget((Qt::DockWidgetArea)dockArea, m_sideBar);
	floating = g_settings->value("floating",false).toBool();
	m_sideBar->setFloating(floating);
	vis = g_settings->value("visible", true).toBool();
	m_sideBar->setVisible(vis);
	g_settings->endGroup();
}

void Workbench::writeSettings() {
	uint area;

	// write mainWindow settings
	g_settings->beginGroup("mainWindow");
	g_settings->setValue("pos", this->pos());
	g_settings->setValue("size", this->size());
	g_settings->setValue("maximized", this->isMaximized());
	g_settings->endGroup();

	// write controlPanel
	g_settings->beginGroup("SidePanel");
	g_settings->setValue("pos", m_sideBar->pos());
	g_settings->setValue("size", m_sideBar->size());
	area = dockWidgetArea(m_sideBar);
	g_settings->setValue("area", area);
	g_settings->setValue("floating", m_sideBar->isFloating());
	g_settings->setValue("visible", m_sideBar->isVisible());
	g_settings->endGroup();
}


Frame *Workbench::getActiveFrame() {
	return m_workspace->getActiveFrame();
}

View *Workbench::getActiveView() {
#if 0
	EditorFrame *container = getActiveFrame();
	if (!container)
		return NULL;

	return container->GetView();
#else
	return NULL;
#endif
}

void Workbench::onViewChanged(int index) {}

void Workbench::onAboutAct() {

}

void Workbench::onEnglish(bool checked) {
	if (checked)
		setLanguage("");
}

void Workbench::onSimplifiedChinese(bool checked) {
	if (checked)
		setLanguage("zh");
}

void Workbench::onStyleWindows(bool checked) {
	if (checked)
		setStyle("windows");
}

void Workbench::onStyleWindowsXP(bool checked) {
	if (checked)
		setStyle("windowsxp");
}

void Workbench::onStylePlastique(bool checked) {
	if (checked)
		setStyle("plastique");
}

void Workbench::onStyleMacintosh(bool checked) {
	if (checked)
		setStyle("macintosh");
}

void Workbench::closeEvent(QCloseEvent *event) {
	writeSettings();
	event->accept();
	g_app->myQuit();
}

void Workbench::onEditorToolTriggered(QAction *action) {
	Tool::Type type = Tool::None;
	if (action) {
		type = (Tool::Type)action->data().toInt();
	}

	g_mapContext->doTool(type);
}

void Workbench::onEditorActionTriggered(QAction *action) {
	Action::Type type = (Action::Type)action->data().toInt();

	g_mapContext->doAction(type);
}

void Workbench::onViewChanged() {
	onHistoryChanged();
	onSelectionChanged();
	onClipboardChanged();

	emit viewChanged();
}

void Workbench::onSelectionChanged() {
}

void Workbench::onClipboardChanged() {
}


void Workbench::onHistoryChanged() {
	m_undoToolButton->setDisabled(true);
	m_redoToolButton->setDisabled(true);
	ui.actionEditRedo->setDisabled(true);
	ui.actionEditUndo->setDisabled(true);

	HistoryManager *history = g_mapContext->getHistory();
	AX_ASSERT(history);

	HistoryList undolist = history->getUndoList();

	if (undolist.size() > 0) {
		m_undoToolButton->setDisabled(false);
		ui.actionEditUndo->setDisabled(false);
		QMenu *menu = m_undoToolButton->menu();
		menu->clear();

		HistoryList::iterator it;

		int count = 0; int memused = 0;
		for (it = undolist.begin(); it != undolist.end(); ++it) {
			if (count == 16) {
				menu->addAction(QString("..."));
			}

			if (count < 16) {
				menu->addAction(u2q((*it)->getMessage()));
			}

			memused += (*it)->getMemoryUsed();
			count++;
		}

		QString temp = QString("Count:%1 Memory Used:%2").arg(count).arg(memused);
		menu->addAction(temp);
	}

	HistoryList redolist = history->getRedoList();
	if (redolist.size() > 0) {
		m_redoToolButton->setDisabled(false);
		ui.actionEditRedo->setDisabled(false);
		QMenu *menu = m_redoToolButton->menu();
		menu->clear();

		HistoryList::iterator it;

		int count = 0; int memused = 0;
		for (it = redolist.begin(); it != redolist.end(); ++it) {
			if (count == 16) {
				menu->addAction(QString("More..."));
			}
			if (count < 16) {
				menu->addAction(u2q((*it)->getMessage()));
			}

			memused += (*it)->getMemoryUsed();
			count++;
		}
		QString temp = QString("Count:%1 MemoryUsed:%2").arg(count).arg(memused);
		menu->addAction(temp);
	}
}


void Workbench::on_actionFollowTerrain_triggered()
{
	bool checked = ui.actionFollowTerrain->isChecked();

	g_mapContext->getMapState()->followTerrain = checked;
}

void Workbench::on_actionTerrainMaterial_triggered()
{
	TerrainMaterialDlg dlg(this);
	dlg.setWindowModality(Qt::WindowModal);
	if (dlg.exec() == QDialog::Rejected)
		return;
}

void Workbench::on_actionConvert_md5mesh_triggered()
{
//	ConvertMD5Mesh dlg(this);
//	if (dlg.exec() == QDialog::Rejected)
//		return;
}

void Workbench::on_actionConvert_md5anim_triggered()
{
//	ConvertMD5Anim dlg(this);
//	if (dlg.exec() == QDialog::Rejected)
//		return;
}

void Workbench::on_selectableType_activated(int index)
{
	int part = 1 << index;

	if (index == ui.selectableType->count() - 1) {
		part = SelectPart::All;
	}

	g_mapContext->getMapState()->selectionPart = part;
}



void Workbench::on_actionToggleFrameMaximun_triggered() {
	m_workspace->on_actionToggleFrameMaximum_triggered();
}

void Workbench::on_actionMinimap_triggered() {
#if 0
	if (!SceneMinimap_)
	{
		SceneMinimap_ = new SceneMinimap(this);
	}

	bool Visible = !SceneMinimap_->isVisible();

	if (Visible)
	{
		//SceneMinimap_->setWindowFlags(Qt::SubWindow);
		SceneMinimap_->show();
		SceneMinimap_->activateWindow();
	}
#endif
}

void Workbench::on_actionMaterialEditor_triggered() {
	showMaterialEditor();
}

void Workbench::on_actionModelViewer_triggered() {
	showModelViewer();
}

void Workbench::showMaterialEditor() {
	if (m_materialEditor == NULL) {
		m_materialEditor = new MaterialEditor();
	}

	m_materialEditor->show();
	m_materialEditor->activateWindow();
}
void Workbench::showModelViewer() {
	if (m_modelViewer == NULL) {
		m_modelViewer = new ModelViewer();
	}

	m_modelViewer->show();
	m_modelViewer->activateWindow();
}


void Workbench::on_actionAbout_triggered()
{
	QMessageBox::about(this, tr("About Axon Editor NexGen"),
		tr("The <b>Application</b> example demonstrates how to "
		"write modern GUI applications using Qt, with a menu bar, "
		"toolbars, and a status bar."));
}

void Workbench::on_actionNew_triggered()
{
	if (g_mapContext->isDirty()) {
		int btn = QMessageBox::question(this, tr("Warning: Discard Changes?"), tr("Map changed. do you want discard changes?"), QMessageBox::Yes|QMessageBox::No);

		if (btn != QMessageBox::Yes)
			return;
	}

	g_mapContext->createNew();
}

void Workbench::on_actionOpen_triggered()
{
	QString fn = FileDialog::getOpenFileName(this, tr("Select Map File"), "/maps", "*.map");

	if (fn.isEmpty())
		return;

	g_mapContext->load(q2u(fn));

	updateTitle();

	m_workspace->refreshBookmarks();	// timlly add.
}

void Workbench::on_actionSave_triggered()
{
	String fn = g_mapContext->getFilename();

	if (fn.empty()) {
		on_actionSaveAs_triggered();
		return;
	}

	g_mapContext->save();
}

void Workbench::on_actionSaveAs_triggered()
{
	QString qfn = FileDialog::getSaveFileName(this, tr("Save Map As"), "/maps", ".map");
	String fn = q2u(qfn);

	if (fn.empty())
		return;

	QString msg = QString(tr("%1: map file already exists, do you want to replace it?")).arg(qfn);

	if (g_fileSystem->isFileExist(fn)) {
		int btn = QMessageBox::question(this, tr("Map File Already Exist."), msg, QMessageBox::Yes|QMessageBox::No);

		if (btn != QMessageBox::Yes)
			return;
	}

	g_mapContext->saveAs(fn);

	updateTitle();
}


void Workbench::on_actionExit_triggered()
{
	close();
}

void Workbench::on_actionSurfaceGen_triggered()
{
	MapTerrain *terrain = g_mapContext->getTerrain();

	if (!terrain)
		return;

	terrain->generateZoneColor();
}

void Workbench::on_actionCreateTerrain_triggered()
{
	MapTerrain *terrain = g_mapContext->getTerrain();

	if (terrain)
		return;

	g_mapContext->createTerrain(1024, 2);
}


void Workbench::on_actionUseSelectionCenter_triggered(bool checked)
{
	if (ui.actionUseSelectionCenter->isChecked()) {
		g_mapContext->getMapState()->transformCenter = TransformTool::SelectionCenter;
	} else {
		g_mapContext->getMapState()->transformCenter = TransformTool::PivotCenter;
	}

	g_mapContext->getMapState()->notify(State::Transform);
}

void Workbench::on_transformSpace_currentIndexChanged(int index)
{
	if (index < TransformTool::WorldSpace || index > TransformTool::ObjectSpace) {
		index = TransformTool::WorldSpace;
	}

	g_mapContext->getMapState()->transformSpace = (TransformTool::Space)index;
	g_mapContext->getMapState()->notify(State::Transform);
}

void Workbench::on_actionUsePivotCenter_triggered()
{
	g_mapContext->getMapState()->transformCenter = TransformTool::PivotCenter;
	g_mapContext->getMapState()->notify(State::Transform);
}

void Workbench::on_actionUseTransformCenter_triggered()
{
	g_mapContext->getMapState()->transformCenter = TransformTool::TransformCenter;
	g_mapContext->getMapState()->notify(State::Transform);
}

void Workbench::onSnapToGridChanged(QAction *action) {
	float f = action->data().toDouble();

	ui.actionSnapToGrid->setIconText(QString("%1").arg(f));

	g_mapContext->getMapState()->snapToGrid = f;
}

void Workbench::onSnapToAngleChanged(QAction *action) {
	float f = action->text().toFloat();

	ui.actionSnapToAngle->setIconText(QString("%1").arg(f));

	g_mapContext->getMapState()->snapToAngle = f;
}


void Workbench::on_actionSnapToGrid_triggered()
{
	g_mapContext->getMapState()->isSnapToGrid = ui.actionSnapToGrid->isChecked();
}

void Workbench::on_actionSnapToAngle_triggered()
{
	g_mapContext->getMapState()->isSnapToAngle = ui.actionSnapToAngle->isChecked();
}

void Workbench::beginProgress(const String &title)
{
	m_statusBar->addWidget(ui.progress);
	ui.progress->show();
	ui.progressLabel->setText(u2q(title));
	ui.progressBar->setValue(0);
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

bool Workbench::showProgress(uint_t percent, const String &msg)
{
	ui.progressLabel->setText(u2q(msg));
	ui.progressBar->setValue(percent);

	QApplication::processEvents();

	return true;
}

void Workbench::endProgress()
{
	m_statusBar->removeWidget(ui.progress);
	ui.progress->hide();
	QApplication::restoreOverrideCursor();
}

void Workbench::activateTool(Tool::Type t) {
	if (t == Tool::None) {
		QAction *action = m_editorTools->checkedAction();
		if (!action) {
			return;
		}
		action->setChecked(false);

	}
	// find action
	QList<QAction *> actions = m_editorTools->actions();
	QAction *action = nullptr;
	AX_FOREACH(action, actions) {
		if ((Tool::Type)action->data().toInt() == t) {
			break;
		}
	}

	onEditorToolTriggered(action);
}

void Workbench::timerEvent(QTimerEvent * event)
{
	if (event->timerId() != m_timerUpdateStatus) {
		Errorf("wrong id");
		return;
	}

	ui.cpuUsage->setText(QString(" CPU: %1% ").arg(OsUtil::getCpuUsage()));
	int mu = OsUtil::getMemoryUsage();

	QString txt = QString(" MEM: %1,%2 K ").arg(mu / 1024000).arg((mu%1024000) / 1024, 3, 10, QLatin1Char('0'));
	ui.memoryUsage->setText(txt);
}

void Workbench::on_actionRunGame_triggered() {
	g_mapContext->runGame();
}


void Workbench::on_actionUIEditor_triggered()
{
#if 0
	if (m_uiEditor == NULL) {
		m_uiEditor = new UiEditor();
	}

	m_uiEditor->show();
	m_uiEditor->activateWindow();
#endif
}

void Workbench::on_actionAnimated_Editor_triggered()
{
#if 0
	if (!AnimatedEditor_) 
	{
		AnimatedEditor_ = new AnimatedEditor();
	}

	AnimatedEditor_->show();
	AnimatedEditor_->activateWindow();
#endif
}

void Workbench::on_actionAbout_Qt_triggered()
{
	QApplication::aboutQt();
}

void Workbench::on_actionSelectByName_triggered()
{
	if (!m_selectByNameDlg)
	{
		m_selectByNameDlg = new SelectByNameDlg();
	}

	//m_selectByNameDlg->refreshDlg();
	m_selectByNameDlg->show();
	m_selectByNameDlg->onDialogActived();
	m_selectByNameDlg->activateWindow();
}

void Workbench::on_actionScriptEditor_triggered() {
	if (!m_scriptEditor) {
		m_scriptEditor = new ScriptEditor();
	}

	m_scriptEditor->show();
}

void Workbench::openScriptFile(const QString &filename)
{
	if (!m_scriptEditor) {
		m_scriptEditor = new ScriptEditor();
	}

	m_scriptEditor->openFile(filename);
	m_scriptEditor->show();
}

void Workbench::on_actionLayerGen_triggered()
{
	MapTerrain *terrain = g_mapContext->getTerrain();

	if (!terrain)
		return;

	terrain->generateLayerAlpha();
}

void Workbench::on_actionSelectAll_triggered()
{
	g_mapContext->selectAll();
}

void Workbench::on_actionSelectNone_triggered()
{
	g_mapContext->selectNone();
}

void Workbench::on_actionSelectInvert_triggered()
{
	g_mapContext->selectInvert();
}


void Workbench::on_actionGfxEditor_triggered()
{
	if (!m_gfxEditor) {
		m_gfxEditor = new GfxEditor();
	}

	m_gfxEditor->show();
}
