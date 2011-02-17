#ifndef AX_DX11_DRIVER_H
#define AX_DX11_DRIVER_H

AX_BEGIN_NAMESPACE

typedef HRESULT (WINAPI *LPCREATEDXGIFACTORY)(REFIID, void **);
typedef HRESULT (WINAPI *LPD3D11CREATEDEVICEANDSWAPCHAIN)(__in_opt IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, __in_ecount_opt( FeatureLevels ) CONST D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, __in_opt CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, __out_opt IDXGISwapChain** ppSwapChain, __out_opt ID3D11Device** ppDevice, __out_opt D3D_FEATURE_LEVEL* pFeatureLevel, __out_opt ID3D11DeviceContext** ppImmediateContext);
typedef HRESULT (WINAPI *LPD3D11CREATEDEVICE)( IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT32, D3D_FEATURE_LEVEL*, UINT, UINT32, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
typedef void (WINAPI *LPD3DX11COMPILEFROMMEMORY)(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude, 
												 LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, ID3DX11ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult);


class DX11_Driver : public IRenderDriver, public ICmdHandler
{
	AX_DECLARE_FACTORY();
	AX_DECLARE_COMMAND_HANDLER(DX11_Driver);

public:
	DX11_Driver();
	virtual ~DX11_Driver();

	// implement IRenderDriver
	virtual void initialize(SyncEvent &syncEvent);
	virtual void finalize();

	virtual const ShaderInfo *findShaderInfo(const FixedString &key);

protected:
	void createInputLayouts();

private:
};

AX_END_NAMESPACE

#endif // AX_DX11_DRIVER_H
