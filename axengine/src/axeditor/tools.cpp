/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

AX_BEGIN_NAMESPACE
	//------------------------------------------------------------------------------
	// class Tool
	//------------------------------------------------------------------------------

	Tool::Tool(Context* context) : m_context(context), m_isPressed(false), m_view(nullptr) {
	}

	Tool::~Tool() {
		if (m_view) {
			m_view->resetCursor();
		}
	}

	void Tool::handleEvent(Input::Event* e) {
		if (e->flags & Input::Event::AltModifier)
			return;

#if 0 // we need shift flags for tools
		if (e->flags & Input::Event::ShiftModifier)
			return;
#endif

		IEventHandler::handleEvent(e);
	}

	void Tool::onMouseDown(Input::Event* e) {
		if (e->key != Key::MouseLeft)
			return;

		m_isPressed = true;
		doPress(e->pos.x, e->pos.y, e->flags, 1);
		e->accepted = true;

		return;
	}

	void Tool::onMouseUp(Input::Event* e) {
		if (e->key != Key::MouseLeft)
			return;

		e->accepted = true;
		m_isPressed = false;

		doRelease(e->pos.x, e->pos.y);
	}

	void Tool::onMouseMove(Input::Event* e) {
		if (e->key == Key::MouseRight)
			return;

		e->accepted = true;

		if (m_isPressed) {
			doDrag(e->pos.x, e->pos.y, e->flags, 1);
		} else {
			doMove(e->pos.x, e->pos.y);
		}
	}

	void Tool::setCursor() {
		if (m_view) m_view->resetCursor();
	}

	//--------------------------------------------------------------------------
	// class SelectTool
	//--------------------------------------------------------------------------

	SelectTool::SelectTool(Context* context) : Tool(context) {
		m_linePrim = nullptr;
		m_meshPrim = nullptr;
		m_selectionSeq = 0;
	}

	SelectTool::~SelectTool() {
		clearAsset();
	}

	void SelectTool::doBindView(View* view) {}

	void SelectTool::doPress(int x, int y, int flags, float pressure) {
		m_beginPos.set(x, y);
		m_curPos.set(x, y);

		m_oldlist = m_context->getSelection();

		if (flags & Input::Event::ControlModifier)
			m_isAdd = true;
		else
			m_isAdd = false;
	}

	void SelectTool::doDrag(int x, int y, int flags, float pressure) {
		Rect r;

		m_curPos.set(x, y);
		r.x = std::min(x, m_beginPos.x);
		r.y = std::min(y, m_beginPos.y);
		r.width = Math::abs(x - m_beginPos.x);
		r.height = Math::abs(y - m_beginPos.y);

		r.inflate(1, 1);

		RenderLine::setupScreenRect(m_linePrim, r, Rgba::Red);

#if 1
		int part = SelectPart::All - SelectPart::Terrain;

		AgentList l;
		Vector3 pos;
		if (!m_view->selectRegion(r, part, pos, l)) {
			m_context->selectNone(false);
		} else {
			m_context->setSelection(l,false);
		}
#endif
		//		m_view->doUpdate();
	}

	void SelectTool::doMove(int x, int y) {
	}

	void SelectTool::doRelease(int x, int y) {

		if (m_beginPos == m_curPos) {
			// point select
			pointSelect();

		} else {
			areaSelect();
		}


		// undo his
		const AgentList& curlist = m_context->getSelection();

		if (m_oldlist.empty() && curlist.empty())
			return;

		SelectHis* his = new SelectHis(m_context, m_oldlist, curlist);
		m_context->addHistory(his);
	}

	void SelectTool::doRender(const RenderCamera& camera) {
		if (m_linePrim && m_beginPos != m_curPos)
			g_renderSystem->addToOverlay(m_linePrim);
	}

	void SelectTool::clearAsset() {
		SafeDelete(m_linePrim);
		SafeDelete(m_meshPrim);
	}

	void SelectTool::setCursor() {
		if (!m_view) {
			return;
		}

		m_view->setCursor(CursorType::Arrow);
	}

	void SelectTool::pointSelect()
	{
		Rect r;

		r.x = m_beginPos.x;
		r.y = m_beginPos.y;
		r.width = 0;
		r.height = 0;

		r.inflate(1, 1);

		clearAsset();

		int part = SelectPart::All - SelectPart::Terrain;

		AgentList l;
		Vector3 pos;

		if (!m_view->selectRegion(r, part, pos, l, true) && !m_isAdd) {
			m_context->selectNone(false);
			return;
		}

		int order = m_selectionSeq % s2i(l.size());

		AgentList::const_iterator it = l.begin();
		for (int i = 0;  it != l.end(); ++it, ++i) {
			if (i == order) {
				AgentList selected;
				selected.push_back(*it);

				if (m_isAdd) {
					Agent* actor = *it;
					if (actor->isSelected()) {
						selected = m_context->getSelection();
						selected.remove(actor);
						m_context->setSelection(selected, false);

					} else {
						m_context->addSelection(selected, false);
					}

				} else {
					m_context->setSelection(*it,false);
				}

				break;
			}
		}

		m_selectionSeq++;
	}

	void SelectTool::areaSelect()
	{
		// area select
		Rect r;

		r.x = std::min(m_curPos.x, m_beginPos.x);
		r.y = std::min(m_curPos.y, m_beginPos.y);
		r.width = Math::abs(m_curPos.x - m_beginPos.x);
		r.height = Math::abs(m_curPos.y - m_beginPos.y);

		r.inflate(1, 1);

		clearAsset();

		int part = SelectPart::All - SelectPart::Terrain;

		AgentList l;
		Vector3 pos;

		if (!m_view->selectRegion(r, part, pos, l, false) && !m_isAdd) {
			m_context->selectNone(false);
			return;
		}

		if (m_isAdd)
			m_context->addSelection(l, false);
		else
			m_context->setSelection(l, false);
	}

	//--------------------------------------------------------------------------
	// class TransformTool
	//--------------------------------------------------------------------------

	TransformTool::TransformTool(Context* context) : SelectTool(context) {
		m_context->attachObserver(this);
		m_context->getState()->attachObserver(this);

		m_isSelectMode = true;
		m_gizmo = nullptr; // need subclass to init this
	}

	TransformTool::~TransformTool() {
		m_context->getState()->setTransformState(false, true, Vector3(0,0,0));
		m_context->getState()->detachObserver(this);
		m_context->detachObserver(this);
	}

	void TransformTool::doBindView(View* view) {
		updateMode();
	}

	void TransformTool::doPress(int x, int y, int flags, float pressure) {
		bool v = begin(true, x, y, flags);

		if (!v) {
			m_isSelectMode = true;
		}

		if (m_isSelectMode) {
			SelectTool::setCursor();
			SelectTool::doPress(x, y, flags, pressure);
			return;
		}
	}

	void TransformTool::doDrag(int x, int y, int flags, float pressure) {
		if (m_isSelectMode) {
			SelectTool::doDrag(x, y, flags, pressure);
			return;
		}

		transform(Vector3(x, y, 0), -1);
	}

	void TransformTool::doMove(int x, int y) {
		if (!m_view)
			return;

		if (m_isSelectMode) {
			SelectTool::doMove(x, y);
			return;
		}

		int id = m_gizmo->doSelect(m_view, x, y);
		if (id == -1) {
			SelectTool::setCursor();
		} else {
			m_isSelectMode = false;
			setCursor();
			m_gizmo->setHighlight(id);
		}
	}

	void TransformTool::doRelease(int x, int y) {
		if (m_isSelectMode) {
			SelectTool::doRelease(x, y);
			updateMode();
			return;
		}

		end();
	}

	void TransformTool::doRender(const RenderCamera& camera) {
		if (!m_view)
			return;

		if (m_isSelectMode) {
			SelectTool::doRender(camera);
		} else {
			Vector3 pos = m_gizmoCenter;

			const RenderCamera& camera = m_view->getCamera();
			Vector3 wpos = camera.worldToScreen(pos);
			Vector3 upos = pos + camera.getViewAxis()[2];
			upos = camera.worldToScreen(upos);
			float dist = (upos - wpos).getLength();
#if 1
			m_distScale = 1.0f / dist;
			//			dist = gEditorParams->gizmoSize / dist;
#endif
			m_gizmo->setup(camera, pos, m_gizmoAxis, m_distScale);

			m_gizmo->doRender();
		}
	}

	void TransformTool::doNotify(IObservable* subject, int arg) {
		if (m_isPressed)
			return;

		if (subject == m_context) {
			if (!(arg & Context::SelectionChanged) &&
				!(arg & Context::ActorTransformed))
				return;
		}

		if (subject == m_context->getState()) {
			if (arg != State::Transform)
				return;
		}

		updateMode();
	}

	void TransformTool::updateMode() {
		const AgentList& actorlist = m_context->getSelection();

		if (actorlist.empty()) {
			m_isSelectMode = true;
			return;
		}

		m_isSelectMode = false;

		switch (m_context->getState()->transformSpace) {
		default:
		case WorldSpace:
			m_gizmoAxis.setIdentity();
			break;
		case ViewSpace:
			m_gizmoAxis = m_view->getCamera().getViewAxis();
			break;
		case ObjectSpace:
			m_gizmoAxis = actorlist.getBackAxis();
			m_gizmoAxis.removeScale();
			break;
		}

		if (m_context->getState()->transformCenter == PivotCenter || m_context->getState()->transformSpace == ObjectSpace) {
			m_gizmoCenter = actorlist.getBackOrigin();
		} else if (m_context->getState()->transformCenter == SelectionCenter) {
			m_gizmoCenter = actorlist.getCenter();
		} else {
			m_gizmoCenter.set(0, 0, 0);
		}

		doUpdateMode();
	}

	bool TransformTool::begin(bool mouse, int x, int y, int flags) {
		m_isMouseInput = mouse;

		if (m_isMouseInput) {
			m_highlit = m_gizmo->doSelect(m_view, x, y);

			if (m_highlit == -1)
				return false;

			// if shift key hold, clone actors
			if (flags & Input::Event::ShiftModifier) {
				AgentList cloned = m_context->getSelection().clone();

				GroupHis* group = new GroupHis(m_context, "Transform Cloned");

				UndeleteHis* his = new UndeleteHis(m_context, "Transform Cloned", cloned);
				History* his2 = m_context->setSelectionHistoried(cloned);

				group->append(his);
				group->append(his2);

				m_context->addHistory(group);
			}
		}

		m_beginTrack.set(x, y);

		const AgentList& actorlist = m_context->getSelection();

		m_beginMatrix = AffineMat(m_gizmoAxis, m_gizmoCenter);
		actorlist.beginTransform();

		return doBegin(x, y);
	}

	bool TransformTool::begin()
	{
		return begin(false, 0, 0, 0);
	}

	void TransformTool::transform(const Vector3& v, int index)
	{
		Vector3 translated;

		if (!doTranslateVector(v, translated))
			return;

		m_context->getState()->setTransformState(false, false, translated);

		AffineMat result;
		if (!doTransform(translated, index, result))
			return;

		//		m_view->doUpdate();
	}

	void TransformTool::end() {
		const AgentList& actorlist = m_context->getSelection();
		Action* his = actorlist.endTransform();

		doEnd(0, 0, his);

		if (his) {
			m_context->addHistory(his);
		}

		updateMode();
	}

	void TransformTool::setCursor() {
		if (m_view) {
			m_view->setCursor(CursorType::SizeAll);
		}
	}

	//------------------------------------------------------------------------------
	// class MoveTool, move tool
	//------------------------------------------------------------------------------

	MoveTool::MoveTool(Context* context) : TransformTool(context)  {
		m_gizmo = new MoveGizmo;
	}

	MoveTool::~MoveTool() {
		SafeDelete(m_gizmo);
	}

	bool MoveTool::doBegin(int x, int y) {
		if (!m_isMouseInput) {
			return true;
		}

		// select plane
		selectClipPlane();

		/*if (m_highlit == MoveGizmo::XYZ)
		{
		mapTerrain *terrain = gEditorContext->getTerrain();

		if (terrain != NULL)
		{
		zMove = terrain->getHeightByPos(m_gizmoCenter);
		}
		}*/

		// set start clip pos
		return clipTest(x, y, m_beginClip);
	}

	void MoveTool::doDrag(int x, int y, int flags, float pressure)
	{
		m_mouseX = x;
		m_mouseY = y;

		TransformTool::doDrag(x, y, flags, pressure);
	}

	bool MoveTool::doTransform(const Vector3& dist, int index, AffineMat& result) {
#if 0
		bool v;
		Vector3 cliped, dist;

		v = clipTest(x, y, cliped);
		dist = cliped - m_beginClip;
		dist = m_gizmoAxis * dist;

		if (m_highlit == MoveGizmo::X) {
			dist.y = dist.z = 0;
		} else if (m_highlit == MoveGizmo::Y) {
			dist.x = dist.z = 0;
		} else if (m_highlit == MoveGizmo::Z) {
			dist.x = dist.y = 0;
		}

		if (gEditorState->isSnapToGrid) {
			dist.x = snap(dist.x, gEditorState->snapToGrid);
			dist.y = snap(dist.y, gEditorState->snapToGrid);
			dist.z = snap(dist.z, gEditorState->snapToGrid);
		}
#else
		//		Vector3 dist(x, y, z);
#endif

		const AgentList& actorlist = m_context->getSelection();

		if (!m_isMouseInput && !m_context->getState()->transformRel && m_context->getState()->transformSpace == WorldSpace) {
			actorlist.setOrigin(index, dist[index]);

			result.setIdentity();

			return true;
		}

		if (m_context->getState()->transformSpace != ObjectSpace) {
			AffineMat mat1, mat2, mat3;
			mat1.setInverse(m_beginMatrix.axis, m_beginMatrix.origin);
			mat2.initTranslate(dist.x, dist.y, dist.z);
			mat3 = m_beginMatrix;

			result = mat3 * mat2 * mat1;

			actorlist.doTransform(result, false);
		} else {
			if (m_highlit == MoveGizmo::XYZ)
			{
				//float z = gEditorContext->getTerrain()->getHeightByPos(Vector3(dist.x, dist.y, dist.z));
				MapTerrain *terrain = m_context->getTerrain();

				if (terrain == NULL || m_view == NULL)
				{
					return false;
				}

				/*float z = terrain->getHeightByPos(m_gizmoCenter) - zMove;
				zMove = terrain->getHeightByPos(m_gizmoCenter);

				result.initTranslate(dist.x, dist.y, z);
				actorlist.doTransform(result, true);*/

				Vector3 from;

				if (!m_view->selectRegion(Rect(m_mouseX, m_mouseY, 1, 1), SelectPart::Terrain, from)) {
					return false;
				}

				AgentList::const_iterator it = actorlist.begin();
				for (; it != actorlist.end(); ++it) 
				{
					(*it)->setOrigin(from);
				}
			}
			else
			{
				result.initTranslate(dist.x, dist.y, dist.z);	
				actorlist.doTransform(result, true);
			}

		}

		// apply to gizmo
		AffineMat temp;

		if (m_context->getState()->transformSpace == ObjectSpace) {
			temp = m_beginMatrix * result;
		} else {
			temp = result * m_beginMatrix;
		}

		m_gizmoCenter = temp.origin;
		m_gizmoAxis = temp.axis;

		return true;
	}

	void MoveTool::doEnd(int x, int y, Action* his) {
		his->setMessage("Move Object");
	}

	void MoveTool::selectClipPlane() {
		const Vector3& forward = m_view->getCamera().getViewAxis()[0];

		AX_ASSERT(m_highlit != -1);

		switch (m_highlit) {
		case MoveGizmo::X:
			{
				float dotxy = Math::abs(m_gizmoAxis[2] | forward);
				float dotxz = Math::abs(m_gizmoAxis[1] | forward);
				if (dotxy > dotxz) {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[2]);
				} else {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[1]);
				}
			}
			break;
		case MoveGizmo::Y:
			{
				float dotxy = Math::abs(m_gizmoAxis[2] | forward);
				float dotyz = Math::abs(m_gizmoAxis[0] | forward);
				if (dotxy > dotyz) {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[2]);
				} else {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[0]);
				}
			}
			break;
		case MoveGizmo::Z:
			{
				float dotxz = Math::abs(m_gizmoAxis[1] | forward);
				float dotyz = Math::abs(m_gizmoAxis[0] | forward);
				if (dotxz > dotyz) {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[1]);
				} else {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[0]);
				}
			}
			break;
		case MoveGizmo::XY:
			m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[2]);
			break;
		case MoveGizmo::YZ:
			m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[0]);
			break;
		case MoveGizmo::XZ:
			m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[1]);
			break;
		case MoveGizmo::XYZ:
			m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[2]);
			//gEditorContext->getTerrain()->getHeightByPos(m_gizmoCenter);
			break;
		default: break;
		}
	}

	bool MoveTool::clipTest(int x, int y, Vector3& result) {
		Vector3 start = m_view->getCamera().getOrigin();
		Vector3 end(x, y, 1);

		end = m_view->getCamera().screenToWorld(end);
		Vector3 dir = (end - start).getNormalized();

		float scale;
		bool v = m_clipPlane.rayIntersection(start, dir, scale);

		result = start + dir * scale;

		return v;
	}

	bool MoveTool::doTranslateVector(const Vector3& vec, Vector3& result)
	{
		bool v = true;

		if (m_isMouseInput) {
			Vector3 cliped;

			v = clipTest(vec.x, vec.y, cliped);
			result = cliped - m_beginClip;
			result = m_gizmoAxis * result;

			if (m_highlit == MoveGizmo::X) {
				result.y = result.z = 0;
			} else if (m_highlit == MoveGizmo::Y) {
				result.x = result.z = 0;
			} else if (m_highlit == MoveGizmo::Z) {
				result.x = result.y = 0;
			}
		} else {
			result = vec;
		}

		if (m_context->getState()->isSnapToGrid) {
			result.x = Internal::snap(result.x, m_context->getState()->snapToGrid);
			result.y = Internal::snap(result.y, m_context->getState()->snapToGrid);
			result.z = Internal::snap(result.z, m_context->getState()->snapToGrid);
		}

		return v;
	}

	void MoveTool::doUpdateMode()
	{
		bool clear = false;
		Vector3 state(0,0,0);

		if (m_context->getState()->transformSpace == WorldSpace && !m_context->getState()->transformRel) {
			const AgentList& actorlist = m_context->getSelection();

			if (actorlist.containsOne()) {
				state = actorlist.getBackOrigin();
			} else {
				clear = true;
			}
		}
#if 0
		gEditorContext->setTransformState(false, state);
		gEditorState->transformState.editable = true;
		gEditorState->transformState.clear = clear;
		gEditorState->transformState.parameter = state;
		gEditorState->notify(State::TransformToApp);
#else
		m_context->getState()->setTransformState(true, clear, state);
#endif
	}

	void MoveTool::setCursor() {
		if (m_view) {
			m_view->setCursor(CursorType::SizeAll);
		}
	}

	//--------------------------------------------------------------------------
	// class RotateTool, rotate tool
	//--------------------------------------------------------------------------

	RotateTool::RotateTool(Context* context) : TransformTool(context) {
		m_gizmo = new RotateGizmo();
	}

	RotateTool::~RotateTool() {
		SafeDelete(m_gizmo);
	}

	bool RotateTool::doBegin(int x, int y) {
		if (!m_isMouseInput) {
			return true;
		}
		selectClipPlane();

		m_angle = 0;
		return clipTest(x, y, m_beginClip);
	}

	bool RotateTool::doTransform(const Vector3& rotate, int index, AffineMat& result ) {
		const AgentList& actorlist = m_context->getSelection();

		if (!m_isMouseInput && !m_context->getState()->transformRel && m_context->getState()->transformSpace == WorldSpace) {
			actorlist.setRotate(index, rotate[index]);

			result.setIdentity();

			return true;
		}

		if (m_context->getState()->transformSpace != ObjectSpace) {
			AffineMat mat1, mat2, mat3;

			mat1.setInverse(m_beginMatrix.axis, m_beginMatrix.origin);
			mat2.initRotation(rotate.x, rotate.y, rotate.z);
			mat3 = m_beginMatrix;

			result = mat3 * mat2 * mat1;

			actorlist.doTransform(result, false);
		} else {
			result.initRotation(rotate.x, rotate.y, rotate.z);

			actorlist.doTransform(result, true);
		}

		if (m_context->getState()->transformSpace != ObjectSpace) {
			return true;
		}

		// apply to gizmo
		AffineMat gizmoMat;
		if (m_isMouseInput && m_context->getState()->snapToAngle) {
			Vector3 r = getRotate(m_angle);
			gizmoMat.initRotation(r.x, r.y, r.z);
		} else {
			gizmoMat = result;
		}

		gizmoMat = m_beginMatrix * gizmoMat;
		m_gizmoCenter = gizmoMat.origin;
		m_gizmoAxis = gizmoMat.axis;

		return true;
	}

	void RotateTool::doEnd(int x, int y, Action* his) {
		((RotateGizmo*)m_gizmo)->disableCrank();
		his->setMessage("Rotate Object");
	}

	Vector3 RotateTool::getRotate(float angle) const {
		angle = Math::r2d(angle);

		Vector3 v;
		float scale = 0;

		switch (m_highlit) {
		case RotateGizmo::X:
			return Vector3(angle, 0, 0);
		case RotateGizmo::Y:
			return Vector3(0, angle, 0);
		case RotateGizmo::Z:
			return Vector3(0, 0, angle);
		case RotateGizmo::Screen:
			return Vector3(angle, 0, 0);
			/*v = m_view->getCamera().getViewAxis()[2];
			scale = angle / v.getLength();
			v = v * scale;
			return v;*/
		default: break;
		}
		return Vector3(angle, 0, 0);
	}

	void RotateTool::selectClipPlane() {
		const Matrix3& viewAxis = m_view->getCamera().getViewAxis();

		AX_ASSERT(m_highlit != -1);

		switch (m_highlit) {
		case RotateGizmo::X:
			m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[0]);
			m_clipLeft = m_gizmoAxis[1];
			m_clipUp = m_gizmoAxis[2];
			break;
		case RotateGizmo::Y:
			m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[1]);
			m_clipLeft = m_gizmoAxis[2];
			m_clipUp = m_gizmoAxis[0];
			break;
		case RotateGizmo::Z:
			m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[2]);
			m_clipLeft = m_gizmoAxis[0];
			m_clipUp = m_gizmoAxis[1];
			break;
		case RotateGizmo::Screen:
			m_clipPlane = Plane(m_gizmoCenter, viewAxis[0]);
			m_clipLeft = viewAxis[1];
			m_clipUp = viewAxis[2];
			break;
		default: break;
		}
	}

	bool RotateTool::clipTest(int x, int y, float& result) {
		Vector3 start = m_view->getCamera().getOrigin();
		Vector3 end(x, y, 1);

		end = m_view->getCamera().screenToWorld(end);
		Vector3 dir = (end - start).getNormalized();

		float scale;
		bool v = m_clipPlane.rayIntersection(start, dir, scale);

		Vector3 intersected = start + dir * scale;
		dir = intersected - m_gizmoCenter;
		if (dir.normalize() < 1e-5)
			return false;

		float dotleft = dir | m_clipLeft;
		float dotup = dir | m_clipUp;

		result = acosf(dotleft);
		if (dotup < 0) {
			result = AX_PI * 2.0f - result;
		}

		return v;
	}

	bool RotateTool::doTranslateVector(const Vector3& vec, Vector3& result) {
		bool v = true;
		if (m_isMouseInput) {
			float cliped;

			v = clipTest(vec.x, vec.y, cliped);

			if (!v) return false;

			float curangle = cliped - m_beginClip;
			const float pi2 = AX_PI * 2.0f;
			if (abs(curangle - m_angle) > AX_PI) {
				// fix round back
				if (m_angle > 0)
					curangle += pi2;
				else
					curangle -= pi2;

				if (curangle > pi2) {
					curangle -= pi2;
				} else if (curangle < -pi2) {
					curangle += pi2;
				}

				cliped = curangle + m_beginClip;
			}

			m_angle = curangle;

			if (m_context->getState()->transformSpace != ObjectSpace) {
				((RotateGizmo*)m_gizmo)->setCrank(m_beginClip, cliped);
			} else {
				((RotateGizmo*)m_gizmo)->setCrank(m_beginClip, m_beginClip-m_angle);
			}

			const AgentList& actorlist = m_context->getSelection();

			result = getRotate(m_angle);

		} else {
			result = vec;
		}

		if (m_context->getState()->isSnapToAngle) {
			result = Internal::snap(result, m_context->getState()->snapToAngle);
		}

		return v;
	}

	void RotateTool::doUpdateMode() {
		bool clear = false;
		Vector3 state(0,0,0);

		if (m_context->getState()->transformSpace == WorldSpace && !m_context->getState()->transformRel) {
			const AgentList& actorlist = m_context->getSelection();

			if (actorlist.containsOne()) {
				Matrix3 axis = actorlist.getBackAxis();
				axis.removeShear();

				Axon::Rotate rotate;
				rotate.fromAxis(axis);
				state = rotate;

			} else {
				clear = true;
			}
		}

		m_context->getState()->setTransformState(true, clear, state);
	}

	void RotateTool::setCursor() {
		if (m_view) {
			m_view->setCursor(CursorType::ViewRotate);
		}
	}

	//--------------------------------------------------------------------------
	// class ScaleTool, scale tool
	//--------------------------------------------------------------------------

	ScaleTool::ScaleTool(Context* context) : TransformTool(context) {
		m_gizmo = new ScaleGizmo();
	}

	ScaleTool::~ScaleTool() {
		SafeDelete(m_gizmo);
	}

	bool ScaleTool::doBegin(int x, int y) {
		if (!m_isMouseInput) {
			return true;
		}

		// select plane
		selectClipPlane();

		// set start clip pos
		return clipTest(x, y, m_beginClip);
	}

	bool ScaleTool::doTransform(const Vector3& dist, int index, AffineMat& result) {
		AgentList actorlist = m_context->getSelection();

		if (m_context->getState()->transformSpace != ObjectSpace) {
			AffineMat mat1, mat2, mat3;
			mat1.setInverse(m_beginMatrix.axis, m_beginMatrix.origin);
			mat2.initScale(dist.x, dist.y, dist.z);
			mat3 = m_beginMatrix;

			result = mat3 * mat2 * mat1;

			actorlist.doTransform(result, false);
		} else {
			result.initScale(dist.x, dist.y, dist.z);

			actorlist.doTransform(result, true);
		}

		// apply to gizmo
		AffineMat temp;

		if (m_context->getState()->transformSpace == ObjectSpace) {
			temp = m_beginMatrix * result;
		} else {
			temp = result * m_beginMatrix;
		}

		m_gizmoCenter = temp.origin;
		m_gizmoAxis = temp.axis;

		return true;
	}

	void ScaleTool::doEnd(int x, int y, Action* his) {
		his->setMessage("Scale Object");
	}

	void ScaleTool::selectClipPlane() {
		const Vector3& forward = m_view->getCamera().getViewAxis()[0];

		AX_ASSERT(m_highlit != -1);

		switch (m_highlit) {
		case ScaleGizmo::X:
			{
				float dotxy = Math::abs(m_gizmoAxis[2] | forward);
				float dotxz = Math::abs(m_gizmoAxis[1] | forward);
				if (dotxy > dotxz) {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[2]);
				} else {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[1]);
				}
			}
			break;
		case ScaleGizmo::Y:
			{
				float dotxy = Math::abs(m_gizmoAxis[2] | forward);
				float dotyz = Math::abs(m_gizmoAxis[0] | forward);
				if (dotxy > dotyz) {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[2]);
				} else {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[0]);
				}
			}
			break;
		case ScaleGizmo::Z:
			{
				float dotxz = Math::abs(m_gizmoAxis[1] | forward);
				float dotyz = Math::abs(m_gizmoAxis[0] | forward);
				if (dotxz > dotyz) {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[1]);
				} else {
					m_clipPlane = Plane(m_gizmoCenter, m_gizmoAxis[0]);
				}
			}
			break;
			/*case ScaleGizmo::XYZ:
			m_clipPlane = Plane(m_gizmoCenter, forward);
			break;*/
		default: break;
		}
	}

	bool ScaleTool::clipTest(int x, int y, Vector3& result) {
		Vector3 start = m_view->getCamera().getOrigin();
		Vector3 end(x, y, 1);

		end = m_view->getCamera().screenToWorld(end);
		Vector3 dir = (end - start).getNormalized();

		float scale;
		bool v = m_clipPlane.rayIntersection(start, dir, scale);

		result = start + dir * scale;

		return v;
	}

	bool ScaleTool::doTranslateVector(const Vector3& vec, Vector3& dist) {
		bool v;

		if (m_isMouseInput) {
			Vector3 cliped;

			v = clipTest(vec.x, vec.y, cliped);

			if (!v)
				return false;

			dist = cliped - m_beginClip;
			dist = m_beginMatrix.axis * dist;

			Vector3 olddist0 = dist;

			float gizmosize = m_context->getState()->gizmoSize;
			dist /= m_distScale;
			dist /= m_context->getState()->gizmoSize;

			Vector3 olddist1 = dist;

			Printf("Scale: %f", dist.x);

			if (_isnan(dist.x)) {
				Errorf("NaN");
			}
#if 0
			if (m_highlit == ScaleGizmo::X) {
				dist.y = dist.z = 0;
			} else if (m_highlit == ScaleGizmo::Y) {
				dist.x = dist.z = 0;
			} else if (m_highlit == ScaleGizmo::Z) {
				dist.x = dist.y = 0;
			}
#else
			if (m_highlit == ScaleGizmo::X) {
				dist.y = dist.z = dist.x;
			} else if (m_highlit == ScaleGizmo::Y) {
				dist.x = dist.z = dist.y;
			} else if (m_highlit == ScaleGizmo::Z) {
				dist.x = dist.y = dist.z;
			}
			// ÆÁ±Îµô´íÇÐËõ·Å -- timlly add
			else
			{
				return false;
			}
#endif

			dist.x = pow(2, dist.x);
			dist.y = pow(2, dist.y);
			dist.z = pow(2, dist.z);
		} else {
			dist = vec;
			if (dist.x == 0.0f) dist.x = 1;
			if (dist.y == 0.0f) dist.y = 1;
			if (dist.z == 0.0f) dist.z = 1;

			v = true;
		}

		return v;
	}

	void ScaleTool::doUpdateMode()
	{

	}

	void ScaleTool::setCursor() {
		if (m_view) {
			m_view->setCursor(CursorType::ViewZoom);
		}
	}

AX_END_NAMESPACE

