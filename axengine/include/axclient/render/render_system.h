/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_SYSTEM_H
#define AX_RENDER_SYSTEM_H

AX_BEGIN_NAMESPACE

class Selection;

class AX_API RenderSystem : public Object, public ICmdHandler, public ITickable
{
public:
	// script
	AX_DECLARE_CLASS(RenderSystem, Object)
		AX_METHOD(info)
		AX_METHOD(testArgs)
	AX_END_CLASS()

	AX_DECLARE_COMMAND_HANDLER(RenderSystem);

	RenderSystem();
	~RenderSystem();

	void initialize();
	void finalize();

	// implement ITickable
	virtual void tick();

	int getFrameNum() const;

	// new rendering
	void beginFrame(RenderTarget *target);
	RenderTarget *getFrameTarget() const { return m_curTarget; }
	void beginScene(const RenderCamera &view);
	void addToScene(RenderWorld *world);
	void addToScene(Primitive *primitive);
	void addToOverlay(Primitive *primitive);
	void endScene();
	void endFrame();

	// new selection
	void beginHitTest(const RenderCamera &view);
	void loadHitId(int id);
	void hitTest(RenderEntity *re);
	void hitTest(Primitive *prim);
	void hitTest(Primitive *prim, const Matrix &matrix);
	HitRecords endHitTest();

	void screenShot(const std::string &name, const Rect &rect);

	void info();
	int testArgs(int arg0, float arg1, const Vector3 &arg2, const Color3 &arg3, const Rect &arg4);

#if 0
	// textures for subscene's render target
	RenderTarget *createWindowTarget(Handle wndId, const String &name);
#endif

	// actor manager register
	void addEntityManager(IEntityManager *manager);
	void removeEntityManager(IEntityManager *manager);
	int getNumEntityManager() const;
	IEntityManager *getEntityManager(int index) const;

	// resource management
	ShadowMap *allocShadowMap(int width, int height);
	void freeShadowMap(ShadowMap *target);

	ReflectionMap *findReflection(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height);

	static Rect getWindowRect(Handle hwnd);
	static Size getScreenSize();

protected:
	// console command
	void texlist_f(const CmdArgs &args);
	void matlist_f(const CmdArgs &args);

public:
	std::string name;

private:
	bool m_isMTrendering;
	bool m_initialized;
	bool m_isSelectMode;
	int m_frameNum;

	RenderTarget *m_curTarget;
	RenderScene *m_curScene;

	// selection camera
	RenderCamera m_selectionCamera;
	Selection *m_selection;

	// actor manager registry
	std::vector<IEntityManager*> m_entityManagers;
};

inline int RenderSystem::getFrameNum() const
{
	return m_frameNum;
}

AX_END_NAMESPACE

#endif // AX_RENDER_SYSTEM_H
