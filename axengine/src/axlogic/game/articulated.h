/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_ARTICULATED_H
#define AX_GAME_ARTICULATED_H

namespace Axon { namespace Game {

	class Articulated : public Animated
	{
		AX_DECLARE_CLASS(Articulated, Animated);
		AX_END_CLASS()

	public:
		Articulated();
		virtual ~Articulated();

		// implement Entity
		virtual void doThink();

	protected:
		// properties
		String m_ragdollName;

	protected:
		PhysicsRagdoll* m_ragdoll;
	};

}} // namespace Axon::Game

#endif // end guardian

