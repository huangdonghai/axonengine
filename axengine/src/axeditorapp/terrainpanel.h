/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXTERRAINPANEL_H
#define CXTERRAINPANEL_H

#include "private.h"
#include "ui_terrainpanel.h"

class TerrainPanel : public QWidget, IObserver {
    Q_OBJECT

public:
    TerrainPanel(QWidget *parent = 0);
    ~TerrainPanel();

	// IObserver
	virtual void doNotify(IObservable* subject, int arg);

protected:
	void setParams();
	void setParamsFramSlider();

private:
    Ui::TerrainPanelClass ui;
	QActionGroup* m_actionGroup;

private slots:
	void on_actionRiverRoad_triggered();
	void on_actionRiver_triggered();
	void on_actionRoad_triggered();
	void on_grassTransferZ_valueChanged(double);
	void on_modelBtn_released();
	void on_grassColor_colorChanged(const QColor&);
	void on_grassName_textEdited(QString);
	void on_textureBtn_released();
	void on_grassMaxScale_valueChanged(double);
	void on_grassMinScale_valueChanged(double);
	void on_grassHeight_valueChanged(double);
	void on_grassWidth_valueChanged(double);
	void on_grassType_currentIndexChanged(int);
	void on_grassIncidence_valueChanged(double);
	void on_grassTypes_itemSelectionChanged();
	void on_grassTypeDelete_released();
	void on_grassTypeAdd_released();
	void on_numGrassPerLevel_editingFinished();
	void on_tableWidget_itemChanged(QTableWidgetItem*);
	void on_tableWidget_currentCellChanged(int,int,int,int);
	void on_tableWidget_cellChanged(int,int);
	void on_toolButton_7_clicked();
	void on_toolButton_clicked();
	void on_checkBox_5_clicked();
	void on_checkBox_6_clicked();
	void on_actionGrassTreeVegetation_triggered();
	void on_heighEdit_clicked();
	void on_toolButton_1_clicked(bool);
	void on_toolButton_1_toggled(bool);
	void on_actionActive_Area_triggered();
	void on_StrengthSlider_valueChanged(int);
	void on_SoftnessSlider_valueChanged(int);
	void on_RadiusSlider_valueChanged(int);
	void on_applyEnv_clicked();
	void on_actionEnv_triggered();
	void on_actionVegetation_triggered();
	void on_actionTerrainPaint_triggered();
	void on_actionHeightEdit_triggered();
	void on_layerList_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
	void on_brushStrengthSpin_valueChanged(int);
	void on_brushSoftnessSpin_valueChanged(int);
	void on_brushRadiusSpin_valueChanged(int);
};

#endif // CXTERRAINPANEL_H
