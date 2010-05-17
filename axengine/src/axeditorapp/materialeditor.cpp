/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "materialeditor.h"
#include "filedialog.h"

static const char *gnRoot = "root";
static const char *gnTexAnim = "texAnim";
static const char *gnVertexDeform = "vtDeform";
static const char *gnSampler = "sampler";
static const char *gnRgbAnim = "rgbAnim";
static const char *gnAlphaAnim = "alphaAnim";


MaterialEditor::MaterialEditor(QWidget *parent)
	: QMainWindow(parent)
	, m_sysNodeDefs(MtlNodeDef::getNodeDefs())
{
	ui.setupUi(this);

//	ui.rollup->initialize();

//	ui.rollup->setGroupVisible("root", false);

	ui.matTree->header()->setHidden(true);
	ui.matTree->setRoot("");
	ui.matTree->setFilter("*.mtr");
	ui.matTree->refresh();

	initWidgets();
}

MaterialEditor::~MaterialEditor()
{

}

void MaterialEditor::initWidgets()
{
	// fill shader widget
	StringSeq ss = g_fileSystem->fileListByExts("shaders/", ".fx", File::List_Nodirectory|File::List_Sorted);
//	ui.shaderName->clear();

	AX_FOREACH(const String &s, ss) {
		QString qs = u2q(PathUtil::getName(s));

		if (!qs[0].isLetter()) {
			continue;
		}
//		ui.shaderName->addItem(qs);
	}

	// fill surface type widget
//	ui.surfaceType->clear();
	for (int i = 0; i < SurfaceType::NUMBER_ALL; i++) {
//		ui.surfaceType->addItem(SurfaceType(i).toString());
	}
}

void MaterialEditor::on_matTree_itemActivated(QTreeWidgetItem *item,int)
{
	FileItem *fitem = (FileItem*)item;

	g_globalData->curMaterial = u2q(fitem->getFileInfo().fullpath);

	initFromMaterial(fitem->getFileInfo().fullpath);
}

void MaterialEditor::on_actionRefresh_triggered()
{

}

void MaterialEditor::initFromMaterial(const String &name)
{
	MaterialPtr mat = Material::load(name);

#if 0
	if (mat->isDefaulted()) {
		return;
	}
#endif

	Shader *shader = mat->getShaderTemplate();
	AX_ASSERT(shader);

#if 0
	// init shader features
	const StringSeq &features = shader->getFeatures();
	const StringSeq &fnames = shader->getFeatureNames();
	const StringSeq &fdescs = shader->getFeatureDescs();

	for (size_t i = 0; i < features.size(); i++) {
		QCheckBox *cb = new QCheckBox(ui.features);
		cb->setText(u2q(fnames[i]));
		cb->setToolTip(u2q(fdescs [i]));
		ui.features->layout()->addWidget(cb);
	}

	// init shader parameter
	Render::ParameterAnno *pa;
	ui.parameters->setInitialInput(nullptr);
	PropertyCollection *collection = new PropertyCollection(QLatin1String("<root>"));

	for (int i = 0; i < shader->getNumTweakable(); i++) {
		pa = shader->getTweakableDef(i);

//		switch (pa->)
	}
#endif
}

