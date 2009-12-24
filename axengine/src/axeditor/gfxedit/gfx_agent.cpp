/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "gfx_local.h"

AX_BEGIN_NAMESPACE

GfxAgent::GfxAgent(GfxContext* ctx, GfxObject::GfxType gfxType) : Agent(ctx)
{

}

GfxAgent::~GfxAgent()
{

}

Agent* GfxAgent::clone() const
{
	return 0;
}

void GfxAgent::doDeleteFlagChanged(bool del)
{

}

void GfxAgent::doRender()
{

}

void GfxAgent::setMatrix(const AffineMat& matrix)
{

}

const AffineMat& GfxAgent::getMatrix() const
{
	return m_gfxObj->getTm();
}

BoundingBox GfxAgent::getBoundingBox()
{
	return m_gfxObj->getBoundingBox();
}

Variant GfxAgent::getProperty(const String& propname)
{
	return Variant();
}

void GfxAgent::setProperty(const String& propname, const Variant& value)
{

}

void GfxAgent::doPropertyChanged()
{

}

Rgb GfxAgent::getColor() const
{
	return Rgb::Black;
}

void GfxAgent::setColor(Rgb val)
{

}

AX_END_NAMESPACE
