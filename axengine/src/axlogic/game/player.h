/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_PLAYER_H
#define AX_GAME_PLAYER_H

namespace Axon { namespace Game {

	class Player : public GameActor
	{
		AX_DECLARE_CLASS(Player, GameActor, "Game.Player")
		AX_END_CLASS()

	public:
		Player();
		virtual ~Player();

		// implement GameActor
		virtual void doThink();
		virtual bool isPlayer() const { return true; }

		void setMatrix(const AffineMat& matrix);
		AffineMat getMatrix() const;
		AffineMat getThirdPersonMatrix() const;

	protected:
		virtual void reload();
		virtual void clear();
		virtual void onMatrixChanged();
		virtual void onPropertyChanged();


	private:
		HavokModel* m_body;
		PhysicsPlayer* m_physicsPlayer;
		int m_health;
	};

}} // namespace Axon::Game

#endif // end guardian

