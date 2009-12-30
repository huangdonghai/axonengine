/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_PRIVATE_H
#define AX_EDITOR_PRIVATE_H

#include <axcore/public.h>
#include <axclient/public.h>
#include <axlogic/public.h>
#include <axeditor/public.h>

AX_BEGIN_NAMESPACE

	struct Internal {
		inline static float snap(float f, float grid) {
			int i = (f + grid * Math::sign(f) * .5f) / grid;
			return grid * i;
		}

		inline static Vector3 snap(const Vector3& v, float grid) {
			Vector3 result;
			result.x = snap(v.x, grid);
			result.y = snap(v.y, grid);
			result.z = snap(v.z, grid);
			return result;
		}
	};

AX_END_NAMESPACE


#endif // AX_EDITOR_PRIVATE_H

