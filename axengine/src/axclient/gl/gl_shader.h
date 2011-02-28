#ifndef AX_GL_SHADER_H
#define AX_GL_SHADER_H

AX_BEGIN_NAMESPACE

class GL_Shader
{
public:
	GL_Shader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm);
	~GL_Shader();

private:
	FixedString m_key;
	CGeffect m_effect;
};

class GL_ShaderManager
{
public:
	GL_ShaderManager();
	~GL_ShaderManager();

	GL_Shader *findShader(const FixedString &nameId, const GlobalMacro &gm, const MaterialMacro &mm);
	const ShaderInfo *findShaderInfo(const FixedString &key);

protected:
	void addShaderInfo(const FixedString &key, ShaderInfo *shaderInfo);

private:
	struct ShaderKey {
		int nameId;
		int gm;
		int mm;

		operator size_t() const { size_t result = nameId; hash_combine(result, gm); hash_combine(result, mm); return result; }
	};
	typedef Dict<ShaderKey, GL_Shader*> ShaderDict;
	ShaderDict m_shaders;
	Dict<FixedString, ShaderInfo *> m_shaderInfoDict;
};

AX_END_NAMESPACE

#endif // AX_GL_SHADER_H
