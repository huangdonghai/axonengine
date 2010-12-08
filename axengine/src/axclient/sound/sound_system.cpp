/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "sound_local.h"

AX_BEGIN_NAMESPACE

AX_BEGIN_COMMAND_MAP(SoundSystem)
	AX_COMMAND_ENTRY("playSound", playSound_f)
AX_END_COMMAND_MAP()

static inline const FMOD_VECTOR *tr(const Vector3 &v) {
	return (const FMOD_VECTOR*)&v[0];
}

struct FMOD_Callback {
	static FMOD_RESULT F_CALLBACK open(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
	{
		File *f = g_fileSystem->openFileRead(name);

		if (!f)
			return FMOD_ERR_FILE_NOTFOUND;

		*handle = f;
		*filesize = f->size();
		*userdata = 0;

		return FMOD_OK;
	}

	static FMOD_RESULT F_CALLBACK close(void *handle, void *userdata)
	{
		File *f = (File*)handle;
		f->close();
		return FMOD_OK;
	}

	static FMOD_RESULT F_CALLBACK read(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
	{
		File *f = (File*)handle;
		*bytesread = f->read(buffer, sizebytes);

		if (*bytesread < sizebytes) {
			return FMOD_ERR_FILE_EOF;
		}

		return FMOD_OK;
	}

	static FMOD_RESULT F_CALLBACK seek(void *handle, unsigned int pos, void *userdata)
	{
		File *f = (File*)handle;
		f->seek(pos, File::SeekSet);
		return FMOD_OK;
	}

	static FMOD_RESULT F_CALLBACK channelCallback(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
	{
		FMOD::Channel *cppchannel = (FMOD::Channel *)channel;

		// More code goes here.
		if (type != FMOD_CHANNEL_CALLBACKTYPE_END)
			return FMOD_OK;

		int index;
		FMOD_RESULT hr = cppchannel->getIndex(&index);
		ERRCHECK(hr);

		g_soundSystem->_hintChannelEnd(index);

		return FMOD_OK;
	}
};

SoundSystem::SoundSystem()
{
	m_activeWorld = 0;

	init();

	g_cmdSystem->registerHandler(this);
	g_system->registerTickable(System::TickEditor, this);
}

SoundSystem::~SoundSystem()
{
	g_system->removeTickable(System::TickEditor, this);
	g_cmdSystem->removeHandler(this);

	m_commandSfx.clear();

	FMOD_RESULT result = m_fmodSystem->release();
	ERRCHECK(result);
}

void SoundSystem::playSound_f( const CmdArgs &args )
{
	std::string name = "sounds/test.ogg";

	if (args.tokened.size() > 1)
		name = args.tokened[1];

	m_commandSfx << createSfx(name);

	if (!m_commandSfx)
		return;

	playSound(ChannelId_Any, m_commandSfx.get());
#if 0
	FMOD::Sound *sound;
	FMOD_RESULT result = m_fmodSystem->createSound(, FMOD_DEFAULT, 0, &sound);
	
	m_fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, 0);

	FMOD::Sound *sound2;
	result = m_fmodSystem->createSound("sounds/test.ogg", FMOD_DEFAULT, 0, &sound2);
#endif
}

SoundFx *SoundSystem::createSfx( const std::string &name )
{
	SfxDict::const_iterator it = m_sfxDict.find(name);

	if (it != m_sfxDict.end()) {
		SoundFx *result = it->second;
		result->incref();
		return result;
	}

	SoundFx *result = new SoundFx();
	FMOD::Sound *fmodSound;

	FMOD_RESULT hr = m_fmodSystem->createSound(name.c_str(), FMOD_3D | FMOD_SOFTWARE, 0, &fmodSound);

	result->m_name = name;
	result->m_fmodSound = fmodSound;

	m_sfxDict[name] = result;

	return result;
}

void SoundSystem::_removeSfx( SoundFx *sfx )
{
	SfxDict::iterator it = m_sfxDict.find(sfx->m_name);
	if (it == m_sfxDict.end()) {
		Errorf("can't find sfx");
	}

	delete sfx;

	m_sfxDict.erase(it);
}

void SoundSystem::tick()
{
	if (m_activeWorld) {
		const Matrix &mat = m_activeWorld->m_listenerMatrix;
		m_fmodSystem->set3DListenerAttributes(0, tr(mat.origin), tr(m_activeWorld->m_listenerVelocity), tr(mat.axis[0]), tr(mat.axis[2]));
	}
	m_fmodSystem->update();
}

void SoundSystem::playSound( int channelId, SoundFx *sfx, LoopingMode looping /*= Looping_None*/ )
{
	_playSound(0, 0, channelId, sfx, looping);
}

void SoundSystem::stopSound( int channelId )
{
	_stopSound(0, 0, channelId);
}

void SoundSystem::_playSound( SoundWorld *world, SoundEntity *entity, int channelId, SoundFx *sfx, LoopingMode looping /*= Looping_None*/, float minDist /*= DEFAULT_MIN_DIST*/, float maxDist /*= DEFAULT_MAX_DIST*/ )
{
	if (!sfx || !sfx->isValid())
		return;

	PlayMode mode = PlayMode_3D;

	if (!world || !entity)
		mode = PlayMode_2D;

	FMOD_RESULT hr;

	FMOD::Channel *fmodChannel = 0;
	SoundChannel *channel = 0;
	SoundKey key(world,entity,channelId);
	int index = -1;

	if (channelId != ChannelId_Any) {
		Dict<SoundKey,int>::iterator it = m_channelDict.find(key);
		if (it != m_channelDict.end()) {
			index = it->second;
			channel = &m_channels[index];
			fmodChannel = channel->m_fmodChannel;
		}
	}

	if (index == -1) {
		hr = m_fmodSystem->playSound(FMOD_CHANNEL_FREE, sfx->m_fmodSound, true, &fmodChannel);
		if (hr != FMOD_OK)
			return;

		hr = fmodChannel->getIndex(&index);
		ERRCHECK(hr);

		channel = &m_channels[index];
		channel->m_fmodChannel = fmodChannel;
	} else {
		channel->m_fmodChannel->stop();

		hr = m_fmodSystem->playSound(FMOD_CHANNEL_REUSE, sfx->m_fmodSound, true, &fmodChannel);
		if (hr != FMOD_OK)
			return;
	}

	FMOD_MODE fmodMode = FMOD_2D;
	if (mode == PlayMode_3D) {
		fmodMode = FMOD_3D | FMOD_3D_LINEARROLLOFF;
		fmodChannel->set3DAttributes(tr(entity->m_matrix.origin), tr(entity->m_velocity));
	}

	if (looping) {
		fmodMode |= FMOD_LOOP_NORMAL;
	}

	fmodChannel->setMode(fmodMode);
	fmodChannel->setLoopCount(looping);
	fmodChannel->set3DMinMaxDistance(minDist, maxDist);

	channel->m_key = key;

	if (channelId != ChannelId_Any) {
		m_channelDict[key] = index;
	}

	fmodChannel->setCallback(&FMOD_Callback::channelCallback);

	hr = fmodChannel->setPaused(false);
	ERRCHECK(hr);
}

void SoundSystem::_stopSound( SoundWorld *world, SoundEntity *entity, int channelId )
{
	if (channelId == ChannelId_Any)
		return;

	SoundKey key(world, entity, channelId);
	Dict<SoundKey,int>::iterator it = m_channelDict.find(key);
	if (it == m_channelDict.end())
		return;

	int index = it->second;
	SoundChannel &ch = m_channels[index];

	AX_ASSERT(key == ch.m_key);

	ch.m_fmodChannel->stop();
}

void SoundSystem::initFMOD()
{
	FMOD_RESULT result;
	unsigned int version;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS caps;
	char name[256];
	int numdrivers;
	const float DISTANCEFACTOR = 1.0f;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

	result = FMOD::System_Create(&m_fmodSystem);
	ERRCHECK(result);

	result = m_fmodSystem->getVersion(&version);
	ERRCHECK(result);

	if (version < FMOD_VERSION) {
		Errorf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
	}

	result = m_fmodSystem->getNumDrivers(&numdrivers);
	ERRCHECK(result);

	if (numdrivers == 0) {
		result = m_fmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ERRCHECK(result);
	} else {
		result = m_fmodSystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
		ERRCHECK(result);

		result = m_fmodSystem->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
		ERRCHECK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
		{                                                   /* You might want to warn the user about this. */
			result = m_fmodSystem->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}

		result = m_fmodSystem->getDriverInfo(0, name, 256, 0);
		ERRCHECK(result);

		if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
		{
			result = m_fmodSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			ERRCHECK(result);
		}
	}

	result = m_fmodSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL|FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
	{
		result = m_fmodSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ERRCHECK(result);

		result = m_fmodSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL|FMOD_INIT_3D_RIGHTHANDED, 0);/* ... and re-init. */
		ERRCHECK(result);
	}


	/* Set the distance units. (meters/feet etc). */
	result = m_fmodSystem->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
	ERRCHECK(result);

	result = m_fmodSystem->setFileSystem(FMOD_Callback::open, FMOD_Callback::close, FMOD_Callback::read, FMOD_Callback::seek, 0);
	ERRCHECK(result);
}

void SoundSystem::init()
{
	initFMOD();

	TypeZeroArray(m_channels);

	for (int i = 0; i < MAX_CHANNELS; i++) {
		 FMOD_RESULT hr = m_fmodSystem->getChannel(i, &m_channels[i].m_fmodChannel);
		 ERRCHECK(hr);

		 m_channels[i].m_fmodChannel->setCallback(FMOD_Callback::channelCallback);
	}
}

void SoundSystem::_hintChannelEnd(int index)
{
	SoundChannel &channel = m_channels[index];

	if (channel.m_key.channelId == ChannelId_Any)
		return;


	Dict<SoundKey,int>::iterator it = m_channelDict.find(channel.m_key);
	if (it == m_channelDict.end())
		return;

	m_channelDict.erase(it);
}

void SoundSystem::setWorld( SoundWorld *world )
{
	m_activeWorld = world;
}

AX_END_NAMESPACE

