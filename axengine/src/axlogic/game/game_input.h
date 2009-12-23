/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GAME_INPUT_H
#define AX_GAME_INPUT_H

AX_BEGIN_NAMESPACE

	class GameInput : public Object, public ICmdHandler
	{
		AX_DECLARE_COMMAND_HANDLER(GameInput);

		AX_DECLARE_CLASS(GameInput, Object)
			AX_METHOD(pauseGame)
			AX_METHOD(moveForward)
			AX_METHOD(moveBackward)
			AX_METHOD(moveLeft)
			AX_METHOD(moveRight)
			AX_METHOD(moveRush)
			AX_METHOD(toggleView)
			AX_METHOD(zoomInView)
			AX_METHOD(zoomOutView)
		AX_END_CLASS()

	public:
		enum ButtonType {
			MoveForward,
			MoveBackward,
			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,

			Rush,

			ToggleView,
			ZoomInView,
			ZoomOutView,

			TriggerStart,
			ButtonMax = TriggerStart + UserInput::Trigger_Max
		};

		GameInput();
		~GameInput();

		void initialize();
		void finalize();

		void startRunning();
		void runFrame(int msec);
		UserInput genUserInput();
		void stopRunning();

	protected:
		void buttonDown(int bt, Key key, int msec);
		void buttonUp(int bt, Key key, int msec);
		float buttonState(int bt);

		// generate user input
		void genKeyMove(UserInput& userInput);
		void genMouseMove(UserInput& userInput);
		void genButtons(UserInput& userInput);


		// console command
		void bind_f(const CmdArgs& args);
		void bindlist_f(const CmdArgs& args);

		// script command
		void pauseGame(int key, bool isdown);
		void moveForward(int key, bool isdown);
		void moveBackward(int key, bool isdown);
		void moveLeft(int key, bool isdown);
		void moveRight(int key, bool isdown);
		void moveUp(int key, bool isdown);
		void moveDown(int key, bool isdown);
		void moveRush(int key, bool isdown);
		void toggleView(int key, bool isdown);
		void zoomInView(int key, bool isdown);
		void zoomOutView(int key, bool isdown);

	private:
		struct Button {
			Key keys[2];		// key holding it down
			int downtime;		// msec timestamp
			int msec;			// msec down this frame if both a down and up happened
			bool isActive;		// current state
			bool wasPressed;		// set when down, not cleared when up
		};

		Button m_buttons[ButtonMax];
		Dict<Key,String>	m_keybinding;

		int m_frameMsec;

		bool m_mousePosInited;
		Point m_mouseDelta[2];
		int m_mouseIndex;
	};

AX_END_NAMESPACE

#endif // end guardian
