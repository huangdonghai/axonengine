/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GFX_ENTITY_H
#define AX_GFX_ENTITY_H

AX_BEGIN_NAMESPACE

class GfxEntity : public RenderEntity
{
public:
	GfxEntity();
	GfxEntity(const String &filename);
	virtual ~GfxEntity();

	// implement RenderEntity
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual Primitives getHitTestPrims();
	virtual void frameUpdate(QueuedScene *qscene);
	virtual void issueToQueue(QueuedScene *qscene);

	// internal use
	void addObject(GfxObject *obj);
	void removeObject(GfxObject *obj);
	void setAnimStartTime(int time);
	void setAnimTime(int time);

protected:

private:
	Sequence<GfxObject*> m_objects;
	Animator *m_animator;
};

AX_END_NAMESPACE

#endif // AX_GFX_ENTITY_H
