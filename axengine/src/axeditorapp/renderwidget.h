/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include "private.h"
#include "frame.h"


//------------------------------------------------------------------------------
// class RenderWidget
//------------------------------------------------------------------------------

class RenderWidget : public QWidget
{
	Q_OBJECT

public:
    RenderWidget(QWidget *parent);
    virtual ~RenderWidget();

	void setStopUpdate(bool stop);

	// need to implement
	virtual void doRender() = 0;

	// no paint engine
	QPaintEngine* paintEngine() const { return NULL; }

protected:
	// implement QT event handler
	virtual void paintEvent(QPaintEvent* pe);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void timerEvent(QTimerEvent *) { update(); }
	void drawAxis();
	void drawFrameNum();

protected:
	Point m_beginTrackPos;
	Render::Camera m_renderCamera;
	bool m_renderCameraAdded;
	bool m_isDrawAxis;
	Render::Target* m_renderTarget;
	Vector3 m_viewOrg;
	Angles m_viewAngles;
	bool m_updateStoped;
	int m_frameOldTime;
	uint_t m_frameNumCur;
	float m_fps;
	float m_mouseDelicacy;

	enum ViewControl { NONE, PAN, ZOOM, ROTATE };
	ViewControl m_viewControl;

	int m_timerUpdate;
};

//------------------------------------------------------------------------------
// class PreviewWidget
//------------------------------------------------------------------------------

class PreviewWidget : public RenderWidget {
public:
	Q_OBJECT

public:
	PreviewWidget(QWidget* parent);
	virtual ~PreviewWidget();

	// implement RenderWidget
	virtual void doRender();

	// public function
	RenderWorld* getRenderWorld() { return m_renderWorld; }
	void setFocusActor(RenderEntity* actor) { m_actor = actor; }

protected:
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void wheelEvent (QWheelEvent * event);

protected:
	enum ViewControl { NONE, ROTATEOBJECT, ZOOMVIEW, ROTATEVIEW };
	Render::World* m_renderWorld;
	Render::Light* m_globalLight;
	Render::Entity* m_actor;
	ViewControl m_viewControl;
};

#if 0
//------------------------------------------------------------------------------
// class uiRenderWidget
//------------------------------------------------------------------------------

class uiRenderWidget : public RenderWidget,public Editor::IFrame {
public:
	Q_OBJECT

public:
	uiRenderWidget(QWidget* parent);
	~uiRenderWidget();

	// implement RenderWidget
	virtual void doRender();
	virtual void setCursor(Editor::CursorType cursor_type);

	virtual Render::Target* getRenderTarget(){ return 0;}
	virtual Rect getRect(){return Rect();}	
	virtual void setCursorPos(const Point& pos){}
	virtual void resetCursor() { unsetCursor(); }
	virtual void setNeedUpdate();
	virtual void setAutoUpdate(bool update);
	Editor::UiContext m_uiContext;
	void setEditor(QWidget* editor){m_parent = editor;}
protected:
	// implement QT event handler
	virtual void paintEvent(QPaintEvent* pe);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	void translateMouseEvent(QMouseEvent* e, Event* xe);
	QWidget*        m_parent;

};

class ParticleEditor;

// timlly add

struct EmitterLine
{
	EmitterLine()
	{
		line = NULL;
		isUse = false;
	}

	~EmitterLine()
	{
        SAFEDELETE(line);
	}

	Render::Line* line;
	bool          isUse;
};

struct ParticleAxis
{
	ParticleAxis()
	{
		static int count = 0;
		isHold          = false;

		isSelected      = false;
        m_isRenderEmitterLine = false;

		plane = NULL;

		isUse = false;
		isRender  = true;

		particle = NULL;

		id  = count ++;
	}

	~ParticleAxis()
	{
		SAFEDELETE(plane);
	}

	Render::Mesh* plane;
	Render::PointPrim* point;

	PARTICLESYSTEM::ParticleObject*  particle;

	int           id;

	bool          isHold;
	bool          isSelected;
	bool          m_isRenderEmitterLine;
	bool          isUse;
    bool          isRender;

	QPoint        holdPos;
};

struct EmitterAngleHelper 
{
	EmitterAngleHelper()
	{
		angleMinLine = NULL;
		angleMaxLine = NULL;

		circle = NULL;
		fan = NULL;
		fanReversed = NULL;
	}

	~EmitterAngleHelper()
	{
		SAFEDELETE(angleMinLine);
		SAFEDELETE(angleMaxLine);
		SAFEDELETE(circle);
		SAFEDELETE(fan);
	}

	Render::Line* angleMinLine;
	Render::Line* angleMaxLine;
	Render::Line* circle;
	Render::Mesh* fan;
	Render::Mesh* fanReversed;	// 用于反面绘制扇形.

	int minLineID, maxLineID;
};


class ParticlePreviewWidget : public PreviewWidget
{
public:
	Q_OBJECT

public:
	friend ParticleEditor;
	typedef std::vector<PARTICLESYSTEM::ParticleEffect*>	ParticleEffectList;
	typedef std::vector<Render::Model*>						ModelList;

	ParticlePreviewWidget(QWidget* parent);
	virtual ~ParticlePreviewWidget();

	virtual void doRender();

	void setParticleEffectList(ParticleEffectList* parEffectList);
	void addParticleEffect(PARTICLESYSTEM::ParticleEffect* parEffect);

	void setParticlePlayer(PARTICLEEDITOR::ParticlePlayer* particlePlayer);
	//void setTimeSlider(QSlider* timeSlider);
	void setKeyFrameSlider(KeyFrameSlider* keyframeSlider);
	void setParticleEditor(ParticleEditor* particleEditor);

	void clearParticleEffectList(bool destroyParticleEffects = false);

	void refresh();

	void updateParticleEffects(float elapsedTime);

	void           setRenderEmitterLineOfSelectedParticle(bool state);

	void           setRenderSelectedRect(bool state){m_isRenderSelectRect = state;}

	void           setRenderAxis(bool state){m_isRenderAxis = state;}

	bool           isRenderAxis(){return m_isRenderAxis;}

	void           setBackColor(const Rgba& color){m_backColor = color;}

	void           showEnv(bool show);

	void           setMouseControlSpeed(float scale)
	{
		m_mouseDelicacy = scale * 4.0f;
		m_tool->setMoveSpeed(m_mouseDelicacy);
	}

	void           setShowHelpText(bool state){m_isShowHelpText = state;}

    ParticleAxis*  getSelectedParticleEffect();

	void           setSelectedParticle(PARTICLESYSTEM::ParticleObject* particle);

	void           setPickedByParticleSystem(bool state);

	void setShowEmitAngleHelper(bool showHelper){m_showEmitAngleHelper = showHelper;}
	bool isShowEmitAngleHelper(){return m_showEmitAngleHelper;}

	void setShowHelperEmitter(PARTICLESYSTEM::ParticleEmitter* emitter) {m_showHelperEmitter = emitter;}
	PARTICLESYSTEM::ParticleEmitter* getShowHelperEmitter() {return m_showHelperEmitter;}

	void            initTexQuad(const String& materialName,Vector2 startTexUV,float uvOffset);
    void            initParticleModel(const String& modelFile);
	void            setShowTexture(bool show){m_isShowTexture = show;};
	bool            isShowTexture(){return m_isShowTexture;}
	void            setShowParticleModel(bool show){m_isShowParticleModel = show;}
	bool            isShowParticleModel(){return m_isShowParticleModel;}
protected:
	virtual void    focusOutEvent(QFocusEvent *);
	virtual void    focusInEvent(QFocusEvent *);

	virtual void    keyPressEvent(QKeyEvent *e);
	virtual void    keyReleaseEvent(QKeyEvent *e);
	virtual void mouseMoveEvent(QMouseEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseReleaseEvent(QMouseEvent * e);

	void doDrawEmitter(PARTICLESYSTEM::ParticleControler::ParticleEmitterList* emitterList);
	void drawEmitter(PARTICLESYSTEM::ParticleObject* particle);

	EmitterLine* getFreeEllipseLine();

	void drawEllipse(Vector3& center,float width,float height,float depth);

	EmitterLine* getFreeBoxLine();

	void drawBox(Vector3& center,float width,float height,float depth);

	void            setupMeshLine(Render::Line*& line,const Vector3& pos,Render::Model*& model,const Vector3& scale,const Vector3& rotate,const Vector3& translate);
	void            drawMeshLine(const String& modelFile,const Vector3& pos,const Vector3& scale,const Vector3& rotate,const Vector3& translate);
	EmitterLine*    getFreeMeshLine();

	void resetLineData();

	void drawParticleAxis(PARTICLESYSTEM::ParticleObject* particle);

	void            drawStaticAxis();

	ParticleAxis* getFreeParticleAxis();

	void setupParticleAxis(ParticleAxis*& axis,const Vector3& pos,float length,Rgba color = Rgba(0,0,255,255));

	void updateSelectParticleEffect(ParticleAxis* axis);

	bool hasParticleEffectHolded();

	size_t          stasticsNumActiveParticle();

	void            showHelpText();
	bool            isShowHelpText(){return m_isShowHelpText;}

	void            initCenterQuadLine();
	void            drawCenterQuadLine();

	void            drawTexQuadOrParticleModel();

protected:

	ParticleEffectList* m_parEffectList;
	ModelList m_modelList;
	PARTICLEEDITOR::ParticlePlayer* m_particlePlayer;	// 播放器信息,用于控制显示

	//QSlider* m_timeSlider;
	KeyFrameSlider* m_keyFrameSlider;
	ParticleEditor* m_particleEditor;

	Editor::ParticleTool*               m_tool;
	Render::Mesh*                       m_texQuad;
	Render::Material*                   m_material;
	Render::Model*                      m_particleModel;
	String                              m_materialName;

	//float m_timeAdjustFactor;
	float m_lastTime;
	bool m_renderWorldAdded;

	Angles              m_angleBeforLocked;
	Vector3             m_posBeforLocked;

	Render::Line*       m_centerQuadLine[202];
	int                 centerQuadLineNum;

	EmitterLine         m_ellipse[1000];
	EmitterLine         m_box[1000];
	EmitterLine         m_mesh[1000];

	ParticleAxis        m_axis[1000];

	float               m_selectedRectWidth;
	float               m_selectedRectHeight;

	bool                m_isRenderSelectRect;
	bool                m_isRenderAxis;

	bool m_showEmitAngleHelper;
	PARTICLESYSTEM::ParticleEmitter* m_showHelperEmitter;
	// hCircleHelper(水平), vCircleHelper(竖直)
	EmitterAngleHelper m_emitterHelper[2];
	static float m_emitterHelperLength;

	bool                m_mouseLeftButtonHold;
	bool                m_isClickedParticleRect;
	bool                m_isLocked;

	Rgba                m_backColor;

	bool                m_pickedByParticleSys;

	bool                m_isShowHelpText;
	bool                m_isShowTexture;
	bool                m_isShowParticleModel;


private:
};
#endif

#endif // RENDERWIDGET_H
