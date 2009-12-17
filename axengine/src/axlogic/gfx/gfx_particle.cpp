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
		: m_speed("Speed")
		, m_variation("Variation")
		, m_spread("Spread")
		, m_lat("Lat")
		, m_gravity("Gravity")
		, m_lifespan("LifeSpan")
		, m_rate("Rate")
		, m_areal("AreaL")
		, m_areaw("AreaW")
		, m_deacceleration("Deacceleration")
		, m_enabled("Enabled")
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
