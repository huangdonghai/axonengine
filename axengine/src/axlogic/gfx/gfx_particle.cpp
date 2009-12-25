/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "gfx_local.h"

namespace {
	AX_USE_NAMESPACE;

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
} // anonymous namespace

AX_BEGIN_NAMESPACE

static BlockAlloc<Particle> ParticleAllocator;

ParticleEmitter::ParticleEmitter()
{

}

ParticleEmitter::~ParticleEmitter()
{

}

Particle* ParticleEmitter::planeEmit(float w, float l, float spd, float var, float spr, float spr2)
{
	return 0;
}

Particle* ParticleEmitter::sphereEmit(float w, float l, float spd, float var, float spr, float spr2)
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

void ParticleEmitter::frameUpdate( QueuedScene *qscene )
{

}

void ParticleEmitter::issueToQueue( QueuedScene *qscene )
{

}

AX_END_NAMESPACE
