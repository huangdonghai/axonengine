/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

Material::Material(const std::string &name)
{
	size_t s = sizeof(Material);
	m_isTexAnim = false;
	m_macroNeedRegen = true;
	m_diffuse.set(1,1,1);
	m_specular.set(1,1,1);
	m_shiness = 10;
	m_detailScale.set(20,20,20,20);

	m_shaderInfo = 0;

	TypeZeroArray(m_features);

	m_name = normalizeKey(name);

	TypeZeroArray(m_textures);

	m_decl = MaterialDecl::load(m_name);

	if (m_decl->isDefaulted()) {
		setTextureSet(m_name);
	} else {
		for (int i = 0; i < MaterialTextureId::MaxType; i++) {
			MaterialDecl::TextureDef *texdef = m_decl->getTextureDef((MaterialTextureId::Type)i);

			if (!texdef)
				continue;

			m_textures[i] = new Texture(texdef->file);
			m_textures[i]->setSamplerState(texdef->desc);
		}
	}

	memcpy(m_features, m_decl->getFeatures(), sizeof(m_features));

	m_macroNeedRegen = true;

	// copy matfile's properties
	m_diffuse = m_decl->m_diffuse;
	m_specular = m_decl->m_specular;
	m_emission = m_decl->m_emission;
	m_shiness = m_decl->m_shiness;
	m_detailScale = m_decl->m_detailScale;

	m_depthWrite = m_decl->m_depthWrite;
	m_depthTest = m_decl->m_depthTest;
	m_cullMode = m_decl->m_cullMode;
	m_wireframed = m_decl->m_wireframed;
	m_blendMode = m_decl->m_blendMode;
	m_sortHint = m_decl->m_sortHint;

	m_shaderName = m_decl->getShaderName();
	m_shaderInfo = g_renderDriver->findShaderInfo(m_shaderName);

	stat_numMaterials.inc();
}

Material::~Material()
{
	stat_numMaterials.dec();

	for (int i = 0; i < ArraySize(m_textures); i++) {
		SafeDelete(m_textures[i]);
	}
}

#if 0
bool Material::init(const FixedString &key)
{

	return true;
}

void Material::enableFeature(const String &name) {
	m_shaderMacroNeedRegen = true;
	m_features.setValue(name, 1);
}

void Material::disableFeature(const String &name) {
	m_shaderMacroNeedRegen = true;
	m_features.setValue(name, 0);
}

void Material::setMacroParameter(const String &name, int value) {
	m_shaderMacroNeedRegen = true;
	m_macroParameters.setValue(name, value);
}
#endif


void Material::clearParameters()
{
	m_shaderParams.clear();
}


void Material::addParameter(const FixedString &name, int count, const float *ptr)
{
	if (!count) {
		count = 1;
	}

	if (!ptr) {
		return;
	}

	m_shaderParams.addParam(name, count, ptr);
}

const FastParams *Material::getParameters() const
{
	return &m_shaderParams;
}


const MaterialMacro &Material::getShaderMacro()
{
	struct MapMacro {
		MaterialTextureId id;
		int macro;
		int layers;
	};

	static MapMacro mapmacros[] = {
		MaterialTextureId::Diffuse, MaterialMacro::M_DIFFUSE, 0,
		MaterialTextureId::Normal, MaterialMacro::M_NORMAL, 0,
		MaterialTextureId::Specular, MaterialMacro::M_SPECULAR, 0,
		MaterialTextureId::Emission, MaterialMacro::M_EMISSION, 0,

		MaterialTextureId::Detail, MaterialMacro::M_DETAIL, 1,
		MaterialTextureId::DetailNormal, MaterialMacro::M_DETAIL_NORMAL, 1,
		MaterialTextureId::LayerAlpha, MaterialMacro::M_LAYERALPHA, 1,

		MaterialTextureId::Detail1, MaterialMacro::M_DETAIL1, 2,
		MaterialTextureId::DetailNormal1, MaterialMacro::M_DETAIL_NORMAL1, 2,
		MaterialTextureId::LayerAlpha1, MaterialMacro::M_LAYERALPHA1, 2,

		MaterialTextureId::Detail2, MaterialMacro::M_DETAIL2, 3,
		MaterialTextureId::DetailNormal2, MaterialMacro::M_DETAIL_NORMAL2, 3,
		MaterialTextureId::LayerAlpha2, MaterialMacro::M_LAYERALPHA2, 3,

		MaterialTextureId::Detail3, MaterialMacro::M_DETAIL3, 4,
		MaterialTextureId::DetailNormal3, MaterialMacro::M_DETAIL_NORMAL3, 4,
		MaterialTextureId::LayerAlpha3, MaterialMacro::M_LAYERALPHA3, 4,
	};

	if (m_macroNeedRegen || r_forceUpdateMaterialMacro.getBool()) {
		m_macroNeedRegen = false;
		int numDetailLayers = 0;

		m_shaderMacro.clear();
#if 0
		if (m_textures[MaterialTextureId::Diffuse])
			m_shaderMacro.setMacro(MaterialMacro::M_DIFFUSE);

		if (m_textures[MaterialTextureId::Normal])
			m_shaderMacro.setMacro(MaterialMacro::M_NORMAL);

		if (m_textures[MaterialTextureId::Specular])
			m_shaderMacro.setMacro(MaterialMacro::M_SPECULAR);

		if (m_textures[MaterialTextureId::Emission])
			m_shaderMacro.setMacro(MaterialMacro::M_EMISSION);

		if (m_textures[MaterialTextureId::Detail]) {
			m_shaderMacro.setMacro(MaterialMacro::M_DETAIL);
		}

		if (m_textures[MaterialTextureId::DetailNormal]) {
			m_shaderMacro.setMacro(MaterialMacro::M_DETAIL_NORMAL);
		}

		if (m_textures[MaterialTextureId::LayerAlpha])
			m_shaderMacro.setMacro(MaterialMacro::M_LAYERALPHA);
#else
		for (int i = 0; i < ArraySize(mapmacros); i++) {
			if (m_textures[mapmacros[i].id]) {
				m_shaderMacro.setMacro(mapmacros[i].macro);
				numDetailLayers = std::max(numDetailLayers, mapmacros[i].layers);
			}
		}
#endif
		m_shaderMacro.setMacro(MaterialMacro::M_NUM_LAYERS, numDetailLayers);

		if (m_isTexAnim) m_shaderMacro.setMacro(MaterialMacro::M_TEXANIM);

		for (int i = 0; i < MAX_FEATURES; i++) {
			if (m_features[i]) {
				m_shaderMacro.setMacro(MaterialMacro::Flag(MaterialMacro::M_FEATURE0 + i));
			}
		}

		m_fastSamplerParams.clear();
		for (int i = 0; i < MaterialTextureId::MaxType; i++) {
			Texture *tex = m_textures[i];
			if (!tex) continue;
			m_fastSamplerParams.addSampler((MaterialTextureId::Type)i, tex->getPHandle(), tex->getSamplerState());
		}
	}

	return m_shaderMacro;
}


void Material::setTexMatrix(const Matrix4 &matrix)
{
	m_macroNeedRegen = true;
	m_isTexAnim = true;
	m_texMatrix = matrix;
}

bool Material::isWireframe() const
{
	if (!m_decl) {
		return false;
	}

	return m_decl->isWireframed();
}

bool Material::isPhysicsHelper() const
{
	if (!m_decl) {
		return true;
	}

	return m_decl->getFlags().isSet(Flag_PhysicsHelper);
}

void Material::setTextureSet( const std::string &texname )
{
	if (Texture::isExist(texname)) {
		m_textures[MaterialTextureId::Diffuse] = new Texture(texname);
	}

	if (Texture::isExist(texname + "_n")) {
		m_textures[MaterialTextureId::Normal] = new Texture(texname + "_n");
	}

	if (Texture::isExist(texname + "_s")) {
		m_textures[MaterialTextureId::Specular] = new Texture(texname + "_s");
	}

	if (Texture::isExist(texname + "_g")) {
		m_textures[MaterialTextureId::Emission] = new Texture(texname + "_g");
	}

	m_macroNeedRegen = true;
}

std::string Material::normalizeKey(const std::string &name)
{
	if (!PathUtil::haveDir(name))
		return "materials/" + name;
	else
		return name;
}

Material * Material::clone() const
{
	Material *result = new Material();
	*result = *this;
	return result;
}

AX_END_NAMESPACE

