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

Material::Material()
{
	size_t s = sizeof(Material);
	m_baseTcAnim = false;
	m_shaderMacroNeedRegen = true;
	m_diffuse = Vector3::One;
	m_specular = Vector3::One;
	m_shiness = 10;
	m_detailScale = 20;
	m_haveDetail = false;

	m_p2tEnabled = false;
	m_p2tWidth = 1;
	m_p2tHeight = 1;

	TypeZeroArray(m_features);
	TypeZeroArray(m_literals);
}

Material::~Material()
{
	// TODO: free texture

//		FreeAsset_(m_matfile);
//		FreeAsset_(m_shaderTemplate);
}

bool Material::init(const String &name)
{
	if (!PathUtil::haveDir(name))
		m_key = "materials/" + name;
	else
		m_key = name;

	TypeZeroArray(m_textures);

	m_decl = MaterialDecl::load(m_key);

	if (m_decl->isDefaulted()) {
		setTextureSet(m_key);
	} else {
		for (int i = 0; i < SamplerType::NUMBER_ALL; i++) {
			TextureDef *texdef = m_decl->getTextureDef(i);

			if (!texdef)
				continue;

			m_textures[i] = Texture::load(texdef->file);

			if (!m_textures[i])
				continue;

			if (texdef->clamp && m_textures[i]) {
				m_textures[i]->setClampMode(Texture::CM_ClampToBorder);
			}

			if (texdef->clampToEdge && m_textures[i]) {
				m_textures[i]->setClampMode(Texture::CM_ClampToEdge);
			}
		}
	}

	memcpy(m_features, m_decl->getFeatures(), sizeof(m_features));

	m_shaderMacroNeedRegen = true;

	// copy matfile's properties
	m_diffuse = m_decl->getDiffuse().rgb().toVector();

	ShaderMacro macro = getShaderMacro();
	macro.mergeFrom(&g_shaderMacro);

	m_shaderNameId = getShaderName();
	m_shaderTemplate = g_shaderManager->findShader(m_shaderNameId, macro);

	return true;
}

const String &Material::getShaderName() const
{
	return m_decl->getShaderName();
}

Shader *Material::getShaderTemplate() const
{
	return m_shaderTemplate;
}

Shader *Material::getRealShader() const
{
	return 0;
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

void Material::setParameter(const String &name, int count, const float *ptr)
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

		if (m_textures[SamplerType::Diffuse])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_DIFFUSE);

		if (m_textures[SamplerType::Normal])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_NORMAL);

		if (m_textures[SamplerType::Specular])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_SPECULAR);

		if (m_textures[SamplerType::Detail]) {
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_DETAIL);
			m_haveDetail = true;
		}

		if (m_textures[SamplerType::DetailNormal]) {
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_DETAIL_NORMAL);
			m_haveDetail = true;
		}

		if (m_textures[SamplerType::Emission])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_EMISSION);

		if (m_textures[SamplerType::LayerAlpha])
			m_shaderMacro.setMacro(ShaderMacro::G_HAVE_LAYERALPHA);

		if (m_baseTcAnim) {
			m_shaderMacro.setMacro(ShaderMacro::G_BASETC_ANIM);
		}

//		m_shaderMacro.setMacro(ShaderMacro::G_MAIN_TEXGEN, m_texgens[SamplerType::Diffuse].type);
//		m_shaderMacro.setMacro(ShaderMacro::G_SPECULAR_TEXGEN, m_texgens[SamplerType::Specular].type);
//		m_shaderMacro.setMacro(ShaderMacro::G_NORMAL_TEXGEN, m_texgens[SamplerType::Normal].type);

		for (int i = 0; i < Shader::MAX_FEATURES; i++) {
			if (m_features[i]) {
				m_shaderMacro.setMacro(ShaderMacro::Flag(ShaderMacro::G_FEATURE0 + i));
			}
		}

		for (int i = 0; i < Shader::MAX_LITERALS; i++) {
			if (m_literals[i]) {
				m_shaderMacro.setMacro(ShaderMacro::Flag(ShaderMacro::G_LITERAL0 + i), m_literals[i]);
			}
		}
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

	return m_decl->getFlags().isSet(MaterialDecl::PhysicsHelper);
}

void Material::setTextureSet( const String &texname )
{
	TexturePtr texture = Texture::load(texname);
	m_textures[SamplerType::Diffuse] = texture;

	texture = Texture::load(texname + "_n");
	m_textures[SamplerType::Normal] = texture;

	texture = Texture::load(texname + "_s");
	m_textures[SamplerType::Specular] = texture;

	texture = Texture::load(texname + "_g");
	m_textures[SamplerType::Emission] = texture;

	m_shaderMacroNeedRegen = true;
}

FixedString Material::normalizeKey( const String &name )
{
	if (!PathUtil::haveDir(name))
		return "materials/" + name;
	else
		return name;
}

MaterialRp Material::load(const String &name)
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
	result->setKey(key);
	ms_materialDict[key] = result;

	return result;
}

MaterialRp Material::loadUnique(const String &name)
{
	std::stringstream ss;
	ss << name << "$" << g_system->generateId();
	FixedString key = normalizeKey(name);
	FixedString uniqueKey = normalizeKey(ss.str());

	Material *result = new Material();
	result->init(key);
	result->setKey(uniqueKey);
	ms_materialDict[uniqueKey] = result;
	return result;
}

void Material::initManager()
{

}

void Material::finalizeManager()
{

}

void Material::_deleteMaterial( Material *mat )
{
#if 0
	mat->m_needDeleteLink.addToEnd(ms_needDeleteLinkHead);
#else
	ms_needDeleteLinkHead.push_back(mat);
#endif
}

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
		ms_materialDict.erase(mat->getKey());
		ms_needDeleteLinkHead.erase(oldIt);
		delete mat;
	}
#endif
}

void Material::deleteThis()
{
	_deleteMaterial(this);
}

void Material::matlist_f(const CmdArgs &args)
{
	Printf("List material(s):\n");

	int count = 0;
	MaterialDict::const_iterator it = ms_materialDict.begin();
	for (; it != ms_materialDict.end(); ++it) {
		Material *mtr = it->second;
		if (!mtr) {
			continue;
		}

		Printf("%4d %s\n",mtr->getRefCount(), mtr->m_key.c_str());
		count++;
	}

	Printf("total %d material(s)\n", count);
}

AX_END_NAMESPACE

