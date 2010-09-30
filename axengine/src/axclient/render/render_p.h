#ifndef AX_RENDER_PRIVATE_H
#define AX_RENDER_PRIVATE_H

#include "selection.h"
#include "render_thread.h"
#include "render_interface.h"
#include "commandbuf.h"
#include "sampler_p.h"
#include "texture_p.h"
#include "material_p.h"
#include "vertexbuffer.h"
#include "primitive_p.h"

AX_BEGIN_NAMESPACE

extern RenderFrame *g_renderFrame;
extern ShaderMacro g_shaderMacro;
extern ConstBuffers g_constBuffers;
extern IRenderDriver *g_renderDriver;

extern ApiWrap *g_apiWrap;
extern BufferManager *g_bufferManager;

// filled by drivers

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
extern TreeManager *g_treeManager;
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

AX_END_NAMESPACE

#endif // AX_RENDER_PRIVATE_H
