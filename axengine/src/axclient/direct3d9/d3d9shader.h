/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9SHADER_H
#define AX_D3D9SHADER_H

#define AX_SU(x, y) D3D9uniform::setUniform(Uniforms::x,y);

AX_BEGIN_NAMESPACE

	class D3D9shader;

	//--------------------------------------------------------------------------
	// class UniformCache
	//--------------------------------------------------------------------------

	class D3D9uniform : public UniformItem {
	public:
		D3D9uniform(UniformItem& item, D3DXHANDLE param);
		virtual ~D3D9uniform();

		bool isCached() const;
		void cache();

		template< class Q >
		static void setUniform(Uniforms::ItemName itemname, const Q& q) {
			UniformItem& item = g_uniforms.getItem(itemname);

			if (item.m_valueType == UniformItem::vt_Texture) {
				item.set(q);
				return;
			}

			if (memcmp(&q, item.m_datap, item.m_dataSize) == 0)
				return;

			item.set(q);

			setUniform(item, &q);
		}

		static void setUniform(UniformItem& item, const void* q);

	public:
		UniformItem* m_src;
		D3DXHANDLE m_param;
	};

	//--------------------------------------------------------------------------
	// class CGsamplerann
	//--------------------------------------------------------------------------

	class D3D9samplerann : public SamplerAnno {
	public:
		String m_paramName;
		D3DXHANDLE m_param;
		int m_register;
	};
	typedef Sequence<D3D9samplerann*>	D3D9sampleranns;

	//--------------------------------------------------------------------------
	// class D3D9pixeltotexel
	//--------------------------------------------------------------------------

	class D3D9pixel2texel {
	public:
		String m_name;
		D3DXHANDLE m_param;
		FloatSeq m_pixelValue;
		FloatSeq m_scaledValue;
	};
	typedef Sequence<D3D9pixel2texel>	D3D9pixel2texels;

	class D3D9pass {
	public:
		friend class D3D9shader;
		
		struct ParamDesc {
			D3DXCONSTANT_DESC d3dDesc;
			const D3D9pixel2texel* p2t;
		};

		D3D9pass(D3D9shader* shader, D3DXHANDLE d3dxhandle);
		~D3D9pass();

		void begin();

	protected:
		void initVs();
		void initPs();
		void initState();
		void initSampler(const D3DXCONSTANT_DESC& desc);

		const D3D9pixel2texel* findPixel2Texel(const String& name);
		void setParameters();
		void setParameter(const ParamDesc& param, const float* value, bool isPixelShader);

	private:
		D3D9shader* m_shader;
		D3DXHANDLE m_d3dxhandle;

		// shader
		IDirect3DVertexShader9* m_vs;
		IDirect3DPixelShader9* m_ps;

		// render state
		DWORD m_depthTest;
		DWORD m_depthWrite;
		DWORD m_cullMode;
		DWORD m_blendEnable;
		DWORD m_blendSrc;
		DWORD m_blendDst;

		// material sampler
		int m_matSamplers[SamplerType::NUMBER_ALL];

		// sys sampler
		Dict<int,int> m_sysSamplers;

		// batch sampler
		D3D9sampleranns m_batchSamplers;

		// local parameter
		Dict<String,ParamDesc> m_vsParameters;
		Dict<String,ParamDesc> m_psParameters;
	};

	class D3D9technique {
	public:
		friend class D3D9shader;

		D3D9technique(D3D9shader* shader, D3DXHANDLE d3dxhandle);
		~D3D9technique();

	private:
		enum {MAX_PASSES = 8};
		D3D9shader* m_shader;
		D3DXHANDLE m_d3dxhandle;

		int m_numPasses;
		D3D9pass* m_passes[MAX_PASSES];
	};

	//--------------------------------------------------------------------------
	// class D3D9shader
	//--------------------------------------------------------------------------


	class D3D9shader : public Shader {
	public:
		friend class D3D9pass;
		friend class D3D9technique;

		D3D9shader();
		virtual ~D3D9shader();

		// implement Shader
		virtual bool doInit(const String& name, const ShaderMacro& macro = g_shaderMacro);
		virtual bool isDepthWrite() const;
		virtual bool haveTextureTarget() const;
		virtual int getNumSampler() const;
		virtual SamplerAnno* getSamplerAnno(int index) const;
		virtual int getNumTweakable() const;
		virtual ParameterAnno* getTweakableDef(int index);
		virtual SortHint getSortHint() const;
		virtual bool haveTechnique(Technique tech) const;

		void setSystemMap(SamplerType maptype, D3D9texture* tex);
		// set pixel to texel conversion paramter
		void setPixelToTexel(int width, int height);

		void setCoupled(Material* mtr);

		ID3DXEffect* getObject() const { return m_object; }

		UINT begin(Technique tech);
		void beginPass(UINT pass);
		void endPass();
		void end();

	protected:
		void initTechniques();
		void initFeatures();
		void initSortHint();

		void initAnnotation();
		void initSamplerAnn(D3DXHANDLE param);
		void initParameterAnn(D3DXHANDLE param);
		void initPixelToTexel(D3DXHANDLE param);

		void initAxonObject();

		D3DXHANDLE findTechnique(Technique tech);
		D3DXHANDLE getUsedParameter(const char* name);
		bool isParameterUsed(D3DXHANDLE param);

	private:
		LPD3DXEFFECT m_object;              // Effect object
		String m_keyString;
		SortHint m_sortHint;

		D3DXHANDLE m_d3dxTechniques[Technique::Number];
		D3D9texture* m_samplerBound[SamplerType::NUMBER_ALL];
		D3DXHANDLE m_curTechnique;

		bool m_haveTextureTarget;

		D3D9sampleranns m_samplerannSeq;

		// pixel2texel
		D3D9pixel2texels pixel2Texels;
		int m_p2tWidth, m_p2tHeight;

		D3D9technique* m_techniques[Technique::Number];
		D3D9technique* m_curTech;
		Material* m_coupled;
	};

	//--------------------------------------------------------------------------
	// class D3D9shadermanager
	//--------------------------------------------------------------------------

	class D3D9shadermanager : public ShaderManager {
	public:
		D3D9shadermanager();
		virtual ~D3D9shadermanager();

		virtual Shader* findShader(const String& name, const ShaderMacro& macro = g_shaderMacro);
		virtual Shader* findShader(const FixedString& nameId, const ShaderMacro& macro);
		virtual void saveShaderCache(const String& name);
		virtual void applyShaderCache(const String& name);

		D3D9shader* findShaderDX(const String& name, const ShaderMacro& macro = g_shaderMacro);

	private:
		typedef Dict<FixedString,Dict<ShaderMacro,D3D9shader*> > ShaderPool;
		ShaderPool m_shaderPool;
		D3D9shader* m_defaulted;
	};


AX_END_NAMESPACE


#endif // end guardian

