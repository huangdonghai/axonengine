/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef CXDISPLAYSETTINGPANEL_H
#define CXDISPLAYSETTINGPANEL_H

#include "ui_displaysettingpanel.h"

class DisplaySettingPanel : public QWidget
{
    Q_OBJECT

public:
    DisplaySettingPanel(QWidget *parent = 0);
    ~DisplaySettingPanel();

private:
    Ui::DisplaySettingPanelClass ui;

private slots:
	void on_r_fog_toggled(bool);
	void on_shaderDebug7_toggled(bool);
	void on_shaderDebug6_toggled(bool);
	void on_shaderDebug5_toggled(bool);
	void on_shaderDebug4_toggled(bool);
	void on_shaderDebug3_toggled(bool);
	void on_shaderDebug2_toggled(bool);
	void on_shaderDebug1_toggled(bool);
	void on_shaderDebug0_toggled(bool);
	void on_r_bloom_toggled(bool);
	void on_r_hdr_toggled(bool);
	void on_r_shadow_2_toggled(bool);
	void on_r_speedtree_toggled(bool);
	void on_r_actor_toggled(bool);
	void on_r_water_toggled(bool);
	void on_r_sky_toggled(bool);
	void on_r_nospecular_toggled(bool);
	void on_r_nobump_toggled(bool);
	void on_r_nodetail_toggled(bool);
	void on_r_noterrain_toggled(bool);
	void on_r_showMemoryInfo_toggled(bool);
	void on_r_showstat_toggled(bool);
	void on_r_wireframe_stateChanged(int);
	void on_r_showPhysics_toggled(bool);
	void on_r_helper_toggled(bool);
};

#endif // CXDISPLAYSETTINGPANEL_H
