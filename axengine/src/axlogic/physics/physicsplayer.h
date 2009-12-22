/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_PHYSICS_PLAYER_H
#define AX_PHYSICS_PLAYER_H

AX_BEGIN_NAMESPACE

	class PhysicsPlayer : public PhysicsEntity {
	public:
		enum MoveType {
			MoveType_Normal,
			MoveType_Dead,
			MoveType_Spectator,
			MoveType_Freeze,
			MoveType_Noclip
		};

		enum MoveFlag {
			MoveFlag_Ducked = 1,
			MoveFlag_Jumped = 2,
			MoveFlag_SteppedUp = 4,
			MoveFlag_SteppedDown = 8,
			MoveFlag_JumpHeld = 16,
			MoveFlag_TimeLand = 32,
			MoveFlag_TimeKnockback = 64,
			MoveFlag_TimeWaterjump = 128,
			MoveFlag_AllTimes = (MoveFlag_TimeWaterjump|MoveFlag_TimeLand|MoveFlag_TimeKnockback),
		};
		typedef Flags_<MoveFlag> MoveFlags;

		enum InputState {
			IN_FORWARD = AX_BIT(0),
			IN_BACKWARD = AX_BIT(1),
			IN_LEFT = AX_BIT(2),
			IN_RIGHT = AX_BIT(3),
			IN_TURN_LEFT = AX_BIT(4),
			IN_TURN_RIGHT = AX_BIT(5),

			IN_STAND = AX_BIT(6),
			IN_CROUCH = AX_BIT(7),
			IN_PRONE = AX_BIT(8),
			IN_SWIN = AX_BIT(9),

			IN_ONGROUND = AX_BIT(10),
			IN_ONLADDER = AX_BIT(11),
			IN_PAIN = AX_BIT(12),
			IN_RUSH = AX_BIT(13),
			IN_JUMP = AX_BIT(14),
			IN_HARDLANDING = AX_BIT(15),
			IN_SOFTLANDING = AX_BIT(16),
			IN_RELOAD = AX_BIT(17),
			IN_ATTACK_HELD = AX_BIT(18),
			IN_WEAPON_FIRED = AX_BIT(19),
		};
		typedef Flags_<InputState> InputStates;

		enum ViewType {
			ViewType_FirstPerson,
			ViewType_ThirdPerson,
		};

		enum WaterLevel {
			WaterLevel_None,
			WaterLevel_Feet,
			WaterLevel_Waist,
			WaterLevel_Head
		};

		friend class Player;

		PhysicsPlayer();
		virtual ~PhysicsPlayer();

		// implement physics entity
		virtual void setActive(bool activate);
		virtual bool isActive() const;
		virtual void setAutoDeactive(bool val);
		virtual void setMatrix(const AffineMat& matrix);
		virtual AffineMat getMatrix() const;
		virtual Type getType() const { return kPlayer; }
		virtual void bind(PhysicsWorld* world);
		virtual void unbind(PhysicsWorld* world);

		// get & set
		void setLinearVelocity(const Vector3& velocity);
		HavokPose* getPose() const { return m_bodyAnimContext.getPose(); }
		AffineMat getThirdPerson();

		void runFrame(const UserInput& userinput, int msec);

	protected:
		void updateViewAxis();

		float getInputScale();
		bool slideMove(bool gravity, bool stepUp, bool stepDown, bool push);
		void waterMove();
		void flyMove();
		void airMove();
		void walkMove();
		void deadMove();
		void noclipMove();
		void spectatorMove();
		void ladderMove();

		void checkGround();

	private:
		class PlayerAnimContext : public AnimationContext {
		public:
			PlayerAnimContext();
			virtual ~PlayerAnimContext();

			void setInputStates(InputStates inputstates);

		protected:
		private:
		};

		hkpCharacterProxy* m_characterProxy;
		PlayerAnimContext m_bodyAnimContext;

		// runtime
		ViewType m_viewType;
		float m_viewDist;

		UserInput m_userInput;
		int m_frameMsec;
		float m_frameSec;
		Angles m_viewAngles;
		Matrix3 m_viewAxis;
		MoveType m_moveType;
		MoveFlags m_moveFlags;
		WaterLevel m_waterLevel;

		float m_speed;

		hkpSurfaceInfo m_ground;

		// animstate
		InputStates m_inputStates;
	};

AX_END_NAMESPACE

#endif // end guardian

