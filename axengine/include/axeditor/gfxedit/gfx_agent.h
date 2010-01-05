/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GFXEDIT_ACTOR_H
#define AX_GFXEDIT_ACTOR_H

AX_BEGIN_NAMESPACE

class GfxContext;

class GfxAgent : public Agent {
public:
	GfxAgent(GfxContext *ctx, GfxObject::GfxType gfxType);
	virtual ~GfxAgent();

	// implement Agent
	virtual Agent *clone() const;
	virtual void deleteFlagChanged(bool del);
	virtual void drawHelper();

	virtual void setMatrix(const AffineMat &matrix);
	virtual const AffineMat &getMatrix() const;

	virtual BoundingBox getBoundingBox();

	virtual Variant getProperty(const String &propname);
	virtual void setProperty(const String &propname, const Variant &value);
	virtual void doPropertyChanged();

	virtual Rgb getColor() const;
	virtual void setColor(Rgb val);

	virtual void addToContext();
	virtual void removeFromContext();

private:
	GfxContext *m_gfxCtx;
	GfxObject *m_gfxObj;
	LinePrim *m_bboxLine;
};

AX_END_NAMESPACE

#endif
