#ifndef AX_RENDER_PRIVATE_H
#define AX_RENDER_PRIVATE_H

#include "selection.h"
#include "render_interface.h"
#include "commandbuf.h"
#include "sampler_p.h"
#include "texture_p.h"
#include "material_p.h"
#include "vertexbuffer.h"
#include "primitive_p.h"
#include "render_thread.h"

AX_BEGIN_NAMESPACE

extern IRenderDriver *g_renderDriver;
extern ApiWrap *g_apiWrap;
extern BufferManager *g_bufferManager;

// filled by drivers
extern ConstBuffer *g_uniformStructs[ConstBuffer::NUMBER_STRUCT];

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
extern TreeManager *g_treeManager;
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

AX_END_NAMESPACE

#endif // AX_RENDER_PRIVATE_H
