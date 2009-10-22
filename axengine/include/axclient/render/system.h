/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_SYSTEM_H
#define AX_RENDER_SYSTEM_H


namespace Axon { namespace Render {

	class Selection;

	class AX_API System : public Object, public ICmdHandler
	{
	public:
		// script
		AX_DECLARE_CLASS(System, Object, "RenderSystem")
			AX_METHOD(info)
			AX_METHOD(test)
		AX_END_CLASS()

		AX_DECLARE_COMMAND_HANDLER(System);

		System();
		~System();

		void initialize();
		void finalize();

		ShaderQuality getShaderQuality();

		const IDriver::Info* getDriverInfo();
		uint_t getBackendCaps();
		int getFrameNum() const;

		// new rendering
		void beginFrame(Target* target);
		Target* getFrameTarget() const { return m_curTarget; }
		void beginScene(const Camera& view);
		void addToScene(World* world);
		void addToScene(Entity* re);
		void addToScene(Primitive* primitive);
		void addToOverlay(Primitive* primitive);
		void endScene();
		void endFrame();

		// new selection
		void beginSelect(const Camera& view);
		void loadSelectId(int id);
		void testActor(Entity* re);
		void testPrimitive(Primitive* prim);
		void testPrimitive(Primitive* prim, const AffineMat& matrix);
		SelectRecordSeq endSelect();

		void screenShot(const String& name, const Rect& rect);

		void info();
		void test(System* system) {}

		// textures for subscene's render target
		Target* createWindowTarget(handle_t wndId, const String& name);

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

		Target* m_curTarget;

		// current view buf
		SceneSeq m_sceneSeq;
		ScenePtr m_curScene;

		// selection camera
		Camera m_selectionCamera;
		Selection* m_selection;

		// actor manager registry
		Sequence<IEntityManager*>	m_entityManagers;
	};

	inline int System::getFrameNum() const {
		return m_frameNum;
	}


}} // namespace Axon::Render

#endif // AX_RENDER_SYSTEM_H
