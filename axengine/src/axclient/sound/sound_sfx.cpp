/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "sound_local.h"

namespace Axon { namespace Sound {


	Sfx::Sfx()
	{

	}

	Sfx::~Sfx()
	{
		SafeRelease(m_fmodSound);
	}

	void Sfx::deleteThis()
	{
		g_soundSystem->_removeSfx(this);
	}

}} // namespace Axon::Sound

