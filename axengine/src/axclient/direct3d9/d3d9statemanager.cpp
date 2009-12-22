/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "d3d9private.h"

AX_BEGIN_NAMESPACE

#if 0
	//--------------------------------------------------------------------------------------
	// Base implementation of a custom ID3DXEffectStateManager interface
	// This implementation does nothing more than forward all state change commands to the
	// appropriate D3D handler.
	// An interface that implements custom state change handling may be derived from this
	// (such as statistical collection, or filtering of redundant state change commands for
	// a subset of states)
	//--------------------------------------------------------------------------------------
	class CBaseStateManager : public CStateManagerInterface
	{
	public:
		CBaseStateManager(LPDIRECT3DDEVICE9 pDevice)
			: m_lRef(1UL),
			d3d9Device(pDevice),
			m_nTotalStateChanges(0),
			m_nTotalStateChangesPerFrame(0),
			m_vertexShader(0),
			m_pixelShader(0)
		{
			// Increment the reference count on the device, because a pointer to it has
			// been copied for later use
			d3d9Device->AddRef();
			m_wszFrameStats[0] = 0;
			TypeZeroArray(m_textures);
		}

		virtual ~CBaseStateManager()
		{
			// Release the reference count held from the constructor
			d3d9Device->Release();
		}

		// Must be invoked by the application anytime it allows state to be
		// changed outside of the D3DX Effect system.
		// An entry-point for this should be provided if implementing custom filtering of redundant
		// state changes.
		virtual void DirtyCachedValues()
		{
		}

		virtual LPCWSTR EndFrameStats()
		{
			if (m_nTotalStateChangesPerFrame != m_nTotalStateChanges)
			{
#if 0
				StringCchPrintf(m_wszFrameStats,
					256,
					L"Frame Stats:\nTotal State Changes (Per Frame): %d",
					m_nTotalStateChanges);
#endif
				m_wszFrameStats[255] = 0;
				m_nTotalStateChangesPerFrame = m_nTotalStateChanges;
			}

			m_nTotalStateChanges = 0;

			return m_wszFrameStats;
		}

	};



	//--------------------------------------------------------------------------------------
	// Implementation of a state manager that filters redundant state change commands.
	// This implementation is useful on PURE devices.
	// PURE HWVP devices do not implement redundant state change filtering.
	// States that may be useful to filter on PURE device are:
	//      Render States
	//      Texture Stage States
	//      Sampler States
	// See the Direct3D SDK Documentation for further details on pure device state change
	// behavior.
	//--------------------------------------------------------------------------------------
#define CACHED_STAGES 2        // The number of stages to cache
	// Remaining stages are simply passed through with no
	// redundancy filtering.
	// For this sample, the first two stages are cached, while
	// the remainder are passed through

	class CPureDeviceStateManager : public CBaseStateManager
	{

	public:
		CPureDeviceStateManager(LPDIRECT3DDEVICE9 pDevice)
			: CBaseStateManager(pDevice),
			cacheRenderStates(),
			vecCacheSamplerStates(CACHED_STAGES),
			vecCacheTextureStates(CACHED_STAGES),
			m_nFilteredStateChanges(0),
			m_nFilteredStateChangesPerFrame(0)
		{
		}
		virtual LPCWSTR EndFrameStats()
		{
			// If either the 'total' state changes or the 'filtered' state changes
			// has changed, re-compute the frame statistics string
			if (0 !=((m_nTotalStateChangesPerFrame - m_nTotalStateChanges)
				|(m_nFilteredStateChangesPerFrame - m_nFilteredStateChanges)))
			{
#if 0
				StringCchPrintf(m_wszFrameStats,
					256,
					L"Frame Stats:\nTotal State Changes (Per Frame): %d\nRedundants Filtered (Per Frame): %d",
					m_nTotalStateChanges, m_nFilteredStateChanges);
#endif
				m_wszFrameStats[255] = 0;

				m_nTotalStateChangesPerFrame = m_nTotalStateChanges;
				m_nFilteredStateChangesPerFrame = m_nFilteredStateChanges;
			}

			m_nTotalStateChanges = 0;
			m_nFilteredStateChanges = 0;

			return m_wszFrameStats;
		}
		// More targeted 'Dirty' commands may be useful.
		void DirtyCachedValues()
		{
			cacheRenderStates.dirtyall();

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
		}

#if 0
		STDMETHOD(SetSamplerState)(THIS_ DWORD dwStage, D3DSAMPLERSTATETYPE d3dSamplerState, DWORD dwValue)
		{
			m_nTotalStateChanges++;

			// If this dwStage is not cached, pass the value through and exit.
			// Otherwise, update the sampler state cache and if the return value is 'true', the 
			// command must be forwarded to the D3D Runtime.
			if (dwStage >= CACHED_STAGES || vecCacheSamplerStates[dwStage].set_val(d3dSamplerState, dwValue))
				return d3d9Device->SetSamplerState(dwStage, d3dSamplerState, dwValue);

			m_nFilteredStateChanges++;

			return S_OK;
		}

		STDMETHOD(SetTextureStageState)(THIS_ DWORD dwStage, D3DTEXTURESTAGESTATETYPE d3dTextureStageState, DWORD dwValue)
		{
			m_nTotalStateChanges++;

			// If this dwStage is not cached, pass the value through and exit.
			// Otherwise, update the texture stage state cache and if the return value is 'true', the 
			// command must be forwarded to the D3D Runtime.
			if (dwStage >= CACHED_STAGES || vecCacheTextureStates[dwStage].set_val(d3dTextureStageState, dwValue))
				return d3d9Device->SetTextureStageState(dwStage, d3dTextureStageState, dwValue);

			m_nFilteredStateChanges++;

			return S_OK;
		}
#endif
	};
#endif

#if 0
	//--------------------------------------------------------------------------------------
	// Create an extended ID3DXEffectStateManager instance
	//--------------------------------------------------------------------------------------
	CStateManagerInterface* CStateManagerInterface::Create(LPDIRECT3DDEVICE9 pDevice)
	{
		CStateManagerInterface* pStateManager = NULL;

		D3DDEVICE_CREATION_PARAMETERS cp;
		memset(&cp, 0, sizeof cp);

		if (SUCCEEDED(pDevice->GetCreationParameters(&cp)))
		{
			// A PURE device does not attempt to filter duplicate state changes (with some
			// exceptions) from the driver.  Such duplicate state changes can be expensive
			// on the CPU.  To create the proper state manager, the application determines
			// whether or not it is executing on a PURE device.
			bool bPureDevice =(cp.BehaviorFlags & D3DCREATE_PUREDEVICE) != 0;

			if (1||bPureDevice)
				pStateManager = new CPureDeviceStateManager(pDevice);
			else
				pStateManager = new CBaseStateManager(pDevice);
		}

		if (NULL == pStateManager)
		{
//			DXUT_ERR_MSGBOX(L"Failed to Create State Manager", E_OUTOFMEMORY);
		}

		return pStateManager;
	}
#endif

	D3D9statemanager::D3D9statemanager()
		: m_lRef(1UL),
		m_nTotalStateChanges(0),
		m_nTotalStateChangesPerFrame(0),
		m_vertexShader(0),
		m_pixelShader(0),
		cacheRenderStates(),
#if 0
		vecCacheSamplerStates(CACHED_STAGES),
		vecCacheTextureStates(CACHED_STAGES),
#endif
		m_nFilteredStateChanges(0),
		m_nFilteredStateChangesPerFrame(0)
	{
		m_depthStencilFormat = TexFormat::AUTO;
		m_depthStencilSurface = 0;
		m_vertexDeclaration = 0;
		TypeZeroArray(m_stageSamplerStates);
		TypeZeroArray(m_textures);
	}

	HRESULT D3D9statemanager::setDepthStencilSurface(IDirect3DSurface9* obj, TexFormat format)
	{
		m_depthStencilFormat = format;

		if (obj == m_depthStencilSurface) {
			return D3D_OK;
		}

		m_depthStencilSurface = obj;
		return d3d9Device->SetDepthStencilSurface(obj);
	}

	void D3D9statemanager::setVertexDeclaration( IDirect3DVertexDeclaration9* vertdecl )
	{
		if (vertdecl == m_vertexDeclaration)
			return;

		m_vertexDeclaration = vertdecl;
		d3d9Device->SetVertexDeclaration(m_vertexDeclaration);
	}

	void D3D9statemanager::setSamplerStateBlock( DWORD stage, Texture::ClampMode clampmode, Texture::FilterMode filtermode )
	{
		int hash = stage << 24 | clampmode << 16 | filtermode;
		HRESULT hr;

		Dict<int,IDirect3DStateBlock9*>::const_iterator it = m_states.find(hash);
		if (it != m_states.end()) {
			if (m_stageSamplerStates[stage] != it->second) {
				V(it->second->Apply());
				m_stageSamplerStates[stage] = it->second;
			}
			return;
		}

		IDirect3DStateBlock9* state = 0;
		d3d9Device->BeginStateBlock();

		switch (clampmode) {
		case Texture::CM_Clamp:
		case Texture::CM_ClampToEdge:
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
			break;
		case Texture::CM_ClampToBorder:
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSW, D3DTADDRESS_BORDER);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_BORDERCOLOR, 0);
			break;
		case Texture::CM_Repeat:
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		}

		switch (filtermode) {
		case Texture::FM_Nearest:
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			break;
		case Texture::FM_Linear:
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			break;
		case Texture::FM_Bilinear:
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			break;
		case Texture::FM_Trilinear:
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			d3d9StateManager->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			break;
		}

		d3d9Device->EndStateBlock(&state);

		m_states[hash] = state;
		state->Apply();
		m_stageSamplerStates[stage] = state;
	}

	STDMETHODIMP D3D9statemanager::SetTexture( THIS_ DWORD dwStage, LPDIRECT3DBASETEXTURE9 pTexture )
	{
		m_nTotalStateChanges++;

		if (m_textures[dwStage] == pTexture)
			return D3D_OK;

		m_textures[dwStage] = pTexture;

		// set sampler state
		D3D9texture* appTex = 0;
		DWORD size = sizeof(void*);

		if (!pTexture) {
			return d3d9Device->SetTexture(dwStage, pTexture);
		}
#if 0
		pTexture->GetPrivateData(d3d9ResGuid, &appTex, &size);
#else
		appTex = D3D9texture::getAppTexture(pTexture);
#endif
		if (appTex/* && size*/) {
			appTex->issueSamplerState(dwStage);
		}
		return d3d9Device->SetTexture(dwStage, pTexture);
	}

	void D3D9statemanager::EndFrameStats()
	{

	}

AX_END_NAMESPACE
