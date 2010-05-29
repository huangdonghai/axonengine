/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "gfx_local.h"

AX_BEGIN_NAMESPACE

namespace {

	float frand()
	{
		return rand()/(float)RAND_MAX;
	}

	float randfloat(float lower, float upper)
	{
		return lower + (upper-lower)*(rand()/(float)RAND_MAX);
	}

	int randint(int lower, int upper)
	{
		return lower + (int)((upper+1-lower)*frand());
	}

	//Generates the rotation matrix based on spread
	Matrix4 SpreadMat;
	void CalcSpreadMatrix(float Spread1, float Spread2, float w, float l)
	{
		int i,j;
		float a[2], c[2], s[2];
		Matrix4 Temp;

		Temp.setIdentity();

		a[0] = randfloat(-Spread1, Spread1)/2.0f;
		a[1] = randfloat(-Spread2, Spread2)/2.0f;

		for (i=0; i<2; i++) {		
			c[i]=cos(a[i]);
			s[i]=sin(a[i]);
		}

		Temp.setIdentity();
		Temp.m[1][1]=c[0];
		Temp.m[2][1]=s[0];
		Temp.m[2][2]=c[0];
		Temp.m[1][2]=-s[0];

		SpreadMat=SpreadMat*Temp;

		Temp.setIdentity();
		Temp.m[0][0]=c[1];
		Temp.m[1][0]=s[1];
		Temp.m[1][1]=c[1];
		Temp.m[0][1]=-s[1];

		SpreadMat=SpreadMat*Temp;

		float Size=abs(c[0])*l+abs(s[0])*w;
		for (i=0; i<3; i++)
			for (j=0; j<3; j++)
				SpreadMat.m[i][j]*=Size;
	}

	template<class T>
	T lifeRamp(float life, float mid, const T &a, const T &b, const T &c)
	{
		if (life<=mid) return interpolate<T>(life / mid,a,b);
		else return interpolate<T>((life-mid) / (1.0f-mid),b,c);
	}

	static BlockAlloc<Particle> ParticleAllocator;

} // anonymous namespace

// 中文恰好是两倍宽度
// 中文恰好是两倍宽度
ParticleEmitter::ParticleEmitter()
{
	// animatable
	m_EmissionSpeed = 1.0f;
	m_SpeedVariation = 0;
	m_VerticalRange = 0;		// Drifting away vertically. (range: 0 to pi)
	m_HorizontalRange = 0;		// They can do it horizontally too! (range: 0 to 2*pi)
	m_Gravity = 0.0f;			// Fall m_down, apple!
	m_Lifespan = 10.0f;			// Everyone has to die.
	m_EmissionRate = 100.0f;	// Stread your particles, emitter.
	m_EmissionAreaLength = 1;	// Well, you can do that in this area.
	m_EmissionAreaWidth = 1;
	m_Gravity2 = 0;				// A second gravity? Its strong.
	m_Enabled = 1.0f;

	// init info
	for (int i = 0; i < 3; i++) {
		m_colors[i] = Vector4::One;
		m_sizes[i] = 0.25f;
	}

	m_mid = 0.5f;
	m_slowdown = 0;
	m_rotation = 0;
//	int m_blend, m_order, m_type;
	m_billboard = true;
	m_flags = 0;

	// tiles
	m_tileRows = 1;
	m_tileCols = 1;

	for (int i = 0; i < m_tileRows; i++) {
		float ds = 1.0f / m_tileCols;
		float dt = 1.0f / m_tileRows;
		float t = float(i) / m_tileRows;
		for (int j = 0; j < m_tileCols; j++) {
			float s = float(j) / m_tileCols;

			Vector4 tc(s, t, s+ds, t+dt);
			m_tiles.push_back(tc);
		}
	}

	// runtime
	m_remain = 0;
	m_mesh = new MeshPrim(Primitive::HintDynamic);
}

ParticleEmitter::~ParticleEmitter()
{

}

Particle *ParticleEmitter::planeEmit(float width, float length, float speed, float variant, float spread, float spread2)
{
	Particle *p = ParticleAllocator.alloc();

	p->m_pos = Vector3(randfloat(-length,length), randfloat(-width,width), 0);
	Vector3 dir = Vector3(0,0,1.0f);

	p->m_dir = dir;//.normalize();
	p->m_down = Vector3(0,0,-1.0f); // dir * -1.0f;
	p->m_speed = dir * speed * (1.0f+randfloat(-variant,variant));

	p->m_life = 0;
	p->m_maxlife = m_Lifespan;
	p->m_origin = p->m_pos;
	p->m_tile = 0;

	// transform to world space
	p->m_pos = m_objToWorld * p->m_pos;

	return p;
}

Particle *ParticleEmitter::sphereEmit(float w, float l, float spd, float var, float spr, float spr2)
{
	return 0;
}

BoundingBox ParticleEmitter::getLocalBoundingBox()
{
	return BoundingBox::UnitBox;
}

BoundingBox ParticleEmitter::getBoundingBox()
{
	return getLocalBoundingBox().getTransformed(m_tm);
}

Primitives ParticleEmitter::getHitTestPrims()
{
	return Primitives();
}

void ParticleEmitter::frameUpdate(RenderScene *qscene)
{
	// object to world
	m_objToWorld = m_entity->getMatrix() * m_tm;

	// spawn new particles
	float dt = qscene->camera.getFrameTime();
	dt = 0.02f;

	float ftospawn = (dt * m_EmissionRate / m_Lifespan) + m_remain;
	if (ftospawn < 1.0f) {
		m_remain = ftospawn;
		if (m_remain<0) 
			m_remain = 0;
	} else {
		int tospawn = (int)ftospawn;

		if ((tospawn + m_particles.size()) > MAX_PARTICLES) // Error check to prevent the program from trying to load insane amounts of particles.
			tospawn = (int)m_particles.size() - MAX_PARTICLES;

		m_remain = ftospawn - (float)tospawn;

		float w = m_EmissionAreaWidth * 0.5f;
		float l = m_EmissionAreaLength * 0.5f;
		float spd = m_EmissionSpeed;
		float var = m_SpeedVariation;
		float spr = m_VerticalRange;
		float spr2 = m_HorizontalRange;
		bool en = m_Enabled != 0;

		if (en) {
			for (int i=0; i<tospawn; i++) {
				Particle *p = planeEmit(w, l, spd, var, spr, spr2);
				m_particles.push_back(p);
			}
		}
	}

	float mspeed = 1.0f;

	for (ParticleList::iterator it = m_particles.begin(); it != m_particles.end(); /*++it*/) {
		Particle &p = *it;
		p.m_speed += p.m_down * m_Gravity * dt - p.m_dir * m_Gravity2 * dt;

		if (m_slowdown>0) {
			mspeed = expf(-1.0f * m_slowdown * p.m_life);
		}
		p.m_pos += p.m_speed * mspeed * dt;

		p.m_life += dt;
		float rlife = p.m_life / p.m_maxlife;
		// calculate size and m_color based on lifetime
		p.m_size = lifeRamp<float>(rlife, m_mid, m_sizes[0], m_sizes[1], m_sizes[2]);
		p.m_color = lifeRamp<Vector4>(rlife, m_mid, m_colors[0], m_colors[1], m_colors[2]);

		// kill off old particles
		if (rlife >= 1.0f) 
			it = m_particles.erase(it);
		else 
			++it;
	}

	//
	// setup mesh
	//

	int numVerts = m_particles.size() * 4;
	int numIndexes = m_particles.size() * 6;
	checkMesh(numVerts, numIndexes);
	const Matrix3& viewaxis = qscene->camera.getViewAxis();

	const Vector3& right = viewaxis[1];
	const Vector3& up = viewaxis[2];

	MeshVertex *verts = m_mesh->lockVertexes();

	for (ParticleList::iterator it = m_particles.begin(); it != m_particles.end(); ++it) {
		Particle &p = *it;
		Vector4 &tc = m_tiles[p.m_tile];

		verts[0].xyz = p.m_pos - (right + up) * p.m_size;
		verts[0].st.set(tc[0], tc[0]);
		verts[0].rgba = p.m_color;

		verts[1].xyz = p.m_pos + (right - up) * p.m_size;
		verts[1].st.set(tc[1], tc[0]);
		verts[1].rgba = p.m_color;

		verts[2].xyz = p.m_pos + (right + up) * p.m_size;
		verts[2].st.set(tc[1], tc[1]);
		verts[2].rgba = p.m_color;

		verts[3].xyz = p.m_pos - (right - up) * p.m_size;
		verts[3].st.set(tc[0], tc[1]);
		verts[3].rgba = p.m_color;

		verts += 4;
	}

	m_mesh->unlockVertexes();
}

void ParticleEmitter::issueToQueue(RenderScene *qscene)
{
	if (!m_Enabled)
		return;

	qscene->addInteraction(m_entity, m_mesh);
}

void ParticleEmitter::checkMesh(int numverts, int numindices)
{
	int oldNumVerts = m_mesh->getNumVertexes();
	int oldNumIndices = m_mesh->getNumIndexes();

	if (numverts <= oldNumVerts && numindices <= oldNumIndices)
		return;

	m_mesh->init(numverts, numindices);

	// fill indices
	ushort_t *indices = m_mesh->lockIndexes();
	for (int i = 0; i < numindices / 6; i++) {
		indices[i*6 + 0] = i * 4;
		indices[i*6 + 1] = i * 4 + 1;
		indices[i*6 + 2] = i * 4 + 2;
		indices[i*6 + 3] = i * 4 + 0;
		indices[i*6 + 4] = i * 4 + 2;
		indices[i*6 + 5] = i * 4 + 3;
	}
	m_mesh->unlockIndexes();
}

AX_END_NAMESPACE
