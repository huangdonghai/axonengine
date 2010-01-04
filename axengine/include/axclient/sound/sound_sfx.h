/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_SOUND_SFX_H
#define AX_SOUND_SFX_H

AX_BEGIN_NAMESPACE

class AX_API SoundFx : public RefObject {
public:
	friend class SoundSystem;

	// implement RefObject
	virtual void deleteThis();

	bool isValid() const { return m_fmodSound != 0; }

private:
	SoundFx();
	virtual ~SoundFx();

	String m_name;
	FMOD::Sound *m_fmodSound;
};

typedef RefPtr<SoundFx> SfxPtr;

AX_END_NAMESPACE

#endif
