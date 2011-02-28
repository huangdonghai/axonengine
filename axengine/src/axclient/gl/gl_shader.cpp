#include "gl_private.h"

AX_BEGIN_NAMESPACE

GL_Shader::GL_Shader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm)
{
	m_key = name;
	std::string fullname = "shaders/" + name.toString() + ".fx";
	fullname = g_fileSystem->dataPathToOsPath(fullname);

	std::vector<std::string> strArgs;
	std::vector<const char *> args;

	StringPairSeq definesG  = gm.getDefines();
	for (size_t i = 0; i < definesG.size(); i++) {
		std::string str;
		StringUtil::sprintf(str, "-D%s=%s", definesG[i].first.c_str(), definesG[i].second.c_str());
		strArgs.push_back(str);
	}

	StringPairSeq definesM  = mm.getDefines();
	for (size_t i = 0; i < definesM.size(); i++) {
		std::string str;
		StringUtil::sprintf(str, "-D%s=%s", definesM[i].first.c_str(), definesM[i].second.c_str());
		strArgs.push_back(str);
	}

	std::vector<std::string>::const_iterator it = strArgs.begin();
	for (; it != strArgs.end(); ++it) {
		args.push_back(it->c_str());
	}
	args.push_back(0);

#if 0
	IoRequest ioRequest(fullname, 0);
	g_fileSystem->syncRead(&ioRequest);

	AX_RELEASE_ASSERT(ioRequest.fileData() && ioRequest.fileSize());

	m_effect = cgCreateEffect(cg_context, (const char *)ioRequest.fileData(), &args[0]);
#else
	m_effect = cgCreateEffectFromFile(cg_context, fullname.c_str(), &args[0]);
#endif
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR) {
		if (error == CG_COMPILER_ERROR) {
			Errorf("Error: %s\n\n" "Cg compiler output...\n%s", string, cgGetLastListing(cg_context));
		} else {
			Errorf("Cg Error: %s", string);
		}
		AX_WRONGPLACE;
	}

	CGparameter param = cgGetNamedEffectParameter(m_effect, "g_time");
	CGbuffer buffer = cgGetEffectParameterBuffer(param);
	int bufferSize = cgGetBufferSize(buffer);

	param = cgGetNamedEffectParameter(m_effect, "g_modelMatrix");
	buffer = cgGetEffectParameterBuffer(param);
	bufferSize = cgGetBufferSize(buffer);
}

GL_Shader::~GL_Shader()
{

}

static void CGIncludeCallback(CGcontext context, const char *filename)
{
	std::string fullname = "shaders/";
	fullname += filename;
	void *filedata;
	size_t filesize = g_fileSystem->readFile(fullname.c_str(), &filedata);
	if (!filesize || !filedata) return;

	cgSetCompilerIncludeString(context, filename, (const char *)filedata);
}


GL_ShaderManager::GL_ShaderManager()
{
	gl_shaderManager = this;

	//cgSetCompilerIncludeCallback(cg_context, &CGIncludeCallback);

	TiXmlDocument doc;
	doc.LoadAxonFile("shaders/shaderlist.xml");
	AX_RELEASE_ASSERT(!doc.Error());

	TiXmlNode *root = doc.FirstChild("shaderlist");

	// no root
	AX_ASSERT(root);

	TiXmlElement *section;
	for (section = root->FirstChildElement("item"); section; section = section->NextSiblingElement("item")) {
		findShader(section->GetText(), g_globalMacro, MaterialMacro());
	}
}

GL_ShaderManager::~GL_ShaderManager()
{

}

GL_Shader *GL_ShaderManager::findShader(const FixedString &nameId, const GlobalMacro &gm, const MaterialMacro &mm)
{
	ShaderKey key;
	key.nameId = nameId.id();
	key.gm = gm.id();
	key.mm = mm.id();

	GL_Shader*& shader = m_shaders[key];

	if (!shader) {
		shader = new GL_Shader(nameId, gm, mm);
	}

	return shader;
}

const ShaderInfo *GL_ShaderManager::findShaderInfo(const FixedString &key)
{
	Dict<FixedString, ShaderInfo *>::const_iterator it = m_shaderInfoDict.find(key);

	if (it != m_shaderInfoDict.end())
		return it->second;

	AX_WRONGPLACE;
	return 0;
}

void GL_ShaderManager::addShaderInfo(const FixedString &key, ShaderInfo *shaderInfo)
{
	if (m_shaderInfoDict.find(key) != m_shaderInfoDict.end())
		return;

	m_shaderInfoDict[key] = shaderInfo;
}

AX_END_NAMESPACE
