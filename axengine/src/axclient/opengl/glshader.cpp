/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"


namespace { namespace Internal {


	inline bool isSamplerType(CGtype type) {
		CGparameterclass pc = cgGetTypeClass( type);

		return pc == CG_PARAMETERCLASS_SAMPLER;
	}

	inline int getAnnoInt(CGparameter param, const char* name, int defaultValue) {
		CGannotation anno = cgGetNamedParameterAnnotation(param, name);

		if (!anno) {
			return defaultValue;
		}

		CGtype type = cgGetAnnotationType(anno);

		// we need int type
		if (type != CG_INT) {
			return defaultValue;
		}

		int nvalues;

		const int* values = cgGetIntAnnotationValues(anno, &nvalues);
		if (!nvalues) {
			return defaultValue;
		}

		return *values;
	}

	inline int getAnnoInt(CGparameter param, const char* annoname) {
		return getAnnoInt(param, annoname, 0);

		CGannotation anno = cgGetNamedParameterAnnotation(param, annoname);

		if (!anno) {
			return 0;
		}

		CGtype type = cgGetAnnotationType(anno);

		// we need int type
		if (type != CG_INT) {
			return 0;
		}

		int nvalues;

		const int* values = cgGetIntAnnotationValues(anno, &nvalues);
		if (!nvalues) {
			return 0;
		}

		return *values;
	}

	inline const char* getAnnoString(CGparameter param, const char* annoname) {
		const char* result = "";

		CGannotation anno = cgGetNamedParameterAnnotation(param, annoname);

		if (!anno) {
			return result;
		}

		CGtype type = cgGetAnnotationType(anno);

		// we need int type
		if (type != CG_STRING) {
			return result;
		}

		result = cgGetStringAnnotationValue(anno);
		return result;
	}

}}


namespace Axon { namespace Render {

#if 0
	ShaderUniform gUniform;
#endif

	//--------------------------------------------------------------------------
	// class GLshader
	//--------------------------------------------------------------------------

	GLshader::GLshader() {
		TypeZeroArray(m_sysSamplers);
		TypeZeroArray(m_samplerBound);
		m_haveTextureTarget = false;
		m_sortHint = SortHint_opacit;
	}

	GLshader::~GLshader() {
//		disconnectSharedParameter();
		cgDestroyEffect(m_cgfx);
	}

	bool GLshader::doInit(const String& name, const ShaderMacro& macro) {
		m_name = name;

		String fullname = "shaders/" + name + ".fx";
		fullname = g_fileSystem->dataPathToOsPath(fullname);

		String path = g_fileSystem->dataPathToOsPath("shaders/");
		path = "-I" + path;

		Sequence<const char*> args;

		args.push_back(path.c_str());
#if 0
		args.push_back("-fastmath");
		args.push_back("-fastprecision");
		args.push_back("-O3");
#endif
		StringSeq defs;
			defs = macro.getCgDefine();
			for (size_t i = 0; i < defs.size(); i++) {
				args.push_back(defs[i].c_str());
			}

		args.push_back(0);

		m_cgfx = cgCreateEffectFromFile(glCgContext, fullname.c_str(), &args[0]);
		GLrender::checkForCgError(fullname.c_str());

		if (!m_cgfx)
			return false;

		for (int i = 0; i < Technique::Number; i++) {
			m_techniques[i] = getSystemTechnique(i);
		}

		// cache system samplers
		static struct SamplerParam {
			SamplerType type;
			const char* paramname;
		} samplername[] = {
			SamplerType::Diffuse, "g_diffuseMap",
			SamplerType::Normal, "g_normalMap",
			SamplerType::Specular, "g_specularMap",
			SamplerType::Detail, "g_detailMap",
			SamplerType::DetailNormal, "g_detailNormalMap",
			SamplerType::Opacit, "g_opacitMap",
			SamplerType::Emission, "g_emissionMap",
			SamplerType::Displacement, "g_displacementMap",
			SamplerType::Envmap, "g_envMap",
			SamplerType::Custom1, "g_customMap1",
			SamplerType::Custom2, "g_customMap2",

			// engine sampler
			SamplerType::TerrainColor, "g_terrainColor",
			SamplerType::TerrainNormal, "g_terrainNormal",
			SamplerType::LayerAlpha, "g_layerAlpha",
		};

		for (size_t i = 0; i < ArraySize(samplername); i++) {
			CGparameter param = getEffectParameter(samplername[i].paramname);
#if 0
			if (!param) {
				continue;
			}
#endif
			m_sysSamplers[samplername[i].type] = param;
		}

		GLrender::checkForCgError(fullname.c_str());

		// connect shared parameters
//		connectSharedParameter();

		// init annotations
		initAnnotation();

		// init shader features
		initFeatures();

		// init macro parameters
		initMacroParameters();

		// init sort hint
		initSortHint();

		// init uniforms
		initUniforms();

		GLrender::checkForCgError(fullname.c_str());

		return true;
	}

	bool GLshader::isDepthWrite() const {
		return true;
	}

	int GLshader::getNumSampler() const {
		return s2i(m_samplerannSeq.size());
	}

	SamplerAnno* GLshader::getSamplerAnno(int index) const {
		AX_ASSERT(index >= 0 && index < getNumSampler());
		return m_samplerannSeq[index];
	}

	int GLshader::getNumTweakable() const {
		return 0;
	}

	ParameterAnno* GLshader::getTweakableDef(int index) {
		return nullptr;
	}

	CGtechnique GLshader::getSystemTechnique(Technique tech) {
		CGparameter script = cgGetNamedEffectParameter(m_cgfx, "Script");

		if (!script) return nullptr;

		CGannotation annon = cgGetNamedParameterAnnotation(script, tech.toString().c_str());
		if (!annon) return nullptr;

		const char* techname = cgGetStringAnnotationValue(annon);
		if (!techname || !techname[0]) return nullptr;

		CGtechnique cgtech = cgGetNamedTechnique(m_cgfx, techname);

		if (cgtech) {
			CGbool v = cgValidateTechnique(cgtech);
			if (!v) {
				Errorf("%s:%d: GLshader::getSystemTechnique: technique '%s' cann't be validated\n", __FILE__, __LINE__, cgGetTechniqueName(cgtech));
			}
			GLrender::checkForCgError();
		}
		return cgtech;
	}


	void GLshader::setSystemMap(SamplerType maptype, GLtexture* tex) {
		if (r_nulldraw->getBool()) {
			return;
		}

		AX_ASSERT(maptype >= 0 && maptype < SamplerType::NUMBER_ALL);

		if (m_sysSamplers[maptype] == 0) {
			g_statistic->incValue(stat_shaderParamNotused);
			return;
		}

		if (tex == nullptr)
			return;

		GLuint obj = tex->getObject();

		if (m_samplerBound[maptype] == obj) {
			g_statistic->incValue(stat_shaderParamCached);
			return;
		}

		g_statistic->incValue(stat_shaderParamUpdated);

		cgGLSetTextureParameter(m_sysSamplers[maptype], obj);
		m_samplerBound[maptype] = obj;
	}

	void GLshader::setInteractionMap() {
		if (r_nulldraw->getBool()) {
			return;
		}

		int count = 0;
		AX_FOREACH(CGsamplerann* sa, m_samplerannSeq) {
			if (sa->m_renderType == SamplerAnno::Reflection) {
				if (!gCurInteraction) {
					continue;
				}

				if (gCurInteraction->numTargets < count) {
					continue;
				}

				Interaction* ia = gCurInteraction;

				Target* target = ia->targets[count];
				GLtarget* textarget = (GLtarget*)target;
				GLtexture* tex = textarget->getTextureGL();

				cgGLSetupSampler(sa->m_param, tex->getObject());
			} else if (sa->m_renderType == SamplerAnno::SceneColor) {
				if (!gWorldFramebuffer) {
					continue;
				}
#if 1
				Target* target = gWorldFramebuffer->allocTarget(Target::PermanentAlloc, TexFormat::RGBA8);
				GLtarget* textarget = (GLtarget*)target;
				GLtexture* tex = textarget->getTextureGL();
#if 0
				gWorldFramebuffer->blitColor(tex);
#else
				tex->copyFramebuffer(target->getRect());
#endif
#else
				GLtexture* tex = gFramebuffer->getBoundColor();
#endif
				cgGLSetupSampler(sa->m_param, tex->getObject());
#if 0
			} else if (sa->m_renderType == SamplerAnno::Zbuffer) {
//				if (gWorldFramebuffer) {
//					GLtexture* tex = gWorldFramebuffer->getBoundDepth();
					GLtexture* tex = gLinearDepth->getTextureGL();
					tex->setDepthTextureMode(GL_LUMINANCE);
					tex->setHardwareShadowMap(false);
					cgGLSetupSampler(sa->m_param, tex->getObject());
//				}
#endif
			}
		}
	}

	void GLshader::setInteractionParameter(const ShaderParams& params) {
		ShaderParams::const_iterator it = params.begin();

		for (; it != params.end(); ++it) {
			const String& n = it->first;
			const FloatSeq& v = it->second;

			setParameter(n.c_str(), &v[0], s2i(v.size()));
		}
	}

	CGpass GLshader::getFirstPass(Technique technique) {
		if (m_techniques[technique])
			return cgGetFirstPass(m_techniques[technique]);

		return 0;
	}

	bool GLshader::setParameter(const char* name, float v) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameter1f(param, v);

		return true;
	}

	bool GLshader::setParameter(const char* name, const Vector2& v) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameter2fv(param, v.toFloatPointer());

		return true;
	}

	bool GLshader::setParameter(const char* name, const Vector3& v) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameter3fv(param, v);

		return true;
	}

	bool GLshader::setParameter(const char* name, const Vector4& v) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameter4fv(param, v);

		return true;
	}

	bool GLshader::setParameter(const char* name, const Matrix4& v) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetMatrixParameterfc(param, v.toFloatPointer());

		return true;
	}

	bool GLshader::setParameter(const char* name, const float* v, int len) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameterValuefc(param, len, v);
		return true;
	}

	bool GLshader::setParameter(const char* name, const Vector2 v[], int len) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameterValuefc(param, len * 2, v[0].toFloatPointer());
		return true;
	}

	bool GLshader::setParameter(const char* name, const Vector3 v[], int len) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameterValuefc(param, len * 3, v[0]);
		return true;
	}

	bool GLshader::setParameter(const char* name, const Vector4 v[], int len) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameterValuefc(param, len * 4, v[0]);
		return true;
	}

	bool GLshader::setParameter(const char* name, const Matrix4 v[], int len) {
		CGparameter param = getEffectParameter(name);
		if (!param) {
			return false;
		}

		cgSetParameterValuefc(param, len * 16, v[0]);
		return true;
	}

	bool GLshader::setParameter(const char* name, GLtexture* texture) {
		return true;
	}

	CGtechnique GLshader::getTechnique(const char* name) {
		return nullptr;
	}

	void GLshader::initAnnotation()
	{
		CGparameter param = cgGetFirstEffectParameter(m_cgfx);
		CGtype type = cgGetParameterType(param);

		for (; param; param = cgGetNextParameter(param)) {
			const char* name = cgGetParameterName(param);
			CGtype type = cgGetParameterType(param);
			if (Internal::isSamplerType(type)) {
				initSamplerAnn(param);
			} else {
				initParameterAnn(param);
			}
		}
	}

	void GLshader::initSamplerAnn(CGparameter param) {
		// get texture parameter
		CGstateassignment sa = cgGetFirstSamplerStateAssignment(param);
		while (sa) {
			CGstate sa_state = cgGetSamplerStateAssignmentState(sa); // = 0x000000de (valid)
			if (sa_state == glShaderManager->m_textureState) {
				break;
			}
			sa = cgGetNextStateAssignment(sa);
		}

		if (!sa) {
			return;
		}
		CGparameter texparam = cgGetTextureStateAssignmentValue(sa);
		if (!texparam) {
			return;
		}

		const char* filename = Internal::getAnnoString(texparam, "file");
		if (!filename || !filename[0]) {
			return;
		}

		if (filename[0] != '$') {
			Printf("%s", filename);

			TexturePtr tex = GLtexture::load(filename);
			GLtexture* gltex = AX_REFPTR_CAST(GLtexture, tex);
			if (gltex) {
				cgGLSetupSampler(param, gltex->getObject());
			}
			return;
		}

		SamplerAnno::RenderType rendertype = SamplerAnno::None;
		if (Striequ(filename, "$SceneColor")) {
			rendertype = SamplerAnno::SceneColor;
		} else if (Striequ(filename, "$Reflection")) {
			rendertype = SamplerAnno::Reflection;
		}

		if (rendertype == SamplerAnno::None) {
			return;
		}

		CGsamplerann* san = new CGsamplerann;

		san->m_param = param;
		san->m_renderType = rendertype;
		san->m_texName = filename;

		m_samplerannSeq.push_back(san);

		m_haveTextureTarget = true;

		return;
	}

	void GLshader::initParameterAnn(CGparameter param) {
		initPixelToTexel(param);

		const char* uiwidget = Internal::getAnnoString(param, "UIWidget");
		if (!uiwidget || !uiwidget[0]) {
			return;
		}

		if (Striequ(uiwidget, "none")) {
			return;
		}

		const char* uiname = Internal::getAnnoString(param, "UIName");
	}


	void GLshader::initPixelToTexel(CGparameter param)
	{
		if (!cgIsParameterUsed(param, m_cgfx)) {
			return;
		}

		const char* pixel_param_name = Internal::getAnnoString(param, "pixelToTexel");
		if (!pixel_param_name || !pixel_param_name[0]) {
			return;
		}

		CGparameter pixel_param = cgGetNamedEffectParameter(m_cgfx, pixel_param_name);

		if (!pixel_param) {
			Errorf("not found pixel paramter for texel parameter");
		}

		int nrows = cgGetParameterRows(pixel_param);
		int ncols = cgGetParameterColumns(pixel_param);
		int asize = cgGetArrayTotalSize(pixel_param);
		int ntotal = nrows*ncols;
		if (asize > 0) ntotal *= asize;

		CGpixeltotexel p2t;
		p2t.m_param = param;
		p2t.m_pixelValue.resize(ntotal);
		cgGetParameterDefaultValuefc(pixel_param, ntotal, &p2t.m_pixelValue[0]);

		pixel2Texels.push_back(p2t);
	}

	void GLshader::setPixelToTexel(int width, int height)
	{
		AX_FOREACH(const CGpixeltotexel& p2t, pixel2Texels) {
			FloatSeq data = p2t.m_pixelValue;

			int numvalue = s2i(data.size()) / 2;
			float invwidth = 1.0f / width;
			float invheight = 1.0f / height;
			for (int i = 0; i < numvalue; i++) {
				data[i*2] *= invwidth;
				data[i*2+1] *= invheight;
			}

			cgSetParameterValuefc(p2t.m_param, numvalue * 2, &data[0]);
		}
	}

	bool GLshader::haveTextureTarget() const {
		return m_haveTextureTarget;
	}

	const StringSeq& GLshader::getFeatures() const {
		return m_features;
	}

	const StringSeq& GLshader::getMacroParameters() const {
		return m_macroParameters;
	}

	void GLshader::initFeatures() {
		CGparameter script = cgGetNamedEffectParameter(m_cgfx, "Script");

		if (!script) return;

		CGannotation annon = cgGetNamedParameterAnnotation(script, "Features");
		if (!annon) return;

		const char* value = cgGetStringAnnotationValue(annon);
		if (!value || !value[0]) return;

		StringList flist = StringUtil::tokenize(value, '|');

		AX_FOREACH(const String& s, flist) {
			StringSeq ftext = StringUtil::tokenizeSeq(s.c_str(), ':');

			if (ftext.size() < 3) {
				continue;
			}

			m_features.push_back(ftext[0]);
			m_featureNames.push_back(ftext[1]);
			m_featureDescs.push_back(ftext[2]);
		}
	}

	void GLshader::initMacroParameters() {
		CGparameter script = cgGetNamedEffectParameter(m_cgfx, "Script");

		if (!script) return;

		CGannotation annon = cgGetNamedParameterAnnotation(script, "MacroParameters");
		if (!annon) return;

		const char* value = cgGetStringAnnotationValue(annon);
		if (!value || !value[0]) return;

		StringList flist = StringUtil::tokenize(value, '|');

		AX_FOREACH(const String& s, flist) {
			StringSeq ftext = StringUtil::tokenizeSeq(s.c_str(), ':');

			if (!ftext.size()) {
				continue;
			}

			m_macroParameters.push_back(ftext[0]);
		}
	}

	void GLshader::initSortHint() {
		CGparameter script = cgGetNamedEffectParameter(m_cgfx, "Script");

		if (!script) return;

		m_sortHint = (SortHint)(Internal::getAnnoInt(script, "SortHint", SortHint_opacit));
	}

	void GLshader::setSU() {
		for (int i = 0; i < m_numUniformCaches; i++) {
			GLuniform* uc = m_uniformCaches[i];

			if (uc->isCached()) {
				g_statistic->incValue(stat_shaderParamCached);
				continue;
			}

			setUniformCache(uc);

			g_statistic->incValue(stat_shaderParamUpdated);
			uc->cache();
		}
	}

	void GLshader::initUniforms() {
		m_numUniformCaches = 0;
		TypeZeroArray(m_uniformCaches);

		for (int i = 0; i<Uniforms::NUM_UNIFORM_ITEMS; i++) {
			UniformItem& item = g_uniforms.getItem(i);
			const char* name = item.getName();
			CGparameter h = getEffectParameter(name);

			if (h) {
//				D3DXPARAMETER_DESC d3dpd;
//				m_object->GetParameterDesc(h, &d3dpd);
//				if (d3dpd.Bytes != item.m_dataSize) {
//					Errorf("parameter size for shader and engine not match");
//				}
				m_uniformCaches[m_numUniformCaches] = new GLuniform(item, h);
				m_numUniformCaches++;
			}
		}
	}

	CGparameter GLshader::getEffectParameter(const char* name) {
		CGparameter param = cgGetNamedEffectParameter(m_cgfx, name);

		if (!param) {
			return 0;
		}

		int index = cgGetParameterBufferIndex(param);
		if (index >= 0) {
			Debugf("1");
		}
		int offset = cgGetParameterBufferOffset(param);

		if (cgIsParameterUsed(param, m_cgfx)) {
			return param;
		} else {
			return 0;
		}
	}

	void GLshader::setUniformCache(GLuniform* uc)
	{
		const UniformItem* ui = uc->m_src;

		switch (ui->m_valueType) {
		case UniformItem::vt_empty:
			break;
		default:
		case UniformItem::vt_float:
#if 0
			if (ui->m_arraySize == 1) {
				cgGLSetParameter1f(uc->m_param, *(const FLOAT*)ui->m_datap);
			} else {
				m_object->SetFloatArray(uc->m_param, (const FLOAT*)ui->m_datap, ui->m_arraySize);
			}
			break;
		case UniformItem::vt_Vector2:
			if (ui->m_arraySize == 1) {
				m_object->SetVector(uc->m_param, (const D3DXVECTOR4*)ui->m_datap);
			} else {
				// TODO: not work
				m_object->SetVectorArray(uc->m_param, (const D3DXVECTOR4*)ui->m_datap, ui->m_arraySize);
			}
			break;
		case UniformItem::vt_Vector3:
			if (ui->m_arraySize == 1) {
				m_object->SetVector(uc->m_param, (const D3DXVECTOR4*)ui->m_datap);
			} else {
				// TODO: not work
				m_object->SetVectorArray(uc->m_param, (const D3DXVECTOR4*)ui->m_datap, ui->m_arraySize);
			}
			break;
		case UniformItem::vt_Matrix3:
			if (ui->m_arraySize == 1) {
				m_object->SetMatrix(uc->m_param, (const D3DXMATRIX*)ui->m_datap);
			} else {
				// TODO: not work
				m_object->SetMatrixArray(uc->m_param, (const D3DXMATRIX*)ui->m_datap, ui->m_arraySize);
			}
			break;
		case UniformItem::vt_Vector4:
			if (ui->m_arraySize == 1) {
				m_object->SetVector(uc->m_param, (const D3DXVECTOR4*)ui->m_datap);
			} else {
				m_object->SetVectorArray(uc->m_param, (const D3DXVECTOR4*)ui->m_datap, ui->m_arraySize);
			}
			break;
		case UniformItem::vt_AffineMat:
			if (ui->m_arraySize == 1) {
				m_object->SetMatrix(uc->m_param, (const D3DXMATRIX*)ui->m_datap);
			} else {
				// TODO: not work
				m_object->SetMatrixArray(uc->m_param, (const D3DXMATRIX*)ui->m_datap, ui->m_arraySize);
			}
			break;
		case UniformItem::vt_Matrix4:
			if (ui->m_arraySize == 1) {
				m_object->SetMatrix(uc->m_param, (const D3DXMATRIX*)ui->m_datap);
			} else {
				m_object->SetMatrixArray(uc->m_param, (const D3DXMATRIX*)ui->m_datap, ui->m_arraySize);
			}
#else
			cgSetParameterValuefc(uc->m_param, ui->m_dataSize/4, (const float*)ui->m_datap);
#endif
			break;
		case UniformItem::vt_Texture:
			if (ui->m_arraySize == 1) {
				GLtexture* tex = *(GLtexture**)(ui->m_datap);
				if (tex) {
					cgGLSetTextureParameter(uc->m_param, tex->getObject());
				} else {
					cgGLSetTextureParameter(uc->m_param, 0);
				}
			} else {
				Errorf("can't use texture parameter in array mode");
			}
			break;
		}
	}

	//--------------------------------------------------------------------------
	// class GLshadermanager
	//--------------------------------------------------------------------------
	CGstate GLshadermanager::m_textureState;

	GLshadermanager::GLshadermanager()
	{
		g_shaderMacro.setMacro(ShaderMacro::G_OPENGL);

		m_textureState = cgGetFirstSamplerState(glCgContext);
		while (m_textureState) {
			const char* n = cgGetStateName(m_textureState);
			if (Striequ("texture", n)) {
				break;
			}
			m_textureState = cgGetNextState(m_textureState);
		}

		m_defaulted = new GLshader();
		bool v = m_defaulted->doInit("blend");
		AX_ASSERT(v);
	}

	GLshadermanager::~GLshadermanager()
	{
	}

	Shader* GLshadermanager::findShader(const String& name, const ShaderMacro& macro)
	{
		return findShaderGL(name, macro);
	}

	Shader* GLshadermanager::findShader( const FixedString& nameId, const ShaderMacro& macro )
	{
		return 0;
	}

	GLshader* GLshadermanager::findShaderGL(const String& name, const ShaderMacro& macro)
	{
		ShaderPool::const_iterator it = m_shaderPool.find(name);
		if (it != m_shaderPool.end()) {
			Dict<ShaderMacro,GLshader*>::const_iterator it2 = it->second.find(macro);
			if (it2 != it->second.end()) {
				return it2->second;
			}
		}

		GLshader*& shader = m_shaderPool[name][macro];

		if (!shader) {
			shader = new GLshader();
			bool v = shader->doInit(name, macro);
			if (!v) {
				delete shader;
				shader = m_defaulted;
				shader->addref();
			}
		}
		return shader;
	}

	void GLshadermanager::saveShaderCache( const String& name )
	{

	}

	void GLshadermanager::applyShaderCache( const String& name )
	{

	}
}} // namespace Axon::Render



