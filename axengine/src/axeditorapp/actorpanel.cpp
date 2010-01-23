/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "actorpanel.h"
#include "workbench.h"

ActorPanel::ActorPanel(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	ui.rollup->initialize();

	g_mapContext->attachObserver(this);

	connect(ui.propEditor, SIGNAL(propertyChanged(IProperty*)), this, SLOT(onPropertyChanged(IProperty*)) );
	connect(ui.scriptProp, SIGNAL(propertyChanged(IProperty*)), this, SLOT(onPropertyChanged(IProperty*)) );
}

ActorPanel::~ActorPanel()
{
	g_mapContext->detachObserver(this);
}

void ActorPanel::doNotify(IObservable *subject, int arg ) {
	if (subject != g_mapContext ) {
		return;
	}

	bool showbasic = false;

	int track = Context::SelectionChanged;

	if (!(arg&track ) ) {
		return;
	}

	const AgentList &actorlist = g_mapContext->getSelection();

	if (!actorlist.containsOne() ) {
		ui.objectName->setText(QString("%1 objects").arg(actorlist.size()));
		ui.propEditor->initFromObject(nullptr );
		ui.scriptProp->initScriptProp(nullptr );
		ui.scriptClass->setText("");
		ui.cppType->setText("");
		ui.scriptFile->setText("");
	} else {
		MapActor *actor = static_cast<MapActor*>(actorlist.back());
		GameObject *node = actor->getGameObject();
		ui.propEditor->initFromObject(node );
		ui.scriptProp->initScriptProp(node );
		ui.objectName->setText(u2q(node->get_objectName()) );
		ui.objectColor->setColor(actor->getColor() );
		const ScriptClass *ci = actor->getGameObject()->getScriptClass();
		if (ci ) {
			ui.scriptClass->setText(u2q(ci->getName()) );
			QString filename = u2q("scripts/"+ ci->getName() );

			for (int i = 0; i < filename.size(); i++ ) {
				if (filename[i] == '.' ) {
					filename[i] = '/';
				}
			}
			ui.scriptFile->setText(filename + ".lua" );
		}
		ui.cppType->setText(u2q(actor->getGameObject()->getMetaInfo()->getName()) );
	}

	if (actorlist.empty() ) {
		showbasic = false;
	} else {
		showbasic = true;
	}

	ui.rollup->setPageVisible(0, showbasic );
	ui.rollup->setPageVisible(1, showbasic );
}

void ActorPanel::on_editScript_clicked() {
	g_workbench->openScriptFile(ui.scriptFile->text() );
}

void ActorPanel::onPropertyChanged(IProperty *property )
{
	g_mapContext->setActorProperty(q2u(property->propertyName()), q2x(property->value()) );
}

void ActorPanel::on_objectColor_colorChanged( const QColor &color )
{
	const AgentList &actorlist = g_mapContext->getSelection();

	actorlist.setColor(q2x(color));
}