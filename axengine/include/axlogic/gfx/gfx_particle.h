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

template<class T_>
class IndexedArrayIterator
{
public:
	AX_STATIC_ASSERT(std::tr1::is_pointer<T_>::value);
	typedef IndexedArrayIterator<T_> _Myt;
	typedef T_ value_type;
	typedef T_ &reference;
	typedef T_ *pointer;
	typedef int size_type;

	reference operator*() {
		return *m_pos;
	}

	pointer operator->() {
		return m_pos;
	}

	_Myt &operator++() {

	}

	_Myt operator++(int) {

	}

private:
	pointer m_pos;
};

template<class T_, int N_>
class IndexedArray {
public:
	AX_STATIC_ASSERT(std::tr1::is_pointer<T_>::value);
	typedef T_ value_type;
	typedef T_ &reference;
	typedef T_ *pointer;
	typedef int size_type;
	typedef IndexedArrayIterator<T_> iterator;
	static const size_type npos = -1;

	IndexedArray();
	~IndexedArray();

	size_type add(const T_ &val);
	reference at(size_type index);

	// iterator
	iterator begin();
	iterator end();

private:
	value_type m_array[N_];
	pointer m_freeList;
	pointer m_start;	// for iterator
	pointer m_end;		// for iterator
};

class Particle {
	friend class ParticleEmitter;
public:
	Particle()
	{
		m_tile = 0;
	}

	~Particle() {}

private:
	IntrusiveLink<Particle> m_link;
	Vector3 m_pos, m_speed, m_down, m_origin, m_dir;
	Vector3 m_corners[4];
	float m_size, m_life, m_maxlife;
	int m_tile;
	Vector4 m_color;
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
	enum {
		MAX_PARTICLES = 10000
	};
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
	virtual void frameUpdate(RenderScene *qscene);
	virtual void issueToQueue(RenderScene *qscene);

protected:
	Particle *planeEmit(float width, float length, float speed, float variant, float spread, float spread2);
	Particle *sphereEmit(float w, float l, float spd, float var, float spr, float spr2);

	void checkMesh(int numverts, int numindices);

private:
	// BEGIN ANIMATABLE PROPERTIES
	float m_EmissionSpeed;
	float m_SpeedVariation;		// Variation in the flying-m_speed. (range: 0 to 1)
	float m_VerticalRange;		// Drifting away vertically. (range: 0 to pi)
	float m_HorizontalRange;	// They can do it horizontally too! (range: 0 to 2*pi)
	float m_Gravity;			// Fall m_down, apple!
	float m_Lifespan;			// Everyone has to die.
	float m_EmissionRate;		// Stread your particles, emitter.
	float m_EmissionAreaLength; // Well, you can do that in this area.
	float m_EmissionAreaWidth;
	float m_Gravity2;			// A second gravity? Its strong.
	float m_Enabled;
	// END ANIMATABLE PROPERTIES

	// init info
	Vector4 m_colors[3];
	float m_sizes[3];
	float m_mid, m_slowdown, m_rotation;
	Vector3 m_pos;
	int m_blend, m_order, m_type;
	int m_tileRows, m_tileCols;
	Sequence<Vector4> m_tiles;
	bool m_billboard;
	int m_flags;

	// runtime
	MeshPrim *m_mesh;
	float m_remain;
	typedef IntrusiveList<Particle> ParticleList;
	ParticleList m_particles;
	Matrix m_objToWorld;
};

AX_END_NAMESPACE

#endif

