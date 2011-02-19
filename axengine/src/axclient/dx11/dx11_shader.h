#ifndef AX_DX11_SHADER_H
#define AX_DX11_SHADER_H

AX_DX11_BEGIN_NAMESPACE

class DX11_Unknown : public IUnknown
{
public:
	DX11_Unknown() {}
	virtual ~DX11_Unknown() {}

	// methods inherited from ID3DXEffectStateManager
	STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv)
	{
		if (iid == IID_IUnknown) {
			*ppv = this;
		} else {
			*ppv = NULL;
			return E_NOINTERFACE;
		}

		AddRef();
		return S_OK;
	}
	STDMETHOD_(ULONG, AddRef)(THIS)
	{
		return(ULONG)InterlockedIncrement(&m_ref);
	}

	STDMETHOD_(ULONG, Release)(THIS)
	{
		if (0L == InterlockedDecrement(&m_ref)) {
			delete this;
			return 0L;
		}

		return m_ref;
	}
private:
	volatile LONG m_ref;
};

class DX11_Shader;

//--------------------------------------------------------------------------

class DX11_SamplerInfo
{
public:
	std::string m_textureName;
	FixedString m_paramName;
	int m_register;
};
typedef std::vector<DX11_SamplerInfo*>	DX11_SamplerInfos;

class DX11_Pass
{
	friend class DX11_Shader;

public:
	struct ParamDesc {
		int setflag;
		D3D11_SHADER_VARIABLE_DESC d3dDesc;
	};

	DX11_Pass(DX11_Shader *shader, ID3DX11EffectPass *d3dxpass);
	~DX11_Pass();

	void begin();

protected:
	void initVs();
	void initPs();
	void initSampler(const D3D11_SHADER_VARIABLE_DESC &desc);
	void setInputLayout();
	void setPrimitiveParameters();
	void setParameter(const ParamDesc &param, const float *value, bool isPixelShader);
	void setParameter(const FixedString &name, int numFloats, const float *data);

private:
	DX11_Shader *m_shader;
	ID3DX11EffectPass *m_d3dxpass;

	// shader
	ID3D11VertexShader *m_vs;
	ID3D11PixelShader *m_ps;

	// sys sampler
	int m_sysSamplers[GlobalTextureId::MaxType];

	// material sampler
	int m_matSamplers[MaterialTextureId::MaxType];

	// batch sampler
	DX11_SamplerInfos m_batchSamplers;

	// local parameter
	int m_setflag;
	Dict<FixedString, ParamDesc> m_parameters;

	// input layout
	ID3D11InputLayout *m_inputLayouts[VertexType::MaxType];
	ID3D11InputLayout *m_inputLayoutsInstanced[VertexType::MaxType];
};

class DX11_Technique
{
	friend class DX11_Shader;

public:
	DX11_Technique(DX11_Shader *shader, ID3DX11EffectTechnique *d3dxhandle);
	~DX11_Technique();

private:
	enum { MAX_PASSES = 8 };
	DX11_Shader *m_shader;
	ID3DX11EffectTechnique *m_d3dxhandle;

	int m_numPasses;
	DX11_Pass *m_passes[MAX_PASSES];
};

//--------------------------------------------------------------------------

class DX11_Shader : public DX11_Unknown
{
	friend class DX11_Pass;
	friend class DX11_Technique;
	friend class DX11_ShaderManager;

public:
	DX11_Shader(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm);
	~DX11_Shader();

	// implement Shader
	ID3DX11Effect *getObject() const { return m_object; }

	UINT begin(Technique tech) { return 0; }
	void beginPass(UINT pass) {}
	void endPass() {}
	void end() {}

	static bool isGlobalReg(int reg) { return reg >= SCENECONST_REG && reg < PRIMITIVECONST_REG; }
	static bool isPrimitiveReg(int reg) { return reg >= PRIMITIVECONST_REG; }

protected:
	void initTechniques();
	void initFeatures();
	void initAxonObject();
	void initShaderInfo();

	ID3DX11EffectTechnique *findTechnique(Technique tech);
//	D3DXHANDLE getUsedParameter(const char *name);
//	bool isParameterUsed(D3DXHANDLE param);
	bool isGlobalTextureUsed(GlobalTextureId id) const;
	bool isMaterialTextureUsed(MaterialTextureId id) const;

private:
	ID3DX11Effect *m_object;              // Effect object
	FixedString m_key;

	ID3DX11EffectTechnique *m_d3dxTechniques[Technique::MaxType];
	ID3D11ShaderResourceView *m_samplerBound[MaterialTextureId::MaxType];
	ID3DX11EffectTechnique *m_curTechnique;

	DX11_SamplerInfos m_samplerInfos;

	DX11_Technique *m_techniques[Technique::MaxType];
	DX11_Technique *m_curTech;

	// shader info
	ShaderInfo m_shaderInfo;
};

//--------------------------------------------------------------------------

class DX11_ShaderManager
{
	friend class DX11_Shader;

public:
	DX11_ShaderManager();
	~DX11_ShaderManager();

	DX11_Shader *findShader(const FixedString &nameId, const GlobalMacro &gm, const MaterialMacro &mm);
	const ShaderInfo *findShaderInfo(const FixedString &key);

protected:
	void _initialize();
	void addShaderInfo(const FixedString &key, ShaderInfo *shaderInfo);

private:
	struct ShaderKey {
		int nameId;
		int gm;
		int mm;

		operator size_t() const { size_t result = nameId; hash_combine(result, gm); hash_combine(result, mm); return result; }
	};
	typedef Dict<ShaderKey, DX11_Shader*> ShaderDict;
	ShaderDict m_shaders;
	ShaderInfoDict m_shaderInfoDict;
};

AX_DX11_END_NAMESPACE

#endif // AX_DX11_SHADER_H
