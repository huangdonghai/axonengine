/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "common.fxh"

#define RotationAffector           0
#define FaceToAxis                 1
#define FlickerAffector            2
#define ScaleAffector              3
#define LinearForceAffector        4
#define PlaneCollisionAffector     5
#define AlphaFaderAffector         6
#define ColorFaderAffector         7
#define AnimationAffector          8

#define S_NUM_AFFECTORS		G_LITERAL0
#define S_AFFECTOR_TYPE0    G_LITERAL1
#define S_AFFECTOR_TYPE1    G_LITERAL2
#define S_AFFECTOR_TYPE2    G_LITERAL3
#define S_AFFECTOR_TYPE3    G_LITERAL4
#define S_AFFECTOR_TYPE4    G_LITERAL5
#define S_AFFECTOR_TYPE5    G_LITERAL6

int getMacro(int index) 
{
	if (index == 0) 
	{
		return S_AFFECTOR_TYPE0;
	}
	else if (index == 1) 
	{
		return S_AFFECTOR_TYPE1;
	} 
	else if (index == 2)
	{
		return S_AFFECTOR_TYPE2;
	}
	else if (index == 3)
	{
		return S_AFFECTOR_TYPE3;
	}
	else if (index == 4)
	{
		return S_AFFECTOR_TYPE4;
	}
	else 
	{
		return S_AFFECTOR_TYPE5;
	}
}
