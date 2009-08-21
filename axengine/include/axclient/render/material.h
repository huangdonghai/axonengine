/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_MATERIAL_H
#define AX_RENDER_MATERIAL_H

namespace Axon { namespace Render {

	class AX_API Material : public Asset {
	public:
		friend class AssetFactory_<Material>;

		enum { AssetType = Asset::kMaterial };

		// implement Asset
		virtual bool doInit(const String& name, intptr_t arg);
		virtual String getKey() const;
		virtual void setKey(const String& newkey);
		virtual int getType() const;

		const String& getShaderName() const;
		Shader* getShaderTemplate() const;
		const FixedString& getShaderNameId() const { return m_shaderNameId; }
		Shader* getRealShader() const;
		bool isWireframe() const;
		bool isPhysicsHelper() const;

		void setTextureSet(const String& texname);

		void setDiffuse(const Vector3& v);
		void setDiffuse(Rgb rgb, float intensity = 1.0f) { m_diffuse = rgb.toVector() * intensity; }
		void setSpecular(const Vector3& v);
		void setSpecular(Rgb rgb, float intensity = 1.0f) { m_specular = rgb.toVector() * intensity; }
		void setShiness(float shiness) { m_shiness = shiness; }
		void setDetailScale(float scale) { m_detailScale = scale; }
		float getDetailScale() const { return m_detailScale; }
		bool haveDetail() const { return m_haveDetail; }

		Vector3 getMatDiffuse() const;
		Vector3 getMatSpecular() const;
		float getMatShiness() const;

		// features and shader parameter
		void setFeature(int index, bool enabled);
		bool isFeatureEnabled(int index) const;
		void setLiteral(int index, int value);
		int getLiteral(int index) const;
		void clearFeatures();
		void clearLiterals();


		// texture setting and getting
		Texture* getTexture(int sample) const;
		void setTexture(int sampler, Texture* tex);

		// parameter setting and getting
		void setParameter(const String& name, int num, const float* ptr);
		const ShaderParams& getParameters() const;

		// shader macro
		const ShaderMacro& getShaderMacro();

		void setBaseTcMatrix(const Matrix4& matrix);
		bool isBaseTcAnim() const { return m_baseTcAnim; }
		const Matrix4* getBaseTcMatrix() const { return &m_baseTcMatrix; }

		void setTexGen(SamplerType st, const TexGen& texgen);
		const TexGen& getTexGen(SamplerType st) const;

		// pixel to texel
		void setPixelToTexel(int width, int height);
		bool isPixelToTexelEnabled() const { return m_p2tEnabled; }
		int getPixelToTexelWidth() const { return m_p2tWidth; }
		int getPixelToTexelHeight() const { return m_p2tHeight; }

	private:
		Material();
		virtual ~Material();

		String m_key;
		MaterialFilePtr m_matfile;

		bool m_shaderMacroNeedRegen;
		ShaderMacro m_shaderMacro;

		Vector3 m_diffuse;
		Vector3 m_specular;
		float m_shiness;
		float m_detailScale;
		bool m_haveDetail;

		Shader* m_shaderTemplate;
		FixedString m_shaderNameId;
		TexturePtr m_textures[SamplerType::NUMBER_ALL];
		TexGen m_texgens[SamplerType::NUMBER_ALL];
		ShaderParams m_shaderParams;

		// texgen etc...
		bool m_baseTcAnim;
		Matrix4 m_baseTcMatrix;
		RgbGen m_rgbGen;
		AlphaGen m_alphaGen;
		VertexDeform m_deform;

		bool m_features[Shader::MAX_FEATURES];
		int m_literals[Shader::MAX_LITERALS];

		// pixel to texel conversion
		bool m_p2tEnabled;
		int m_p2tWidth, m_p2tHeight;
	};

	class MaterialFactory: public AssetFactory_<Material> {
	public:
		virtual String generateKey(const String& name, intptr_t arg);
	};

	inline void splitVectorToColor(const Vector3& v, float& multiply, Rgb& color) {
		multiply = v.getMax();
		if (multiply < 1e-6) {
			color.fromVector(Vector3(0,0,0));
		} else {
			color.fromVector(v / multiply);
		}
	}

	inline void Material::setDiffuse(const Vector3& v) {
		m_diffuse = v;
	}

	inline void Material::setSpecular(const Vector3& v) {
		m_specular = v;
	}


	inline Vector3 Material::getMatDiffuse() const {
		return m_diffuse;
	}

	inline Vector3 Material::getMatSpecular() const {
		return m_specular;
	}

	inline float Material::getMatShiness() const {
		return m_shiness;
	}

	inline void Material::setFeature(int index, bool enabled) {
		AX_ASSERT(index >= 0 && index < Shader::MAX_FEATURES);
		if (m_features[index] != enabled) {
			m_shaderMacroNeedRegen = true;
			m_features[index] = enabled;
		}
	}

	inline bool Material::isFeatureEnabled(int index) const {
		AX_ASSERT(index >= 0 && index < Shader::MAX_FEATURES);
		return m_features[index];
	}

	inline void Material::setLiteral(int index, int value) {
		AX_ASSERT(index >= 0 && index < Shader::MAX_LITERALS);
		if (m_literals[index] != value) {
			m_literals[index] = value;
			m_shaderMacroNeedRegen = true;
		}
	}
	inline int Material::getLiteral(int index) const {
		AX_ASSERT(index >= 0 && index < Shader::MAX_LITERALS);
		return m_literals[index];
	}

	inline void Material::clearFeatures() {
		m_shaderMacroNeedRegen = true;
		TypeZeroArray(m_features);
	}

	inline void Material::clearLiterals() {
		m_shaderMacroNeedRegen = true;
		TypeZeroArray(m_literals);
	}


	inline void Material::setPixelToTexel(int width, int height) {
		m_p2tEnabled = true;
		m_p2tWidth = width;
		m_p2tHeight = height;
	}

	inline Texture* Material::getTexture(int sampler) const {
		AX_ASSERT(sampler >= 0 && sampler < SamplerType::NUMBER_ALL);
		return m_textures[sampler].get();
	}

	inline void Material::setTexture(int sampler, Texture* tex) {
		m_shaderMacroNeedRegen = true;
		AX_ASSERT(sampler >= 0 && sampler < SamplerType::NUMBER_ALL);
		m_textures[sampler] = tex;
	}


	AX_DECLARE_REFPTR(Material);

}} // namespace Axon::Render

#endif // AX_MATERIAL_H
