/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "gfx_local.h"

AX_BEGIN_NAMESPACE

GfxObject::GfxObject()
{
	m_tm.setIdentity();
	m_entity = 0;
}

GfxObject::~GfxObject()
{

}

BoundingBox GfxObject::getLocalBoundingBox()
{
	return BoundingBox::UnitBox;
}

BoundingBox GfxObject::getBoundingBox()
{
	return getLocalBoundingBox().getTransformed(m_tm);
}

Primitives GfxObject::getHitTestPrims()
{
	return Primitives();
}

void GfxObject::frameUpdate( RenderScene *qscene )
{

}

void GfxObject::issueToQueue( RenderScene *qscene )
{

}

AX_END_NAMESPACE
