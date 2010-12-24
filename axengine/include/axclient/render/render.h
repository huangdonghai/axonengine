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

struct RenderClearer {
	Rgba color;
	float depth;
	int stencil;
	bool isClearColor : 1;
	bool isClearDepth : 1;
	bool isClearStencil : 1;

	RenderClearer() : color(Rgba::Black), depth(1.0f), stencil(0), isClearColor(false), isClearDepth(false), isClearStencil(false)
	{}

	void clearDepth(bool enable, float ref = 1.0f)
	{
		isClearDepth = enable;
		depth = ref;
	}

	void clearColor(bool enable, Rgba ref = Rgba::Zero)
	{
		isClearColor = enable;
		color = ref;
	}

	void clearStencil(bool enable, int ref)
	{
		isClearStencil = enable;
		stencil = ref;
	}
};

AX_END_NAMESPACE

#include "render_state.h"
#include "query.h"
#include "sampler.h"
#include "texture.h"
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
#include "frame.h"
#include "render_system.h"

AX_BEGIN_NAMESPACE

extern RenderDriverInfo g_renderDriverInfo;

AX_END_NAMESPACE

#endif // end guardian

