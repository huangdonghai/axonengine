#include "dx11_private.h"

AX_DX11_BEGIN_NAMESPACE

DX11_Pass::DX11_Pass(DX11_Shader *shader, ID3DX11EffectPass *d3dxpass)
{
	m_shader = shader;
	m_d3dxpass = d3dxpass;
	m_setflag = 0;

	memset(m_sysSamplers, -1, sizeof(m_sysSamplers));
	memset(m_matSamplers, -1, sizeof(m_matSamplers));

	TypeZeroArray(m_inputLayouts);
	TypeZeroArray(m_inputLayoutsInstanced);

	initVs();
	initPs();
}

DX11_Pass::~DX11_Pass()
{
}

void DX11_Pass::initVs()
{
	D3DX11_PASS_SHADER_DESC psd;

	V(m_d3dxpass->GetVertexShaderDesc(&psd));
	AX_ASSERT(psd.ShaderIndex);

	D3DX11_EFFECT_SHADER_DESC esd;
	ID3D11ShaderReflection *reflection = 0;
	V(psd.pShaderVariable->GetShaderDesc(0, &esd));
	V(D3DReflect(esd.pBytecode, esd.BytecodeLength, IID_ID3D11ShaderReflection, (void **)&reflection));
}

void DX11_Pass::initPs()
{

}

void DX11_Pass::setInputLayout()
{
	ID3D11InputLayout **ils = m_inputLayouts;

	if (g_curInstanced)
		ils = m_inputLayoutsInstanced;

	ID3D11InputLayout *il = 0;
	if (ils[g_curVertexType]) {
		il = ils[g_curVertexType];
	} else {
		D3DX11_PASS_DESC desc;
		m_d3dxpass->GetDesc(&desc);
		il = g_stateManager->findInputLayout(g_curVertexType, g_curInstanced, desc.pIAInputSignature, desc.IAInputSignatureSize);
		ils[g_curVertexType] = il;
	}

	g_context->IASetInputLayout(il);
}






DX11_Technique::DX11_Technique(DX11_Shader *shader, ID3DX11EffectTechnique *d3dxhandle)
{
	m_shader = shader;
	m_d3dxhandle = d3dxhandle;

	HRESULT hr;

	D3DX11_TECHNIQUE_DESC desc;
	V(d3dxhandle->GetDesc(&desc));

	m_numPasses = desc.Passes;

	for (int i = 0; i < m_numPasses; i++) {
		m_passes[i] = new DX11_Pass(shader, d3dxhandle->GetPassByIndex(i));
	}
}

DX11_Technique::~DX11_Technique()
{

}



//extern LPD3DX11COMPILEFROMMEMORY dx11_D3DX11CompileFromMemory;

DX11_Shader::DX11_Shader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm)
{
	class DX11_Include : public ID3D10Include
	{
	public:
		STDMETHOD(Open)(THIS_ D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
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

	m_key = name;
	std::string fullname = "shaders/" + name.toString() + ".fx";

	std::vector<D3D10_SHADER_MACRO> d3dxmacros;
	D3D10_SHADER_MACRO d3dxmacro;

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

	IoRequest ioRequest(0, fullname);
	g_fileSystem->syncRead(&ioRequest);

	AX_ASSURE(ioRequest.fileData() && ioRequest.fileSize());

	DX11_Include d3dInc;
	UINT shaderFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_NO_PRESHADER;
	UINT effectFlags = 0;
	ID3D10Blob *shaderBlob = 0;
	ID3D10Blob *errMsgBlob = 0;

	HRESULT hr;
	D3DX11CompileFromMemory((LPCSTR)ioRequest.fileData(),
		ioRequest.fileSize(), fullname.c_str(), &d3dxmacros[0], &d3dInc,
		0, "fx_5_0", shaderFlags, effectFlags, 0, &shaderBlob, &errMsgBlob, &hr);

	if (FAILED(hr)) {
		if (errMsgBlob) {
			Errorf("%s", errMsgBlob->GetBufferPointer());
		}
		return;
	}

	// D3DX11CreateEffectFromMemory
	hr = D3DX11CreateEffectFromMemory(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 0, g_device, &m_object);
	if (FAILED(hr)) {
		Errorf("can't create effect");
		return;
	}

	// init techniques
	initTechniques();

	// init shader features
	initFeatures();

	// init axon object
	initAxonObject();

	// init shader info
	initShaderInfo();
}

DX11_Shader::~DX11_Shader()
{}

void DX11_Shader::initTechniques()
{
	for (int i = 0; i < Technique::MaxType; i++) {
		m_d3dxTechniques[i] = findTechnique((Technique::Type)i);
	}
}

void DX11_Shader::initFeatures()
{

}

void DX11_Shader::initAxonObject()
{
	for (int i = 0; i < Technique::MaxType; i++) {
		if (!m_d3dxTechniques[i]) {
			m_techniques[i] = 0;
			continue;
		}

		m_techniques[i] = new DX11_Technique(this, m_d3dxTechniques[i]);
	}
}

void DX11_Shader::initShaderInfo()
{
	for (int i = 0; i < Technique::MaxType; i++)
		m_shaderInfo.m_haveTechnique[i] = m_techniques[i] != 0;

	m_shaderInfo.m_needReflection = isMaterialTextureUsed(MaterialTextureId::Reflection);
	m_shaderInfo.m_needSceneColor = isGlobalTextureUsed(GlobalTextureId::SceneColor);

	// add to shader manager
	g_shaderManager->addShaderInfo(m_key, &m_shaderInfo);
}

ID3DX11EffectTechnique *DX11_Shader::findTechnique(Technique tech)
{
	ID3DX11EffectVariable *var = m_object->GetVariableByName("Script");

	if (!var || !var->IsValid()) return 0;

	ID3DX11EffectVariable *anno = var->GetAnnotationByName(tech.toString().c_str());
	if (!anno || !anno->IsValid()) return 0;

	const char *techName = 0;
	V(anno->AsString()->GetString(&techName));

	if (!techName || !techName[0]) return 0;

	ID3DX11EffectTechnique *result = m_object->GetTechniqueByName(techName);

	if (!result || !result->IsValid()) return 0;
	return result;
}

bool DX11_Shader::isGlobalTextureUsed(GlobalTextureId id) const
{
	return false;
}

bool DX11_Shader::isMaterialTextureUsed(MaterialTextureId id) const
{
	return false;
}



DX11_ShaderManager::DX11_ShaderManager()
{
	g_shaderManager = this;
	_initialize();
}

DX11_ShaderManager::~DX11_ShaderManager()
{

}

DX11_Shader * DX11_ShaderManager::findShader(const FixedString &nameId, const GlobalMacro &gm, const MaterialMacro &mm)
{
	ShaderKey key;
	key.nameId = nameId.id();
	key.gm = gm.id();
	key.mm = mm.id();

	DX11_Shader*& shader = m_shaders[key];

	shader = new DX11_Shader(nameId, gm, mm);

	return shader;
}

const ShaderInfo * DX11_ShaderManager::findShaderInfo(const FixedString &key)
{
	ShaderInfoDict::const_iterator it = m_shaderInfoDict.find(key);

	if (it != m_shaderInfoDict.end())
		return it->second;

	AX_WRONGPLACE;
	return 0;
}

void DX11_ShaderManager::_initialize()
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
}

void DX11_ShaderManager::addShaderInfo( const FixedString &key, ShaderInfo *shaderInfo )
{
	if (m_shaderInfoDict.find(key) != m_shaderInfoDict.end())
		return;

	m_shaderInfoDict[key] = shaderInfo;
}

AX_DX11_END_NAMESPACE
