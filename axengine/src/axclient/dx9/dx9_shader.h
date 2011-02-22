/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_DX9_SHADER_H
#define AX_DX9_SHADER_H

AX_BEGIN_NAMESPACE

class DX9_Unknown : public IUnknown
{
public:
	DX9_Unknown() {}
	virtual ~DX9_Unknown() {}

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

class DX9_Shader;

//--------------------------------------------------------------------------

class DX9_SamplerInfo
{
public:
	std::string m_textureName;
	FixedString m_paramName;
	D3DXHANDLE m_param;
	int m_register;
};
typedef std::vector<DX9_SamplerInfo*>	DX9_SamplerInfos;

class DX9_Pass
{
	friend class DX9_Shader;

public:
	struct ParamDesc {
		int setflag;
		D3DXCONSTANT_DESC d3dDesc;
	};

	DX9_Pass(DX9_Shader *shader, D3DXHANDLE d3dxhandle);
	~DX9_Pass();

	void begin();

protected:
	void initVs();
	void initPs();
	void initSampler(const D3DXCONSTANT_DESC &desc);
	void setPrimitiveParameters();
	void setParameter(const FixedString &name, int numFloats, const float *data);

private:
	DX9_Shader *m_shader;
	D3DXHANDLE m_d3dxhandle;

	// shader
	IDirect3DVertexShader9 *m_vs;
	IDirect3DPixelShader9 *m_ps;

	// sys sampler
	int m_sysSamplers[GlobalTextureId::MaxType];

	// material sampler
	int m_matSamplers[MaterialTextureId::MaxType];

	// batch sampler
	DX9_SamplerInfos m_batchSamplers;

	// local parameter
	int m_setflag;
	Dict<FixedString, ParamDesc> m_parameters;
};

class DX9_Technique
{
	friend class DX9_Shader;

public:
	DX9_Technique(DX9_Shader *shader, D3DXHANDLE d3dxhandle);
	~DX9_Technique();

private:
	enum {MAX_PASSES = 8};
	DX9_Shader *m_shader;
	D3DXHANDLE m_d3dxhandle;

	int m_numPasses;
	DX9_Pass *m_passes[MAX_PASSES];
};

//--------------------------------------------------------------------------

class DX9_Shader : public DX9_Unknown
{
	friend class DX9_Pass;
	friend class DX9_Technique;
	friend class DX9_ShaderManager;

public:
	DX9_Shader();
	~DX9_Shader();

	// implement Shader
	bool init(const FixedString &name, const GlobalMacro &gm, const MaterialMacro &mm);
	ID3DXEffect *getObject() const { return m_object; }

	UINT begin(Technique tech);
	void beginPass(UINT pass);
	void endPass();
	void end();

	static bool isGlobalReg(int reg) { return reg >= SCENECONST_REG && reg < PRIMITIVECONST_REG; }
	static bool isPrimitiveReg(int reg) { return reg >= PRIMITIVECONST_REG; }

protected:
	void initTechniques();
	void initFeatures();

	void initAnnotation();
	void initSamplerAnn(D3DXHANDLE param);
	void initAxonObject();

	void initShaderInfo();

	D3DXHANDLE findTechnique(Technique tech);
	D3DXHANDLE getUsedParameter(const char *name);
	bool isParameterUsed(D3DXHANDLE param);
	bool isGlobalTextureUsed(GlobalTextureId id) const;
	bool isMaterialTextureUsed(MaterialTextureId id) const;

private:
	LPD3DXEFFECT m_object;              // Effect object
	FixedString m_key;

	D3DXHANDLE m_d3dxTechniques[Technique::MaxType];
	D3DXHANDLE m_curTechnique;

	DX9_SamplerInfos m_samplerInfos;

	// pixel2texel
	DX9_Technique *m_techniques[Technique::MaxType];
	DX9_Technique *m_curTech;

	// shader info
	ShaderInfo m_shaderInfo;
};

//--------------------------------------------------------------------------

class DX9_ShaderManager
{
	friend class DX9_Shader;

public:
	DX9_ShaderManager();
	~DX9_ShaderManager();

	DX9_Shader *findShader(const FixedString &nameId, const GlobalMacro &gm, const MaterialMacro &mm);
	void saveShaderCache(const std::string &name);
	void applyShaderCache(const std::string &name);

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
	typedef Dict<ShaderKey, DX9_Shader*> ShaderDict;
	ShaderDict m_shaders;
	DX9_Shader *m_defaulted;
	ShaderInfoDict m_shaderInfoDict;
};


AX_END_NAMESPACE


#endif // end guardian

