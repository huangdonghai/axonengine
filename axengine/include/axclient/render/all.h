/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_ALL_H
#define AX_RENDER_ALL_H

namespace Axon { namespace Render {

	class Material;
	class Font;

#if 0
	enum Pass {
		ZPass, Opacit, Translucet, ShadowGen
	};
#endif

	enum Spec {
		VeryLow, Low, Middle, High, VeryHigh
	};

}} // namespace Axon::Render

#include "query.h"
#include "texture.h"
#include "textureatlas.h"
#include "shader.h"
#include "material.h"
#include "camera.h"
#include "target.h"
#include "interaction.h"
#include "primitive.h"
#include "entity.h"
#include "light.h"
#include "fog.h"
#include "terrain.h"
#include "driver.h"
#include "font.h"
#include "wind.h"
#include "timeofday.h"
#include "outdoorenv.h"
#include "world.h"
#include "queue.h"
#include "system.h"

#endif // end guardian

