/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_ANIMATED_H
#define AX_GAME_ANIMATED_H

namespace Axon { namespace Game {

	class Animated : public Entity {
	public:
		AX_DECLARE_CLASS(Animated, Entity, "Game.Physics.Animated");
			AX_METHOD(loadAsset)
		AX_END_CLASS()

		Animated();
		virtual ~Animated();

		// implement Entity
		virtual void doThink();

	protected:
		void clear();
		// script method
		void loadAsset(const LuaTable& t);


	protected:
		PhysicsRig* m_rig;
		PhysicsPose* m_pose;
		PhysicsModel* m_model;
		PhysicsAnimation* m_animation;
		PhysicsAnimator* m_animator;
	};

}} // namespace Axon::Game

#endif // end guardian

