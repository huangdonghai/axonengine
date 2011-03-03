/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_EDITOR_MAP_PERSPECTIVEVIEW_H
#define AX_EDITOR_MAP_PERSPECTIVEVIEW_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class PerspectiveView, for Perspective map rendering and editing
//--------------------------------------------------------------------------

class PerspectiveView : public MapView, public ITickable, public IObserver
{
	friend class Context;
	friend class MapContext;

public:
	PerspectiveView(MapContext *context);
	~PerspectiveView();


protected:
	void updateMove();

	// implement View
	virtual void doRender();
	virtual void checkViewOrg();
	virtual void bindFrame(IViewFrame *container);
	virtual void preUpdate() { updateMove(); }

	// implement ITickable
	virtual void tick();

	// implement IEventHandler
	virtual void handleEvent(InputEvent *e);
	virtual void onKeyDown(InputEvent *e);
	virtual void onKeyUp(InputEvent *e);
	virtual void onMouseDown(InputEvent *e);
	virtual void onMouseUp(InputEvent *e);
	virtual void onMouseMove(InputEvent *e);
	virtual void onMouseWheel(InputEvent *e);

	// implement IObserver
	virtual void beNotified(IObservable *subject, int arg);

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
};

AX_END_NAMESPACE

#endif
