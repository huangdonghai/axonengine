/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

RenderLight::RenderLight() : RenderEntity(kLight)
{
	m_type = kGlobal;
	m_castShadowMap = 0;
	m_radius = 0;
	m_spotAngle = 60;
	m_color = Color3(1,1,1);
	m_skyColor = Color3(0.5,0.5,0.5);
	m_envColor = Color3(0,0,0);
	m_hdrStops = 0;

	m_shadowGen = 0;

	m_preferShadowMapSize = 256;
}

RenderLight::RenderLight(Type t, const Vector3 &pos, Rgb color) : RenderEntity(kLight)
{
	m_type = t;
	setOrigin(pos);
	m_color = Color3(1,1,1);
	m_skyColor = Color3(0.5,0.5,0.5);
	m_envColor = Color3(0,0,0);
	m_spotAngle = 60;
	m_castShadowMap = 0;
	m_radius = 0;
	m_hdrStops = 0;

	m_shadowGen = 0;

	m_preferShadowMapSize = 256;
}

RenderLight::RenderLight(Type t, const Vector3 &pos, Rgb color, float radius) : RenderEntity(kLight)
{
	m_type = t;
	setOrigin(pos);
	m_color = Color3(1,1,1);
	m_skyColor = Color3(0.5,0.5,0.5);
	m_envColor = Color3(0,0,0);
	m_spotAngle = 60;
	m_castShadowMap = 0;
	m_radius = radius;
	m_hdrStops = 0;

	m_shadowGen = 0;

	m_preferShadowMapSize = 256;
}

RenderLight::~RenderLight()
{
#if 0
	m_shadowLink.removeFromList();
#else
	if (m_shadowLink.isLinked())
		m_world->m_shadowLink.erase(this);
#endif
}

BoundingBox RenderLight::getLocalBoundingBox()
{
	if (m_type == kGlobal) {
		return BoundingBox::LargestBox;
	}

	if (m_type == kPoint) {
		return BoundingBox(-m_radius,-m_radius,-m_radius,m_radius,m_radius,m_radius);
	}

	if (m_type == kSpot) {
		float angle = Math::d2r(m_spotAngle * 0.5f);
		float len, height;
		Math::sincos(angle,len,height);
		len *= m_radius / height;
		height = m_radius;
		return BoundingBox(-len,-len,-height,len,len,0);
	}

	return BoundingBox::UnitBox;
}

BoundingBox RenderLight::getBoundingBox()
{
	if (m_type == kGlobal) {
		return BoundingBox::LargestBox;
	}

	return getLocalBoundingBox().getTransformed(m_affineMat);
}

#if 0
void RenderLight::fillQueued(QueuedLight *queued)
{
	m_queuedLight = queued;

	if (!queued) {
		return;
	}

	queued->preQueued = this;

	queued->type = m_type;
	queued->matrix = m_affineMat;

	queued->pos = m_affineMat.origin;
	if (m_type == RenderLight::kGlobal) {
		queued->pos.xyz().normalize();
		queued->pos.w = 0;
	}

	queued->color = m_color;

	queued->skyColor = m_skyColor;
	queued->envColor = m_envColor;
	queued->radius = m_radius;
}
#endif

void RenderLight::issueToScene(RenderScene *qscene)
{
	if (qscene->sceneType != RenderScene::WorldMain)
		return;

	prepareLightBuffer(qscene);
}

void RenderLight::prepareLightBuffer(RenderScene *scene)
{
	if (m_type == kGlobal) {
		prepareLightBuffer_Global(scene);
	} else if (m_type == kPoint) {
		prepareLightBuffer_Point(scene);
	} else if (m_type == kSpot) {
		prepareLightBuffer_Spot(scene);
	}
}

void RenderLight::prepareLightBuffer_Global(RenderScene *scene)
{
	const RenderCamera &camera = scene->camera;
	float znear = camera.znear();
	float zfar = camera.zfar();

	camera.calcPointsAlongZdist(m_lightVolume, znear * 1.1);
	camera.calcPointsAlongZdist(&m_lightVolume[4], zfar * 0.9f);
}

void RenderLight::prepareLightBuffer_Point(RenderScene *scene)
{
	// calculate light volume
	const Vector3 &origin = getOrigin();
	const Matrix3 &viewaxis = getAxis();

	for (int i = 0; i < 2; i++) {
		Vector3 back_forward = viewaxis[0] *((i==0) ? -1.0f : 1.0f);
		for (int j = 0; j < 2; j++) {
			Vector3 left_right = viewaxis[1] *((j==0) ? -1.0f : 1.0f);
			for (int k = 0; k < 2; k++) {
				Vector3 down_up = viewaxis[2] *((k==0) ? -1.0f : 1.0f);

				m_lightVolume[i*4+j*2+k] = origin +(back_forward+left_right+down_up) * m_radius;
			}
		}
	}

	// calculate texture projection matrix
	m_projMatrix.fromAxisInverse(m_affineMat.axis, m_affineMat.origin);
	m_projMatrix.scale(1.0f/m_radius, 1.0f/m_radius, 1.0f/m_radius);

	// check if intersert near clip plane
	if (m_linkedBbox.pointDistance(scene->camera.origin()) > scene->camera.znear() * 4.0f) {
		m_isIntersectsNearPlane = false;
	} else {
		m_isIntersectsNearPlane = true;
	}
}

void RenderLight::prepareLightBuffer_Spot(RenderScene *scene)
{
	// calculate light volume
	const Vector3 &origin = getOrigin();
	const Matrix3 &viewaxis = getAxis();
	const Vector3 &back_forward = -viewaxis[2] * m_radius;

	float extend = m_radius * atanf(Math::d2r(m_spotAngle*0.5f));

	m_lightVolume[0] = origin;
	m_lightVolume[1] = origin;
	m_lightVolume[2] = origin;
	m_lightVolume[3] = origin;

	for (int j = 0; j < 2; j++) {
		Vector3 left_right = viewaxis[0] *((j==0) ? 1.0f : -1.0f);
		for (int k = 0; k < 2; k++) {
			Vector3 down_up = viewaxis[1] *((k==0) ? -1.0f : 1.0f);

			m_lightVolume[4+j*2+k] = origin + back_forward + (left_right+down_up) * extend;
		}
	}

	// calculate texture projection matrix
	m_projMatrix.fromAxisInverse(m_affineMat.axis, m_affineMat.origin);
	m_projMatrix.scale(1.0f/extend, 1.0f/extend, 1.0f/m_radius);

	const Matrix4 &m = m_projMatrix;
	Vector3 p1 = origin;
	Vector3 p2 = m_lightVolume[4];

	p1 = m * p1;
	p2 = m * p2;

	// check if intersect near clip plane
	if (m_linkedBbox.pointDistance(scene->camera.origin()) > scene->camera.znear() * 4.0f) {
		m_isIntersectsNearPlane = false;
	} else {
		m_isIntersectsNearPlane = true;
	}
}

void RenderLight::initShadowGenerator()
{
	if (!m_castShadowMap)
		return;

	int csmSize = r_shadowMapSize.getInteger();
	csmSize = Math::nearestPowerOfTwo(csmSize);

	int localShadowSize = Math::nearestPowerOfTwo(m_preferShadowMapSize);

	if (m_type == kGlobal) {
		m_shadowGen = new ShadowGenerator(this, 4, csmSize);
	} else if (m_type == kPoint) {
		m_shadowGen = new ShadowGenerator(this, 6, localShadowSize);
	} else {
		m_shadowGen = new ShadowGenerator(this, 1, localShadowSize);
	}
}

void RenderLight::clearShadowGenerator()
{
	SafeDelete(m_shadowGen);
}

bool RenderLight::checkShadow(RenderScene *qscene)
{
	if (!m_castShadowMap) {
		return false;
	}

	if (!m_shadowGen) {
		initShadowGenerator();
	}

	if (isGlobal()) {
		genShadowMap(qscene);
		return false;
	}

	return true;
}

#if 0
void RenderLight::linkShadow()
{
#if 0
	m_shadowLink.addToFront(m_world->m_shadowLink);
#else
	m_world->m_shadowLink.push_front(this);
#endif
}

RenderLight *RenderLight::unlinkShadow()
{
	if (m_shadowLink.isInList()) {
		RenderLight *l = m_shadowLink.getNext();
		m_shadowLink.removeFromList();
		return l;
	}

	return 0;
}
#endif

void RenderLight::freeShadowMap()
{
	AX_ASSERT(m_shadowGen);

	m_shadowGen->unuseShadowMap();
}

bool RenderLight::genShadowMap(RenderScene *qscene)
{
	AX_ASSERT(m_shadowGen);

	if (isVisible() || isGlobal()) {
		m_shadowGen->update(qscene);
		return true;
	}

	return false;
}

int RenderLight::getShadowMemoryUsed() const
{
	if (!m_shadowGen)
		return 0;

	return m_shadowMemoryUsed;
}

void RenderLight::setLightColor(const Color3 &color, float intensity, float specularX)
{
	m_color = color * intensity;
}

void RenderLight::setSkyColor(const Color3 &color, float skyIntensity /*= 1.0f*/)
{
	m_skyColor = color * skyIntensity;
}

void RenderLight::setEnvColor(const Color3 &color, float envIntensity /*= 1.0f*/)
{
	m_envColor = color * envIntensity;
}


AX_END_NAMESPACE
