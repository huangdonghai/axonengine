/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

AX_BEGIN_NAMESPACE

static ID3DXEffectPool *s_effectPool = NULL;   // Effect pool for sharing parameters

// cache system samplers
static struct SamplerParam {
	SamplerType type;
	const char *paramname;
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


class EffectHelper
{
public:
	EffectHelper(ID3DXEffect *obj) : m_object(obj) {}

	String getString(D3DXHANDLE h) {
		const char *str;

		HRESULT hr = m_object->GetString(h, &str);
		if (SUCCEEDED(hr) && str && str[0]) {
			return str;
		}

		return String();
	}

	int getInt(D3DXHANDLE h) {
		int result = 0;

		HRESULT hr = m_object->GetInt(h, &result);
		if (SUCCEEDED(hr)) {
			return result;
		}

		return 0;
	}

	String getAnno(D3DXHANDLE param, const char *anno_name) {
		String result;
		D3DXHANDLE anno = m_object->GetAnnotationByName(param, anno_name);

		if (!anno) {
			return result;
		}

		return getString(anno);
	}

private:
	ID3DXEffect *m_object;
};

class D3D9include : public ID3DXInclude
{
public:
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {
		String filename = "shaders/";
		filename += pFileName;
		*pBytes = g_fileSystem->readFile(filename, (void**)ppData);

		if (*ppData && *pBytes)
			return S_OK;
		else
			return S_FALSE;
	}

	STDMETHOD(Close)(THIS_ LPCVOID pData) {
		g_fileSystem->freeFile((void**)pData);
		return S_OK;
	}
};

//--------------------------------------------------------------------------
// class UniformCache
//--------------------------------------------------------------------------

D3D9uniform::D3D9uniform(UniformItem &item, D3DXHANDLE param)
	: UniformItem(item)
{
	m_src = &item;
	m_param = param;
}

D3D9uniform::~D3D9uniform()
{
}

bool D3D9uniform::isCached() const
{
	return memcmp(m_datap, m_src->m_datap, m_dataSize) == 0;
}

void D3D9uniform::cache()
{
	memcpy(m_datap, m_src->m_datap, m_dataSize);
}

void D3D9uniform::setUniform( UniformItem &item, const void *q )
{
		// direct set
	HRESULT hr;

	switch (item.m_valueType) {
	case UniformItem::vt_empty:
		break;
	case UniformItem::vt_float:
	case UniformItem::vt_Vector3:
	case UniformItem::vt_Vector4:
		{
			int n = (item.m_dataSize + 15) / 16;
			const float *data = (const float*)q;
			if (item.m_vsregister) {
				V(d3d9Device->SetVertexShaderConstantF(item.m_vsregister, data, n));
			}

			if (item.m_psregister) {
				V(d3d9Device->SetPixelShaderConstantF(item.m_psregister, data, n));
			}
		}
		break;
	case UniformItem::vt_Vector2:
		if (item.m_arraySize == 1 ) {
			const float *data = (const float*)q;
			if (item.m_vsregister) {
				V(d3d9Device->SetVertexShaderConstantF(item.m_vsregister, data, 1));
			}

			if (item.m_psregister) {
				V(d3d9Device->SetPixelShaderConstantF(item.m_psregister, data, 1));
			}
		} else {
			Vector4 vec4[8];
			const Vector2 *vec2 = (const Vector2*)q;
			for (int i = 0; i < item.m_arraySize; i++) {
				vec4[i].set(vec2[i].x, vec2[i].y, 0, 0);
			}
			if (item.m_vsregister) {
				V(d3d9Device->SetVertexShaderConstantF(item.m_vsregister, vec4[0].c_ptr(), item.m_arraySize));
			}

			if (item.m_psregister) {
				V(d3d9Device->SetPixelShaderConstantF(item.m_psregister, vec4[0].c_ptr(), item.m_arraySize));
			}
		}
		break;
	case UniformItem::vt_Matrix3:
		{
			Matrix3 &axis = *(Matrix3*)q;
			Matrix4 matrix(axis, Vector3::Zero);
			if (item.m_vsregister) {
				V(d3d9Device->SetVertexShaderConstantF(item.m_vsregister, matrix.c_ptr(), 1));
			}

			if (item.m_psregister) {
				V(d3d9Device->SetPixelShaderConstantF(item.m_psregister, matrix.c_ptr(), 1));
			}
		}
		break;
	case UniformItem::vt_Matrix:
		if (item.m_arraySize == 1) {
			Matrix &am = *(Matrix*)q;
			Matrix4 matrix = am.toMatrix4().getTranspose();

			if (item.m_vsregister) {
				V(d3d9Device->SetVertexShaderConstantF(item.m_vsregister, matrix.c_ptr(), 3));
			}

			if (item.m_psregister) {
				V(d3d9Device->SetPixelShaderConstantF(item.m_psregister, matrix.c_ptr(), 3));
			}
		} else {
			Errorf("not support");
		}
		break;
	case UniformItem::vt_Matrix4:
		if (item.m_arraySize == 1) {
			Matrix4 matrix = ((const Matrix4*)item.m_datap)->getTranspose();
			if (item.m_vsregister) {
				V(d3d9Device->SetVertexShaderConstantF(item.m_vsregister, matrix.c_ptr(), 4));
			}

			if (item.m_psregister) {
				V(d3d9Device->SetPixelShaderConstantF(item.m_psregister, matrix.c_ptr(), 4));
			}
		} else {
			const Matrix4 *old = (const Matrix4*)q;
			Matrix4 matrix[8];
			for (int i = 0; i < item.m_arraySize; i++) {
				matrix[i] = old[i].getTranspose();
			}

			if (item.m_vsregister) {
				V(d3d9Device->SetVertexShaderConstantF(item.m_vsregister, matrix[0].c_ptr(), 4 * item.m_arraySize));
			}

			if (item.m_psregister) {
				V(d3d9Device->SetPixelShaderConstantF(item.m_psregister, matrix[0].c_ptr(), 4 * item.m_arraySize));
			}
		}
		break;
	case UniformItem::vt_Texture:
		break;
	}
}
//--------------------------------------------------------------------------
// class D3D9shader
//--------------------------------------------------------------------------

D3D9shader::D3D9shader()
{
	m_p2tWidth = 0;
	m_p2tHeight = 0;
	m_haveTextureTarget = false;
	m_sortHint = SortHint_opacit;
	m_curTechnique = 0;

	m_coupled = 0;
}

D3D9shader::~D3D9shader()
{}

bool D3D9shader::doInit(const String &name, const ShaderMacro &macro)
{
	m_keyString = name;
	String fullname = "shaders/" + name + ".fx";
	String ospath = g_fileSystem->dataPathToOsPath(fullname);

	String path = g_fileSystem->dataPathToOsPath("shaders/");
	path = "-I" + path;

	Sequence<D3DXMACRO> d3dxmacros;

	StringPairSeq defines;
	D3DXMACRO d3dxmacro;
	defines  = macro.getDefines();
	for (size_t i = 0; i < defines.size(); i++) {
		d3dxmacro.Name = defines[i].first.c_str();
		d3dxmacro.Definition = defines[i].second.c_str();
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

#if 0
	(hr = D3DXCreateEffectFromFile(d3d9Device, u2w(ospath).c_str(), &d3dxmacros[0], NULL, dwFlags, 0, &m_object, &errbuf));
#else
	void *filebuf;
	size_t filesize;

	filesize = g_fileSystem->readFile(fullname, &filebuf);
	if (!filesize || !filebuf)
		return false;
	D3D9include d3d9Inc;

	hr = D3DXCreateEffect(d3d9Device, filebuf, filesize, &d3dxmacros[0], &d3d9Inc, dwFlags, 0, &m_object, &errbuf);
#endif
	if (FAILED(hr)) {
		if (errbuf) {
			Errorf("%s", errbuf->GetBufferPointer());
		}
		return false;
	}

	m_object->SetStateManager(d3d9StateManager);

	// init techniques
	initTechniques();

	// init annotations
	initAnnotation();

	// init shader features
	initFeatures();

	// init sort hint
	initSortHint();

	// init axon object
	initAxonObject();

	return true;
}

bool D3D9shader::isDepthWrite() const {
	return true;
}

bool D3D9shader::haveTextureTarget() const {
	return m_haveTextureTarget;
}

int D3D9shader::getNumSampler() const {
	return s2i(m_samplerannSeq.size());
}

SamplerAnno *D3D9shader::getSamplerAnno(int index) const {
	return m_samplerannSeq[index];
}

int D3D9shader::getNumTweakable() const {
	return 0;
}

ParameterAnno *D3D9shader::getTweakableDef(int index) {
	return 0;
}


Shader::SortHint D3D9shader::getSortHint() const {
	return m_sortHint;
}

bool D3D9shader::haveTechnique(Technique tech) const {
	return m_techniques[tech] != 0;
}

void D3D9shader::initTechniques() {
	for (int i = 0; i < Technique::Number; i++) {
		m_d3dxTechniques[i] = findTechnique(i);
	}
}

void D3D9shader::initFeatures() {

}

void D3D9shader::initSortHint() {
	D3DXHANDLE script = m_object->GetParameterByName(0, "Script");

	if (!script) return;

	D3DXHANDLE anno = m_object->GetAnnotationByName(script, "SortHint");

	if (!anno) return;

	m_sortHint = (SortHint)EffectHelper(m_object).getInt(anno);
}

void D3D9shader::initAnnotation() {
	D3DXEFFECT_DESC effectDesc;
	HRESULT hr;

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

void D3D9shader::initSamplerAnn(D3DXHANDLE param) {
	EffectHelper helper(m_object);

	D3DXPARAMETER_DESC paramDesc;
	HRESULT hr;

	V(m_object->GetParameterDesc(param, &paramDesc));

	String paramname = paramDesc.Name;
	String texname = paramname + "_tex";

	D3DXHANDLE texparam = m_object->GetParameterByName(0, texname.c_str());
	if (!texparam) {
		return;
	}
	V(m_object->GetParameterDesc(texparam, &paramDesc));

	for (UINT i = 0; i < paramDesc.Annotations; i++) {
		D3DXHANDLE anno = m_object->GetAnnotation(texparam, i);

		if (!anno) {
			continue;
		}

		D3DXPARAMETER_DESC annoDesc;
		V(m_object->GetParameterDesc(anno, &annoDesc));

		if (!Striequ(annoDesc.Name, "file")) {
			continue;
		}

		String filename = helper.getString(anno);

		if (filename.empty()) {
			break;
		}

		if (filename[0] != '$') {
#if 0
			D3D9texture *tex = FindAsset_<D3D9texture>(filename);
			if (tex) {
				m_object->SetTexture(texparam, tex->getObject());
			}
#endif
			return;
		}

		SamplerAnno::RenderType rendertype = SamplerAnno::None;
		if (Striequ(filename.c_str(), "$SceneColor")) {
			rendertype = SamplerAnno::SceneColor;
		} else if (Striequ(filename.c_str(), "$Reflection")) {
			rendertype = SamplerAnno::Reflection;
		}

		if (rendertype == SamplerAnno::None) {
			return;
		}

		D3D9samplerann *san = new D3D9samplerann;

		san->m_param = texparam;
		san->m_renderType = rendertype;
		san->m_paramName = paramname;
		san->m_texName = filename;

		m_samplerannSeq.push_back(san);

		m_haveTextureTarget = true;
	}
}

void D3D9shader::initParameterAnn(D3DXHANDLE param) {
	initPixelToTexel(param);
}

D3DXHANDLE D3D9shader::findTechnique(Technique tech)
{
	EffectHelper helper(m_object);

	D3DXHANDLE script = m_object->GetParameterByName(0, "Script");

	if (!script) return 0;

	D3DXHANDLE annon = m_object->GetAnnotationByName(script, tech.toString().c_str());
	if (!annon) return 0;

	String techname = helper.getString(annon);

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

D3DXHANDLE D3D9shader::getUsedParameter(const char *name)
{
	D3DXHANDLE param = m_object->GetParameterByName(0, name);

	if (!param) {
		return 0;
	}

	int i;
	for (i = 0; i < Technique::Number; i++) {
		if (!m_d3dxTechniques[i]) {
			continue;
		}

		if (m_object->IsParameterUsed(param, m_d3dxTechniques[i])) {
			break;
		}
	}

	if (i == Technique::Number) {
		return 0;
	} else {
		return param;
	}
}

void D3D9shader::setSystemMap(SamplerType maptype, D3D9texture *tex)
{
	AX_ASSERT(maptype >= 0 && maptype < SamplerType::NUMBER_ALL);

	m_samplerBound[maptype] = tex;
}

void D3D9shader::initPixelToTexel(D3DXHANDLE param)
{
	if (!isParameterUsed(param)) {
		return;
	}

	EffectHelper helper(m_object);

	String pixel_param_name = helper.getAnno(param, "pixelToTexel");
	if (pixel_param_name.empty()) {
		return;
	}

	D3DXHANDLE pixel_param = m_object->GetParameterByName(0, pixel_param_name.c_str());

	if (!pixel_param) {
		Errorf("not found pixel parameter for texel parameter");
	}

	D3DXPARAMETER_DESC texelDesc;
	D3DXPARAMETER_DESC pixelDesc;
	m_object->GetParameterDesc(param, &texelDesc);
	m_object->GetParameterDesc(pixel_param, &pixelDesc);

	D3D9pixel2texel p2t;
	p2t.m_name = texelDesc.Name;
	p2t.m_param = param;
	p2t.m_pixelValue.resize(pixelDesc.Bytes / sizeof(float));
	m_object->GetValue(pixel_param, &p2t.m_pixelValue[0], (UINT)p2t.m_pixelValue.size()*sizeof(float));

	pixel2Texels.push_back(p2t);
}

void D3D9shader::setPixelToTexel(int width, int height)
{
	if (width == m_p2tWidth && height == m_p2tHeight) {
		return;
	}

	float invwidth = 1.0f / width;
	float invheight = 1.0f / height;

	for (size_t i = 0; i < pixel2Texels.size(); i++) {
		D3D9pixel2texel &p2t = pixel2Texels[i]; 
		FloatSeq &data = p2t.m_scaledValue = p2t.m_pixelValue;

		int numvalue = s2i(data.size()) / 2;
		for (int j = 0; j < numvalue; j++) {
			data[j*2] *= invwidth;
			data[j*2+1] *= invheight;
		}

		m_object->SetValue(p2t.m_param, &data[0], numvalue * 2 * sizeof(float));
	}

	m_p2tWidth = width;
	m_p2tHeight = height;
}

bool D3D9shader::isParameterUsed(D3DXHANDLE param)
{
	if (!param) {
		return false;
	}

	int i;
	for (i = 0; i < Technique::Number; i++) {
		if (!m_d3dxTechniques[i]) {
			continue;
		}

		if (m_object->IsParameterUsed(param, m_d3dxTechniques[i])) {
			break;
		}
	}

	if (i == Technique::Number) {
		return false;
	} else {
		return true;
	}
}

void D3D9shader::initAxonObject()
{
	for (int i = 0; i < Technique::Number; i++) {
		if (!m_d3dxTechniques[i]) {
			m_techniques[i] = 0;
			continue;
		}

		m_object->SetTechnique(m_d3dxTechniques[i]);
		m_techniques[i] = new D3D9technique(this, m_d3dxTechniques[i]);
	}

}

void D3D9shader::setCoupled( Material *mtr )
{
	m_coupled = mtr;
}

UINT D3D9shader::begin( Technique tech )
{
	if (!m_techniques[tech])
		return 0;

	m_curTech = m_techniques[tech];
	return m_curTech->m_numPasses;
}

void D3D9shader::beginPass( UINT pass )
{
	m_curTech->m_passes[pass]->begin();
}

void D3D9shader::endPass()
{

}

void D3D9shader::end()
{

}

//--------------------------------------------------------------------------
// class D3D9shadermanager
//--------------------------------------------------------------------------

D3D9shadermanager::D3D9shadermanager()
{
	HRESULT hr;
	V(D3DXCreateEffectPool( &s_effectPool ));
	m_defaulted = new D3D9shader();
	bool v = m_defaulted->doInit("blend");
	AX_ASSERT(v);
}

D3D9shadermanager::~D3D9shadermanager()
{

}

Shader *D3D9shadermanager::findShader(const String &name, const ShaderMacro &macro)
{
	return findShader(FixedString(name), macro);
}

Shader *D3D9shadermanager::findShader( const FixedString &nameId, const ShaderMacro &macro )
{
	D3D9_SCOPELOCK;

	D3D9shader*& shader = m_shaderPool[nameId][macro];

	if (!shader) {
		shader = new D3D9shader();
		bool v = shader->doInit(nameId, macro);
		if (!v) {
			delete shader;
			shader = m_defaulted;
			shader->addref();
		}
	}

	return shader;
}

D3D9shader *D3D9shadermanager::findShaderDX(const String &name, const ShaderMacro &macro)
{
	D3D9_SCOPELOCK;

	return (D3D9shader*)findShader(name, macro);
}

void D3D9shadermanager::saveShaderCache( const String &name )
{
	String filename;
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
	ShaderPool::const_iterator it = m_shaderPool.begin();

	for (; it != m_shaderPool.end(); ++it) {
		const Dict<ShaderMacro,D3D9shader*>& shaders = it->second;
		Dict<ShaderMacro,D3D9shader*>::const_iterator it2 = shaders.begin();

		for (; it2 != shaders.end(); ++it2) {
			TiXmlElement *item = new TiXmlElement("cacheItem");
			item->SetAttribute("name", it->first.toString().c_str());
			item->SetAttribute("macro", it2->first.toString());
			root->LinkEndChild(item);
		}
	}

	doc.SaveFile(filename);
}

void D3D9shadermanager::applyShaderCache( const String &name )
{
	String filename;
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
	String shadername = item->Attribute("name");
	ShaderMacro macro;
	for (; item; item = item->NextSiblingElement()) {
		shadername = item->Attribute("name");
		macro.fromString(item->Attribute("macro"));

		findShader(shadername, macro);
	}
}

D3D9technique::D3D9technique( D3D9shader *shader, D3DXHANDLE d3dxhandle )
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
		m_passes[i] = new D3D9pass(shader, shader->m_object->GetPass(d3dxhandle, i));
		shader->m_object->EndPass();
	}
	shader->m_object->End();
}

D3D9technique::~D3D9technique()
{

}

D3D9pass::D3D9pass( D3D9shader *shader, D3DXHANDLE d3dxhandle )
{
	m_shader = shader;
	m_d3dxhandle = d3dxhandle;

	memset(m_matSamplers, -1, sizeof(m_matSamplers));

	initState();
	initVs();
	initPs();
}

D3D9pass::~D3D9pass()
{

}

void D3D9pass::initVs()
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
			if (g_uniforms.isVsregisterShared(constDesc.RegisterIndex))
				continue;

			ParamDesc paramDesc;
			paramDesc.d3dDesc = constDesc;
			paramDesc.p2t = 0;

			m_vsParameters[constDesc.Name] = paramDesc;
		}

		continue;
	}
}

void D3D9pass::initPs()
{
	HRESULT hr;

	D3DXPASS_DESC desc;
	V(m_shader->m_object->GetPassDesc(m_d3dxhandle, &desc));

	LPD3DXCONSTANTTABLE constTable;

#if 1
	V(D3DXGetShaderConstantTable(desc.pPixelShaderFunction, &constTable));
#else
	DWORD func[4*1024];
	UINT funcSize;
	m_ps->GetFunction(&func, &funcSize);
	AX_ASSERT(funcSize < 4*1024);
	V(D3DXGetShaderConstantTable(func, &constTable));
#endif
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
			if (g_uniforms.isPsregisterShared(constDesc.RegisterIndex))
				continue;

			ParamDesc paramDesc;
			paramDesc.d3dDesc = constDesc;
			paramDesc.p2t = findPixel2Texel(constDesc.Name);

			m_psParameters[constDesc.Name] = paramDesc;
		}

		continue;
	}
}


const D3D9pixel2texel*  D3D9pass::findPixel2Texel( const String &name )
{
	D3D9pixel2texels::const_iterator it = m_shader->pixel2Texels.begin();

	for (; it != m_shader->pixel2Texels.end(); ++it) {
		if (it->m_name == name)
			return &*it;
	}

	return 0;
}


void D3D9pass::initState()
{
#if 0
	HRESULT hr;
	V(d3d9Device->GetVertexShader(&m_vs));
	V(d3d9Device->GetPixelShader(&m_ps));

	V(d3d9Device->GetRenderState(D3DRS_ZENABLE, &m_depthTest));
	V(d3d9Device->GetRenderState(D3DRS_ZWRITEENABLE, &m_depthWrite));
	V(d3d9Device->GetRenderState(D3DRS_CULLMODE, &m_cullMode));
	V(d3d9Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &m_blendEnable));
	V(d3d9Device->GetRenderState(D3DRS_SRCBLEND, &m_blendSrc));
	V(d3d9Device->GetRenderState(D3DRS_DESTBLEND, &m_blendDst));
#else
	m_vs = d3d9StateManager->getVertexShader();
	m_ps = d3d9StateManager->getPixelShader();
	m_depthTest = d3d9StateManager->getRenderState(D3DRS_ZENABLE);
	m_depthWrite = d3d9StateManager->getRenderState(D3DRS_ZWRITEENABLE);
	m_cullMode = d3d9StateManager->getRenderState(D3DRS_CULLMODE);
	m_blendEnable = d3d9StateManager->getRenderState(D3DRS_ALPHABLENDENABLE);
	m_blendSrc = d3d9StateManager->getRenderState(D3DRS_SRCBLEND);
	m_blendDst = d3d9StateManager->getRenderState(D3DRS_DESTBLEND);
#endif
}

void D3D9pass::initSampler( const D3DXCONSTANT_DESC &desc )
{
	// check material sampler
	for (UINT i = 0; i < ArraySize(samplername); i++) {
		if (Strequ(samplername[i].paramname, desc.Name)) {
			m_matSamplers[samplername[i].type] = desc.RegisterIndex;
			return;
		}
	}

	// check system sampler
	for (int i = 0; i < Uniforms::NUM_UNIFORM_ITEMS; i++) {
		UniformItem &item = g_uniforms.getItem(i);

		if (item.m_valueType != UniformItem::vt_Texture)
			continue;

		if (Strequ(item.m_name, desc.Name)) {
			m_sysSamplers[i] = desc.RegisterIndex;
			return;
		}
	}

	// check batch sampler
	for (int i = 0; i < s2i(m_shader->m_samplerannSeq.size()); i++) {
		D3D9samplerann *bs = m_shader->m_samplerannSeq[i];
		if (bs->m_paramName == desc.Name) {
			D3D9samplerann *newbs = new D3D9samplerann();
			*newbs = *bs;
			newbs->m_register = desc.RegisterIndex;
			m_batchSamplers.push_back(newbs);
			return;
		}
	}

	// local sampler
//		m_psParameters[desc.Name] = desc;
}

void D3D9pass::begin()
{
	Material *mtr = m_shader->m_coupled;

	// set shader
	d3d9StateManager->SetVertexShader(m_vs);
	d3d9StateManager->SetPixelShader(m_ps);

	// set render state
	d3d9StateManager->SetRenderState(D3DRS_ZENABLE, m_depthTest);
	d3d9StateManager->SetRenderState(D3DRS_ZWRITEENABLE, m_depthWrite);
	d3d9StateManager->SetRenderState(D3DRS_CULLMODE, m_cullMode);
	d3d9StateManager->SetRenderState(D3DRS_ALPHABLENDENABLE, m_blendEnable);

	if (m_blendEnable) {
		d3d9StateManager->SetRenderState(D3DRS_SRCBLEND, m_blendSrc);
		d3d9StateManager->SetRenderState(D3DRS_DESTBLEND, m_blendDst);
	}

	setParameters();

	// set mtr sampler
	if (mtr) {
		for (int i = 0; i < SamplerType::NUMBER_ALL; i++) {
			if (m_matSamplers[i] == -1)
				continue;

			Texture *tex = mtr->getTexture(i);
			d3d9StateManager->setTexture(m_matSamplers[i], tex);
		}
	} else {
		for (int i = 0; i < SamplerType::NUMBER_ALL; i++) {
			if (m_matSamplers[i] == -1)
				continue;

			Texture *tex = m_shader->m_samplerBound[i];
			d3d9StateManager->setTexture(m_matSamplers[i], tex);
		}
	}

	// set sys sampler
	Dict<int,int>::const_iterator it = m_sysSamplers.begin();
	for (; it != m_sysSamplers.end(); ++it) {
		int f = it->first;
		int s = it->second;
		UniformItem &item = g_uniforms.getItem(f);
		Texture *tex = *(Texture**)item.m_datap;
		d3d9StateManager->setTexture(s, tex);
	}

	// set batch sampler
	int count = 0;
	for (size_t i = 0; i < m_batchSamplers.size(); i++) {
		D3D9samplerann *sa = m_batchSamplers[i];
		if (sa->m_renderType == SamplerAnno::Reflection) {
			if (!d3d9Interaction) {
				continue;
			}

			if (d3d9Interaction->numTargets < count) {
				continue;
			}

			Interaction *ia = d3d9Interaction;

			RenderTarget *target = ia->targets[count];
			D3D9target *textarget = (D3D9target*)target;
			D3D9texture *tex = textarget->getTextureDX();
			tex->setClampMode(Texture::CM_ClampToEdge);

			d3d9StateManager->setTexture(sa->m_register, tex);
		} else if (sa->m_renderType == SamplerAnno::SceneColor) {
			Rect r = d3d9BoundTarget->getRect();

			D3D9target *target = d3d9TargetManager->allocTargetDX(RenderTarget::TemporalAlloc, r.width, r.height, TexFormat::BGRA8);
			D3D9texture *tex = target->getTextureDX();
			tex->setClampMode(Texture::CM_ClampToEdge);
			target->copyFramebuffer(r);
			d3d9StateManager->setTexture(sa->m_register, tex);
		}
	}
}

void D3D9pass::setParameters()
{
	const ShaderParams *mtrparams = 0;
	if (m_shader->m_coupled) {
		mtrparams = &m_shader->m_coupled->getParameters();

		if (mtrparams->empty())
			mtrparams = 0;
	}

	// set constant
	for (Dict<String,ParamDesc>::const_iterator it = m_vsParameters.begin(); it != m_vsParameters.end(); ++it) {
		const ParamDesc &desc = it->second;
		const float *value = 0;

		if (mtrparams) {
			ShaderParams::const_iterator it2 = mtrparams->find(it->first);
			if (it2 != mtrparams->end()) {
				value = &it2->second[0];
			}
		}

		setParameter(desc, value, false);
	}

	for (Dict<String,ParamDesc>::const_iterator it = m_psParameters.begin(); it != m_psParameters.end(); ++it) {
		const ParamDesc &desc = it->second;
		const float *value = 0;

		if (mtrparams) {
			ShaderParams::const_iterator it2 = mtrparams->find(it->first);
			if (it2 != mtrparams->end()) {
				value = &it2->second[0];
			}
		}

		setParameter(desc, value, true);
	}
}

void D3D9pass::setParameter( const ParamDesc &param, const float *value, bool isPixelShader )
{
	const float *realvalue = (const float*)param.d3dDesc.DefaultValue;

	if (value)
		realvalue = value;

	// check pt2 first
	if (param.p2t && isPixelShader) {
		realvalue = &param.p2t->m_scaledValue[0];
	}

	if (!realvalue)
		return;

	if (isPixelShader) {
		d3d9Device->SetPixelShaderConstantF(param.d3dDesc.RegisterIndex, realvalue, param.d3dDesc.RegisterCount);
	} else {
		d3d9Device->SetVertexShaderConstantF(param.d3dDesc.RegisterIndex, realvalue, param.d3dDesc.RegisterCount);
	}
}

AX_END_NAMESPACE

