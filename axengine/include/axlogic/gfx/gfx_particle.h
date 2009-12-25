/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GFX_PARTICLE_H
#define AX_GFX_PARTICLE_H

AX_BEGIN_NAMESPACE

struct Particle {
	Vector3 pos, speed, down, origin, dir;
	Vector3 corners[4];
	float size, life, maxlife;
	int tile;
	Vector4 color;
};

class ParticleEmitter : public GfxObject
{
	AX_DECLARE_CLASS(ParticleEmitter, GfxObject)
		AX_SIMPLEPROP(EmissionSpeed)
		AX_SIMPLEPROP(SpeedVariation)
		AX_SIMPLEPROP(VerticalRange)
		AX_SIMPLEPROP(HorizontalRange)
		AX_SIMPLEPROP(Gravity)
		AX_SIMPLEPROP(Lifespan)
		AX_SIMPLEPROP(EmissionRate)
		AX_SIMPLEPROP(EmissionAreaLength)
		AX_SIMPLEPROP(EmissionAreaWidth)
		AX_SIMPLEPROP(Gravity2)
		AX_SIMPLEPROP(Enabled)
	AX_END_CLASS()

public:
	enum EmitterShape {
		kPlane, kSphere
	};

	ParticleEmitter();
	virtual ~ParticleEmitter();

	// implement GfxObject
	virtual GfxType getGfxType() const { return kParticleEmitter; }
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual Primitives getHitTestPrims();
	virtual void frameUpdate(QueuedScene *qscene);
	virtual void issueToQueue(QueuedScene *qscene);

protected:
	Particle *planeEmit(float w, float l, float spd, float var, float spr, float spr2);
	Particle *sphereEmit(float w, float l, float spd, float var, float spr, float spr2);

private:
	// BEGIN ANIMATABLE PROPERTIES
	float m_EmissionSpeed;
	float m_SpeedVariation;		// Variation in the flying-speed. (range: 0 to 1)
	float m_VerticalRange;		// Drifting away vertically. (range: 0 to pi)
	float m_HorizontalRange;	// They can do it horizontally too! (range: 0 to 2*pi)
	float m_Gravity;			// Fall down, apple!
	float m_Lifespan;			// Everyone has to die.
	float m_EmissionRate;		// Stread your particles, emitter.
	float m_EmissionAreaLength; // Well, you can do that in this area.
	float m_EmissionAreaWidth;
	float m_Gravity2;			// A second gravity? Its strong.
	float m_Enabled;
	// END ANIMATABLE PROPERTIES

	Vector4 m_colors[3];
	float m_sizes[3];
	float m_mid, m_slowdown, m_rotation;
	Vector3 m_pos;
	List<Particle*> m_particles;
	int m_blend, m_order, m_type;
	int m_manim, m_mtime;
	int m_rows, m_cols;
	Sequence<Vector4> m_tiles;
	bool m_billboard;
	float m_rem;
	int m_flags;
};

AX_END_NAMESPACE

#endif

