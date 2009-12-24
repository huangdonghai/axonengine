/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_SOUND_H
#define AX_GAME_SOUND_H

AX_BEGIN_NAMESPACE

	class GameSound : public GameActor
	{
		AX_DECLARE_CLASS(GameSound, GameActor)
			AX_METHOD(loadSound)
			AX_METHOD(activeSound)
		AX_END_CLASS()

	public:
		GameSound();
		virtual ~GameSound();

		// implement Node
		virtual void doHitTest() const;
		virtual void doDebugRender() const;
		virtual void clear();

		// implement GameActor
		virtual void doThink();

		// method
		void loadSound(const Variant& v);
		void activeSound(bool isActive);

	private:
		SfxPtr m_sfx;
		float m_minDist, m_maxDist;
		bool m_looping;
		float m_interval;
	};

AX_END_NAMESPACE

#endif