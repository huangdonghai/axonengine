/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "displaysettingpanel.h"
#include "private.h"

DisplaySettingPanel::DisplaySettingPanel(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

	ui.rollup->initialize();

	ui.r_wireframe->setChecked(r_wireframe.getBool());
	ui.r_showstat->setChecked(r_showStat.getInteger() == 3);
	ui.r_showMemoryInfo->setChecked(r_showMemoryInfo.getBool());
	ui.r_noterrain->setChecked(!r_terrain.getBool());
	ui.r_nodetail->setChecked(!r_detail.getBool());
	ui.r_nobump->setChecked(!r_bumpmap.getBool());
	ui.r_nospecular->setChecked(!r_specular.getBool());
	ui.r_sky->setChecked(!r_sky.getBool());
	ui.r_water->setChecked(!r_water.getBool());
	ui.r_actor->setChecked(!r_entity.getBool());
	ui.r_speedtree->setChecked(!r_speedtree.getBool());
	ui.r_shadow_2->setChecked(!r_shadow.getBool());
	ui.r_fog->setChecked(!r_fog.getBool());
	ui.r_hdr->setChecked(!r_hdr.getBool());
	ui.r_bloom->setChecked(!r_bloom.getBool());

	ui.r_showPhysics->setChecked(r_showPhysics.getBool());
	ui.r_helper->setChecked(r_helper.getBool());

	ui.shaderDebug0->setChecked(true);
}

DisplaySettingPanel::~DisplaySettingPanel() {
}


void DisplaySettingPanel::on_r_wireframe_stateChanged(int i) {
	r_wireframe.setInt(i);
}

void DisplaySettingPanel::on_r_showstat_toggled(bool checked) {
	if (checked) {
		r_showStat.setInt(3);
	} else {
		r_showStat.setInt(0);
	}
}

void DisplaySettingPanel::on_r_showMemoryInfo_toggled(bool checked) {
	r_showMemoryInfo.setInt(checked);
}

void DisplaySettingPanel::on_r_noterrain_toggled(bool checked) {
	r_terrain.setInt(!checked);
}

void DisplaySettingPanel::on_r_nodetail_toggled(bool checked) {
	r_detail.setInt(!checked);
}

void DisplaySettingPanel::on_r_nobump_toggled(bool checked) {
	r_bumpmap.setInt(!checked);
}

void DisplaySettingPanel::on_r_nospecular_toggled(bool checked) {
	r_specular.setInt(!checked);
}

void DisplaySettingPanel::on_r_sky_toggled(bool checked) {
	r_sky.setInt(!checked);
}

void DisplaySettingPanel::on_r_water_toggled(bool checked) {
	r_water.setInt(!checked);
}

void DisplaySettingPanel::on_r_actor_toggled(bool checked) {
	r_entity.setInt(!checked);
}

void DisplaySettingPanel::on_r_speedtree_toggled(bool checked) {
	r_speedtree.setInt(!checked);
}

void DisplaySettingPanel::on_r_shadow_2_toggled(bool checked) {
	r_shadow.setInt(!checked);
}

void DisplaySettingPanel::on_r_fog_toggled(bool checked) {
	r_fog.setInt(!checked);
}

void DisplaySettingPanel::on_r_hdr_toggled(bool checked) {
	r_hdr.setInt(!checked);
}

void DisplaySettingPanel::on_r_bloom_toggled(bool checked) {
	r_bloom.setInt(!checked);
}

void DisplaySettingPanel::on_shaderDebug0_toggled(bool checked) {
	r_shaderDebug.setInt(0);
}

void DisplaySettingPanel::on_shaderDebug1_toggled(bool checked)
{
	r_shaderDebug.setInt(1);
}

void DisplaySettingPanel::on_shaderDebug2_toggled(bool checked)
{
	r_shaderDebug.setInt(2);
}

void DisplaySettingPanel::on_shaderDebug3_toggled(bool checked)
{
	r_shaderDebug.setInt(3);
}

void DisplaySettingPanel::on_shaderDebug4_toggled(bool checked)
{
	r_shaderDebug.setInt(4);
}

void DisplaySettingPanel::on_shaderDebug5_toggled(bool checked)
{
	r_shaderDebug.setInt(5);
}

void DisplaySettingPanel::on_shaderDebug6_toggled(bool checked)
{
	r_shaderDebug.setInt(6);
}

void DisplaySettingPanel::on_shaderDebug7_toggled(bool checked)
{
	r_shaderDebug.setInt(7);
}

void DisplaySettingPanel::on_r_showPhysics_toggled( bool checked )
{
	r_showPhysics.setInt(checked);
}

void DisplaySettingPanel::on_r_helper_toggled( bool checked )
{
	r_helper.setInt(checked);
}