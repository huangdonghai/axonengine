/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXEDITORNEXGEN_H
#define CXEDITORNEXGEN_H

#include "private.h"

#include "terrainpanel.h"
#include "sidepanel.h"
#include "workspace.h"
#include "modelviewer.h"
#include "materialeditor.h"
#include "camerasetting.h"
#include "selectbynamedlg.h"
#include "scripteditor.h"
#include "gfxeditor.h"

#include "ui_workbench.h"

class Workbench : public QMainWindow, public IObserver, public IProgressHandler {
    Q_OBJECT

public:
    Workbench(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~Workbench();

	// implement IObserver
	virtual void doNotify(IObservable* subjest, int arg);

	// implement IProgressHandler
	virtual void beginProgress(const String& title);
	virtual bool showProgress(uint_t percent, const String& msg);
	virtual void endProgress();

	// implement IWorkbench, for editordll use
	virtual void activateTool(Tool::Type t);


	void updateTitle();

	// for status bar
	void updateStatusInfo(const Vector3& vieworg, const Vector3& cursorpos);
	void setLanguage(const QString& lang);
	void setStyle(const QString& style);

	// editor tools and editor actions
	void addEditorTool(int toolType, QAction* action);
	void addEditorAction(int actionType, QAction* action);

	Frame* getActiveFrame();
	View* getActiveView();

	void showMaterialEditor();
	void showModelViewer();

	void openScriptFile(const QString& filename);

//	SceneMinimap* getSceneMinimap(){return SceneMinimap_;}
	SidePanel* getSidePanel(){return m_sidePanel;}

protected:
	// inherited event
	virtual void closeEvent(QCloseEvent *event);
	virtual void timerEvent(QTimerEvent * event);

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void createDockablePanel();
	void readSettings();
	void writeSettings();

signals:
	void viewChanged();
	void selectionChanged();
	void clipboardChanged();
	void historyChanged();

public slots:
	void onViewChanged();
	void onSelectionChanged();
	void onClipboardChanged();
	void onHistoryChanged();

private slots:
	void on_actionGfxEditor_triggered();
	void on_actionSelectInvert_triggered();
	void on_actionSelectNone_triggered();
	void on_actionSelectAll_triggered();
	void on_actionLayerGen_triggered();
	void on_actionScriptEditor_triggered();
	void on_actionSelectByName_triggered();
	void on_actionAbout_Qt_triggered();
	void on_actionAnimated_Editor_triggered();
	void on_actionToggleFrameMaximun_triggered();
	void on_actionMinimap_triggered();
	void on_actionUIEditor_triggered();
	void on_actionRunGame_triggered();
	void on_actionSnapToAngle_triggered();
	void on_actionSnapToGrid_triggered();
	void on_actionUseTransformCenter_triggered();
	void on_actionUsePivotCenter_triggered();
	void on_transformSpace_currentIndexChanged(int);
	void on_actionUseSelectionCenter_triggered(bool checked);
	void on_actionCreateTerrain_triggered();
	void on_actionSurfaceGen_triggered();
	void on_actionExit_triggered();
	void on_actionSaveAs_triggered();
	void on_actionSave_triggered();
	void on_actionOpen_triggered();
	void on_actionNew_triggered();
	void on_actionAbout_triggered();
	void on_actionTerrainMaterial_triggered();
	void on_actionModelViewer_triggered();
	void on_actionMaterialEditor_triggered();
	void on_selectableType_activated(int);
	void on_actionConvert_md5mesh_triggered();
	void on_actionConvert_md5anim_triggered();
	void on_actionFollowTerrain_triggered();

	void onViewChanged(int index);

	void onAboutAct();

	void onEnglish(bool);
	void onSimplifiedChinese(bool);

	void onStyleWindows(bool);
	void onStyleWindowsXP(bool);
	void onStylePlastique(bool);
	void onStyleMacintosh(bool);

	void onEditorToolTriggered(QAction* action );
	void onEditorActionTriggered(QAction* action);

	void onSnapToGridChanged(QAction* action);
	void onSnapToAngleChanged(QAction* action);

	void onViewLabelDoubleClicked(QPoint);

public:
	// HACK, other class need this
	Ui::WorkbenchClass ui;

private:
	// tool bar
	QToolButton* m_undoToolButton;
	QToolButton* m_redoToolButton;
	QComboBox* m_selectableType;

	// status bar
	QStatusBar* m_statusBar;

	// -- dockable bar --
	QDockWidget* m_sideBar;			// side panel
	SidePanel* m_sidePanel;

	// central widget
	Workspace* m_workspace;

	// editor tools and editor action
	QActionGroup* m_editorTools;
	QActionGroup* m_editorActions;

	// model viewer
	ModelViewer* m_modelViewer;
	// material editor
	MaterialEditor* m_materialEditor;

	CameraSetting m_cameraDlg;
	SelectByNameDlg* m_selectByNameDlg;
	ScriptEditor* m_scriptEditor;
	GfxEditor* m_gfxEditor;

	int m_timerUpdateStatus;
};

#endif // CXEDITORNEXGEN_H
