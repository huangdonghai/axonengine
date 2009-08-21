/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

namespace Axon { namespace Editor { namespace MapEdit {

	//------------------------------------------------------------------------------
	// class Actor
	//------------------------------------------------------------------------------

	MapActor::MapActor() : Actor(g_mapContext) {
		m_gameNode = nullptr;
		m_bboxLine = nullptr;
	}

	MapActor::~MapActor() {
		SafeDelete(m_gameNode);
		SafeDelete(m_bboxLine);
	}

	void MapActor::doSelect() {
		g_renderSystem->loadSelectId(m_id);
		m_gameNode->doSelectTest();
	}


	void MapActor::setMatrix(const AffineMat& matrix) {
		m_gameNode->setMatrix_p(matrix);
	}

	void MapActor::doRender() {
		if (!r_helper->getBool()) {
			return;
		}

		if (m_isSelected && !m_isDeleted) {
			Render::Line::setupBoundingBox(m_bboxLine, m_gameNode->getOrigin_p(), m_gameNode->getAxis_p(), m_gameNode->getLocalBoundingBox(), 1.05f);
			g_renderSystem->addToScene(m_bboxLine);
		}

		m_gameNode->doDebugRender();
	}

	MapActor* MapActor::clone() const {
		return nullptr;
	}


	void MapActor::writeXml(File* f, int indent) const {
		String indstr(indent*2, ' ');
#define INDENT if (indent) f->printf("%s", indstr.c_str());

		INDENT; f->printf("<actor type=\"%s\" id=\"%d\"\n", typeToString(getType()), m_id);
		INDENT;	f->printf("  matrix=\"%s\"\n", m_gameNode->getMatrix_p().toString().c_str());
		INDENT; f->printf("  color=\"%s\"\n", getColor().toString().c_str());
		INDENT; f->printf(">\n");

		m_gameNode->writeXml(f, indent + 1);

		INDENT; f->printf("</actor>\n");

#undef INDENT
	}

	void MapActor::readXml(const TiXmlElement* node) {

	}

	const char* MapActor::typeToString(Type t) {
#define AX_ENUM_ITEM(e) case e: return #e;
		switch (t) {
			AX_ENUM_ITEM(kNone)
			AX_ENUM_ITEM(kStatic)
			AX_ENUM_ITEM(kSpeedTree)
			AX_ENUM_ITEM(kBrush)
			AX_ENUM_ITEM(kEntity)
		}
#undef AX_ENUM_ITEM

		return nullptr;
	}

	MapActor::Type MapActor::stringToType(const char* str) {
#define AX_ENUM_ITEM(e) { #e, e },

		static struct {
			const char* str;
			Type t;
		} nameToType[] = {
			AX_ENUM_ITEM(kNone)
			AX_ENUM_ITEM(kStatic)
			AX_ENUM_ITEM(kSpeedTree)
			AX_ENUM_ITEM(kBrush)
			AX_ENUM_ITEM(kEntity)
		};
#undef AX_ENUM_ITEM

		for (size_t i = 0; i < ArraySize(nameToType); i++) {
			if (Strequ(str, nameToType[i].str)) {
				return nameToType[i].t;
			}
		}

		return kNone;
	}

	Axon::Variant MapActor::getProperty( const String& propname )
	{
		if (m_gameNode)
			return m_gameNode->getProperty(propname.c_str());

		return Variant();
	}

	void MapActor::setProperty(const String& name, const Variant& value)
	{
		if (m_gameNode) {
			m_gameNode->setProperty(name.c_str(), value);
			m_gameNode->doPropertyChanged();
		}
	}

	void MapActor::doPropertyChanged()
	{
		if (m_gameNode)
			m_gameNode->doPropertyChanged();
	}

	void MapActor::bindToGame()
	{
		AX_ASSURE(!m_isInGame);
		getMapContext()->getGameWorld()->addNode(m_gameNode);
		m_isInGame = true;
	}

	void MapActor::unbindToGame()
	{
		AX_ASSURE(m_isInGame);
		getMapContext()->getGameWorld()->removeNode(m_gameNode);
		m_isInGame = false;
	}

	Rgb MapActor::getColor() const
	{
		return m_gameNode->getInstanceColor();
	}

	void MapActor::setColor( Rgb val )
	{
		m_gameNode->setInstanceColor(val.toVector());
	}

	void MapActor::doDeleteFlagChanged( bool del )
	{
		if (del) {
			static_cast<MapContext*>(m_context)->getGameWorld()->removeNode(m_gameNode);
		} else {
			static_cast<MapContext*>(m_context)->getGameWorld()->addNode(m_gameNode);
		}
	}



	//--------------------------------------------------------------------------
	// class Static
	//--------------------------------------------------------------------------

	Static::Static() {
		MapContext* mapContext = static_cast<MapContext*>(m_context);
		MapState* mapState = mapContext->getMapState();

		m_gameFixed = new Game::StaticFixed();
		m_gameNode = m_gameFixed;

		m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(mapState->staticModelName)));
	}

	Static::Static(const String& nametemplate) {
		m_gameFixed = new Game::StaticFixed();
		m_gameNode = m_gameFixed;

		m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(nametemplate)));
	}

	Static::~Static() {
	}

	void Static::doRender() {
		if (m_isDeleted)
			return;

		MapActor::doRender();

		if (!m_actorDirty)
			return;

		// do other things here...

		m_actorDirty = false;
	}

	MapActor* Static::clone() const {
		Static* result = new Static();

		AX_ASSERT(result);

		result->m_gameNode->copyPropertiesFrom(m_gameNode);
		result->setMatrix(getMatrix());
		result->setColor(getColor());
		result->bindToGame();

		return result;
	}

	//--------------------------------------------------------------------------
	// class SpeedTree
	//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	SpeedTree::SpeedTree() {
		MapContext* mapContext = static_cast<MapContext*>(m_context);
		MapState* mapState = mapContext->getMapState();

		m_gameFixed = new Game::TreeFixed();
		m_gameNode = m_gameFixed;

		m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(mapState->treeFilename)));
	}

	SpeedTree::SpeedTree(const String& nametemplate) {
		m_gameFixed = new Game::TreeFixed();
		m_gameNode = m_gameFixed;

		m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(nametemplate)));
	}

	SpeedTree::~SpeedTree() {
	}

	void SpeedTree::doRender() {
		if (m_isDeleted)
			return;

		MapActor::doRender();

		if (!m_actorDirty)
			return;

		// do other things here...

		m_actorDirty = false;
	}

	MapActor* SpeedTree::clone() const {
		SpeedTree* result = new SpeedTree();

		AX_ASSERT(result);

		result->m_gameNode->copyPropertiesFrom(m_gameNode);
		result->setMatrix(getMatrix());
		result->setColor(getColor());
		result->bindToGame();

		return result;
	}
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

}}} // namespace Axon::Editor::MapEdit

