/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

AX_BEGIN_NAMESPACE

static float gOrthoScale = 1.0f;

MapView::MapView( MapContext *ctx ) : View(ctx)
{

}

MapView::~MapView()
{

}
OrthoView::OrthoView(MapContext *con) : MapView(con) {
}

OrthoView::~OrthoView() {

}

// implement view
void OrthoView::doRender() {}

bool OrthoView::handleEvent(const InputEvent &e) {
	return false;
}

TopView::TopView(MapContext *con) : OrthoView(con) {
	m_title = "Top";
	Angles angle(90,90,0);
	m_eyeMatrix.setAxis(angle);
}

FrontView::FrontView(MapContext *con) : OrthoView(con) {
	m_title = "Front";
	Angles angle(0,90,0);
	m_eyeMatrix.setAxis(angle);
}

LeftView::LeftView(MapContext *con) : OrthoView(con) {
	m_title = "Left";
	Angles angle(0,0,0);
	m_eyeMatrix.setAxis(angle);
}


AX_END_NAMESPACE
