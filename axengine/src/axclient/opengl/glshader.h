/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef GL_SHADER_H
#define GL_SHADER_H

#define AX_SU(x, y) g_uniforms.getItem(Uniforms::x).set(y)

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class GLuniform
//--------------------------------------------------------------------------

class GLuniform : public UniformItem {
public:
	GLuniform(const UniformItem &item, CGparameter param);
	virtual ~GLuniform();

	bool isCached() const;
	void cache();

public:
	const UniformItem *m_src;
	CGparameter m_param;
};

inline GLuniform::GLuniform(const UniformItem &item, CGparameter param)
	: UniformItem(item)
{
	m_src = &item;
	m_param = param;
}

inline GLuniform::~GLuniform()
{

}

inline bool GLuniform::isCached() const {
	return ::memcmp(m_datap, m_src->m_datap, m_dataSize) == 0;
}

inline void GLuniform::cache() {
	::memcpy(m_datap, m_src->m_datap, m_dataSize);
}

//--------------------------------------------------------------------------
// class CGsamplerann
//--------------------------------------------------------------------------

class CGsamplerann : public SamplerInfo {
public:
	CGparameter m_param;
};

typedef Sequence<CGsamplerann*>	CGsamplerannSeq;

class CGpixeltotexel {
public:
	CGparameter m_param;
	FloatSeq m_pixelValue;
};
typedef Sequence<CGpixeltotexel>	CGpixeltotexelSeq;


//--------------------------------------------------------------------------
// class GLshader
//--------------------------------------------------------------------------

class GLshader : public Shader {
public:
	GLshader();
	virtual ~GLshader();

	// implement Shader
	virtual bool doInit(const String &name, const ShaderMacro &macro = g_shaderMacro);
	virtual bool isDepthWrite() const;
	virtual bool haveTextureTarget() const;
	virtual int getNumSampler() const;
	virtual SamplerInfo *getSamplerAnno(int index) const;
	virtual int getNumTweakable() const;
	virtual ParameterInfo *getTweakableDef(int index);
	virtual const StringSeq &getFeatures() const;
	virtual const StringSeq &getFeatureNames() const { return m_featureNames; }
	virtual const StringSeq &getFeatureDescs() const { return m_featureDescs; }
	virtual const StringSeq &getMacroParameters() const;
	virtual SortHint getSortHint() const { return m_sortHint; }
	virtual bool haveTechnique(Technique tech) const { return m_techniques[tech] != 0; }
	virtual const ShaderInfo *getShaderInfo() const { return 0; }

	void setSystemMap(SamplerType maptype, GLtexture *tex);
	void setInteractionMap();
	void setInteractionParameter(const ShaderParams &params);

	void setSU();

	void draw();
	CGpass getFirstPass(Technique technique);

	// direct set parameters
	bool setParameter(const char *name, float v);
	bool setParameter(const char *name, const Vector2 &v);
	bool setParameter(const char *name, const Vector3 &v);
	bool setParameter(const char *name, const Vector4 &v);
	bool setParameter(const char *name, const Matrix4 &v);

	bool setParameter(const char *name, const float *v, int len);
	bool setParameter(const char *name, const Vector2 v[], int len);
	bool setParameter(const char *name, const Vector3 v[], int len);
	bool setParameter(const char *name, const Vector4 v[], int len);
	bool setParameter(const char *name, const Matrix4 v[], int len);

	bool setParameter(const char *name, GLtexture *texture);

	// set pixel to texel conversion paramter
	void setPixelToTexel(int width, int height);

	CGtechnique getTechnique(const char *name);

protected:
	CGtechnique getSystemTechnique(Technique tech);
	CGparameter getEffectParameter(const char *name);
	void initFeatures();
	void initMacroParameters();
	void initSortHint();
	void initUniforms();

	void initAnnotation();
	void initSamplerAnn(CGparameter param);
	void initParameterAnn(CGparameter param);
	void initPixelToTexel(CGparameter param);
	void setUniformCache(GLuniform *uc);

private:
	String m_name;
	CGeffect m_cgfx;
	CGtechnique m_techniques[Technique::Number];
	CGparameter m_sysSamplers[SamplerType::NUMBER_ALL];
	GLuint m_samplerBound[SamplerType::NUMBER_ALL];

	SortHint m_sortHint;
	CGsamplerannSeq m_samplerannSeq;
	StringSeq m_features;
	StringSeq m_featureNames;
	StringSeq m_featureDescs;
	StringSeq m_macroParameters;

	bool m_haveTextureTarget;

	int m_numUniformCaches;
	GLuniform *m_uniformCaches[Uniforms::NUM_UNIFORM_ITEMS];

	// pixel2texel
	CGpixeltotexelSeq pixel2Texels;
};

//--------------------------------------------------------------------------
// class GLshadermanager
//--------------------------------------------------------------------------

class GLshadermanager : public ShaderManager {
public:
	GLshadermanager();
	virtual ~GLshadermanager();

	virtual Shader*findShader(const String &name, const ShaderMacro &macro = g_shaderMacro);
	virtual Shader *findShader(const FixedString &nameId, const ShaderMacro &macro);
	virtual void saveShaderCache(const String &name);
	virtual void applyShaderCache(const String &name);

	GLshader *findShaderGL(const String &name, const ShaderMacro &macro = g_shaderMacro);

private:
	typedef Dict<String,Dict<ShaderMacro,GLshader*> > ShaderPool;
	ShaderPool m_shaderPool;
	GLshader *m_defaulted;

public:
	static CGstate m_textureState;
};

AX_END_NAMESPACE

#endif // end guardian

