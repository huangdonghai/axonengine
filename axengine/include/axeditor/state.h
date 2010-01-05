/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_STATE_H
#define AX_EDITOR_STATE_H

AX_BEGIN_NAMESPACE

class AX_API State : public IObservable {
public:
	enum ObserverFlag {
		TerrainBrush, Transform, TransformToApp
	};

	State();
	virtual ~State();

	void setTransformState(bool editable, bool clear, const Vector3 &parameter);

	float moveSpeed;
	SelectPart selectionPart;

	// transform, app->dll
	int cursorSelectSize;
	int gizmoSize;
	TransformTool::Space transformSpace;
	TransformTool::Center transformCenter;
	bool transformRel;		// relatively transform

	// transform, dll->app
	struct TransformState {
		bool editable;
		bool clear;
		Vector3 parameter;
	} transformState;

	// snap
	bool isSnapToGrid;
	float snapToGrid;
	bool isSnapToAngle;
	float snapToAngle;
};

AX_END_NAMESPACE

#endif // AX_EDITOR_STATE_H

