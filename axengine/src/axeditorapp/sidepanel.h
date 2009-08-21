/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AXSIDEPANEL_H
#define AXSIDEPANEL_H

#include "private.h"
#include "terrainpanel.h"
#include "creationpanel.h"
#include "actorpanel.h"
#include "displaysettingpanel.h"
#include "toolspanel.h"

class TabBarHideText : public QTabBar {
public:
	TabBarHideText(QWidget* parent/* =0 */) : QTabBar(parent) {}

protected:
	// override
	virtual QSize tabSizeHint(int index) const {
		QSize size = QTabBar::tabSizeHint(index);
		size.setWidth(size.width() - 12);
		return size;
	}
};

class SidePanel : public QTabWidget {
    Q_OBJECT

public:
    SidePanel(QWidget *parent = 0);
    ~SidePanel();

	int addPanel(QWidget* child, QIcon& icon, const QString& tooltip);

	DisplaySettingPanel* getDisplaySettingPanel(){return m_settingPanel;}

	TerrainPanel* getTerrainPanel(){return m_terrainPanel;}

private:
	// self
	TabBarHideText* m_tabBar;
	QTabBar* m_oldTabBar;

	// pages
	TerrainPanel* m_terrainPanel;
	CreationPanel* m_creationPanel;
	ActorPanel* m_actorPanel;
	DisplaySettingPanel* m_settingPanel;
	ToolsPanel* m_toolsPanel;
};

#endif // AXSIDEPANEL_H
