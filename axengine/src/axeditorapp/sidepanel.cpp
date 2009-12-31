/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "SidePanel.h"
#include <QtGui/QGridLayout>

SidePanel::SidePanel(QWidget *parent)
    : QTabWidget(parent)
{
	// store old tabbar
	m_oldTabBar = tabBar();

	m_tabBar = new TabBarHideText(this);
	m_tabBar->setIconSize(QSize(22, 22));
	m_tabBar->setDrawBase(false);
#if 0
	mTabBar->addTab(QIcon(":/images/terrain.png"), tr(""));
	mTabBar->addTab(QIcon(":/images/create.png"), tr(""));
	mTabBar->addTab(QIcon(":/images/modify.png"), tr(""));
	mTabBar->addTab(QIcon(":/images/display.png"), tr(""));
	mTabBar->addTab(QIcon(":/images/tools.png"), tr(""));
#else
	m_terrainPanel = new TerrainPanel(this);
	m_creationPanel = new CreationPanel(this);
	m_actorPanel = new ActorPanel(this);
	m_settingPanel = new DisplaySettingPanel(this);
	m_toolsPanel = new ToolsPanel(this);
	addPanel(m_creationPanel, QIcon(":/images/create.png"), tr("Create Object"));
	addPanel(m_terrainPanel, QIcon(":/images/terrain.png"), tr("Terrain Editing"));
	addPanel(m_actorPanel, QIcon(":/images/modify.png"), tr("Modify Object"));
	addPanel(m_settingPanel, QIcon(":/images/display.png"), tr("Display Setting"));
	addPanel(m_toolsPanel, QIcon(":/images/tools.png"), tr("Useful tools"));
#endif

	setTabBar(m_tabBar);
}

SidePanel::~SidePanel()
{
}

int SidePanel::addPanel(QWidget *child, QIcon &icon, const QString &tooltip) {
	int index;

	index = QTabWidget::addTab(child, icon, QString());
	m_tabBar->addTab(icon, QString());
	QTabWidget::setTabToolTip(index, tooltip);
	m_tabBar->setTabToolTip(index, tooltip);

	return index;
}

