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

	struct Particle
	{
		Vector3 pos, speed, down, origin, dir;
		Vector3 corners[4];
		float size, life, maxlife;
		int tile;
		Vector4 color;
	};

	class ParticleEmitter : public GfxObject
	{
		AX_DECLARE_CLASS(ParticleEmitter, GfxObject, "ParticleEmitter")
			AX_SIMPLEPROP(speed)
			AX_SIMPLEPROP(variation)
			AX_SIMPLEPROP(spread)
			AX_SIMPLEPROP(lat)
			AX_SIMPLEPROP(gravity)
			AX_SIMPLEPROP(lifespan)
			AX_SIMPLEPROP(rate)
			AX_SIMPLEPROP(areal)
			AX_SIMPLEPROP(areaw)
			AX_SIMPLEPROP(deacceleration)
			AX_SIMPLEPROP(enabled)
		AX_END_CLASS()

	public:
		enum EmitterShape {
			kPlane, kSphere
		};

		ParticleEmitter();
		virtual ~ParticleEmitter();

		// implement GfxObject
		virtual GfxType getGfxType() const { return kParticleEmitter; }
		virtual void update();
		virtual void render();

	protected:
		Particle planeEmit(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
		Particle sphereEmit(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);

	private:
		// BEGIN ANIMATABLE PROPERTIES
		float m_speed;
		float m_variation;
		float m_spread;
		float m_lat;
		float m_gravity;
		float m_lifespan;
		float m_rate;
		float m_areal;
		float m_areaw;
		float m_deacceleration;
		float m_enabled;
		// END ANIMATABLE PROPERTIES

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

