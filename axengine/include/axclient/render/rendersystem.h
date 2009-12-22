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

	class AX_API RenderSystem : public Object, public ICmdHandler
	{
	public:
		// script
		AX_DECLARE_CLASS(RenderSystem, Object, "RenderSystem")
			AX_METHOD(info)
			AX_METHOD(test)
		AX_END_CLASS()

		AX_DECLARE_COMMAND_HANDLER(RenderSystem);

		RenderSystem();
		~RenderSystem();

		void initialize();
		void finalize();

		ShaderQuality getShaderQuality();

		const IRenderDriver::Info* getDriverInfo();
		uint_t getBackendCaps();
		int getFrameNum() const;

		// new rendering
		void beginFrame(RenderTarget* target);
		RenderTarget* getFrameTarget() const { return m_curTarget; }
		void beginScene(const RenderCamera& view);
		void addToScene(RenderWorld* world);
		void addToScene(RenderEntity* re);
		void addToScene(Primitive* primitive);
		void addToOverlay(Primitive* primitive);
		void endScene();
		void endFrame();

		// new selection
		void beginSelect(const RenderCamera& view);
		void loadSelectId(int id);
		void testActor(RenderEntity* re);
		void testPrimitive(Primitive* prim);
		void testPrimitive(Primitive* prim, const AffineMat& matrix);
		SelectRecordSeq endSelect();

		void screenShot(const String& name, const Rect& rect);

		void info();
		void test(RenderSystem* system) {}

		// textures for subscene's render target
		RenderTarget* createWindowTarget(handle_t wndId, const String& name);

		// actor manager register
		void addEntityManager(IEntityManager* manager);
		void removeEntityManager(IEntityManager* manager);
		int getNumEntityManager() const;
		IEntityManager* getEntityManager(int index) const;

	protected:
		// console command
		void texlist_f(const CmdArgs& args);
		void matlist_f(const CmdArgs& args);

	public:
		String name;

	private:
		bool m_isMTrendering;
		bool m_initialized;
		ShaderQuality m_shaderQuality;
		bool m_isSelectMode;
		int m_frameNum;

		RenderTarget* m_curTarget;

		// current view buf
		SceneSeq m_sceneSeq;
		ScenePtr m_curScene;

		// selection camera
		RenderCamera m_selectionCamera;
		Selection* m_selection;

		// actor manager registry
		Sequence<IEntityManager*>	m_entityManagers;
	};

	inline int RenderSystem::getFrameNum() const {
		return m_frameNum;
	}


AX_END_NAMESPACE

#endif // AX_RENDER_SYSTEM_H
