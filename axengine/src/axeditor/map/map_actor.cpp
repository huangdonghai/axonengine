/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

AX_BEGIN_NAMESPACE

	//------------------------------------------------------------------------------
	// class Entity, Editor Entity
	//------------------------------------------------------------------------------

	MapActor::MapActor(const String& type) {
		m_iconPrim = 0;
		GameWorld* gameworld = getMapContext()->getGameWorld();
		m_gameEntity = gameworld->createEntity(type.c_str());
		m_gameObj = m_gameEntity;

//		bindToGame();

		const ClassInfo* ci = m_gameEntity->getClassInfo();
		if (!ci) {
			return;
		}

		String icon = ci->getField("Editor.icon");
		if (icon.empty()) {
			return;
		}

		TexturePtr tex = Texture::load("editor/icons/" + icon);

		if (!tex) {
			return;
		}

		MaterialPtr mat = Material::loadUnique("_icon");
		AX_ASSERT(mat);
		mat->setTexture(SamplerType::Diffuse, tex.get());

		m_iconPrim = MeshPrim::createScreenQuad(MeshPrim::HintDynamic, Rect(-1,-1,2,2), Rgba::White, mat.get());
	}

	MapActor::~MapActor() {
	}


	void MapActor::doRender()
	{
		if (m_isDeleted)
			return;

		if (m_iconPrim) {
			Vector4 param = m_gameEntity->getOrigin_p();
			param.w = 0.25f;
			m_iconPrim->getMaterial()->setParameter("s_iconparam", 4, param);
			g_renderSystem->addToScene(m_iconPrim);
		}

		return MapAgent::doRender();
	}

	MapAgent* MapActor::clone() const
	{
		// create entity
		GameWorld* gameworld = getMapContext()->getGameWorld();
		MapActor* newent = new MapActor(m_gameEntity->getClassInfo()->m_className);

		newent->m_gameEntity->copyPropertiesFrom(this->m_gameEntity);
		newent->m_gameEntity->autoGenerateName();
		newent->setMatrix(getMatrix());
		newent->setColor(getColor());

		newent->bindToGame();
		return newent;
	}

AX_END_NAMESPACE
