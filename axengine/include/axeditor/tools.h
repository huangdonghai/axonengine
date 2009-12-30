/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_TOOLS_H
#define AX_EDITOR_TOOLS_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// class Tool
	//--------------------------------------------------------------------------

	class AX_API Tool : public IInputHandler {
	public:
		typedef RenderCamera RenderCamera;
		// editor tool
		enum Type {
			None,

			Select, Move, Rotate, Scale,

			UserDefined = 128,

			MaxType = 256
		};

		Tool(Context* context);
		virtual ~Tool();

		void setView(View* view);
		View* getView();

		virtual void doBindView(View* view) = 0;
		virtual void doPress(int x, int y, int flags, float pressure) = 0;
		virtual void doDrag(int x, int y, int flags, float pressure) = 0;
		virtual void doMove(int x, int y) = 0;
		virtual void doRelease(int x, int y) = 0;
		virtual void doRender(const RenderCamera& camera) = 0;
		virtual void setCursor();

		virtual void handleEvent(InputEvent* e);
	protected:
		// implement IInputHandler
		virtual void onMouseDown(InputEvent* e);
		virtual void onMouseUp(InputEvent* e);
		virtual void onMouseMove(InputEvent* e);

	protected:
		Context* m_context;
		bool m_isPressed;
		View* m_view;
	};

	inline void Tool::setView(View* view) {
		if (m_view != view) {
			m_view = view;
			doBindView(m_view);
			setCursor();
		}
	}

	inline View* Tool::getView() {
		return m_view;
	}

	class ToolFactory {
	public:
		virtual Tool* create(Context* context) = 0;
	};

	template< class T >
	class ToolFactory_ : public ToolFactory {
		virtual Tool* create(Context* context) {
			return new T(context);
		}
	};

	//--------------------------------------------------------------------------
	// class SelectTool
	//--------------------------------------------------------------------------

	class SelectTool : public Tool {
	public:
		SelectTool(Context* context);
		virtual ~SelectTool();

		virtual void doBindView(View* view);
		virtual void doPress(int x, int y, int flags, float pressure);
		virtual void doDrag(int x, int y, int flags, float pressure);
		virtual void doMove(int x, int y);
		virtual void doRelease(int x, int y);
		virtual void doRender(const RenderCamera& camera);

		virtual void setCursor();

	protected:
		void clearAsset();
		void pointSelect();
		void areaSelect();

	private:
		LinePrim* m_linePrim;
		MeshPrim* m_meshPrim;
		Point m_beginPos;
		Point m_curPos;
		int m_selectionSeq;
		AgentList m_oldlist;
		bool m_isAdd;
	};

	//--------------------------------------------------------------------------
	// class TransformTool
	//--------------------------------------------------------------------------

	class TransformTool : public SelectTool, public IObserver {
	public:
		enum Space {
			WorldSpace, ViewSpace, ObjectSpace
		};

		enum Center {
			PivotCenter, SelectionCenter, TransformCenter
		};

		TransformTool(Context* context);
		virtual ~TransformTool();

		// Tool
		virtual void doBindView(View* view);
		virtual void doPress(int x, int y, int flags, float pressure);
		virtual void doDrag(int x, int y, int flags, float pressure);
		virtual void doMove(int x, int y);
		virtual void doRelease(int x, int y);
		virtual void doRender(const RenderCamera& camera);
		virtual void setCursor();

		// IObserver
		virtual void doNotify(IObservable* subjest, int arg);

		// immediately transform for use input from UI
		virtual bool begin();
		virtual void transform(const Vector3& v, int index);
		virtual void end();

	protected:
		bool begin(bool mouseInput, int x, int y, int flags);

		// pure virtual need be implemented in subclass
		virtual bool doBegin(int x, int y) = 0;
		virtual bool doTranslateVector(const Vector3& v, Vector3& result) = 0;
		virtual bool doTransform(const Vector3& v, int index, AffineMat& result) = 0;
		virtual void doEnd(int x, int y, Action* his) = 0;
		virtual void doUpdateMode() = 0;

	protected:
		void updateMode();

	protected:
		TransformGizmo* m_gizmo;
		bool m_isSelectMode;
		Vector3 m_gizmoCenter;
		Matrix3 m_gizmoAxis;
		AffineMat m_beginMatrix;
		int m_highlit;
		Vector2 m_beginTrack;
		float m_distScale;
		bool m_isMouseInput;

		// params
		Vector3 m_vecParam;
		int m_mouseFlags;
	};

	//--------------------------------------------------------------------------
	// class MoveTool, move tool
	//--------------------------------------------------------------------------

	class MoveTool : public TransformTool {
	public:
		MoveTool(Context* context);
		virtual ~MoveTool();

		virtual void setCursor();

	protected:
		// implement TransformTool
		virtual bool doBegin(int x, int y);
		virtual bool doTranslateVector(const Vector3& v, Vector3& result);
		virtual bool doTransform(const Vector3& v, int index, AffineMat& result);
		virtual void doEnd(int x, int y, Action* his);
		virtual void doUpdateMode();

		//virtual void doMove(int x, int y);
		virtual void doDrag(int x, int y, int flags, float pressure);

	protected:
		void selectClipPlane();
		bool clipTest(int x, int y, Vector3& result);

	protected:
		Plane m_clipPlane;
		Vector3 m_beginClip;
		int m_mouseX;
		int m_mouseY;
	};

	//--------------------------------------------------------------------------
	// class RotateTool, rotate tool
	//--------------------------------------------------------------------------

	class RotateTool : public TransformTool {
	public:
		RotateTool(Context* context);
		virtual ~RotateTool();

		virtual void setCursor();

	protected:
		// implement TransformTool
		virtual bool doBegin(int x, int y);
		virtual bool doTranslateVector(const Vector3& v, Vector3& result);
		virtual bool doTransform(const Vector3& v, int index, AffineMat& result);
		virtual void doEnd(int x, int y, Action* his);
		virtual void doUpdateMode();

	protected:
		Vector3 getRotate(float angle) const;
		void selectClipPlane();
		bool clipTest(int x, int y, float& result);

	protected:
		Plane m_clipPlane;
		Vector3 m_clipLeft;
		Vector3 m_clipUp;
		float m_beginClip;
		float m_angle;
	};

	//--------------------------------------------------------------------------
	// class ScaleTool, scale tool
	//--------------------------------------------------------------------------

	class ScaleTool : public TransformTool {
	public:
		typedef ScaleGizmo::SelectId SelectId;

		ScaleTool(Context* context);
		virtual ~ScaleTool();

		virtual void setCursor();

	protected:
		// implement TransformTool
		virtual bool doBegin(int x, int y);
		virtual bool doTranslateVector(const Vector3& v, Vector3& result);
		virtual bool doTransform(const Vector3& v, int index, AffineMat& result);
		virtual void doEnd(int x, int y, Action* his);
		virtual void doUpdateMode();

	protected:
		void selectClipPlane();
		bool clipTest(int x, int y, Vector3& result);

	private:
		Plane m_clipPlane;
		Vector3 m_beginClip;
	};


AX_END_NAMESPACE

#endif

