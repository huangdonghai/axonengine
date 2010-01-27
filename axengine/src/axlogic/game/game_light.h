/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_LIGHT_H
#define AX_GAME_LIGHT_H

AX_BEGIN_NAMESPACE

class GameLight : public GameActor
{
	AX_DECLARE_CLASS(GameLight, GameActor)
//		AX_METHOD(loadLight)
		AX_METHOD(activeLight)
	AX_END_CLASS()

public:
	GameLight();
	virtual ~GameLight();

	// implement Node
	virtual void doHitTest() const;
	virtual void doDebugRender() const;

	// implememt GameActor
	virtual void doThink();

	// method
//	void loadLight(const LuaTable &v);
	void activeLight(bool isActive);

protected:
	virtual void onReload();
	virtual void onReset();

	void setupHelper();
	void setupPointPrim();
	void setupSpotPrim();

private:
	RenderLight *m_renderLight;
	MeshPrim *m_pointPrim;
	MeshPrim *m_spotPrim;
};


AX_END_NAMESPACE

#endif // AX_GAME_LIGHT_H

