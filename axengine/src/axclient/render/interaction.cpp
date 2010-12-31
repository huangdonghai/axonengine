/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

static int eyeInWaterSort[] = {
	MaterialDecl::SortHint_Opacit,
	MaterialDecl::SortHint_Decal,
	MaterialDecl::SortHint_AboveWater,
	MaterialDecl::SortHint_Water,
	MaterialDecl::SortHint_UnderWater,
};


void Interaction::calcSort(bool eyeInWater)
{
	sortkey = MaterialDecl::SortHint_Opacit << 28;

	Material *mat = primitive->getMaterial();

	if (!mat)
		return;

	const ShaderInfo *shader = mat->getShaderInfo();

	if (!shader)
		return;

	sortkey = mat->getSortHint();

	Texture *tex = mat->getTexture(MaterialTextureId::Diffuse);

	if (eyeInWater)
		sortkey = eyeInWaterSort[sortkey];

	int distance = 0;
	if (entity)
		distance = entity->getDistance() * 1024;

	sortkey = (sortkey << 28) + ((uint_t)(distance) &0x0fffffff);
}


AX_END_NAMESPACE

