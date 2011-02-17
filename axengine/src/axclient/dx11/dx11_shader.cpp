#include "dx11_private.h"

AX_BEGIN_NAMESPACE

extern LPD3DX11COMPILEFROMMEMORY dx11_D3DX11CompileFromMemory;

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
	dx11_D3DX11CompileFromMemory((LPCSTR)ioRequest.fileData(),
		ioRequest.fileSize(), fullname.c_str(), &d3dxmacros[0], &d3dInc,
		0, "fx_5_0", shaderFlags, effectFlags, 0, &shaderBlob, &errMsgBlob, &hr);

	if (FAILED(hr)) {
		if (errMsgBlob) {
			Errorf("%s", errMsgBlob->GetBufferPointer());
		}
		return;
	}

	// D3DX11CreateEffectFromMemory
}

DX11_Shader::~DX11_Shader()
{

}

DX11_ShaderManager::DX11_ShaderManager()
{
	dx11_shaderManager = this;
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

AX_END_NAMESPACE
