/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9TEXTURE_H
#define AX_D3D9TEXTURE_H

AX_BEGIN_NAMESPACE

	class D3D9texture : public Texture {
	public:
		friend class D3D9texturemanager;

		D3D9texture();
		virtual ~D3D9texture();

		// implement Asset
		virtual bool doInit(const String& name, intptr_t arg);
		virtual void initialize(const FixedString& name, InitFlags flags);
		virtual void initialize(TexFormat format, int width, int height, InitFlags flags = 0);
		virtual bool isInitialized() { return m_initialized; }
		virtual void getSize(int& width, int& height, int& depth);
		virtual void setClampMode(ClampMode clampmode);
		virtual void setFilterMode(FilterMode filtermode);
		virtual void setBorderColor(const Rgba& color);
		virtual void setHardwareShadowMap(bool enable);
		virtual TexFormat getFormat();

		virtual void saveToFile(const String& filename);
		virtual void uploadSubTextureIm(const Rect& rect, const void* pixels, TexFormat format = TexFormat::AUTO);
		virtual void generateMipmapIm();

		// d3d specified
		void getSize(int& width, int& height);
		IDirect3DTexture9* getObject() { return m_object; }
		FilterMode getFilterMode() { return m_filterMode; }
		ClampMode getClampMode() { return m_clampMode; }
		void copyFramebuffer(const Rect& r);
		bool isHardwareShadowMap() const { return m_hardwareShadowMap; }

		// set sampler state to device
		void issueSamplerState(DWORD dwStage);

		// manager
		static void initManager();
		static void finalizeManager();
		static D3D9texture* getAppTexture(LPDIRECT3DBASETEXTURE9 d3dtex);
		static void syncFrame();
		// end manager

	protected:
		bool loadFile2D(const String& filename);
		void setPrivateData();
		// Ya..., I have a loooooongest function name. who can write a longer?
		bool checkIfSupportHardwareMipmapGeneration(D3DFORMAT d3dformat, DWORD d3dusage);
		void uploadSubTextureIm(int level, const Rect& rect, const void* pixels, TexFormat format = TexFormat::AUTO);

	private:
		bool m_initialized;
		InitFlags m_initFlags;
		TexFormat m_format;
		LPDIRECT3DTEXTURE9 m_object;
		bool m_dataLoaded;		// Is data have loaded?
		int m_width;
		int m_height;
		int m_depth;
		bool m_isMipmaped;
		void* m_compressedData;
		bool m_isLoadFromMemory;
		uint_t m_videoMemoryUsed;
		bool m_hardwareGenMipmap;

		// sampler state
		FilterMode m_filterMode;
		ClampMode m_clampMode;
		D3DCOLOR m_borderColor;
		bool m_hardwareShadowMap;
	};

	class D3D9texturemanager : public TextureManager, public ICmdHandler {
	public:
		AX_DECLARE_COMMAND_HANDLER(D3D9texturemanager);

		D3D9texturemanager();
		virtual ~D3D9texturemanager();

		void addToDict(LPDIRECT3DBASETEXTURE9 d3dtex, D3D9texture* tex);
		void removeFromDict(LPDIRECT3DBASETEXTURE9 d3dtex);
		D3D9texture* getTex(LPDIRECT3DBASETEXTURE9 d3dtex) const;

		void syncFrame();

		// implement TextureManager
		virtual TextureRp createObject();

	protected:
		// console command
		void dumpTex_f(const CmdArgs& param);

	private:
		Dict<LPDIRECT3DBASETEXTURE9, D3D9texture*> m_texDict;
	};

	inline void D3D9texturemanager::addToDict( LPDIRECT3DBASETEXTURE9 d3dtex, D3D9texture* tex )
	{
		m_texDict[d3dtex] = tex;
	}

	inline D3D9texture* D3D9texturemanager::getTex( LPDIRECT3DBASETEXTURE9 d3dtex ) const
	{
		Dict<LPDIRECT3DBASETEXTURE9, D3D9texture*>::const_iterator it = m_texDict.find(d3dtex);

		if (it == m_texDict.end())
			return 0;

		return it->second;
	}

	inline void D3D9texturemanager::removeFromDict( LPDIRECT3DBASETEXTURE9 d3dtex )
	{
		Dict<LPDIRECT3DBASETEXTURE9, D3D9texture*>::const_iterator it = m_texDict.find(d3dtex);

		if (it == m_texDict.end())
			return;

		m_texDict.erase(it);
	}

AX_END_NAMESPACE


#endif // end guardian

