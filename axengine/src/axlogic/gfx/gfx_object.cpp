/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "gfx_local.h"

namespace Axon { namespace Gfx {

	GfxObject::GfxObject()
		: m_pos("Position")
		, m_rotations("Rotation")
		, m_scale("Scale")
	{
		m_subAnims.push_back(&m_pos);
		m_subAnims.push_back(&m_rotations);
		m_subAnims.push_back(&m_scale);
	}

	GfxObject::~GfxObject()
	{

	}

}} // namespace Axon::Gfx
