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
	m_shaderMacroNeedRegen = true;
	m_diffuse.set(1,1,1);
	m_specular.set(1,1,1);
	m_shiness = 10;
	m_detailScale = 20;
	m_haveDetail = false;

	m_shaderInfo = 0;

	TypeZeroArray(m_features);

	m_name = normalizeKey(name);

	m_shaderParams = 0;

	TypeZeroArray(m_textures);

	m_decl = MaterialDecl::load(m_name);

	if (m_decl->isDefaulted()) {
		setTextureSet(m_name);
	} else {
		for (int i = 0; i < MaterialTextureId::MaxType; i++) {
			MaterialDecl::TextureDef *texdef = m_decl->getTextureDef(i);

			if (!texdef)
				continue;

			m_textures[i] = new Texture(texdef->file);
			m_textures[i]->setSamplerState(texdef->desc);
		}
	}

	memcpy(m_features, m_decl->getFeatures(), sizeof(m_features));

	m_shaderMacroNeedRegen = true;

	// copy matfile's properties
	m_diffuse = m_decl->m_diffuse;
	m_specular = m_decl->m_specular;
	m_emission = m_decl->m_emission;
	m_shiness = m_decl->m_shiness;
	m_detailScale = m_decl->m_detailScale;

	m_depthWrite = m_decl->m_depthWrite;
	m_depthTest = m_decl->m_depthTest;
	m_twoSided = m_decl->m_twoSided;
	m_wireframed = m_decl->m_wireframed;
	m_blendMode = m_decl->m_blendMode;
	m_sortHint = m_decl->m_sortHint;

	m_shaderName = m_decl->getShaderName();
	m_shaderInfo = g_renderDriver->findShaderInfo(m_shaderName);
}

Material::~Material()
{
	SafeDelete(m_shaderParams);

	for (int i = 0; i < ArraySize(m_textures); i++) {
		SafeDelete(m_textures[i]);
	}
}

bool Material::init(const FixedString &key)
{

	return true;
}

#if 0
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
	if (m_shaderParams)
		m_shaderParams->clear();
}


void Material::addParameter(const FixedString &name, int count, const float *ptr)
{
	if (!count) {
		count = 1;
	}

	if (!ptr) {
		return;
	}

	if (!m_shaderParams)
		m_shaderParams = new FastParams();

	m_shaderParams->addParam(name, count, ptr);
}

const FastParams *Material::getParameters() const
{
	return m_shaderParams;
}


const ShaderMacro &Material::getShaderMacro()
{
	if (m_shaderMacroNeedRegen  || r_forceUpdateMaterialMacro.getBool()) {
		m_shaderMacroNeedRegen = false;
		m_haveDetail = false;

		m_shaderMacro.clear();

		if (m_textures[MaterialTextureId::Diffuse])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_DIFFUSE);

		if (m_textures[MaterialTextureId::Normal])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_NORMAL);

		if (m_textures[MaterialTextureId::Specular])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_SPECULAR);

		if (m_textures[MaterialTextureId::Detail]) {
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_DETAIL);
			m_haveDetail = true;
		}

		if (m_textures[MaterialTextureId::DetailNormal]) {
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_DETAIL_NORMAL);
			m_haveDetail = true;
		}

		if (m_textures[MaterialTextureId::Emission])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_EMISSION);

		if (m_textures[MaterialTextureId::LayerAlpha])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_LAYERALPHA);

		if (m_isTexAnim) {
			m_shaderMacro.setMacro(ShaderMacro::G_TEXANIM);
		}

//		m_shaderMacro.setMacro(ShaderMacro::G_MAIN_TEXGEN, m_texgens[SamplerType::Diffuse].type);
//		m_shaderMacro.setMacro(ShaderMacro::G_SPECULAR_TEXGEN, m_texgens[SamplerType::Specular].type);
//		m_shaderMacro.setMacro(ShaderMacro::G_NORMAL_TEXGEN, m_texgens[SamplerType::Normal].type);

		for (int i = 0; i < MAX_FEATURES; i++) {
			if (m_features[i]) {
				m_shaderMacro.setMacro(ShaderMacro::Flag(ShaderMacro::G_FEATURE0 + i));
			}
		}
	}

	return m_shaderMacro;
}


void Material::setTexMatrix(const Matrix4 &matrix)
{
	m_shaderMacroNeedRegen = true;
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

	m_shaderMacroNeedRegen = true;
}

std::string Material::normalizeKey(const std::string &name)
{
	if (!PathUtil::haveDir(name))
		return "materials/" + name;
	else
		return name;
}

AX_END_NAMESPACE

