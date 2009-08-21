/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon { namespace Render {


	Material::Material() {
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

	Material::~Material() {
		// TODO: free texture

//		FreeAsset_(m_matfile);
//		FreeAsset_(m_shaderTemplate);
	}

	bool Material::doInit(const String& name, intptr_t arg) {
		if (!PathUtil::haveDir(name))
			m_key = "materials/" + name;
		else
			m_key = name;

		TypeZeroArray(m_textures);
		TypeZeroArray(m_texgens);

		m_matfile = FindAsset_<MaterialFile>(m_key);

		if (m_matfile->isDefaulted()) {
			if (arg) {
				const char* parent = (const char*)arg;
				MaterialFilePtr parentmat = FindAsset_<MaterialFile>(String("materials/") + parent);
				if (!parentmat->isDefaulted()) {
					m_matfile = parentmat;
				}
			}
			setTextureSet(m_key);
		} else {
			for (int i = 0; i < SamplerType::NUMBER_ALL; i++) {
				TextureDef* texdef = m_matfile->getTextureDef(i);

				if (!texdef)
					continue;

				m_textures[i] = FindAsset_<Texture>(texdef->file);

				if (texdef->clamp && m_textures[i]) {
					m_textures[i]->setClampMode(Texture::CM_ClampToBorder);
				}

				if (texdef->clampToEdge && m_textures[i]) {
					m_textures[i]->setClampMode(Texture::CM_ClampToEdge);
				}
			}
		}

		memcpy(m_features, m_matfile->getFeatures(), sizeof(m_features));

		m_shaderMacroNeedRegen = true;

		// copy matfile's properties
		m_diffuse = m_matfile->getDiffuse().rgb().toVector();

		ShaderMacro macro = getShaderMacro();
		macro.mergeFrom(&g_shaderMacro);

		m_shaderNameId = getShaderName();
		m_shaderTemplate = g_shaderManager->findShader(m_shaderNameId, macro);

		return true;
	}


	String Material::getKey() const {
		return m_key;
	}

	void Material::setKey(const String& newkey) {
		m_key = newkey;
	}

	int Material::getType() const { return AssetType; }

	const String& Material::getShaderName() const {
		return m_matfile->getShaderName();
	}

	Shader* Material::getShaderTemplate() const {
		return m_shaderTemplate;
	}

	Shader* Material::getRealShader() const {
		return 0;
	}

#if 0
	void Material::enableFeature(const String& name) {
		m_shaderMacroNeedRegen = true;
		m_features.setValue(name, 1);
	}

	void Material::disableFeature(const String& name) {
		m_shaderMacroNeedRegen = true;
		m_features.setValue(name, 0);
	}

	void Material::setMacroParameter(const String& name, int value) {
		m_shaderMacroNeedRegen = true;
		m_macroParameters.setValue(name, value);
	}
#endif

	void Material::setParameter(const String& name, int count, const float* ptr) {
		if (!count) {
			count = 1;
		}

		if (!ptr) {
			return;
		}

		FloatSeq& value = m_shaderParams[name];
		value.resize(count);
		::memcpy(&value[0], ptr, count * sizeof(float));
	}

	const ShaderParams& Material::getParameters() const {
		return m_shaderParams;
	}


	const ShaderMacro& Material::getShaderMacro() {
		if (m_shaderMacroNeedRegen  || r_forceUpdateMaterialMacro->getBool()) {
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

			m_shaderMacro.setMacro(ShaderMacro::G_MAIN_TEXGEN, m_texgens[SamplerType::Diffuse].type);
//			m_shaderMacro.setMacro(ShaderMacro::G_SPECULAR_TEXGEN, m_texgens[SamplerType::Specular].type);
//			m_shaderMacro.setMacro(ShaderMacro::G_NORMAL_TEXGEN, m_texgens[SamplerType::Normal].type);

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


	void Material::setBaseTcMatrix(const Matrix4& matrix) {
		m_shaderMacroNeedRegen = true;
		m_baseTcAnim = true;
		m_baseTcMatrix = matrix;
	}

	void Material::setTexGen(SamplerType st, const TexGen& texgen) {
		m_shaderMacroNeedRegen = true;
		m_texgens[st] = texgen;
	}

	const TexGen& Material::getTexGen(SamplerType st) const {
		return m_texgens[st];
	}

	bool Material::isWireframe() const
	{
		if (!m_matfile) {
			return false;
		}

		return m_matfile->isWireframed();
	}

	bool Material::isPhysicsHelper() const
	{
		if (!m_matfile) {
			return true;
		}

		return m_matfile->getFlags().isSet(MaterialFile::PhysicsHelper);
	}

	void Material::setTextureSet( const String& texname )
	{
		TexturePtr texture = FindAsset_<Texture>(texname);
		if (!texture->isDefaulted())
			m_textures[SamplerType::Diffuse] = texture;

		texture = FindAsset_<Texture>(texname + "_n");
		if (!texture->isDefaulted())
			m_textures[SamplerType::Normal] = texture;

		texture = FindAsset_<Texture>(texname + "_s");
		if (!texture->isDefaulted())
			m_textures[SamplerType::Specular] = texture;

		texture = FindAsset_<Texture>(texname + "_g");
		if (!texture->isDefaulted())
			m_textures[SamplerType::Emission] = texture;

		m_shaderMacroNeedRegen = true;
	}

	String MaterialFactory::generateKey(const String& name, intptr_t arg) {
		if (!PathUtil::haveDir(name))
			return "materials/" + name;
		else
			return name;
	}

}} // namespace Axon::Render

