/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "dx9_private.h"

AX_BEGIN_NAMESPACE

namespace {
	static ID3DXEffectPool *s_effectPool = NULL;   // Effect pool for sharing parameters

	// cache system samplers
	struct SamplerParam {
		int type;
		const char *paramname;
	};

	static SamplerParam s_materialTextureNames[] = {
		MaterialTextureId::Diffuse, "g_diffuseMap",
		MaterialTextureId::Normal, "g_normalMap",
		MaterialTextureId::Specular, "g_specularMap",
		MaterialTextureId::Opacit, "g_opacitMap",
		MaterialTextureId::Emission, "g_emissionMap",
		MaterialTextureId::Displacement, "g_displacementMap",
		MaterialTextureId::Env, "g_envMap",

		// terrain sampler
		MaterialTextureId::TerrainColor, "g_terrainColor",
		MaterialTextureId::TerrainNormal, "g_terrainNormal",

		MaterialTextureId::Detail, "g_detailMap",
		MaterialTextureId::Detail1, "g_detailMap1",
		MaterialTextureId::Detail2, "g_detailMap2",
		MaterialTextureId::Detail3, "g_detailMap3",

		MaterialTextureId::DetailNormal, "g_detailNormalMap",
		MaterialTextureId::DetailNormal1, "g_detailNormalMap1",
		MaterialTextureId::DetailNormal2, "g_detailNormalMap2",
		MaterialTextureId::DetailNormal3, "g_detailNormalMap3",

		MaterialTextureId::LayerAlpha, "g_layerAlpha",
		MaterialTextureId::LayerAlpha1, "g_layerAlpha1",
		MaterialTextureId::LayerAlpha2, "g_layerAlpha2",
		MaterialTextureId::LayerAlpha3, "g_layerAlpha3",

		// other
		MaterialTextureId::Reflection, "g_reflectionMap",
		MaterialTextureId::LightMap, "g_lightMap",
	};

	static SamplerParam s_globalTextureNames[] = {
		GlobalTextureId::RtDepth, "g_rtDepth",
		GlobalTextureId::Rt0, "g_rt0",
		GlobalTextureId::Rt1, "g_rt1",
		GlobalTextureId::Rt2, "g_rt2",
		GlobalTextureId::Rt3, "g_rt3",
		GlobalTextureId::SceneColor, "g_sceneColor",
		GlobalTextureId::ShadowMap, "g_shadowMap",
		GlobalTextureId::ShadowMapCube, "g_shadowMapCube",
	};

	class EffectHelper
	{
	public:
		EffectHelper(ID3DXEffect *obj) : m_object(obj)
		{}

		std::string getString(D3DXHANDLE h)
		{
			const char *str;

			HRESULT hr = m_object->GetString(h, &str);
			if (SUCCEEDED(hr) && str && str[0]) {
				return str;
			}

			return std::string();
		}

		int getInt(D3DXHANDLE h)
		{
			int result = 0;

			HRESULT hr = m_object->GetInt(h, &result);
			if (SUCCEEDED(hr)) {
				return result;
			}

			return 0;
		}

		std::string getAnno(D3DXHANDLE param, const char *anno_name)
		{
			std::string result;
			D3DXHANDLE anno = m_object->GetAnnotationByName(param, anno_name);

			if (!anno) {
				return result;
			}

			return getString(anno);
		}

	private:
		ID3DXEffect *m_object;
	};

	class DX9_Include : public ID3DXInclude
	{
	public:
		STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
		{
			std::string filename = "shaders/";
			filename += pFileName;
			*pBytes = g_fileSystem->readFile(filename, (void**)ppData);

			if (*ppData && *pBytes)
				return S_OK;
			else
				return S_FALSE;
		}

		STDMETHOD(Close)(THIS_ LPCVOID pData)
		{
			g_fileSystem->freeFile((void**)pData);
			return S_OK;
		}
	};
} // namespace

DX9_Technique::DX9_Technique(DX9_Shader *shader, D3DXHANDLE d3dxhandle)
{
	m_shader = shader;
	m_d3dxhandle = d3dxhandle;

	HRESULT hr;

	D3DXTECHNIQUE_DESC desc;
	V(shader->m_object->GetTechniqueDesc(d3dxhandle, &desc));

	m_numPasses = desc.Passes;

	UINT checkpass;
	shader->m_object->Begin(&checkpass, D3DXFX_DONOTSAVESTATE);
	AX_ASSURE(checkpass == m_numPasses);
	for (int i = 0; i < m_numPasses; i++) {
		shader->m_object->BeginPass(i);
		m_passes[i] = new DX9_Pass(shader, shader->m_object->GetPass(d3dxhandle, i));
		shader->m_object->EndPass();
	}
	shader->m_object->End();
}

DX9_Technique::~DX9_Technique()
{}

DX9_Pass::DX9_Pass(DX9_Shader *shader, D3DXHANDLE d3dxhandle)
{
	m_shader = shader;
	m_d3dxhandle = d3dxhandle;
	m_setflag = 0;

	memset(m_sysSamplers, -1, sizeof(m_sysSamplers));
	memset(m_matSamplers, -1, sizeof(m_matSamplers));

	V(dx9_device->GetVertexShader(&m_vs));
	V(dx9_device->GetPixelShader(&m_ps));
#if 0
	initState();
#endif
	initVs();
	initPs();
}

DX9_Pass::~DX9_Pass()
{}

void DX9_Pass::initVs()
{
	HRESULT hr;

	D3DXPASS_DESC desc;
	V(m_shader->m_object->GetPassDesc(m_d3dxhandle, &desc));

	LPD3DXCONSTANTTABLE constTable;
	D3DXGetShaderConstantTable(desc.pVertexShaderFunction, &constTable);

	D3DXCONSTANTTABLE_DESC tableDesc;
	constTable->GetDesc(&tableDesc);

	for (UINT i = 0; i < tableDesc.Constants; i++) {
		D3DXHANDLE curConst = constTable->GetConstant(0, i);
		D3DXCONSTANT_DESC constDesc;
		UINT count;
		constTable->GetConstantDesc(curConst, &constDesc, &count);

		if (constDesc.RegisterSet == D3DXRS_SAMPLER) {
			// error

		} else {
			if (!DX9_Shader::isPrimitiveReg(constDesc.RegisterIndex))
				continue;

			ParamDesc paramDesc;
			paramDesc.d3dDesc = constDesc;
			m_parameters[constDesc.Name] = paramDesc;
		}

		continue;
	}
}

void DX9_Pass::initPs()
{
	HRESULT hr;

	D3DXPASS_DESC desc;
	V(m_shader->m_object->GetPassDesc(m_d3dxhandle, &desc));

	LPD3DXCONSTANTTABLE constTable;

	V(D3DXGetShaderConstantTable(desc.pPixelShaderFunction, &constTable));

	D3DXCONSTANTTABLE_DESC tableDesc;
	V(constTable->GetDesc(&tableDesc));

	D3DXHANDLE s_skyColor = constTable->GetConstantByName(0, "s_skyColor");

	for (UINT i = 0; i < tableDesc.Constants; i++) {
		D3DXHANDLE curConst = constTable->GetConstant(0, i);
		D3DXCONSTANT_DESC constDesc;
		UINT count;
		constTable->GetConstantDesc(curConst, &constDesc, &count);

		if (constDesc.RegisterSet == D3DXRS_SAMPLER) {
			initSampler(constDesc);
		} else {
			if (!DX9_Shader::isPrimitiveReg(constDesc.RegisterIndex))
				continue;

			ParamDesc paramDesc;
			paramDesc.setflag = 0;
			paramDesc.d3dDesc = constDesc;
			m_parameters[constDesc.Name] = paramDesc;
		}

		continue;
	}
}

void DX9_Pass::initSampler(const D3DXCONSTANT_DESC &desc)
{
	// check global sampler
	for (int i = 0; i < ArraySize(s_globalTextureNames); i++) {
		if (Strequ(s_globalTextureNames[i].paramname, desc.Name)) {
			m_sysSamplers[s_globalTextureNames[i].type] = desc.RegisterIndex;
			return;
		}
	}

	// check material sampler
	for (int i = 0; i < ArraySize(s_materialTextureNames); i++) {
		if (Strequ(s_materialTextureNames[i].paramname, desc.Name)) {
			m_matSamplers[s_materialTextureNames[i].type] = desc.RegisterIndex;
			return;
		}
	}

	// check batch sampler
	for (int i = 0; i < s2i(m_shader->m_samplerInfos.size()); i++) {
		DX9_SamplerInfo *bs = m_shader->m_samplerInfos[i];
		if (bs->m_paramName == desc.Name) {
			DX9_SamplerInfo *newbs = new DX9_SamplerInfo();
			*newbs = *bs;
			newbs->m_register = desc.RegisterIndex;
			m_batchSamplers.push_back(newbs);
			return;
		}
	}
}

extern phandle_t dx9_curGlobalTextures[GlobalTextureId::MaxType];
extern SamplerDesc dx9_curGlobalTextureSamplerDescs[GlobalTextureId::MaxType];
extern FastTextureParams dx9_curMaterialTextures;

static inline IDirect3DBaseTexture9 *H2T(phandle_t h)
{
	DX9_Resource *res = h->castTo<DX9_Resource *>();
	AX_ASSERT(res->m_type == DX9_Resource::kTexture);
	return res->m_texture;
}

void DX9_Pass::begin()
{
	// set shader
	dx9_stateManager->setVertexShader(m_vs);
	dx9_stateManager->setPixelShader(m_ps);

	// set primitive parameters
	setPrimitiveParameters();

	// set global textures
	for (int i = 0; i < GlobalTextureId::MaxType; i++) {
		if (m_sysSamplers[i] >= 0 && dx9_curGlobalTextures[i]) {
			dx9_stateManager->setTexture(m_sysSamplers[i], H2T(dx9_curGlobalTextures[i]));
			dx9_stateManager->setSamplerState(m_sysSamplers[i], dx9_curGlobalTextureSamplerDescs[i]);
		} else {
			//dx9_stateManager->setTexture(m_sysSamplers[i], 0);
		}
	}

	for (int i = 0; i < dx9_curMaterialTextures.m_numItems; i++) {
		FastTextureParams::Item &item = dx9_curMaterialTextures.m_items[i];
		int index = item.id;
		if (m_matSamplers[index] >= 0) {
			dx9_stateManager->setTexture(m_matSamplers[index], H2T(item.handle));
			dx9_stateManager->setSamplerState(m_matSamplers[index], item.samplerState);
		}
	}
}

extern FastParams dx9_curParams1;
extern FastParams dx9_curParams2;

void DX9_Pass::setPrimitiveParameters()
{
	m_setflag++;
	// set params1
	for (int i = 0; i < dx9_curParams1.m_numItems; i++) {
		FixedString name(dx9_curParams1.m_items[i].nameId);
		setParameter(name, dx9_curParams1.m_items[i].count, &dx9_curParams1.m_floatData[dx9_curParams1.m_items[i].offset]);
	}

	// set params2
	for (int i = 0; i < dx9_curParams2.m_numItems; i++) {
		FixedString name(dx9_curParams2.m_items[i].nameId);
		setParameter(name, dx9_curParams2.m_items[i].count, &dx9_curParams2.m_floatData[dx9_curParams2.m_items[i].offset]);
	}

	// if not set by material parameter, set it to default value
	Dict<FixedString, ParamDesc>::iterator it = m_parameters.begin();
	for (; it != m_parameters.end(); ++it) {
		ParamDesc &param = it->second;

		if (param.setflag == m_setflag)
			continue;

		if (!param.d3dDesc.DefaultValue)
			continue;

		dx9_device->SetPixelShaderConstantF(param.d3dDesc.RegisterIndex, (const float*)param.d3dDesc.DefaultValue, param.d3dDesc.RegisterCount);
		dx9_device->SetVertexShaderConstantF(param.d3dDesc.RegisterIndex, (const float*)param.d3dDesc.DefaultValue, param.d3dDesc.RegisterCount);
	}
}

void DX9_Pass::setParameter(const ParamDesc &param, const float *value, bool isPixelShader)
{
}

void DX9_Pass::setParameter(const FixedString &name, int numFloats, const float *data)
{
	Dict<FixedString, ParamDesc>::iterator it = m_parameters.find(name);

	if (it == m_parameters.end()) {
		return;
	}

	ParamDesc &param = it->second;
	param.setflag = m_setflag;

	dx9_device->SetPixelShaderConstantF(param.d3dDesc.RegisterIndex, data, param.d3dDesc.RegisterCount);
	dx9_device->SetVertexShaderConstantF(param.d3dDesc.RegisterIndex, data, param.d3dDesc.RegisterCount);
}



//--------------------------------------------------------------------------
// class D3D9Shader
//--------------------------------------------------------------------------

DX9_Shader::DX9_Shader()
{
	m_shaderInfo.m_needReflection = false;
	m_curTechnique = 0;
}

DX9_Shader::~DX9_Shader()
{}

bool DX9_Shader::init(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm)
{
	m_key = name;
	std::string fullname = "shaders/" + name.toString() + ".fx";

	std::vector<D3DXMACRO> d3dxmacros;
	D3DXMACRO d3dxmacro;

	StringPairSeq definesG  = gm.getDefines();
	for (size_t i = 0; i < definesG.size(); i++) {
		d3dxmacro.Name = definesG[i].first.c_str();
		d3dxmacro.Definition = definesG[i].second.c_str();
		d3dxmacros.push_back(d3dxmacro);
	}

	StringPairSeq definesM  = mm.getDefines();
	for (size_t i = 0; i < definesM.size(); i++) {
		d3dxmacro.Name = definesM[i].first.c_str();
		d3dxmacro.Definition = definesM[i].second.c_str();
		d3dxmacros.push_back(d3dxmacro);
	}

	d3dxmacro.Name = 0;
	d3dxmacro.Definition = 0;

	d3dxmacros.push_back(d3dxmacro);

	HRESULT hr;
	LPD3DXBUFFER errbuf = 0;

	DWORD dwFlags = D3DXSHADER_NO_PRESHADER;

#if defined(_DEBUG) || defined(DEBUG)
//		dwFlags |= D3DXSHADER_DEBUG;
#endif

	void *filebuf;
	size_t filesize;

	filesize = g_fileSystem->readFile(fullname, &filebuf);
	if (!filesize || !filebuf)
		return false;
	DX9_Include d3d9Inc;

	double t0 = OsUtil::seconds();
	hr = D3DXCreateEffect(dx9_device, filebuf, filesize, &d3dxmacros[0], &d3d9Inc, dwFlags, 0, &m_object, &errbuf);
	double t1 = OsUtil::seconds();
	double t = t1 - t0;

	if (FAILED(hr)) {
		if (errbuf) {
			Errorf("%s", errbuf->GetBufferPointer());
		}
		return false;
	}

#if 0
	m_object->SetStateManager(d3d9StateManager);
#endif

	// init techniques
	initTechniques();

	// init annotations
	initAnnotation();

	// init shader features
	initFeatures();

	// init axon object
	initAxonObject();

	// init shader info
	initShaderInfo();

	return true;
}

void DX9_Shader::initTechniques()
{
	for (int i = 0; i < Technique::MaxType; i++) {
		m_d3dxTechniques[i] = findTechnique((Technique::Type)i);
	}
}

void DX9_Shader::initFeatures()
{}

void DX9_Shader::initAnnotation()
{
	D3DXEFFECT_DESC effectDesc;

	V(m_object->GetDesc(&effectDesc));

	for (UINT i = 0; i < effectDesc.Parameters; i++) {
		D3DXHANDLE param = m_object->GetParameter(0, i);
		if (!param) {
			continue;
		}

		D3DXPARAMETER_DESC paramDesc;
		V(m_object->GetParameterDesc(param, &paramDesc));

		if (paramDesc.Type >= D3DXPT_SAMPLER && paramDesc.Type <= D3DXPT_SAMPLERCUBE) {
			initSamplerAnn(param);
		} else {
			initParameterAnn(param);
		}
	}
}

void DX9_Shader::initSamplerAnn(D3DXHANDLE param)
{
	EffectHelper helper(m_object);

	D3DXPARAMETER_DESC paramDesc;
	HRESULT hr;

	V(m_object->GetParameterDesc(param, &paramDesc));

	std::string paramname = paramDesc.Name;
	std::string texname = paramname + "_tex";

	D3DXHANDLE texparam = m_object->GetParameterByName(0, texname.c_str());
	if (!texparam) {
		return;
	}
	V(m_object->GetParameterDesc(texparam, &paramDesc));

	for (UINT i = 0; i < paramDesc.Annotations; i++) {
		D3DXHANDLE anno = m_object->GetAnnotation(texparam, i);

		if (!anno)
			continue;

		D3DXPARAMETER_DESC annoDesc;
		V(m_object->GetParameterDesc(anno, &annoDesc));

		if (!Striequ(annoDesc.Name, "file"))
			continue;

		std::string filename = helper.getString(anno);

		if (filename.empty())
			break;

		DX9_SamplerInfo *san = new DX9_SamplerInfo;

		san->m_param = texparam;
		san->m_paramName = paramname;
		san->m_textureName = filename;

		m_samplerInfos.push_back(san);

		m_shaderInfo.m_needReflection = true;
	}
}

void DX9_Shader::initParameterAnn(D3DXHANDLE param)
{
#if 0
	initPixelToTexel(param);
#endif
}

D3DXHANDLE DX9_Shader::findTechnique(Technique tech)
{
	EffectHelper helper(m_object);

	D3DXHANDLE script = m_object->GetParameterByName(0, "Script");

	if (!script) return 0;

	D3DXHANDLE annon = m_object->GetAnnotationByName(script, tech.toString().c_str());
	if (!annon) return 0;

	std::string techname = helper.getString(annon);

	if (techname.empty()) return 0;

	D3DXHANDLE d3dtech = m_object->GetTechniqueByName(techname.c_str());

	if (!d3dtech) {
		return 0;
	}

#if 0
	HRESULT hr;
	V(m_object->ValidateTechnique(d3dtech));

	if (FAILED(hr)) {
		Errorf("%s(%d): technique '%s' cann't be validated\n", __FILE__, __LINE__, techname.c_str());
	}
#endif

	return d3dtech;
}

D3DXHANDLE DX9_Shader::getUsedParameter(const char *name)
{
	D3DXHANDLE param = m_object->GetParameterByName(0, name);

	if (!param) {
		return 0;
	}

	int i;
	for (i = 0; i < Technique::MaxType; i++) {
		if (!m_d3dxTechniques[i]) {
			continue;
		}

		if (m_object->IsParameterUsed(param, m_d3dxTechniques[i])) {
			break;
		}
	}

	if (i == Technique::MaxType) {
		return 0;
	} else {
		return param;
	}
}

bool DX9_Shader::isParameterUsed(D3DXHANDLE param)
{
	if (!param)
		return false;

	int i;
	for (i = 0; i < Technique::MaxType; i++) {
		if (!m_d3dxTechniques[i])
			continue;

		if (m_object->IsParameterUsed(param, m_d3dxTechniques[i]))
			break;
	}

	if (i == Technique::MaxType)
		return false;
	else
		return true;
}

void DX9_Shader::initAxonObject()
{
	for (int i = 0; i < Technique::MaxType; i++) {
		if (!m_d3dxTechniques[i]) {
			m_techniques[i] = 0;
			continue;
		}

		m_object->SetTechnique(m_d3dxTechniques[i]);
		m_techniques[i] = new DX9_Technique(this, m_d3dxTechniques[i]);
	}
}

UINT DX9_Shader::begin(Technique tech)
{
	if (!m_techniques[tech])
		return 0;

	m_curTech = m_techniques[tech];
	return m_curTech->m_numPasses;
}

void DX9_Shader::beginPass(UINT pass)
{
	m_curTech->m_passes[pass]->begin();
}

void DX9_Shader::endPass()
{}

void DX9_Shader::end()
{}

void DX9_Shader::initShaderInfo()
{
	for (int i = 0; i < Technique::MaxType; i++)
		m_shaderInfo.m_haveTechnique[i] = m_techniques[i] != 0;

	m_shaderInfo.m_needReflection = isMaterialTextureUsed(MaterialTextureId::Reflection);
	m_shaderInfo.m_needSceneColor = isGlobalTextureUsed(GlobalTextureId::SceneColor);

	// add to shader manager
	dx9_shaderManager->addShaderInfo(m_key, &m_shaderInfo);
}

bool DX9_Shader::isGlobalTextureUsed(GlobalTextureId id) const
{
	for (int i = 0; i < Technique::MaxType; i++) {
		DX9_Technique *tech = m_techniques[i];

		if (!tech)
			continue;

		for (int j = 0; j < tech->m_numPasses; j++) {
			DX9_Pass * pass = tech->m_passes[j];
			if (!pass) continue;

			if (pass->m_sysSamplers[id] != -1)
				return true;
		}
	}
	return false;
}

bool DX9_Shader::isMaterialTextureUsed(MaterialTextureId id) const
{
	for (int i = 0; i < Technique::MaxType; i++) {
		DX9_Technique *tech = m_techniques[i];

		if (!tech)
			continue;

		for (int j = 0; j < tech->m_numPasses; j++) {
			DX9_Pass * pass = tech->m_passes[j];
			if (!pass) continue;

			if (pass->m_matSamplers[id] != -1)
				return true;
		}
	}
	return false;
}



//--------------------------------------------------------------------------
// class D3D9shadermanager
//--------------------------------------------------------------------------

DX9_ShaderManager::DX9_ShaderManager()
{
	// HACK
	dx9_shaderManager = this;

	V(D3DXCreateEffectPool( &s_effectPool ));
	m_defaulted = new DX9_Shader();
#if 0
	bool v = m_defaulted->init("blend");
	AX_ASSERT(v);

	DX9_Shader *blinn = new DX9_Shader();
	v = blinn->init("_alluniforms");

	AX_ASSERT(v);

	blinn->checkGlobalStruct();
#endif

	_initialize();
}

DX9_ShaderManager::~DX9_ShaderManager()
{}

DX9_Shader *DX9_ShaderManager::findShader(const FixedString &nameId, const GlobalMacro &gm, const MaterialMacro &mm)
{
	ShaderKey key;
	key.nameId = nameId.id();
	key.gm = gm.id();
	key.mm = mm.id();

	DX9_Shader*& shader = m_shaders[key];

	if (!shader) {
		shader = new DX9_Shader();
		bool v = shader->init(nameId, gm, mm);
		if (!v) {
			delete shader;
			shader = m_defaulted;
			shader->AddRef();
		}
	}

	return shader;
}

void DX9_ShaderManager::saveShaderCache(const std::string &name)
{
#if 0
	std::string filename;
	if (filename.empty())
		filename = "shaders/shaderCaches.xml";
	else
		filename = "shaders/" + name;

	filename = g_fileSystem->dataPathToOsPath(filename);

	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "utf-8", "" );
	TiXmlElement * root = new TiXmlElement( "shaderCaches" );
	root->SetAttribute("version", ShaderMacro::VERSION);

	doc.LinkEndChild(decl);
	doc.LinkEndChild(root);

	// iterator shaders
	ShaderDict::const_iterator it = m_shaders.begin();

	for (; it != m_shaders.end(); ++it) {
		const Dict<ShaderMacro,DX9_Shader*>& shaders = it->second;
		Dict<ShaderMacro,DX9_Shader*>::const_iterator it2 = shaders.begin();

		for (; it2 != shaders.end(); ++it2) {
			TiXmlElement *item = new TiXmlElement("cacheItem");
			item->SetAttribute("name", it->first.toString().c_str());
			item->SetAttribute("macro", it2->first.toString());
			root->LinkEndChild(item);
		}
	}

	doc.SaveFile(filename);
#endif
}

void DX9_ShaderManager::applyShaderCache(const std::string &name)
{
#if 0
	std::string filename;
	if (filename.empty())
		filename = "shaders/shaderCaches.xml";
	else
		filename = "shaders/" + name;

	filename = g_fileSystem->dataPathToOsPath(filename);
	TiXmlDocument doc;

	if (!doc.LoadFile(filename, TIXML_ENCODING_UTF8))
		return;

	TiXmlElement *root = doc.RootElement();
	int version = 0;
	root->Attribute("version", &version);
	if (version != ShaderMacro::VERSION)
		return;

	TiXmlElement *item = root->FirstChildElement();
	std::string shadername = item->Attribute("name");
	ShaderMacro macro;
	for (; item; item = item->NextSiblingElement()) {
		shadername = item->Attribute("name");
		macro.fromString(item->Attribute("macro"));

		findShader(shadername, macro);
	}
#endif
}

void DX9_ShaderManager::_initialize()
{
	TiXmlDocument doc;
	doc.LoadAxonFile("shaders/shaderlist.xml");
	AX_ASSURE(!doc.Error());

	TiXmlNode *root = doc.FirstChild("shaderlist");

	// no root
	AX_ASSERT(root);

	TiXmlElement *section;
	for (section = root->FirstChildElement("item"); section; section = section->NextSiblingElement("item")) {
		findShader(section->GetText(), g_globalMacro, MaterialMacro());
	}
#if 0
	// add to ShaderInfoDict
	ShaderDict::const_iterator it = m_shaders.begin();

	for (; it != m_shaders.end(); ++it) {
		const FixedString &name = it->first;

		Dict<ShaderMacro,DX9_Shader*>::const_iterator it2 = it->second.begin();

		if (it2 != it->second.end())
			m_shaderInfoDict[name] = &it2->second->m_shaderInfo;
	}
#endif
}

const ShaderInfo *DX9_ShaderManager::findShaderInfo(const FixedString &key)
{
	ShaderInfoDict::const_iterator it = m_shaderInfoDict.find(key);

	if (it != m_shaderInfoDict.end())
		return it->second;

	Errorf("Can't found shader info for '%s'", key.c_str());
	return 0;
}

void DX9_ShaderManager::addShaderInfo( const FixedString &key, ShaderInfo *shaderInfo )
{
	if (m_shaderInfoDict.find(key) != m_shaderInfoDict.end())
		return;

	m_shaderInfoDict[key] = shaderInfo;
}

AX_END_NAMESPACE

