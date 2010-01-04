/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_SOUND_SYSTEM_H
#define AX_SOUND_SYSTEM_H

AX_BEGIN_NAMESPACE

class AX_API SoundSystem : public ICmdHandler, public ITickable {
public:
	AX_DECLARE_COMMAND_HANDLER(SoundSystem);

	SoundSystem();
	~SoundSystem();

	// implement ITickable
	virtual void tick();

	SoundFx *createSfx(const String &name);
	SoundFx *createStream();

	void setWorld(SoundWorld *world);

	void playSound(int channelId, SoundFx *sfx, LoopingMode looping = Looping_None);
	void stopSound(int channelId);

	// BEGIN INTERNAL USE
	void _removeSfx(SoundFx *sfx);
	void _playSound(SoundWorld *world, SoundEntity *entity, int channelId, SoundFx *sfx, LoopingMode looping = Looping_None, float minDist = DEFAULT_MIN_DIST, float maxDist = DEFAULT_MAX_DIST);
	void _stopSound(SoundWorld *world, SoundEntity *entity, int channelId);
	void _hintChannelEnd(int index);
	// END INTERNAL USE

protected:
	void init();
	void initFMOD();
	void shutdown();

	// console command
	void playSound_f(const CmdArgs &args);

private:
	FMOD::System *m_fmodSystem;
	typedef Dict<String, SoundFx*, hash_pathname, equal_pathname> SfxDict;
	SfxDict m_sfxDict;
	SfxPtr m_commandSfx;

	SoundChannel m_channels[MAX_CHANNELS];
	Dict<SoundKey,int> m_channelDict;

	SoundWorld *m_activeWorld;
};

AX_END_NAMESPACE

#endif
