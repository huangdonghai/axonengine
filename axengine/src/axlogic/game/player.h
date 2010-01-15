/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_PLAYER_H
#define AX_GAME_PLAYER_H

AX_BEGIN_NAMESPACE

	class Player : public GameActor
	{
		AX_DECLARE_CLASS(Player, GameActor)
		AX_END_CLASS()

	public:
		Player();
		virtual ~Player();

		// implement GameActor
		virtual void doThink();
		virtual bool isPlayer() const { return true; }

		void setMatrix(const Matrix3x4 &matrix);
		Matrix3x4 getMatrix() const;
		Matrix3x4 getThirdPersonMatrix() const;

	protected:
		virtual void reload();
		virtual void clear();
		virtual void onMatrixChanged();
		virtual void onPropertyChanged();


	private:
		HavokModel *m_body;
		PhysicsPlayer *m_physicsPlayer;
		int m_health;
	};

AX_END_NAMESPACE

#endif // end guardian

