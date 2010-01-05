/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE

State::State() {
	moveSpeed = 10;
	selectionPart = SelectPart::All;

	cursorSelectSize = 5;
	gizmoSize = 100;
	transformSpace = TransformTool::WorldSpace;
	transformCenter = TransformTool::PivotCenter;
	transformRel = false;

	// transform dll -> app
	transformState.editable = false;
	transformState.clear = true;
	transformState.parameter.set(0, 0, 0);

	isSnapToGrid = true;
	snapToGrid = 0.125f;
	isSnapToAngle = true;
	snapToAngle = 5;
}

State::~State() {}

void State::setTransformState(bool editable, bool clear, const Vector3 &parameter)
{
	transformState.editable = editable;
	transformState.clear = clear;
	transformState.parameter = parameter;
	notify(TransformToApp);
}

AX_END_NAMESPACE

