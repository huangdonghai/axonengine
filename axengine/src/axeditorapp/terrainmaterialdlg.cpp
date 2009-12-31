/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "terrainmaterialdlg.h"
#include "workbench.h"


TerrainMaterialDlg::TerrainMaterialDlg(QWidget *parent)
    : QDialog(parent)
	, m_materialDef(nullptr)
{
	ui.setupUi(this);

	ui.layerList->header()->hide();

	m_heightImage = nullptr;
	m_slopeImage = nullptr;

	m_terrain = g_mapContext->getTerrain();

	if (m_terrain) {
		m_heightImage = m_terrain->copyFloatHeight(256);
		m_slopeImage = m_terrain->copySlopeImage(256);
	}

	initMaterialDef();
}

TerrainMaterialDlg::~TerrainMaterialDlg() {
	SafeDelete(m_materialDef);
	SafeDelete(m_heightImage);
	SafeDelete(m_slopeImage);
}

void TerrainMaterialDlg::initMaterialDef() {
	if (m_terrain) {
		m_materialDef = m_terrain->getMaterialDef()->clone();
	} else {
		m_materialDef = new MapMaterialDef();
	}

	ui.layerList->clear();

	for (int i = 0; i < m_materialDef->getNumLayers(); i++) {
		MapLayerDef *l = m_materialDef->getLayerDef(i);

		QTreeWidgetItem *item = new QTreeWidgetItem(ui.layerList, QStringList(u2q(l->name)));
		item->setData(0, Qt::UserRole, l->id);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}

	QTreeWidgetItem *item = ui.layerList->topLevelItem(0);
	ui.layerList->setCurrentItem(item);
//	on_layerList_currentItemChanged(item, nullptr);
}

MapLayerDef *TerrainMaterialDlg::getLayerDef(QTreeWidgetItem *item) {
	int id = item->data(0, Qt::UserRole).toInt();

	return m_materialDef->findLayerDefById(id);
}

void TerrainMaterialDlg::writeToDef(QTreeWidgetItem *item) {
	if (!item)
		return;

	MapLayerDef *l = getLayerDef(item);

	if (!l) return;

	QColor qc = ui.colorView->color();
	l->color.set(qc.red(), qc.green(), qc.blue());
	l->surfaceType = ui.surfaceType->currentIndex();
	l->baseImage = m_baseImage;
	l->isAutoGen = ui.autoGen->isChecked();
	l->altitudeRange.x = ui.altitudeLow->value();
	l->altitudeRange.y = ui.altitudeHigh->value();
	l->slopeRange.x = ui.slopeLow->value();
	l->slopeRange.y = ui.slopeHigh->value();
	l->isUseDetail = ui.detail->isChecked();
	l->detailMat = q2u(ui.detailMat->text());
	l->uvScale.x = ui.uScale->value();
	l->uvScale.y = ui.vScale->value();
	l->isVerticalProjection = ui.vertical->isChecked();
}

void TerrainMaterialDlg::readFromDef(QTreeWidgetItem *item) {
	if (!item)
		return;

	MapLayerDef *l = getLayerDef(item);

	if (!l) return;

	ui.colorView->setColor(QColor(l->color.r, l->color.g, l->color.b));
	m_baseImage = l->baseImage;

	QString temp;
	temp.sprintf("%d", l->id);
	ui.id->setText(temp);
	ui.surfaceType->setCurrentIndex(l->surfaceType);

	if (l->isAutoGen)
		ui.autoGen->setChecked(true);
	else
		ui.autoGen->setChecked(false);

	ui.altitudeLow->setValue(l->altitudeRange.x);
	ui.altitudeHigh->setValue(l->altitudeRange.y);

	ui.slopeLow->setValue(l->slopeRange.x);
	ui.slopeHigh->setValue(l->slopeRange.y);

	if (l->isUseDetail) {
		ui.detail->setChecked(true);
	} else {
		ui.detail->setChecked(false);
	}

	ui.detailMat->setText(u2q(l->detailMat));
	ui.uScale->setValue(l->uvScale.x);
	ui.vScale->setValue(l->uvScale.y);

	if (l->isVerticalProjection)
		ui.vertical->setCheckState(Qt::Checked);
	else
		ui.vertical->setCheckState(Qt::Unchecked);

	updateBaseImage();
}

void TerrainMaterialDlg::updataMaskPreview() {
	if (!m_heightImage || !m_slopeImage)
		return;

	float isAutoGen = ui.autoGen->isChecked();

	if (!isAutoGen) {
		ui.maskPreview->clear();
		return;
	}

	float altitudelow = ui.altitudeLow->value();
	float altitudehigh = ui.altitudeHigh->value();
	int slopelow = ui.slopeLow->value();
	int slopehigh = ui.slopeHigh->value();

	QImage image(256, 256, QImage::Format_ARGB32);

	const float *ph = (const float*)m_heightImage->getData(0);
	const byte_t *ps = m_slopeImage->getData(0);

	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			float h = *ph++;
			int s = *ps++;

			if (h >= altitudelow && h <= altitudehigh && s >= slopelow && s <= slopehigh) {
				image.setPixel(x, y, 0xffffffff);
			} else {
				image.setPixel(x, y, 0xff000000);
			}
		}
	}

	QPixmap pm = QPixmap::fromImage(image);
	ui.maskPreview->setPixmap(pm);
}

void TerrainMaterialDlg::on_add_clicked()
{
	if (ui.layerList->topLevelItemCount() >= Map::MaxLayers)
		return;

	MapLayerDef *layer = m_materialDef->createLayerDef();

	if (!layer)
		return;

	QTreeWidgetItem *item = new QTreeWidgetItem(ui.layerList, QStringList(u2q(layer->name)));
	item->setData(0, Qt::UserRole, layer->id);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ui.layerList->editItem(item);
	ui.layerList->setCurrentItem(item);
}

void TerrainMaterialDlg::on_del_clicked()
{
	int idx2 = ui.layerList->currentIndex().row();
	QTreeWidgetItem *item = ui.layerList->currentItem();
	if (!item)
		return;

	MapLayerDef *l = getLayerDef(item);

	if (!l)
		return;

	m_materialDef->deleteLayerDef(l);
	l = nullptr;

	int idx = ui.layerList->indexOfTopLevelItem(item);

	QTreeWidgetItem *backup = item;
	QTreeWidgetItem *taken = ui.layerList->takeTopLevelItem(idx);

	AX_ASSERT(taken == backup);
	SafeDelete(taken);
}

void TerrainMaterialDlg::on_moveUp_clicked()
{
	if (ui.layerList->topLevelItemCount() < 2)
		return;

	QTreeWidgetItem *item = ui.layerList->currentItem();
	if (!item) return;

	int idx = ui.layerList->indexOfTopLevelItem(item);
	if (idx == 0)
		return;

	QTreeWidgetItem *taken = ui.layerList->takeTopLevelItem(idx);

	ui.layerList->insertTopLevelItem(idx - 1, taken);

	ui.layerList->setCurrentItem(taken);
}

void TerrainMaterialDlg::on_moveDown_clicked()
{
	if (ui.layerList->topLevelItemCount() < 2)
		return;

	QTreeWidgetItem *item = ui.layerList->currentItem();
	if (!item) return;

	int idx = ui.layerList->indexOfTopLevelItem(item);
	if (idx == ui.layerList->topLevelItemCount() - 1)
		return;

	QTreeWidgetItem *taked = ui.layerList->takeTopLevelItem(idx);

	ui.layerList->insertTopLevelItem(idx + 1, taked);

	ui.layerList->setCurrentItem(taked);
}

void TerrainMaterialDlg::on_openMatEditor_clicked()
{
	g_workbench->showMaterialEditor();
}
#include "filedialog.h"
void TerrainMaterialDlg::on_selectDetail_clicked()
{
	QString file = FileDialog::getOpenFileName(this, "Select Detail", "textures/terrain/detail", "*.dds");
	if (file.isEmpty()) {
		return;
	}
	ui.detailMat->setText(file);
}

void TerrainMaterialDlg::on_layerList_currentItemChanged(QTreeWidgetItem *cur,QTreeWidgetItem *prev)
{
	writeToDef(prev);

	if (cur==nullptr) {
		ui.layerProp->setDisabled(true);
		ui.moveUp->setDisabled(true);
		ui.moveDown->setDisabled(true);
		return;
	}

	ui.layerProp->setEnabled(true);
	ui.moveUp->setEnabled(true);
	ui.moveDown->setEnabled(true);

	readFromDef(cur);
	updataMaskPreview();
}

void TerrainMaterialDlg::apply()
{
	AX_ASSERT(ui.layerList->topLevelItemCount() == m_materialDef->getNumLayers());

	// save current to layerdef
	writeCurrentDef();

	// reorder material def for layerList
	for (int i = 0; i < ui.layerList->topLevelItemCount(); i++) {
		QTreeWidgetItem *item = ui.layerList->topLevelItem(i);
		m_materialDef->setLayer(i, getLayerDef(item));
	}

	if (m_terrain) {
		g_mapContext->setTerrainMaterialDef(m_materialDef);
		m_materialDef = nullptr;
	}
}

void TerrainMaterialDlg::on_apply_clicked()
{
	int cur = ui.layerList->indexOfTopLevelItem(ui.layerList->currentItem());

	apply();

	initMaterialDef();

	if (cur != -1) {
		QTreeWidgetItem *item = ui.layerList->topLevelItem(cur);
		if (!item)
			return;

		ui.layerList->setCurrentItem(item);
	}
}

void TerrainMaterialDlg::on_layerList_itemChanged(QTreeWidgetItem *item,int column)
{
	if (column != 0)
		return;

	MapLayerDef *l = getLayerDef(item);

	if (!l) return;

	l->name = q2u(item->text(0));
}

void TerrainMaterialDlg::on_ok_clicked()
{
	apply();

	accept();
}

void TerrainMaterialDlg::on_cancel_clicked()
{
	reject();
}

void TerrainMaterialDlg::on_altitudeLow_editingFinished()
{
	updataMaskPreview();
}

void TerrainMaterialDlg::on_altitudeHigh_editingFinished()
{
	updataMaskPreview();
}

void TerrainMaterialDlg::on_slopeLow_editingFinished()
{
	updataMaskPreview();
}

void TerrainMaterialDlg::on_slopeHigh_editingFinished()
{
	updataMaskPreview();
}

void TerrainMaterialDlg::on_autoGen_clicked()
{
	updataMaskPreview();
}

#include "filedialog.h"

void TerrainMaterialDlg::on_selectBase_clicked()
{
	QString filename = FileDialog::getOpenFileName(this, "Select Base Image", "textures/terrain/base", "*.tga");

	if (filename.isEmpty())
		return;

	m_baseImage = q2u(filename);

	updateBaseImage();
}

void TerrainMaterialDlg::updateBaseImage() {
	Image image;
	if (!image.loadFileByType(m_baseImage, Image::TGA, Image::NoCompressed | Image::ExpandAlpha)) {
		ui.baseImagePreview->clear();
		ui.baseImageSize->clear();
	} else {
		const byte_t *bits = image.getData(0, TexFormat::BGRA8);

		QImage imageQ(const_cast<byte_t*>(bits), image.getWidth(), image.getHeight(), QImage::Format_ARGB32);
		QPixmap pixmap = QPixmap::fromImage(imageQ);

		ui.baseImagePreview->setPixmap(pixmap);
		ui.baseImageSize->setText(QString("%1x%2").arg(image.getWidth()).arg(image.getHeight()));
	}

	ui.baseFilename->setText(u2q(PathUtil::removeDir(m_baseImage)));
}

void TerrainMaterialDlg::writeCurrentDef() {
	QTreeWidgetItem *item = ui.layerList->currentItem();
	writeToDef(item);
}

