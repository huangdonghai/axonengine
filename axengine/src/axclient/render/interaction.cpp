/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"


namespace Axon { namespace Render {

	static int eyeInWaterSort[] = {
		Shader::SortHint_opacit,
		Shader::SortHint_decal,
		Shader::SortHint_aboveWater,
		Shader::SortHint_water,
		Shader::SortHint_underWater,
	};


	void Interaction::calcSort(bool eyeInWater) {
		sortkey = Shader::SortHint_opacit << 28;

		Material* mat = primitive->getMaterial();

		if (!mat) {
			return;
		}

		Shader* shader = mat->getShaderTemplate();

		if (!shader) {
			return;
		}

		sortkey = shader->getSortHint();

		Texture* tex = mat->getTexture(SamplerType::Diffuse);

		if (eyeInWater) {
			sortkey = eyeInWaterSort[sortkey];
		}

		int distance = 0;
		if (qactor) {
			distance = qactor->distance * 1024;
		}

		sortkey = (sortkey << 28) + ((uint_t)(distance) &0x0fffffff);
	}

	void Interaction::setupShader() {

	}

}} // namespace Axon::Render

