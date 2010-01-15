/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "gfx_local.h"

AX_BEGIN_NAMESPACE

GfxAgent::GfxAgent(GfxContext *ctx, GfxObject::GfxType gfxType)
	: Agent(ctx)
	, m_gfxCtx(ctx)
	, m_gfxObj(0)
	, m_bboxLine(0)
{
	switch (gfxType) {
	case GfxObject::kParticleEmitter:
		m_gfxObj = new ParticleEmitter();
	}
}

GfxAgent::~GfxAgent()
{
	SafeDelete(m_gfxObj);
	SafeDelete(m_bboxLine);
}

Agent *GfxAgent::clone() const
{
	return 0;
}

void GfxAgent::deleteFlagChanged(bool del)
{

}

void GfxAgent::drawHelper()
{
	if (!r_helper->getBool()) {
		return;
	}

	if (m_isSelected && !m_isDeleted) {
		const Matrix3x4 &mat = m_gfxObj->getTm();
		LinePrim::setupBoundingBox(m_bboxLine, mat.origin, mat.axis, m_gfxObj->getLocalBoundingBox(), 1.05f);
		g_renderSystem->addToScene(m_bboxLine);
	}

//	m_gfxObj->doDebugRender();
}

void GfxAgent::setMatrix(const Matrix3x4 &matrix)
{
	m_gfxObj->setTm(matrix);
}

const Matrix3x4 &GfxAgent::getMatrix() const
{
	return m_gfxObj->getTm();
}

BoundingBox GfxAgent::getBoundingBox()
{
	return m_gfxObj->getBoundingBox();
}

Variant GfxAgent::getProperty(const String &propname)
{
	return Variant();
}

void GfxAgent::setProperty(const String &propname, const Variant &value)
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

void GfxAgent::addToContext()
{
	m_gfxCtx->getGfxEntity()->addObject(m_gfxObj);
}

void GfxAgent::removeFromContext()
{
	m_gfxCtx->getGfxEntity()->removeObject(m_gfxObj);
}

AX_END_NAMESPACE
