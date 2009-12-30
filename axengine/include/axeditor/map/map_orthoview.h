/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_MAP_ORTHOVIEW_H
#define AX_EDITOR_MAP_ORTHOVIEW_H

AX_BEGIN_NAMESPACE

class MapView : public View {
public:
	enum Type {
		Top, Left, Front, Perspective, Number
	};

	MapView(MapContext* ctx);
	virtual ~MapView();
};

class OrthoView : public MapView {
public:
	OrthoView(MapContext* con);
	virtual ~OrthoView();

protected:
	// implement view
	virtual void doRender();
	virtual bool handleEvent(const InputEvent& e);

protected:
	float m_scale;
};

class TopView : public OrthoView {
public:
	TopView(MapContext* con);
};

class FrontView : public OrthoView {
public:
	FrontView(MapContext* con);
};

class LeftView : public OrthoView {
public:
	LeftView(MapContext* con);
};

AX_END_NAMESPACE

#endif // end guardian

