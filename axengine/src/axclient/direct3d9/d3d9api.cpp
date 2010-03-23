#include "d3d9private.h"

AX_BEGIN_NAMESPACE

handle_t D3D9_Api::createTexture2D( TexFormat format, int width, int height, int flags /*= 0*/ )
{

}

void D3D9_Api::uploadTexture( handle_t htex, int level, void *pixels, TexFormat format /*= TexFormat::AUTO*/ )
{

}

void D3D9_Api::uploadSubTexture( handle_t htex, const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/ )
{

}

void D3D9_Api::generateMipmap( handle_t htex )
{

}

void D3D9_Api::deleteTexture2D( handle_t htex )
{

}
AX_END_NAMESPACE
