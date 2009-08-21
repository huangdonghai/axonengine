/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "private.h"
#include "NewMapDlg.h"

NewMapDlg::NewMapDlg(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.heightmapSize, SIGNAL(currentIndexChanged(int)), this, SLOT(OnParamsChanged()));
	connect(ui.metersPerTile, SIGNAL(currentIndexChanged(int)), this, SLOT(OnParamsChanged()));
	connect(ui.maxHeight, SIGNAL(currentIndexChanged(int)), this, SLOT(OnParamsChanged()));
	connect(ui.metersPerTexel, SIGNAL(currentIndexChanged(int)), this, SLOT(OnParamsChanged()));
	connect(ui.useDefaults, SIGNAL(clicked()), this, SLOT(OnSetDefaultValue()));

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(OnAccept()));

	OnParamsChanged();
}

NewMapDlg::~NewMapDlg()
{

}

void
NewMapDlg::OnSetDefaultValue() {
	ui.heightmapSize->setCurrentIndex(4);
	ui.metersPerTile->setCurrentIndex(1);
	ui.maxHeight->setCurrentIndex(1);
	ui.metersPerTexel->setCurrentIndex(1);
}

void
NewMapDlg::OnParamsChanged() {
	longlong_t heightmap_size = (1 << ui.heightmapSize->currentIndex()) * 1024;
	longlong_t meters_per_tile = ui.metersPerTile->currentText().toInt();
	longlong_t max_height = ui.maxHeight->currentText().toInt();
	float meters_per_texel = ui.metersPerTexel->currentText().toFloat();

#if 0
	gEditorSystem->setEditorParam("Map.HeightMapSize",	Variant(heightmap_size));
	gEditorSystem->setEditorParam("Map.MetersPerTile",	Variant(meters_per_tile));
	gEditorSystem->setEditorParam("Map.MaxHeight",		Variant(max_height));
	gEditorSystem->setEditorParam("Map.MetersPerTexel", Variant(meters_per_texel));
#endif

	longlong_t map_size = (heightmap_size * meters_per_tile) * (heightmap_size * meters_per_tile) / (1024 * 1024);
	longlong_t texel_size = (heightmap_size * meters_per_tile) * (heightmap_size * meters_per_tile) / (meters_per_texel * meters_per_texel);

	// origin data size
	longlong_t heightmap_datasize = heightmap_size * heightmap_size * 2 / (1024 * 1024);
	longlong_t texture_datasize = texel_size * 3 / (1024 * 1024);
	longlong_t alphamap_datasize = (texel_size) / (1024 * 1024);
	longlong_t ambient_datasize = (texel_size) / (1024 * 1024);
	longlong_t lightmap_datasize = (texel_size) / (1024 * 1024);
	longlong_t total_datasize = heightmap_datasize + texture_datasize + alphamap_datasize + ambient_datasize + lightmap_datasize;

	// compressed data size
	longlong_t compressed_heightmap_datasize = heightmap_datasize / 4;
	longlong_t compressed_texture_datasize = texture_datasize / 6;
	longlong_t compressed_alphamap_datasize = alphamap_datasize / 4;
	longlong_t compressed_ambient_datasize = alphamap_datasize / 4;
	longlong_t compressed_lightmap_datasize = lightmap_datasize / 4;

	longlong_t compressed_total_datasize = compressed_heightmap_datasize
									+ compressed_texture_datasize
									+ compressed_alphamap_datasize
									+ compressed_ambient_datasize
									+ compressed_lightmap_datasize;

	// when save to disk file size
	longlong_t gzip_ratio = 2;
	longlong_t heightmap_disksize = compressed_heightmap_datasize / gzip_ratio;
	longlong_t texture_disksize = compressed_texture_datasize / gzip_ratio;
	longlong_t alphamap_disksize = compressed_alphamap_datasize / gzip_ratio;
	longlong_t ambient_disksize = compressed_ambient_datasize / gzip_ratio;
	longlong_t lightmap_disksize = compressed_lightmap_datasize / gzip_ratio;
	longlong_t total_disksize = heightmap_disksize + texture_disksize + alphamap_disksize + ambient_disksize + lightmap_disksize;

	QString tmp, tmp1, tmp3;
	ui.mapSize->setText(tmp.setNum(map_size) + tr(" KM^2"));
	ui.heightMapDataSize->setText(tmp.setNum(heightmap_datasize) + "," + tmp1.setNum(compressed_heightmap_datasize) + "," + tmp3.setNum(heightmap_disksize) + tr(" MB"));
	ui.textureDataSize->setText(tmp.setNum(texture_datasize) + "," + tmp1.setNum(compressed_texture_datasize) + "," + tmp3.setNum(texture_disksize) + tr(" MB"));
	ui.alphamapDataSize->setText(tmp.setNum(alphamap_datasize) + "," + tmp1.setNum(compressed_alphamap_datasize) + "," + tmp3.setNum(alphamap_disksize) + tr(" MB"));
	ui.ambientDataSize->setText(tmp.setNum(ambient_datasize) + "," + tmp1.setNum(compressed_ambient_datasize) + "," + tmp3.setNum(ambient_disksize) + tr(" MB"));
	ui.lightmapDataSize->setText(tmp.setNum(lightmap_datasize) + "," + tmp1.setNum(compressed_lightmap_datasize) + "," + tmp3.setNum(lightmap_disksize) + tr(" MB"));
	ui.totalDataSize->setText(tmp.setNum(total_datasize) + "," + tmp1.setNum(compressed_total_datasize) + "," + tmp3.setNum(total_disksize) + tr(" MB"));
}

void
NewMapDlg::OnAccept() {
	String new_mapname = q2u(ui.mapName->text());
	bool usePerlinNoise = ui.usePerlinNoise->isChecked();
	bool generateHeight = ui.generateHeight->isChecked();
	bool generateColor = ui.generateColor->isChecked();
	int noiseScale = ui.noiseScale->currentText().toInt();

#if 0
	gEditorSystem->setEditorParam("Map.NewMapName", Variant(new_mapname));
	gEditorSystem->setEditorParam("Map.usePerlinNoise",	Variant(usePerlinNoise));
	gEditorSystem->setEditorParam("Map.generateHeight",	Variant(generateHeight));
	gEditorSystem->setEditorParam("Map.generateHeight",	Variant(generateColor));
	gEditorSystem->setEditorParam("Map.noiseScale",	Variant(noiseScale));
#endif

	String map_name = "Maps/";
#if 0
	map_name += gEditorSystem->getEditorParam("Map.NewMapName").ToString();
#endif
	// check map directory if already exist
	if (g_fileSystem->getFileModifyTime(map_name, NULL)) {
		QMessageBox mb(tr("Create New Map")
			, tr("A map already exists at that location.\n Do you want replace it?")
			, QMessageBox::Warning
			, QMessageBox::Yes | QMessageBox::Default
			, QMessageBox::No | QMessageBox::Escape
			, QMessageBox::NoButton);
		mb.exec();

		return;
	}

	emit accept();

	return;
}




void NewMapDlg::on_okButton_clicked()
{

}