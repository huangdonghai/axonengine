/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class Actor
//--------------------------------------------------------------------------

RenderEntity::RenderEntity(Kind k)
	: m_kind(k)
	, m_flags(0)
{
	m_world = 0;
	m_distance = 0;
	m_lodRatio = 1.0f;
	m_lod = 0;
	m_affineMat.setIdentity();
	m_instanceParam.set(1,1,1,1);

	m_visQuery = g_queryManager->allocQuery();
	m_visQuery->setType(Query::QueryType_Vis);
	m_shadowQuery = g_queryManager->allocQuery();
	m_shadowQuery->setType(Query::QueryType_Shadow);

	m_viewDistCulled = false;
	m_queryCulled = false;
	m_visFrameId = 0;

//	m_link.setOwner(this);
}

RenderEntity::~RenderEntity()
{
	g_queryManager->freeQuery(m_shadowQuery);
	g_queryManager->freeQuery(m_visQuery);

	if (m_world) {
		m_world->removeEntity(this);
	}
}

const Vector3 &RenderEntity::getOrigin() const
{
	return m_affineMat.origin;
}

void RenderEntity::setOrigin(const Vector3 &origin)
{
	m_affineMat.origin = origin;
}

const Matrix3 &RenderEntity::getAxis() const
{
	return m_affineMat.axis;
}

void RenderEntity::setAxis(const Angles &angles)
{
	m_affineMat.setAxis(angles);
	m_instanceParam.w = 1.0f;
}

void RenderEntity::setAxis(const Angles &angles, float scale)
{
	m_affineMat.setAxis(angles, scale);
	m_instanceParam.w = scale;
}

const Matrix &RenderEntity::getMatrix() const
{
	return m_affineMat;
}

void RenderEntity::setMatrix(const Matrix &mat)
{
	m_affineMat = mat;
	m_instanceParam.w = m_affineMat.axis[0].getLength();
}


void RenderEntity::refresh()
{
	if (!isInWorld()) {
		return;
	}

	m_world->addEntity(this);
}

int RenderEntity::getFlags() const
{
	return m_flags;
}

void RenderEntity::setFlags(int flags)
{
	m_flags = flags;
}

void RenderEntity::addFlags(int flags)
{
	m_flags |= flags;
}

bool RenderEntity::isFlagSet(Flag flag)
{
	return(m_flags & flag) != 0;
}

void RenderEntity::setQueued(QueuedEntity *queued)
{
	m_queued = queued;
	m_queued->matrix = m_affineMat;
	m_queued->instanceParam = m_instanceParam;
	m_queued->flags = m_flags;
	m_queued->distance = m_distance;
}

QueuedEntity *RenderEntity::getQueued() const
{
	return m_queued;
}


// read only
Matrix4 RenderEntity::getModelMatrix() const
{
	return m_affineMat.toMatrix4();
}

void RenderEntity::update(QueuedScene *qscene, Plane::Side side)
{
	m_cullSide = side;
	calculateLod(qscene);

	frameUpdate(qscene);

	if (!m_viewDistCulled && !m_queryCulled && m_world)
		m_visFrameId = m_world->getVisFrameId();
}

void RenderEntity::frameUpdate(QueuedScene *qscene)
{
	// do nothing
}

void RenderEntity::calculateLod(QueuedScene *qscene)
{
	if (!m_world)
		return;

	const Vector3 &org = qscene->camera.getOrigin();
	m_distance = m_linkedBbox.pointDistance(org);

	if (m_distance < 1) {
		m_viewDistCulled = false;
		return;
	}
	
	float n = r_nearLod->getFloat();
	float f = r_farLod->getFloat();

	m_lod = 1.0f - (m_distance-n) / (f - n);
	m_lod *= m_lodRatio;
	m_lod = Math::saturate(m_lod);

	float forcelod = r_forceLod->getFloat();
	if (forcelod >= 0.0f && forcelod <= 1.0f) {
		m_lod = forcelod;
	}

	float extands = m_linkedExtends;
	float ratio = extands / m_distance / qscene->camera.getFovX() * 90 * 1024;

	if (ratio < r_viewDistCull->getFloat())
		m_viewDistCulled = true;
	else
		m_viewDistCulled = false;

	int hardwareQuery = r_hardwareQuery->getInteger();

	if (hardwareQuery == 1) {
		return;
	}

	m_queryCulled = false;
	if (!hardwareQuery) {
		return;
	}

	if (m_viewDistCulled)
		return;

	if (m_distance < 5) {
		return;
	}

	int updateframe = 1024 / ratio;

	int worldframe = m_world->getVisFrameId();

	if (m_visQuery->m_resultFrame < 0) {
		m_visQuery->issueQuery(worldframe, m_linkedBbox);
		return;
	}

	if (worldframe - m_visQuery->m_resultFrame < 20) {
		if (m_visQuery->m_result == 0) {
			// if is occluded, query every frame to avoid flick
			m_queryCulled = true;
			updateframe /= 2;
		}
	}

	updateframe = Math::clamp(updateframe, 0, 20);

	if (worldframe - m_visQuery->m_resultFrame >= updateframe) {
		m_visQuery->issueQuery(worldframe, m_linkedBbox);
	}
}

bool RenderEntity::isVisable() const
{
	if (!m_world)
		return true;

	return m_visFrameId == m_world->getVisFrameId();
}

void RenderEntity::updateCsm(QueuedScene *qscene, Plane::Side side)
{
	if (!r_csmCull->getBool())
		return;

	if (m_shadowQuery->m_resultFrame == m_world->getShadowFrameId()) {
		return;
	}

	if (side != Plane::Front)
		return;

	m_shadowQuery->issueQuery(m_world->getShadowFrameId(), m_linkedBbox);
}

bool RenderEntity::isCsmCulled() const
{
	if (!r_csmCull->getBool())
		return false;

	if (m_shadowQuery->m_resultFrame < 0)
		return false;

	if (m_shadowQuery->m_resultFrame != m_world->getShadowFrameId())
		return false;

	return m_shadowQuery->m_result == 0;
}

void RenderEntity::setInstanceColor(const Vector3 &color)
{
	m_instanceParam.x = color.x;
	m_instanceParam.y = color.y;
	m_instanceParam.z = color.z;
}

Vector3 RenderEntity::getInstanceColor() const
{
	return m_instanceParam.xyz();
}

AX_END_NAMESPACE


