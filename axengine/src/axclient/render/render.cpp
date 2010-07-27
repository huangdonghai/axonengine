#include "../private.h"

AX_BEGIN_NAMESPACE

FontPtr g_defaultFont;
FontPtr g_consoleFont;
FontPtr g_miniFont;

RenderDriverInfo g_renderDriverInfo;
RenderFrame *g_renderFrame;
ShaderMacro g_shaderMacro;
ConstBuffers g_constBuffers;
IRenderDriver *g_renderDriver;


// module object
ApiWrap *g_apiWrap = 0;
RenderContext *g_renderContext = 0;
BufferManager *g_bufferManager = 0;

AX_END_NAMESPACE