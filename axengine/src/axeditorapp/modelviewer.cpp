/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "modelviewer.h"
#include "filedialog.h"

ModelViewer::ModelViewer(QWidget *parent)
	: QMainWindow(parent)
	, m_renderModel(nullptr)
{
	ui.setupUi(this);

	ui.mainToolBar->insertWidget(ui.actionRefresh, ui.modelType);

	ui.treeWidget->setRoot("/models");
	ui.treeWidget->setFilter("*.mesh;*.ase;*.hkm");
	ui.treeWidget->refresh();
	
	on_modelType_activated(0);
}

ModelViewer::~ModelViewer() {
	ui.previewWidget->getRenderWorld()->removeActor(m_renderModel);
	SafeDelete(m_renderModel);
}


void ModelViewer::on_modelType_activated(int index) {
#if 0
	FileInfoSeq infos = gFileSystem->getFileInfos("/models", "*.model;*.ase", File::List_sort);

	ui.treeWidget->clear();
	for (size_t i = 0; i < infos.size(); i++) {
		FileItem* item = new FileItem(ui.treeWidget, infos[i]);
	}
#endif
}

void ModelViewer::on_treeWidget_itemDoubleClicked(QTreeWidgetItem* item, int) {
	FileItem* f_item = dynamic_cast<FileItem*>(item);
	const FileInfo& finfo = f_item->getFileInfo();

	if (!f_item)
		return;

	if (f_item->isDirectory()) {
#if 0
		FileInfoSeq infos = gFileSystem->getFileInfos(finfo.fullpath, "*.model;*.ase", File::List_sort);
		for (size_t i = 0; i < infos.size(); i++) {
			FileItem* child = new FileItem(item, infos[i]);
//			f_item->addChild(item);
		}
#endif
		ui.treeWidget->expandItem(item);

//		f_item->setExpanded(true);
	} else {
		RenderWorld* world = ui.previewWidget->getRenderWorld();
		if (m_renderModel) {
			world->removeActor(m_renderModel);
			delete m_renderModel;
		}

		m_renderModel = new HavokModel(finfo.fullpath);
//		m_renderModel->setAnimation("models/crouch.anim");
		world->addActor(m_renderModel);

		ui.previewWidget->setFocusActor(m_renderModel);
		ui.previewWidget->update();
	}
}