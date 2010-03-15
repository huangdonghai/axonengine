/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_ALL_H
#define AX_RENDER_ALL_H

AX_BEGIN_NAMESPACE

class TextureFr;

class NewTexture
{
public:
	enum InitFlag {
		IF_NoMipmap = 1,
		IF_NoDownsample = 2,
		IF_AutoGenMipmap = 4,
		IF_RenderTarget = 8
	};

	typedef Flags_<InitFlag> InitFlags;

	enum TexType {
		TT_2D,
		TT_3D,
		TT_CUBE,
	};

	enum ClampMode {
		CM_Repeat,
		CM_Clamp,
		CM_ClampToEdge,	// only used in engine internal
		CM_ClampToBorder // only used in engine internal
	};

	enum FilterMode {
		FM_Nearest,
		FM_Linear,
		FM_Bilinear,
		FM_Trilinear
	};

	enum BorderColor {
		BC_Zero, BC_One
	};

	NewTexture();
	NewTexture(const String &name);
	NewTexture(const String &debugname, TexFormat format, int width, int height);
	~NewTexture();

	bool isNull() const { return m_fr == 0; }

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	void generateMipmap();

	// texture parameters
	void setClampMode(ClampMode clampmwode);
	void setFilterMode(FilterMode filtermode);
	void setBorderColor(BorderColor bordercolor);

private:
	TextureFr *m_fr;
};

class MaterialFr;
class NewMaterial
{
public:
	NewMaterial();
	NewMaterial(const String &name);
	~NewMaterial();

private:
	MaterialFr *m_fr;
};

class PrimitiveBackend;

class Element
{
public:
	enum ElementType {
		NoneType = 0,		// for error checks
		PointType,
		LineType,
		MeshType,			// raw mesh
		TextType,
		GuiType,
		ChunkType,
		GroupType,			// grouped primitive, maybe for gui system
		ReferenceType,		// a reference to another primitive, but can use different entity and material etc...
		InstancingType		// geometry instancing type, instancing a primitive many time with different parameter
	};

	enum Hint {
		HintStatic,				// will alloc in video memory, never change
		HintDynamic,			// 
		HintFrame,				// only draw in one frame, will auto deleted when render driver cached it
	};

private:
	PrimitiveBackend *m_fr;
};


AX_END_NAMESPACE

#include "query.h"
#include "texture.h"
#include "sampler.h"
#include "textureatlas.h"
#include "materialdecl.h"
#include "shader.h"
#include "material.h"
#include "camera.h"
#include "target.h"
#include "interaction.h"
#include "primitive.h"
#include "render_entity.h"
#include "light.h"
#include "fog.h"
#include "terrain.h"
#include "driver.h"
#include "font.h"
#include "wind.h"
#include "timeofday.h"
#include "outdoorenv.h"
#include "render_world.h"
#include "queue.h"
#include "render_system.h"

#endif // end guardian

