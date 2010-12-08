/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef CXTERRAINMATERIALDLG_H
#define CXTERRAINMATERIALDLG_H

#include "private.h"
#include "ui_terrainmaterialdlg.h"


class TerrainMaterialDlg : public QDialog
{
    Q_OBJECT

public:
    TerrainMaterialDlg(QWidget *parent = 0);
    ~TerrainMaterialDlg();

protected:
	void initMaterialDef();
	MapLayerDef *getLayerDef(QTreeWidgetItem *item);
	void writeToDef(QTreeWidgetItem *item);
	void writeCurrentDef();
	void readFromDef(QTreeWidgetItem *item);
	void updataMaskPreview();
	void updateBaseImage();
	void apply();

private:
	Ui::TerrainMaterialDlgClass ui;

private slots:
	void on_selectBase_clicked();
	void on_autoGen_clicked();
	void on_slopeHigh_editingFinished();
	void on_slopeLow_editingFinished();
	void on_altitudeHigh_editingFinished();
	void on_altitudeLow_editingFinished();
	void on_cancel_clicked();
	void on_ok_clicked();
	void on_layerList_itemChanged(QTreeWidgetItem*,int);
	void on_apply_clicked();
	void on_layerList_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
	void on_selectDetail_clicked();
	void on_openMatEditor_clicked();
	void on_moveDown_clicked();
	void on_moveUp_clicked();
	void on_del_clicked();
	void on_add_clicked();

private:
	MapTerrain *m_terrain;
	MapMaterialDef *m_materialDef;
	Image *m_heightImage;
	Image *m_slopeImage;
	std::string m_baseImage;
};

#endif // CXTERRAINMATERIALDLG_H
