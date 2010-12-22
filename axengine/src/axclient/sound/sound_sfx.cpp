/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "sound_local.h"

AX_BEGIN_NAMESPACE


SoundFx::SoundFx()
{

}

SoundFx::~SoundFx()
{
	SafeRelease(m_fmodSound);
}

void SoundFx::onDestroy()
{
	g_soundSystem->_removeSfx(this);
}

AX_END_NAMESPACE

