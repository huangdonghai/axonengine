/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Map {

	LayerDef::LayerDef(int _id) : id(_id) {}

	LayerDef::~LayerDef() {}

	LayerDef* LayerDef::clone() const {
		LayerDef* result = new LayerDef(id);

		result->name = name;
		result->surfaceType = surfaceType;
		result->color = color;

		result->baseImage = baseImage;
		result->genBaseFromDetail = genBaseFromDetail;

		result->isUseDetail = isUseDetail;
		result->detailMat = detailMat;
		result->uvScale = uvScale;
		result->isVerticalProjection = isVerticalProjection;

		result->isAutoGen = isAutoGen;
		result->altitudeRange = altitudeRange;
		result->slopeRange = slopeRange;

		return result;
	}


	MaterialDef::MaterialDef() {
		m_maxLayerId = 0;
		m_numLayers = 0;
	}

	MaterialDef::~MaterialDef() {
		for (int i = 0; i < m_numLayers; i++) {
			delete m_layerDefs[i];
		}
	}

	int MaterialDef::getNumLayers() const {
		return m_numLayers;
	}

	LayerDef* MaterialDef::getLayerDef(int idx) const {
		AX_ASSERT(idx >= 0 && idx < m_numLayers);
		return m_layerDefs[idx];
	}

	LayerDef* MaterialDef::findLayerDefById(int id) const {
		for (int i = 0; i < m_numLayers; i++) {
			if (m_layerDefs[i]->id == id)
				return m_layerDefs[i];
		}

		return nullptr;
	}


	LayerDef* MaterialDef::createLayerDef() {
		if (m_numLayers == MaxLayers)
			return nullptr;

		LayerDef* layer = new LayerDef(m_maxLayerId++);

		layer->surfaceType = SurfaceType::Dust;
		layer->color = Rgba::White;

		layer->isUseDetail = false;
		layer->uvScale.set(0.25f, 0.25f);
		layer->isVerticalProjection = false;

		layer->isAutoGen = true;
		layer->altitudeRange.set(-1024, 1024);
		layer->slopeRange.set(0, 90);

		m_layerDefs[m_numLayers++] = layer;

		return layer;
	}

	void MaterialDef::deleteLayerDef(LayerDef* l) {
		if (m_numLayers == 0)
			return;

		int i;
		for (i = 0; i < m_numLayers; i++) {
			if (m_layerDefs[i] == l)
				break;
		}

		if (i == m_numLayers) {
			// not found, fire error
			Errorf("%s: cann't found LayerDef", __func__);
		}

		delete l;
		// move layers after this forward
		for (; i < m_numLayers - 1; i++) {
			m_layerDefs[i] = m_layerDefs[i+1];
		}

		m_numLayers--;
	}

	void MaterialDef::moveUpLayerDef(LayerDef* l) {
		if (m_numLayers == 0)
			return;

		int i;
		for (i = 0; i < m_numLayers; i++) {
			if (m_layerDefs[i] == l)
				break;
		}

		if (i == m_numLayers) {
			// not found, fire error
			Errorf("%s: cann't found LayerDef", __func__);
		}

		// is first layer, can't move
		if (i == 0)
			return;

		m_layerDefs[i] = m_layerDefs[i-1];
		m_layerDefs[i-1] = l;
	}

	void MaterialDef::moveDownLayerDef(LayerDef* l) {
		if (m_numLayers == 0)
			return;

		int i;
		for (i = 0; i < m_numLayers; i++) {
			if (m_layerDefs[i] == l)
				break;
		}

		if (i == m_numLayers) {
			// not found, fire error
			Errorf("%s: cann't found LayerDef", __func__);
		}

		// is last layer, can't move
		if (i == m_numLayers - 1)
			return;

		m_layerDefs[i] = m_layerDefs[i+1];
		m_layerDefs[i+1] = l;
	}

	MaterialDef* MaterialDef::clone() const {
		MaterialDef* result = new MaterialDef();

		result->m_numLayers = m_numLayers;
		result->m_maxLayerId = m_maxLayerId;

		for (int i = 0; i < m_numLayers; i++) {
			result->m_layerDefs[i] = m_layerDefs[i]->clone();
		}

		return result;
	}

	void MaterialDef::setLayer(int idx, LayerDef* l) {
		AX_ASSERT(idx >= 0 && idx < m_numLayers);
		int oldindex = findLayerIndex(l);
		AX_ASSERT(oldindex >= 0 && oldindex < m_numLayers);

		std::swap(m_layerDefs[idx], m_layerDefs[oldindex]);
	}

	inline const char* xgetAttr(const TiXmlElement* elem, const char* name, const char* defaultstr) {
		const char* result = elem->Attribute(name);
		if (result) return result;
		return defaultstr;
	}

	void MaterialDef::parseXml(const TiXmlElement* node) {
		node->Attribute("maxLayerId", &m_maxLayerId);
		node->Attribute("numLayers", &m_numLayers);

		const TiXmlElement* child = node->FirstChildElement();

		int count = 0;
		for (; child; child = child->NextSiblingElement()) {
			if (child->ValueTStr() != "layer")
				continue;

			int id = atoi(child->Attribute("id"));
			LayerDef* l = new LayerDef(id);
			const char* attr;

			l->name = child->Attribute("name");
			l->surfaceType = atoi(child->Attribute("surfaceType"));
			l->color.parseRgb(child->Attribute("color"));

			l->baseImage = xgetAttr(child, "baseImage", "default.tga");
			l->genBaseFromDetail = !!atoi(xgetAttr(child, "genBaseFromDetail", "0"));

			attr = child->Attribute("isUseDetail");
			if (!attr) attr = "0";
			l->isUseDetail = !!atoi(attr);
			l->detailMat = child->Attribute("detailMat");
			l->uvScale.parse(child->Attribute("uvScale"));
			l->isVerticalProjection = !!atoi(child->Attribute("isVerticalProjection"));
			l->isAutoGen = !!atoi(child->Attribute("isAutoGen"));
			l->altitudeRange.parse(child->Attribute("altitudeRange"));
			l->slopeRange.parse(child->Attribute("slopeRange"));

			m_layerDefs[count] = l;
			count++;
		}

		AX_ASSERT(count == m_numLayers);
	}

	inline void Indent(File*f, const char* ind) {
		if (ind)
			f->printf("%s", ind);
	}

	void MaterialDef::writeToFile(File* f, int indent) {
#define INDENT Indent(f, ind.c_str())
		String ind(indent*2, ' ');

		INDENT;f->printf("<materialDef maxLayerId=\"%d\" numLayers=\"%d\">\n", m_maxLayerId, m_numLayers);

		// write layers
		for (int i = 0; i < m_numLayers; i++) {
			INDENT;f->printf("  <layer\n");
			INDENT;f->printf("    id=\"%d\"\n", m_layerDefs[i]->id);
			INDENT;f->printf("    name=\"%s\"\n", m_layerDefs[i]->name.c_str());
			INDENT;f->printf("    surfaceType=\"%d\"\n", m_layerDefs[i]->surfaceType);
			INDENT;f->printf("    color=\"%s\"\n", m_layerDefs[i]->color.toStringRgb().c_str());
			INDENT;f->printf("    baseImage=\"%s\"\n", m_layerDefs[i]->baseImage.c_str());
			INDENT;f->printf("    genBaseFromDetail=\"%d\"\n", m_layerDefs[i]->genBaseFromDetail);
			INDENT;f->printf("    isUseDetail=\"%d\"\n", m_layerDefs[i]->isUseDetail);
			INDENT;f->printf("    detailMat=\"%s\"\n", m_layerDefs[i]->detailMat.c_str());
			INDENT;f->printf("    uvScale=\"%f,%f\"\n", m_layerDefs[i]->uvScale.x, m_layerDefs[i]->uvScale.y);
			INDENT;f->printf("    isVerticalProjection=\"%d\"\n", m_layerDefs[i]->isVerticalProjection);
			INDENT;f->printf("    isAutoGen=\"%d\"\n", m_layerDefs[i]->isAutoGen);
			INDENT;f->printf("    altitudeRange=\"%f,%f\"\n", m_layerDefs[i]->altitudeRange.x, m_layerDefs[i]->altitudeRange.y);
			INDENT;f->printf("    slopeRange=\"%f,%f\"\n", m_layerDefs[i]->slopeRange.x, m_layerDefs[i]->slopeRange.y);
			INDENT;f->printf("  />\n");
		}
		INDENT;f->printf("</materialDef>\n");
#undef INDENT
	}

	int MaterialDef::findLayerIndex( LayerDef* l )
	{
		for (int i = 0; i < m_numLayers; i++) {
			if (m_layerDefs[i] == l)
				return i;
		}

		return -1;
	}

	EnvDef::EnvDef()
		: m_textureSkyBox("textures/skys/day/box")
		, m_skyModel()
		, m_materialOcean("material/ocean")
	{
		m_fogColor.set(119,171,201);
		m_fogDensity = 0.0004f;
		m_viewDist = 576;

		m_haveSkyBox = true;
		m_haveSkyModel = false;

		m_haveGlobalLight = true;
		m_castShadow = true;
		m_sunColor.set(255, 255, 255);
		m_sunColorX = 1;
		m_skyColor.set(255, 255, 255);
		m_skyColorX = 1;
		m_envColor.set(0,0,0);
		m_envColorX = 1;
		m_latitude = 0;
		m_dayTime = 10;

		m_haveOcean = true;
		m_shoreSize = 2.0f;
		m_oceanFogColor.set(119,201,171);
		m_oceanFogColor *= 0.25f;
		m_oceanFogDensity = 0.04f;
		m_caustics = true;
	}

	EnvDef::~EnvDef() {
	}

	void EnvDef::parseXml(const TiXmlElement* node) {
		this->readProperties(node);
	}

	void EnvDef::writeToFile(File* f, int indent/*=0 */) {
#define INDENT Indent(f, ind.c_str())
		String ind(indent*2, ' ');

		INDENT;f->printf("<envDef\n");
		writeProperties(f, indent++);
		INDENT;f->printf("/>\n");

#undef INDENT
	}

}} // namespace Axon::Map

