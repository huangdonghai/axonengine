/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "gfx_local.h"

namespace Axon { namespace Gfx {

	ParticleEmitter::ParticleEmitter()
		: m_speed(0)
		, m_variation(0)
		, m_spread(0)
		, m_lat(0)
		, m_gravity(0)
		, m_lifespan(0)
		, m_rate(0)
		, m_areal(0)
		, m_areaw(0)
		, m_deacceleration(0)
		, m_enabled(0)
	{

	}

	ParticleEmitter::~ParticleEmitter()
	{

	}

	void ParticleEmitter::update()
	{

	}

	void ParticleEmitter::render()
	{

	}

	Particle ParticleEmitter::planeEmit( int anim, int time, float w, float l, float spd, float var, float spr, float spr2 )
	{
		return Particle();
	}

	Particle ParticleEmitter::sphereEmit( int anim, int time, float w, float l, float spd, float var, float spr, float spr2 )
	{
		return Particle();
	}

}} // namespace Axon::Gfx
