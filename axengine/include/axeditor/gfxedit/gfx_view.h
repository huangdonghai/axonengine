/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GFXEDIT_VIEW_H
#define AX_GFXEDIT_VIEW_H

AX_BEGIN_NAMESPACE

class GfxView : public View
{
	friend class GfxContext;

public:
	GfxView(GfxContext *ctx);
	virtual ~GfxView();

protected:
	// implement View
	virtual void preUpdate() { updateMove(); }
	virtual void doRender();

	// implement IInputHandler
	virtual void handleEvent(InputEvent *e);
	virtual void onKeyDown(InputEvent *e);
	virtual void onKeyUp(InputEvent *e);
	virtual void onMouseDown(InputEvent *e);
	virtual void onMouseUp(InputEvent *e);
	virtual void onMouseMove(InputEvent *e);
	virtual void onMouseWheel(InputEvent *e);

	void updateMove();

private:
	enum Tracking {
		kNone, kRotateHead, kRotate, kPan, kZoom, kHorizonPan
	};

	enum Moving {
		Accel = 1, Front = 2, Left = 4, Right = 8, Back = 16,
		MoveMask = Front + Left + Right + Back
	};

	Tracking m_tracking;
	int m_moving;
	Point m_trackingPos;
	Vector3 m_trackingCenter;
	bool m_isTrackingCenterSet;
	LinePrim *m_gridLines;
};

AX_END_NAMESPACE

#endif
