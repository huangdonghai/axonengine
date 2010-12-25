/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

// static member
Material::MaterialDict Material::ms_materialDict;
IntrusiveList<Material, &Material::m_needDeleteLink> ms_needDeleteLinkHead;

Material::Material(const std::string &name)
{
	size_t s = sizeof(Material);
	m_baseTcAnim = false;
	m_shaderMacroNeedRegen = true;
	m_diffuse = Vector3::One;
	m_specular = Vector3::One;
	m_shiness = 10;
	m_detailScale = 20;
	m_haveDetail = false;

	m_shaderInfo = 0;

#if 0
	m_p2tEnabled = false;
	m_p2tWidth = 1;
	m_p2tHeight = 1;
#endif
	TypeZeroArray(m_features);
#if 0
	TypeZeroArray(m_literals);
#endif
	FixedString key = normalizeKey(name);
	init(key);
}

Material::~Material()
{
}

bool Material::init(const FixedString &key)
{
	m_key = key;

	TypeZeroArray(m_textures);

	m_decl = MaterialDecl::load(m_key);

	if (m_decl->isDefaulted()) {
		setTextureSet(m_key);
	} else {
		for (int i = 0; i < MaterialTextureId::MaxType; i++) {
			TextureDef *texdef = m_decl->getTextureDef(i);

			if (!texdef)
				continue;

			m_textures[i] = new Texture(texdef->file);

			if (!m_textures[i])
				continue;

			SamplerDesc desc = m_textures[i]->getSamplerState();

			if (texdef->clampToBorder && m_textures[i]) {
				desc.clampMode = SamplerDesc::ClampMode_Border;
				m_textures[i]->setSamplerState(desc);
			}

			if (texdef->clampToEdge && m_textures[i]) {
				desc.clampMode = SamplerDesc::ClampMode_Clamp;
				m_textures[i]->setSamplerState(desc);
			}
		}
	}

	memcpy(m_features, m_decl->getFeatures(), sizeof(m_features));

	m_shaderMacroNeedRegen = true;

	// copy matfile's properties
	m_diffuse = m_decl->getDiffuse().rgb().toVector();

	ShaderMacro macro = getShaderMacro();
	macro.mergeFrom(&g_shaderMacro);

	m_shaderName = m_decl->getShaderName();
	m_shaderInfo = g_renderDriver->findShaderInfo(m_shaderName);

	m_renderStateId = m_decl->m_renderStateId;

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

void Material::setParameter(const std::string &name, int count, const float *ptr)
{
	if (!count) {
		count = 1;
	}

	if (!ptr) {
		return;
	}

	FloatSeq &value = m_shaderParams[name];
	value.resize(count);
	::memcpy(&value[0], ptr, count * sizeof(float));
}

const ShaderParams &Material::getParameters() const
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

		if (m_baseTcAnim) {
			m_shaderMacro.setMacro(ShaderMacro::G_BASETC_ANIM);
		}

//		m_shaderMacro.setMacro(ShaderMacro::G_MAIN_TEXGEN, m_texgens[SamplerType::Diffuse].type);
//		m_shaderMacro.setMacro(ShaderMacro::G_SPECULAR_TEXGEN, m_texgens[SamplerType::Specular].type);
//		m_shaderMacro.setMacro(ShaderMacro::G_NORMAL_TEXGEN, m_texgens[SamplerType::Normal].type);

		for (int i = 0; i < MaterialDecl::MAX_FEATURES; i++) {
			if (m_features[i]) {
				m_shaderMacro.setMacro(ShaderMacro::Flag(ShaderMacro::G_FEATURE0 + i));
			}
		}
#if 0
		for (int i = 0; i < MaterialDecl::MAX_LITERALS; i++) {
			if (m_literals[i]) {
				m_shaderMacro.setMacro(ShaderMacro::Flag(ShaderMacro::G_LITERAL0 + i), m_literals[i]);
			}
		}
#endif
	}

	return m_shaderMacro;
}


void Material::setBaseTcMatrix(const Matrix4 &matrix)
{
	m_shaderMacroNeedRegen = true;
	m_baseTcAnim = true;
	m_baseTcMatrix = matrix;
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

	return m_decl->getFlags().isSet(MaterialDecl::Flag_PhysicsHelper);
}

void Material::setTextureSet( const std::string &texname )
{
	Texture *texture = new Texture(texname);
	m_textures[MaterialTextureId::Diffuse] = texture;

	texture = new Texture(texname + "_n");
	m_textures[MaterialTextureId::Normal] = texture;

	texture = new Texture(texname + "_s");
	m_textures[MaterialTextureId::Specular] = texture;

	texture = new Texture(texname + "_g");
	m_textures[MaterialTextureId::Emission] = texture;

	m_shaderMacroNeedRegen = true;
}

FixedString Material::normalizeKey(const std::string &name)
{
	if (!PathUtil::haveDir(name))
		return "materials/" + name;
	else
		return name;
}

#if 0
Material *new Material(const String &name)
{
	FixedString key = normalizeKey(name);
	MaterialDict::const_iterator it = ms_materialDict.find(key);

	if (it != ms_materialDict.end()) {
		Material *mat = it->second;
		if (mat->m_needDeleteLink.isLinked())
			ms_needDeleteLinkHead.erase(mat);
		mat->incref();
		return mat;
	}

	Material *result = new Material();
	result->init(key);
	ms_materialDict[key] = result;

	return result;
}

Material *Material::loadUnique(const String &name)
{
	std::stringstream ss;
	ss << name << "$" << g_system->generateId();
	FixedString key = normalizeKey(name);
	FixedString uniqueKey = normalizeKey(ss.str());

	Material *result = new Material();
	result->init(key);
	result->m_key = uniqueKey;
	ms_materialDict[uniqueKey] = result;
	return result;
}

void Material::initManager()
{

}

void Material::finalizeManager()
{

}
#endif

void Material::_deleteMaterial( Material *mat )
{
#if 0
	mat->m_needDeleteLink.addToEnd(ms_needDeleteLinkHead);
#else
	ms_needDeleteLinkHead.push_back(mat);
#endif
}

#if 0
void Material::syncFrame()
{
#if 0
	Link<Material>* node = ms_needDeleteLinkHead.getNextNode();

	while (node) {
		Material *owner = node->getOwner();
		Link<Material>* next = node->getNextNode();

		node->removeFromList();
		ms_materialDict.erase(owner->getKey());
		delete owner;

		node = next;
	}

	ms_needDeleteLinkHead.clearList();
#else
	IntrusiveList<Material, &Material::m_needDeleteLink>::iterator it = ms_needDeleteLinkHead.begin();

	while (it != ms_needDeleteLinkHead.end()) {
		IntrusiveList<Material, &Material::m_needDeleteLink>::iterator oldIt = it;
		++it;
		Material *mat = &*oldIt;
		ms_materialDict.erase(mat->m_key);
		ms_needDeleteLinkHead.erase(oldIt);
		delete mat;
	}
#endif
}
#endif

void Material::deleteThis()
{
	_deleteMaterial(this);
}

void Material::matlist_f(const CmdArgs &args)
{
	Printf("std::list material(s):\n");

	int count = 0;
	MaterialDict::const_iterator it = ms_materialDict.begin();
	for (; it != ms_materialDict.end(); ++it) {
		Material *mtr = it->second;
		if (!mtr) {
			continue;
		}

		Printf("%s\n", mtr->m_key.c_str());
		count++;
	}

	Printf("total %d material(s)\n", count);
}

AX_END_NAMESPACE

