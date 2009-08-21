/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9_STATEMANAGER_H
#define AX_D3D9_STATEMANAGER_H

namespace Axon { namespace Render {

	extern IDirect3DDevice9* d3d9Device;


	template <typename _Kty, typename _Ty>
	class multicache {
	protected:
		Dict<_Kty, _Ty> cache;         // A map provides a fast look-up of contained states
		// and furthermore ensures that duplicate key values
		// are not present.
		// Additionally, dirtying the cache can be done by
		// clear()ing the container.
	public:
		// Command to dirty all cached values
		inline void dirtyall() {
			cache.clear();
		}

		// Command to dirty one key value
		inline void dirty(_Kty key) {
			Dict<_Kty, _Ty>::iterator it = cache.find(key);
			if (cache.end() != it)
				cache.erase(it);
		}

		// Called to update the cache
		// The return value indicates whether or not the update was a redundant change.
		// A value of 'true' indicates the new state was unique, and must be submitted
		// to the D3D Runtime.
		inline bool set_val(_Kty key, _Ty value) {
			Dict<_Kty, _Ty>::iterator it = cache.find(key);
			if (cache.end() == it)
			{
				cache.insert(Dict<_Kty, _Ty>::value_type(key, value));
				return true;
			}
			if (it->second == value)
				return false;
			it->second = value;
			return true;
		}

		inline _Ty get_val(_Kty key) {
			Dict<_Kty, _Ty>::iterator it = cache.find(key);
			if (cache.end() == it)
			{
				return 0;
			}

			return it->second;
		}
	};

	class D3D9statemanager : public ID3DXEffectStateManager
	{
	public:
		enum {
			MAX_STAGES = 32
		};
		D3D9statemanager();

		virtual ~D3D9statemanager()
		{
		};       // virtual destructor, for cleanup purposes

		// More targeted 'Dirty' commands may be useful.
		void DirtyCachedValues()
		{
			cacheRenderStates.dirtyall();
#if 0
			std::vector <samplerStageCache>::iterator it_samplerStages;
			for (it_samplerStages = vecCacheSamplerStates.begin();
				it_samplerStages != vecCacheSamplerStates.end();
				it_samplerStages++)
				(*it_samplerStages).dirtyall();

			std::vector <textureStateStageCache>::iterator it_textureStages;
			for (it_textureStages = vecCacheTextureStates.begin();
				it_textureStages != vecCacheTextureStates.end();
				it_textureStages++)
				(*it_textureStages).dirtyall();
#endif
		}

		void EndFrameStats();
#if 0
		virtual DWORD getRenderState(D3DRENDERSTATETYPE state) { return 0; }
		virtual IDirect3DVertexShader9* getVertexShader() { return 0;}
		virtual IDirect3DPixelShader9* getPixelShader() { return 0; }
		virtual void setTexture(int stage, Texture* tex) {}
#endif
		// my wrapper
		HRESULT setDepthStencilSurface(IDirect3DSurface9* obj, TexFormat format);

		TexFormat getDepthStencilFormat() const { return m_depthStencilFormat; }
		void setVertexDeclaration(IDirect3DVertexDeclaration9* vertdecl);

		void setSamplerStateBlock(DWORD stage, Texture::ClampMode clampmode, Texture::FilterMode filtermode);

		void onReset();
		void onDeviceLost();
#if 0
		// Create a state manager interface for the device
		static CStateManagerInterface* Create(LPDIRECT3DDEVICE9 pDevice);
#endif
		// methods inherited from ID3DXEffectStateManager
		STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv)
		{
			if (iid == IID_IUnknown || iid == IID_ID3DXEffectStateManager)
			{
				*ppv = static_cast<ID3DXEffectStateManager*>(this);
			}
			else
			{
				*ppv = NULL;
				return E_NOINTERFACE;
			}

			reinterpret_cast<IUnknown*>(this)->AddRef();
			return S_OK;
		}
		STDMETHOD_(ULONG, AddRef)(THIS)
		{
			return(ULONG)InterlockedIncrement(&m_lRef);
		}
		STDMETHOD_(ULONG, Release)(THIS)
		{
			if (0L == InterlockedDecrement(&m_lRef))
			{
				delete this;
				return 0L;
			}

			return m_lRef;
		}
#if 0
		STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE d3dRenderState, DWORD dwValue)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetRenderState(d3dRenderState, dwValue);
		}
#endif
		STDMETHOD(SetSamplerState)(THIS_ DWORD dwStage, D3DSAMPLERSTATETYPE d3dSamplerState, DWORD dwValue)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetSamplerState(dwStage, d3dSamplerState, dwValue);
		}
		STDMETHOD(SetTextureStageState)(THIS_ DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTextureStageState, DWORD dwValue)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetTextureStageState(dwStage, d3dTextureStageState, dwValue);
		}
		STDMETHOD(SetTexture)(THIS_ DWORD dwStage, LPDIRECT3DBASETEXTURE9 pTexture);

		void setTexture(int stage, Texture* tex) {
			if (!tex) {
				SetTexture(stage, 0);
				return;
			}

			D3D9texture* d3d9tex = (D3D9texture*)tex;
			SetTexture(stage, d3d9tex->getObject());
		}

		STDMETHOD(SetVertexShader)(THIS_ LPDIRECT3DVERTEXSHADER9 pShader)
		{
			m_nTotalStateChanges++;
			if (pShader == m_vertexShader)
				return D3D_OK;

			m_vertexShader = pShader;
			return d3d9Device->SetVertexShader(pShader);
		}
		STDMETHOD(SetPixelShader)(THIS_ LPDIRECT3DPIXELSHADER9 pShader)
		{
			m_nTotalStateChanges++;
			if (pShader == m_pixelShader) {
				return D3D_OK;
			}

			m_pixelShader = pShader;
			return d3d9Device->SetPixelShader(pShader);
		}
		STDMETHOD(SetFVF)(THIS_ DWORD dwFVF)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetFVF(dwFVF);
		}
		STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetTransform(State, pMatrix);
		}
		STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9 *pMaterial)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetMaterial(pMaterial);
		}
		STDMETHOD(SetLight)(THIS_ DWORD Index, CONST D3DLIGHT9 *pLight)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetLight(Index, pLight);
		}
		STDMETHOD(LightEnable)(THIS_ DWORD Index, BOOL Enable)
		{
			m_nTotalStateChanges++;
			return d3d9Device->LightEnable(Index, Enable);
		}
		STDMETHOD(SetNPatchMode)(THIS_ FLOAT NumSegments)
		{
			m_nTotalStateChanges++;
			return d3d9Device->SetNPatchMode(NumSegments);
		}
		STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
		{
			m_nTotalStateChanges++;
			if (g_uniforms.isVsregisterShared(RegisterIndex))
				return D3D_OK;

			return d3d9Device->SetVertexShaderConstantF(RegisterIndex,
				pConstantData,
				RegisterCount);
		}
		STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
		{
			m_nTotalStateChanges++;
			if (g_uniforms.isVsregisterShared(RegisterIndex))
				return D3D_OK;

			return d3d9Device->SetVertexShaderConstantI(RegisterIndex,
				pConstantData,
				RegisterCount);
		}
		STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
		{
			m_nTotalStateChanges++;
			if (g_uniforms.isVsregisterShared(RegisterIndex))
				return D3D_OK;

			return d3d9Device->SetVertexShaderConstantB(RegisterIndex,
				pConstantData,
				RegisterCount);
		}
		STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
		{
			m_nTotalStateChanges++;
			if (g_uniforms.isPsregisterShared(RegisterIndex))
				return D3D_OK;

			return d3d9Device->SetPixelShaderConstantF(RegisterIndex,
				pConstantData,
				RegisterCount);
		}
		STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
		{
			m_nTotalStateChanges++;
			if (g_uniforms.isPsregisterShared(RegisterIndex))
				return D3D_OK;
			return d3d9Device->SetPixelShaderConstantI(RegisterIndex,
				pConstantData,
				RegisterCount);
		}
		STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
		{
			m_nTotalStateChanges++;
			if (g_uniforms.isPsregisterShared(RegisterIndex))
				return D3D_OK;
			return d3d9Device->SetPixelShaderConstantB(RegisterIndex,
				pConstantData,
				RegisterCount);
		}

		DWORD getRenderState(D3DRENDERSTATETYPE state) {
			return cacheRenderStates.get_val(state);
		}

		IDirect3DVertexShader9* getVertexShader() {
			return m_vertexShader;
		}

		IDirect3DPixelShader9* getPixelShader() {
			return m_pixelShader;
		}


		// methods inherited from ID3DXEffectStateManager
		STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE d3dRenderState, DWORD dwValue)
		{
			m_nTotalStateChanges++;

			// Update the render state cache
			// If the return value is 'true', the command must be forwarded to
			// the D3D Runtime.
			if (cacheRenderStates.set_val(d3dRenderState, dwValue))
				return d3d9Device->SetRenderState(d3dRenderState, dwValue);

			m_nFilteredStateChanges++;

			return S_OK;
		}

	protected:
		LONG m_lRef;
		UINT m_nTotalStateChanges;
		UINT m_nTotalStateChangesPerFrame;
		WCHAR m_wszFrameStats[256];
		LPDIRECT3DVERTEXSHADER9 m_vertexShader;
		LPDIRECT3DPIXELSHADER9 m_pixelShader;
		LPDIRECT3DBASETEXTURE9 m_textures[32];

	protected:
		typedef multicache <D3DSAMPLERSTATETYPE, DWORD> samplerStageCache;
		typedef multicache <D3DTEXTURESTAGESTATETYPE, DWORD> textureStateStageCache;

	protected:
		multicache <D3DRENDERSTATETYPE, DWORD> cacheRenderStates;    // cached Render-States
#if 0
		std::vector <samplerStageCache> vecCacheSamplerStates;       // cached Sampler States
		std::vector <textureStateStageCache> vecCacheTextureStates;       // cached Texture Stage States
#endif
		UINT m_nFilteredStateChanges;                            // Statistics -- # of redundant
		// states actually filtered
		UINT m_nFilteredStateChangesPerFrame;

	private:
		IDirect3DSurface9* m_depthStencilSurface;
		TexFormat m_depthStencilFormat;
		IDirect3DVertexDeclaration9* m_vertexDeclaration;
		Dict<int,IDirect3DStateBlock9*> m_states;
		IDirect3DStateBlock9* m_stageSamplerStates[MAX_STAGES];
	};

}} // namespace Axon::Render

#endif // end guardian
