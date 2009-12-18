/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Game {

	typedef Render::Primitive renderPrim;

	GameLight::GameLight() {
		m_renderLight = 0;
		m_pointPrim = 0;
		m_spotPrim = 0;
	}

	GameLight::~GameLight() {
		SafeDelete(m_renderLight);
		SafeDelete(m_pointPrim);
		SafeDelete(m_spotPrim);
	}

	void GameLight::doSelectTest() const
	{
		if (m_pointPrim) {
			g_renderSystem->testPrimitive(m_pointPrim, m_renderLight->getMatrix());
		}

		if (m_spotPrim) {
			g_renderSystem->testPrimitive(m_spotPrim, m_renderLight->getMatrix());
		}
	}

	void GameLight::doDebugRender() const
	{
		if (m_pointPrim) {
//			gRenderSystem->addToScene(m_pointPrim);
		}
	}

	void GameLight::doThink() {
		GameEntity::doThink();
	}

	void GameLight::loadLight(const Variant& v) {
		clear();

		if (v.type != Variant::kTable) {
			return;
		}

		LuaTable table = v;

		table.beginRead();
		m_renderLight = new Render::Light();
		float spotangle = table.get("spotAngle");
		m_renderLight->setLightType(RenderLight::Type(table.get("type").toInt()));
		m_renderLight->setSpotAngle(spotangle);
		m_renderLight->setLightColor(table.get("color"),table.get("intensity").toFloat(),table.get("specularX").toFloat());
		m_renderLight->setRadius(table.get("radius"));
		m_renderLight->setCastShadowMap(table.get("castShadowMap").toBool());
		m_renderLight->setMatrix(m_matrix_p);
		m_renderLight->setShadowMapSize(table.get("shadowMapSize").toInt());
		table.endRead();

		setupHelper();

		setRenderEntity(m_renderLight);
	}

	void GameLight::activeLight(bool isActive) {
		if (isActive) {
			m_world->getRenderWorld()->addActor(m_renderLight);
		} else {
			m_world->getRenderWorld()->removeActor(m_renderLight);
		}
	}
#if 0
	void GameLight::onMatrixChanged()
	{
		if (!m_renderLight) {
			return;
		}

		setupHelper();
		m_renderLight->setMatrix(matrix_p_);
		m_renderLight->updateToWorld();
	}
#endif
	void GameLight::setupHelper()
	{
		if (m_renderLight->getLightType() == Render::Light::kSpot) {
			setupSpotPrim();
		} else {
			setupPointPrim();
		}
	}

	void GameLight::setupPointPrim()
	{
		if (!m_pointPrim) {
			m_pointPrim = new Render::Mesh(renderPrim::Static);
			m_pointPrim->initialize(6, 8*3);

			static ushort_t s_indices[] = {
				0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1,
				5, 2, 1, 5, 3, 2, 5, 4, 3, 5, 1, 4,
			};

			ushort_t* idx = m_pointPrim->lockIndexes();
			memcpy(idx, s_indices, sizeof(s_indices));
			m_pointPrim->unlockIndexes();
		}

		Rgb color = m_renderLight->getLightColor().xyz();
		const float size = 0.5f;
		const float height = size * AX_SQRT1_2;

		Vertex* verts = m_pointPrim->lockVertexes();

		Matrix3 axis = Matrix3::getIdentity();
		Vector3 forward = axis[0].getNormalized();
		Vector3 left = axis[1].getNormalized();
		Vector3 up = axis[2].getNormalized();

		Vector3 origin = Vector3(0,0,0);

		verts[0].xyz = origin - up * height;
		verts[1].xyz = origin + left * height;
		verts[2].xyz = origin + forward * height;
		verts[3].xyz = origin - left * height;
		verts[4].xyz = origin - forward * height;
		verts[5].xyz = origin + up * height;

		for (int i = 0; i<6; i++) {
			static const float colorx[] = { 0.3f, 0.9f, 0.8f, 0.7f, 0.6f,1.0f };
			verts[i].rgba.set(color*colorx[i]);
		}

		m_pointPrim->unlockVertexes();

		m_renderLight->clearHelperPrims();
		m_renderLight->addHelperPrim(m_pointPrim);
	}

	void GameLight::setupSpotPrim()
	{
		if (!m_spotPrim) {
			m_spotPrim = new Render::Mesh(renderPrim::Static);
			m_spotPrim->initialize(6*3, 6*3);

			static ushort_t s_indices[] = {
				0, 1, 2, 3, 4, 5, 6, 7, 8,
				9, 10, 11, 12, 13, 14, 15, 16, 17
			};

			ushort_t* idx = m_spotPrim->lockIndexes();
			memcpy(idx, s_indices, sizeof(s_indices));
			m_spotPrim->unlockIndexes();
		}

		Rgb color = m_renderLight->getLightColor().xyz();
		const float spotangle = Math::d2r(m_renderLight->getSpotAngle());
		const float size = 0.5f;
		float len, height;
		Math::sincos(spotangle * 0.5f, len, height);
		len *= size;
		height *= size;

		Vector3 pos[5];

		pos[0].set(0,0,0);
		pos[1].set(-len, -len, -height);
		pos[2].set(len, -len, -height);
		pos[3].set(len, len, -height);
		pos[4].set(-len, len, -height);

		Rgba facecolor[5];
		facecolor[0].set(color * 0.5f);
		facecolor[1].set(color * 0.7f);
		facecolor[2].set(color * 1.0f);
		facecolor[3].set(color * 0.6f);
		facecolor[4].set(color * 0.4f);

		static int s_posindices[18] = {
			0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 2, 3, 1, 3, 4
		};

		static int s_colorindices[18] = {
			0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 4
		};

		Vertex* verts = m_spotPrim->lockVertexes();
		for (int i=0; i<18; i++) {
			verts[i].xyz = pos[s_posindices[i]];
			verts[i].rgba = facecolor[s_colorindices[i]];
		}
		m_spotPrim->unlockVertexes();

		m_renderLight->clearHelperPrims();
		m_renderLight->addHelperPrim(m_spotPrim);
	}

	void GameLight::clear()
	{
		setRenderEntity(0);
		SafeDelete(m_renderLight);
	}

}} // namespace Axon::Game
