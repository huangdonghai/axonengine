/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class Agent
//--------------------------------------------------------------------------

MapAgent::MapAgent() : Agent(g_mapContext)
{
	m_gameObj = nullptr;
	m_bboxLine = nullptr;
}

MapAgent::~MapAgent() {
	SafeDelete(m_gameObj);
	SafeDelete(m_bboxLine);
}

void MapAgent::doHitTest() {
	g_renderSystem->loadHitId(m_id);
	m_gameObj->doHitTest();
}


void MapAgent::setMatrix(const AffineMat &matrix) {
	m_gameObj->setMatrix_p(matrix);
}

void MapAgent::drawHelper() {
	if (!r_helper->getBool()) {
		return;
	}

	if (m_isSelected && !m_isDeleted) {
		LinePrim::setupBoundingBox(m_bboxLine, m_gameObj->getOrigin_p(), m_gameObj->getAxis_p(), m_gameObj->getLocalBoundingBox(), 1.05f);
		g_renderSystem->addToScene(m_bboxLine);
	}

	m_gameObj->doDebugRender();
}

MapAgent *MapAgent::clone() const {
	return nullptr;
}


void MapAgent::writeXml(File *f, int indent) const {
	String indstr(indent*2, ' ');
#define INDENT if (indent) f->printf("%s", indstr.c_str());

	INDENT; f->printf("<actor type=\"%s\" id=\"%d\"\n", typeToString(getType()), m_id);
	INDENT;	f->printf("  matrix=\"%s\"\n", m_gameObj->getMatrix_p().toString().c_str());
	INDENT; f->printf("  color=\"%s\"\n", getColor().toString().c_str());
	INDENT; f->printf(">\n");

	m_gameObj->writeXml(f, indent + 1);

	INDENT; f->printf("</actor>\n");

#undef INDENT
}

void MapAgent::readXml(const TiXmlElement *node) {

}

const char *MapAgent::typeToString(Type t) {
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

MapAgent::Type MapAgent::stringToType(const char *str) {
#define AX_ENUM_ITEM(e) { #e, e },

	static struct {
		const char *str;
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

Axon::Variant MapAgent::getProperty( const String &propname )
{
	if (m_gameObj)
		return m_gameObj->getProperty(propname.c_str());

	return Variant();
}

void MapAgent::setProperty(const String &name, const Variant &value)
{
	if (m_gameObj) {
		m_gameObj->setProperty(name.c_str(), value);
		m_gameObj->doPropertyChanged();
	}
}

void MapAgent::doPropertyChanged()
{
	if (m_gameObj)
		m_gameObj->doPropertyChanged();
}

void MapAgent::addToContext()
{
	AX_ASSURE(!m_isInGame);
	getMapContext()->getGameWorld()->addObject(m_gameObj);
	m_isInGame = true;
}

void MapAgent::removeFromContext()
{
	AX_ASSURE(m_isInGame);
	getMapContext()->getGameWorld()->removeObject(m_gameObj);
	m_isInGame = false;
}

Rgb MapAgent::getColor() const
{
	return m_gameObj->getInstanceColor();
}

void MapAgent::setColor( Rgb val )
{
	m_gameObj->setInstanceColor(val.toVector());
}

//--------------------------------------------------------------------------
// class MapStatic
//--------------------------------------------------------------------------

MapStatic::MapStatic() {
	MapContext *mapContext = static_cast<MapContext*>(m_context);
	MapState *mapState = mapContext->getMapState();

	m_gameFixed = new StaticFixed();
	m_gameObj = m_gameFixed;

	m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(mapState->staticModelName)));
}

MapStatic::MapStatic(const String &nametemplate) {
	m_gameFixed = new StaticFixed();
	m_gameObj = m_gameFixed;

	m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(nametemplate)));
}

MapStatic::~MapStatic() {
}

void MapStatic::drawHelper() {
	if (m_isDeleted)
		return;

	MapAgent::drawHelper();

	if (!m_actorDirty)
		return;

	// do other things here...

	m_actorDirty = false;
}

MapAgent *MapStatic::clone() const {
	MapStatic *result = new MapStatic();

	AX_ASSERT(result);

	result->m_gameObj->copyPropertiesFrom(m_gameObj);
	result->setMatrix(getMatrix());
	result->setColor(getColor());
	result->addToContext();

	return result;
}

//--------------------------------------------------------------------------
// class MapSpeedTree
//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
MapSpeedTree::MapSpeedTree() {
	MapContext *mapContext = static_cast<MapContext*>(m_context);
	MapState *mapState = mapContext->getMapState();

	m_gameFixed = new TreeFixed();
	m_gameObj = m_gameFixed;

	m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(mapState->treeFilename)));
}

MapSpeedTree::MapSpeedTree(const String &nametemplate) {
	m_gameFixed = new TreeFixed();
	m_gameObj = m_gameFixed;

	m_gameFixed->set_objectName(g_scriptSystem->generateObjectName(PathUtil::getName(nametemplate)));
}

MapSpeedTree::~MapSpeedTree() {
}

void MapSpeedTree::drawHelper() {
	if (m_isDeleted)
		return;

	MapAgent::drawHelper();

	if (!m_actorDirty)
		return;

	// do other things here...

	m_actorDirty = false;
}

MapAgent *MapSpeedTree::clone() const {
	MapSpeedTree *result = new MapSpeedTree();

	AX_ASSERT(result);

	result->m_gameObj->copyPropertiesFrom(m_gameObj);
	result->setMatrix(getMatrix());
	result->setColor(getColor());
	result->addToContext();

	return result;
}
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

AX_END_NAMESPACE

