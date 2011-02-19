/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "dx9_private.h"

AX_BEGIN_NAMESPACE

AX_BEGIN_CLASS_MAP(AxDX9)
	AX_CLASS_ENTRY("Driver", DX9_Driver)
AX_END_CLASS_MAP()

DX9_Window *dx9_internalWindow;
DX9_Driver *dx9_driver;
IDirect3D9 *dx9_api;
IDirect3DDevice9 *dx9_device;
DX9_ShaderManager *dx9_shaderManager;
DX9_StateManager *dx9_stateManager;
ConstBuffers *dx9_constBuffers;
IDirect3DVertexDeclaration9 *dx9_vertexDeclarations[VertexType::MaxType];
IDirect3DVertexDeclaration9 *dx9_vertexDeclarationsInstanced[VertexType::MaxType];

AX_END_NAMESPACE

