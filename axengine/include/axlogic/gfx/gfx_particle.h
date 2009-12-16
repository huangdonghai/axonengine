/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GFX_PARTICLE_H
#define AX_GFX_PARTICLE_H

namespace Axon { namespace Gfx {

	struct Particle {
		Vector3 pos, speed, down, origin, dir;
		Vector3 corners[4];
		float size, life, maxlife;
		int tile;
		Vector4 color;
	};

	class ParticleEmitter : public GfxObject
	{
	public:
		enum EmitterShape {
			kPlane, kSphere
		};

		ParticleEmitter();
		virtual ~ParticleEmitter();

		// implement GfxObject
		virtual Type getGfxType() const { return kParticleEmitter; }
		virtual void update();
		virtual void render();

	protected:
		Particle planeEmit(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
		Particle sphereEmit(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);

	private:
		FloatTrack m_speed;
		FloatTrack m_variation;
		FloatTrack m_spread;
		FloatTrack m_lat;
		FloatTrack m_gravity;
		FloatTrack m_lifespan;
		FloatTrack m_rate;
		FloatTrack m_areal;
		FloatTrack m_areaw;
		FloatTrack m_deacceleration;
		FloatTrack m_enabled;

		Vector4 m_colors[3];
		float m_sizes[3];
		float m_mid, m_slowdown, m_rotation;
		Vector3 m_pos;
		List<Particle> m_particles;
		int m_blend, m_order, m_type;
		int m_manim, m_mtime;
		int m_rows, m_cols;
		Sequence<Vector4> m_tiles;
		bool m_billboard;
		float m_rem;
		int m_flags;
	};

}} // namespace Axon::Gfx

#endif

