#include "dx11_private.h"

AX_BEGIN_NAMESPACE

DX11_Window *dx11_internalWindow;
DX11_Driver *dx11_driver;
ID3D11Device *dx11_device;
ID3D11DeviceContext *dx11_context;

DX11_ShaderManager *dx11_shaderManager;
DX11_StateManager *dx11_stateManager;
ConstBuffers *dx11_constBuffers;

ID3D11InputLayout *dx11_inputLayouts[VertexType::kNumber];
ID3D11InputLayout *dx11_inputLayoutInstanced[VertexType::kNumber];

AX_END_NAMESPACE
