/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_ANIMATED_H
#define AX_GAME_ANIMATED_H

AX_BEGIN_NAMESPACE

class Animated : public GameActor
{
	AX_DECLARE_CLASS(Animated, GameActor);
//		AX_METHOD(loadAsset)
	AX_END_CLASS()

public:
	Animated();
	virtual ~Animated();

	// implement GameActor
	virtual void doThink();

protected:
	void onReset();
	// script method
//	void loadAsset(const LuaTable &t);


protected:
	HavokRig *m_rig;
	HavokPose *m_pose;
	HavokModel *m_model;
	HavokAnimation *m_animation;
	HavokAnimator *m_animator;
};

AX_END_NAMESPACE

#endif // end guardian

