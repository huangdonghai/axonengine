/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE


	Axon::String Point::toString() const {
		String result;
		StringUtil::sprintf(result, "%d %d", x, y);
		return result;
	}

	bool Point::fromString(const char* str) {
		int v = sscanf(str, "%d %d", &x, &y);
		AX_ASSERT(v == 2);
		return v == 2;
	}

AX_END_NAMESPACE
