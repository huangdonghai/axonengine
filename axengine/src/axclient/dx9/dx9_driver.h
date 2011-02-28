/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_DX9_DRIVER_H
#define AX_DX9_DRIVER_H

AX_BEGIN_NAMESPACE

class DX9_Driver : public IRenderDriver, public ICmdHandler
{
	AX_DECLARE_FACTORY();
	AX_DECLARE_COMMAND_HANDLER(DX9_Driver);

public:
	DX9_Driver();
	virtual ~DX9_Driver();

	// implement IRenderDriver
	virtual bool initialize();
	virtual void finalize();

	virtual const ShaderInfo *findShaderInfo(const FixedString &key);
	virtual const ShaderInfo *findShaderInfo(const FixedString &key, const GlobalMacro &gm, const MaterialMacro &mm);

	void reset(int width, int height, bool fullscreen);
	void onReset();
	void onDeviceLost();

	IDirect3DSurface9 *getDepthStencil(const Size &size);
	IDirect3DSurface9 *getNullTarget(const Size &size);

protected:
	bool checkTextureFormatSupport(TexFormat format, D3DFORMAT d3dformat);
	bool checkHardwareMipmapGenerationSupport(TexFormat format, D3DFORMAT d3dformat);
	bool checkRenderTargetFormatSupport(TexFormat format, D3DFORMAT d3dformat);
	void checkFormats();
	void createVertexDecl();

private:
	Size m_depthStencilSurfaceSize;
	IDirect3DSurface9 *m_depthStencilSurface;
	Size m_nullSurfaceSize;
	IDirect3DSurface9 *m_nullSurface;
};

AX_END_NAMESPACE


#endif // end guardian

