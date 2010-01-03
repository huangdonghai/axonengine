/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

AX_BEGIN_NAMESPACE

AX_BEGIN_CLASS_MAP(axdirect3d9)
	AX_CLASS_ENTRY("driver", D3D9driver)
AX_END_CLASS_MAP()

D3D9window *d3d9InternalWindow;
D3D9driver *d3d9Driver;
IRenderDriver::Info *d3d9DriverInfo;
RenderQueue *d3d9Queue;
SyncMutex d3d9Mutex;

IDirect3D9 *d3d9Api;
IDirect3DDevice9 *d3d9Device;

D3D9shadermanager *d3d9ShaderManager;
D3D9targetmanager *d3d9TargetManager;
D3D9querymanager *d3d9QueryManager;
D3D9primitivemanager *d3d9PrimitiveManager;
D3D9thread *d3d9Thread;
D3D9vertexbuffermanager *d3d9VertexBufferManager;
D3D9postprocess *d3d9Postprocess;
D3D9statemanager *d3d9StateManager;
D3D9draw *d3d9Draw;
// {92F6401F-2E38-4ac6-8F10-3B28A89079EA}
const GUID d3d9ResGuid = { 0x92f6401f, 0x2e38, 0x4ac6, { 0x8f, 0x10, 0x3b, 0x28, 0xa8, 0x90, 0x79, 0xea } };

bool d3d9NULL;
bool d3d9NVDB;

RenderCamera *d3d9Camera;

AX_END_NAMESPACE

