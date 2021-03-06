/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

namespace Axon { namespace Editor {

	//------------------------------------------------------------------------------
	// class View
	//------------------------------------------------------------------------------

	View::View(Context* context)
		: m_context(context)
		, m_frame(NULL)
		, m_frameNumCur(0)
		, m_cursorPos(0.0f, 0.0f, 0.0f)
		, m_title("untitled")
	{
		m_font = g_consoleFont;
#if 0
		m_eyeAxis.setIdentity();
#else
		m_eyeMatrix.setIdentity();
		m_eyeMatrix.setOrigin(0, 0, 20);
#endif
		m_camera.setClearColor(Rgba::MdGrey);
		m_frameOldTime = OsUtil::milliseconds();
		m_frameTime = 0;

		m_autoUpdate = false;
	}

	View::~View() {
	}

	// implement View
	void View::bindFrame(IFrame* container) {
		m_frame = container;
	}

	String& View::getTitle() {
		return m_title;
	}

	void View::doUpdate() {
		if (g_gameSystem->isRunning()) {
			return;
		}

		uint_t now = OsUtil::milliseconds();
		m_frameTime = now - m_frameOldTime;
		m_frameOldTime = now;

		if (m_frameTime > 2000) {
//			Printf("DEBUG");
		}

		preUpdate();

		m_camera.setTarget(m_frame->getRenderTarget());
		m_camera.setViewRect(m_frame->getRenderTarget()->getRect());
		m_camera.setOrigin(m_eyeMatrix.origin);
		m_camera.setViewAxis(m_eyeMatrix.axis);
		m_camera.setFov(0, 60);
		m_camera.setTime(now);

		g_renderSystem->beginFrame(m_camera.getTarget());
#if 0
		gRenderSystem->beginScene(m_camera);
#endif
		if (!m_context->isLoading()) {
			doRender();
		}
#if 0
		gRenderSystem->endScene();
#endif
		drawAxis();

		//t = Milliseconds();
		g_renderSystem->endFrame();

		m_frameNumCur++;
	}

	void View::drawAxis() {
		Render::Camera rv;

		float line_length = 1.0f;
		float view_size = line_length * 2.0f + 1.0f;

		rv.setTarget(m_frame->getRenderTarget());
		rv.setOrigin(Vector3(0,0,0));
		rv.setViewAxis(m_camera.getViewAxis());
		rv.setOrtho(view_size, view_size, view_size);
		rv.setViewRect(Rect(0, -12, 60, 60));

		g_renderSystem->beginScene(rv);

		Render::Line* axis_line = Render::Line::createAxis(Render::Primitive::OneFrame, line_length);
		g_renderSystem->addToScene(axis_line);

#if 1
		line_length += 0.2f;
		Render::Text* text = Render::Text::createSimpleText(Render::Primitive::OneFrame, Vector3(line_length, 0, 0), Rgba::Red, "x");
		g_renderSystem->addToScene(text);

		text = Render::Text::createSimpleText(Render::Primitive::OneFrame, Vector3(0, line_length, 0), Rgba::Green, "y");
		g_renderSystem->addToScene(text);

		text = Render::Text::createSimpleText(Render::Primitive::OneFrame, Vector3(0, 0, line_length), Rgba::Blue, "z");
		g_renderSystem->addToScene(text);

		g_renderSystem->endScene();

		drawFrameNum();
#endif
	}

	void View::drawFrameNum() {
		Render::Camera rv;

		rv.setTarget(m_frame->getRenderTarget());
		rv.setOverlay(m_camera.getViewRect());

		Rect rect;
		String text;

#if 1
		const int AccTimeInterval = 1000; // per second calc
		static int OneSecondTimeAcc = 0;
		static int OneSecondFrameAcc = 0;
		++OneSecondFrameAcc;
		if (m_frameTime > 0) {
			OneSecondTimeAcc += m_frameTime;
			if (OneSecondTimeAcc >= AccTimeInterval) {
				m_fps = (float)OneSecondFrameAcc / (float)OneSecondTimeAcc * 1000.0f;
				OneSecondTimeAcc = 0;
				OneSecondFrameAcc = 0;
			}
		}
#else
		if (m_frameTime > 0) {
			m_fps = 1000.0f / m_frameTime;
		}
#endif

		m_frameNumCur++;
		StringUtil::sprintf(text, "FPS:%3.1f FT:%3d FE:%d BE:%d", m_fps, m_frameTime, g_statistic->getValue(stat_frontendTime), g_statistic->getValue(stat_backendTime));

		rect.x = 10;
		rect.y = m_camera.getViewRect().height - 24;
		rect.width = 120;
		rect.height = 24;
		Render::Text* prim = Render::Text::createText(Render::Primitive::OneFrame, rect, m_font, text, Rgba::Green, Render::Text::Left);

		g_renderSystem->beginScene(rv);
		g_renderSystem->addToScene(prim);
		g_renderSystem->endScene();
	}

	void View::handleEvent(Input::Event* e) {
		Tool* t = m_context->getTool();

		if (t) {
			t->setView(this);
			t->handleEvent(e);
		}

		if (!e->accepted) {
			IEventHandler::handleEvent(e);
		}

#if 1
		doUpdate();
#endif
	}

	bool View::traceWorld(int x, int y, Vector3& result, int part) {
		if (selectRegion(Rect(x-1, y-1, 3, 3), part, result)) {
			return true;
		}

		Plane ground(0, 0, 1, 0);
		Vector3 start = m_camera.screenToWorld(Vector3(x, y, -1));
		Vector3 end = m_camera.screenToWorld(Vector3(x, y, 1));
		Vector3 dir = (end - start).getNormalized();
		float scale;

		bool v = ground.rayIntersection(start, dir, scale);

		if (!v)
			return false;

		result = start + dir * scale;

		if (m_context->getState()->isSnapToGrid) {
			result = Internal::snap(result, m_context->getState()->snapToGrid);
		}

		m_context->getState()->setTransformState(false, false, result);

		return true;
	}

	bool View::traceWorld(Vector3& result) {
		Point center = m_frame->getRect().getCenter();
		return traceWorld(center.x, center.y, result, SelectPart::All);
	}

	void View::beginSelect(const Rect& r) {
		Render::Camera cam = m_camera.createSelectionCamera(r);

		g_renderSystem->beginSelect(cam);
	}

	int View::endSelect() {
		Render::SelectRecordSeq records = g_renderSystem->endSelect();

		if (records.empty())
			return -1;

		float minz = 1.0f;
		int nearest = -1;
		for (size_t i = 0; i < records.size(); i++) {
			int id = records[i].name;

			if (records[i].minz < minz) {
				minz = records[i].minz;
				nearest = id;
			}
		}

		return nearest;
	}

	bool View::selectRegion(const Rect& rect, SelectPart part, OUT Vector3& pos, OUT ActorList& retlist, bool onlynearest) {
		retlist.clear();

		Render::Camera cam = m_camera.createSelectionCamera(rect);

		g_renderSystem->beginSelect(cam);

		m_context->doSelect(cam, part);

		Render::SelectRecordSeq records = g_renderSystem->endSelect();

		if (records.empty())
			return false;

		float minz = 1.0f;
		Actor* minzActor = 0;
		for (size_t i = 0; i < records.size(); i++) {
			int id = records[i].name;
			Actor* ed = m_context->findActor(id);

			AX_ASSERT(ed);

			if (!onlynearest) {
				retlist.push_back(ed);
			}

			if (records[i].minz < minz) {
				minz = records[i].minz;
				minzActor = ed;
			}
		}

		if (onlynearest && minzActor) {
			retlist.push_back(minzActor);
		}

		Point center = rect.getCenter();
		Vector3 v(center.x, center.y, minz);

		pos = m_cursorPos = cam.screenToWorld(v);

		return true;
	}

	bool View::selectRegion(const Rect& rect, SelectPart part, OUT Vector3& pos) {
		Render::Camera cam = m_camera.createSelectionCamera(rect);

		g_renderSystem->beginSelect(cam);
#if 0
		// select terrain
		if (part & SelectPart::Terrain) {
			Map::Terrain* terrain = m_context->getTerrain();
			if (terrain)
				terrain->doSelect(cam);
		}
#endif
		m_context->doSelect(cam, part);

		Render::SelectRecordSeq records = g_renderSystem->endSelect();

		if (records.empty())
			return false;

		float minz = 1.0f;
		for (size_t i = 0; i < records.size(); i++) {
			if (records[i].minz < minz)
				minz = records[i].minz;
		}

		Point center = rect.getCenter();
		Vector3 v(center.x, center.y, minz);

		pos = m_cursorPos = cam.screenToWorld(v);

		return true;
	}

	void View::setAutoUpdate(AutoUpdate update) {
		if (update == Force) {
			m_frame->setAutoUpdate(true);
		} else if (update == Default) {
			m_frame->setAutoUpdate(m_autoUpdate);
		} else {
			m_frame->setAutoUpdate(false);
		}
	}


}} // namespace Axon::Editor
