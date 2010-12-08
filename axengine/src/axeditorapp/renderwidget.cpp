/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "renderwidget.h"

//------------------------------------------------------------------------------
// class RenderWidget
//------------------------------------------------------------------------------

RenderWidget::RenderWidget(QWidget *parent)
	: QWidget(parent, Qt::MSWindowsOwnDC)
	, m_viewOrg(5, 0, 1)
	, m_viewAngles(0, 180, 0)
	, m_mouseDelicacy(1.0f)
	, m_viewControl(NONE)
	, m_updateStoped(false)
	, m_isDrawAxis(true)
	, m_renderTarget(nullptr)
	, m_renderCameraAdded(false)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_PaintUnclipped);
//	setAttribute(Qt::WA_OpaquePaintEvent);
	setAutoFillBackground(true); // for compatibility
	m_timerUpdate = startTimer(30);
}

RenderWidget::~RenderWidget()
{
	this->killTimer(m_timerUpdate);
}

void RenderWidget::setStopUpdate(bool stop)
{
	m_updateStoped = stop;
}

void RenderWidget::paintEvent(QPaintEvent *pe)
{
	if (m_updateStoped)
		return;

	if (m_renderTarget == nullptr)
		m_renderTarget = new RenderTarget(Handle(winId()), "RenderWidget");

	QSize s = size();
	Rect rect(0, 0, s.width(), s.height());

	m_renderCamera.setTarget(m_renderTarget);
	m_renderCamera.setViewRect(rect);
	m_renderCamera.setOrigin(m_viewOrg);
	m_renderCamera.setViewAngles(m_viewAngles);
	m_renderCamera.setFov(90);
	m_renderCamera.setTime(OsUtil::seconds());
	m_renderCamera.setClearColor(Rgba::MdGrey);

	/*if (m_renderCameraAdded == false)
	{
		m_renderCameraAdded = true;*/

		g_renderSystem->beginFrame(m_renderTarget);
		g_renderSystem->beginScene(m_renderCamera);

		doRender();

		if (m_isDrawAxis)
		{
			float line_length = 1.0f;
			LinePrim *axis_line = LinePrim::createAxis(Primitive::HintFrame, line_length);
			g_renderSystem->addToScene(axis_line);
		}
		
		g_renderSystem->endScene();

		drawAxis();

		g_renderSystem->endFrame();
	
		/*m_renderCameraAdded = false;
	}*/
}

void RenderWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (m_viewControl == NONE)
		return;

	int dx = event->x() - m_beginTrackPos.x * m_mouseDelicacy;
	int dy = event->y() - m_beginTrackPos.y * m_mouseDelicacy;

	m_beginTrackPos.x = event->x();
	m_beginTrackPos.y = event->y();

	Matrix3 axis;
	axis.fromAngles(m_viewAngles);

	if (PAN == m_viewControl) {
		m_viewOrg += axis[1] * dx * 0.2f;
		m_viewOrg += axis[2] * dy * 0.2f;
	} else if (ZOOM == m_viewControl) {
		m_viewOrg += axis[0] * (dx + dy) * 0.2f;
	} else if (ROTATE == m_viewControl) {
		m_viewAngles.yaw -= dx * 0.4f;
		m_viewAngles.pitch += dy * 0.4f;
		m_viewAngles.pitch = Math::clamp(m_viewAngles.pitch, -90.f, 90.f);
	}

	update();
}

void RenderWidget::mousePressEvent(QMouseEvent * event)
{
	Qt::MouseButton button = event->button();

	if (button & Qt::LeftButton) {
		m_viewControl = PAN;
	} else if (button & Qt::MidButton) {
		m_viewControl = ZOOM;
	} else {
		m_viewControl = ROTATE;
	}

	m_beginTrackPos.x = event->x();
	m_beginTrackPos.y = event->y();
}

void RenderWidget::mouseReleaseEvent(QMouseEvent * event)
{
	m_viewControl = NONE;
}

void RenderWidget::drawAxis()
{
	RenderCamera camera;

	float line_length = 1.0f;
	float view_size = line_length * 2.0f + 1.0f;

	camera.setTarget(m_renderTarget);
	camera.setOrigin(Vector3(0,0,0));
	camera.setViewAxis(m_renderCamera.getViewAxis());
	camera.setOrtho(view_size, view_size, view_size);
	camera.setViewRect(Rect(0, -12, 60, 60));

	g_renderSystem->beginScene(camera);

#if 0
	Render::Line *axis_line = Render::Line::createAxis(Render::Primitive::OneFrame, line_length);
	line_length += 0.2f;
	Render::Text *text = Render::Text::createSimpleText(Render::Primitive::OneFrame, Vector3(line_length, 0, 0), Color4::Red, "x");
	gRenderSystem->addToScene(text);

	text = Render::Text::createSimpleText(Render::Primitive::OneFrame, Vector3(0, line_length, 0), Color4::Green, "y");
	gRenderSystem->addToScene(text);

	text = Render::Text::createSimpleText(Render::Primitive::OneFrame, Vector3(0, 0, line_length), Color4::Blue, "z");
	gRenderSystem->addToScene(text);

	gRenderSystem->addToScene(axis_line);
#endif
	g_renderSystem->endScene();

//	drawFrameNum();
}

void RenderWidget::drawFrameNum()
{
	RenderCamera rv;

	rv.setTarget(m_renderTarget);
	rv.setOverlay(Rect(0,0,size().width(),size().height()));

	Rect rect;
	std::string text;

	double now = OsUtil::seconds();
	float frame_time = now - m_frameOldTime;

	if (frame_time > 0) {
		m_fps = 1.0f / frame_time;
		m_frameOldTime = now;
	}

	m_frameNumCur++;
#if 0
	Sprintf(text, "FT:%d FPS:%3.1f 中文", frame_time, m_fps);
#else
	text = l2u("中华人民共和国");
#endif
	rect.x = 0;
	rect.y = rv.getViewRect().height - 24;
	rect.width = 120;
	rect.height = g_consoleFont->getHeight();
	TextPrim *prim = TextPrim::createText(Primitive::HintFrame, rect, g_consoleFont.get(), text, Rgba::Green);

	g_renderSystem->beginScene(rv);
	g_renderSystem->addToScene(prim);
	g_renderSystem->endScene();
}

//------------------------------------------------------------------------------
// class PreviewWidget
//------------------------------------------------------------------------------

PreviewWidget::PreviewWidget(QWidget *parent)
	: RenderWidget(parent)
	, m_viewControl(NONE)
	, m_actor(NULL)
{
	m_renderWorld = new RenderWorld;
	m_renderWorld->initialize();

//	m_globalLight = new renderLight(renderLight::kGlobal, Vector3(1,1,1), Rgb::White);
//	m_renderWorld->addActor(m_globalLight);


//	mModel = gRenderSystem->CreateReModel(L"Models/test");
//	mRenderWorld->AddRenderable(mModel);
}

PreviewWidget::~PreviewWidget()
{
	delete m_renderWorld;
}

void PreviewWidget::doRender()
{
	g_renderSystem->addToScene(m_renderWorld);
}

void PreviewWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (m_viewControl == NONE)
		return;

	int dx = event->x() - m_beginTrackPos.x;
	int dy = event->y() - m_beginTrackPos.y;

	m_beginTrackPos.x = event->x();
	m_beginTrackPos.y = event->y();

	Matrix3 axis;
	axis.fromAngles(m_viewAngles);

	if (ROTATEOBJECT == m_viewControl) {
		if (m_actor != NULL) {
#if 1
			Angles angles = m_actor->getAxis().toAngles();
			angles.yaw -= dx * 0.4f * m_mouseDelicacy;
			angles.pitch += dy * 0.4f * m_mouseDelicacy;
			angles.pitch = Math::clamp(m_viewAngles.pitch, -90.f, 90.f);
			m_actor->setAxis(angles);
#else
			const Matrix &mat = m_actor->getMatrix();
			Matrix other;
			other.setOrigin(0, 0, 0);

#endif
		}
	} else if (ZOOMVIEW == m_viewControl) {
		m_viewOrg += axis[0] * (dx + dy) * 0.2f * m_mouseDelicacy;
	} else if (ROTATEVIEW == m_viewControl) {
		m_viewOrg += axis[1] * dx * 0.02f * m_mouseDelicacy;
		m_viewOrg += axis[2] * dy * 0.02f * m_mouseDelicacy;
	}

	update();
}

void PreviewWidget::mousePressEvent(QMouseEvent * event)
{
	Qt::MouseButton button = event->button();

	if (button & Qt::LeftButton) {
		m_viewControl = ROTATEOBJECT;
	} else if (button & Qt::MidButton) {
		m_viewControl = ZOOMVIEW;
	} else {
		m_viewControl = ROTATEVIEW;
	}

	m_beginTrackPos.x = event->x();
	m_beginTrackPos.y = event->y();
}

void PreviewWidget::wheelEvent(QWheelEvent * event) {
	Matrix3 axis;
	axis.fromAngles(m_viewAngles);
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	m_viewOrg += axis[0] * numSteps * 0.2f * m_mouseDelicacy;
	update();
}


void PreviewWidget::mouseReleaseEvent(QMouseEvent * event)
{
	m_viewControl = NONE;
}


#if 0
//------------------------------------------------------------------------------
// class uiRenderWidget
//------------------------------------------------------------------------------


uiRenderWidget::uiRenderWidget(QWidget *parent)
	: RenderWidget(parent){
		m_uiContext.bindFrame(this);
}

uiRenderWidget::~uiRenderWidget() {
	SafeDelete(m_renderTarget);
}

void uiRenderWidget::paintEvent(QPaintEvent *pe) {
	if (m_updateStoped)
		return;

	if (m_renderTarget == nullptr)
		m_renderTarget = gRenderSystem->createWindowTarget(winId(), "RenderWidget");


	QSize s = size();
	Rect rect(0, 0, s.width(), s.height());

	m_renderCamera.setTarget(m_renderTarget);
	m_renderCamera.setViewRect(rect);
	m_renderCamera.setOrigin(m_viewOrg);
	m_renderCamera.setViewAngles(m_viewAngles);
	m_renderCamera.setFov(90);
	m_renderCamera.setTime(Milliseconds());
	m_renderCamera.setClearColor(Rgba(0,94,94,255));
	m_renderCamera.setOverlay(rect);
	gRenderSystem->beginFrame(m_renderTarget);
	gRenderSystem->beginScene(m_renderCamera);
	
	doRender();

	gRenderSystem->endScene();
	gRenderSystem->endFrame();
}

void uiRenderWidget::doRender() {

	gUiSystem->doRender();
	m_uiContext.doRender();
	
	
}

void uiRenderWidget::setAutoUpdate(bool update){
	UiEditor *editor = dynamic_cast<UiEditor*>(m_parent);
	editor->setEditorCtrl(m_uiContext.getCurrentWidget());
}

void uiRenderWidget::setNeedUpdate(){
	update();
	UiEditor *editor = dynamic_cast<UiEditor*>(m_parent);
	editor->refreshCtrlPos();
	editor->refreshCtrlExt();
	
}

void uiRenderWidget::mouseMoveEvent(QMouseEvent *event){
	Event xe;

	TypeZero(&xe);

	xe.type = Event::MouseMove;

	translateMouseEvent(event, &xe);
	m_uiContext.handleEvent(&xe);
}

void uiRenderWidget::mousePressEvent(QMouseEvent *event){
	Event xe;

	TypeZero(&xe);

	xe.type = Event::MouseDown;

	translateMouseEvent(event, &xe);
	m_uiContext.handleEvent(&xe);
}

void uiRenderWidget::mouseReleaseEvent(QMouseEvent *event){
	Event xe;

	TypeZero(&xe);

	xe.type = Event::MouseUp;

	translateMouseEvent(event, &xe);
	m_uiContext.handleEvent(&xe);
}

void uiRenderWidget::translateMouseEvent(QMouseEvent *e, Event *xe) {
	Qt::MouseButtons btns = e->buttons();
	Qt::MouseButton btn = e->button();

	if (e->type() == QEvent::MouseMove) {
		if (btns & Qt::LeftButton)
			xe->key = Key::MouseLeft;
		else if ( btns & Qt::RightButton)
			xe->key = Key::MouseRight;
		else if (btns & Qt::MidButton)
			xe->key = Key::MouseMiddle;
	} else {
		if (btn == Qt::LeftButton) {
			xe->key = Key::MouseLeft;
		} else if (btn == Qt::RightButton) {
			xe->key = Key::MouseRight;
		} else if (btn == Qt::MidButton) {
			xe->key = Key::MouseMiddle;
		}
	}

	xe->pos.x = e->x();
	xe->pos.y = e->y();

	Qt::KeyboardModifiers mod = e->modifiers();
	if (mod & Qt::ShiftModifier)
		xe->flags |= Event::ShiftModifier;
	if (mod & Qt::AltModifier)
		xe->flags |= Event::AltModifier;
	if (mod & Qt::ControlModifier)
		xe->flags |= Event::ControlModifier;

	xe->time = Milliseconds();
}

void uiRenderWidget::setCursor(CursorType cursor_type) {
	using namespace Editor;

	switch (cursor_type) {
	case CursorType::Default:
		unsetCursor();
		break;
	case CursorType::Arrow:
		QWidget::setCursor(Qt::ArrowCursor);
		break;
	case CursorType::UpArrow:		
		QWidget::setCursor(Qt::UpArrowCursor);
		break;
	case CursorType::Cross:		
		QWidget::setCursor(Qt::CrossCursor);
		break;
	case CursorType::Wait:		
		QWidget::setCursor(Qt::WaitCursor);
		break;
	case CursorType::IBeam:		
		QWidget::setCursor(Qt::IBeamCursor);
		break;
	case CursorType::SizeVer:		
		QWidget::setCursor(Qt::SizeVerCursor);
		break;
	case CursorType::SizeHor:		
		QWidget::setCursor(Qt::SizeHorCursor);
		break;
	case CursorType::SizeFDiag:	
		QWidget::setCursor(Qt::SizeFDiagCursor);
		break;
	case CursorType::SizeBDiag:	
		QWidget::setCursor(Qt::SizeBDiagCursor);
		break;
	case CursorType::SizeAll:		
		QWidget::setCursor(Qt::SizeAllCursor);
		break;
	case CursorType::Blank:		
		QWidget::setCursor(Qt::BlankCursor);
		break;
	case CursorType::SplitV:		
		QWidget::setCursor(Qt::SplitVCursor);
		break;
	case CursorType::SplitH:		
		QWidget::setCursor(Qt::SplitHCursor);
		break;
	case CursorType::PointingHand:
		QWidget::setCursor(Qt::PointingHandCursor);
		break;
	case CursorType::Forbidden:	
		QWidget::setCursor(Qt::ForbiddenCursor);
		break;
	case CursorType::WhatsThis:	
		QWidget::setCursor(Qt::WhatsThisCursor);
		break;
	case CursorType::Busy:		
		QWidget::setCursor(Qt::BusyCursor);
		break;

	case CursorType::ViewRotate:
		QWidget::setCursor(gGlobalData->cursorViewRotate);
		break;
	case CursorType::ViewZoom:
		QWidget::setCursor(gGlobalData->cursorViewZoom);
		break;
	case CursorType::ViewPan:
		QWidget::setCursor(gGlobalData->cursorViewPan);
		break;
	}
}

// ======================================================
// ParticlePreviewWidget -- timlly add
// ======================================================

float ParticlePreviewWidget::m_emitterHelperLength = 3;
Render::Line *selectedLine = NULL;
bool emitterHelperHandling = false;

ParticlePreviewWidget::ParticlePreviewWidget(QWidget *parent)
	: PreviewWidget(parent),
	m_parEffectList(NULL),
	m_renderWorldAdded(false),
	m_isRenderSelectRect(true),
	m_isRenderAxis(true),
	m_isLocked(false),
	m_mouseLeftButtonHold(false),
	m_pickedByParticleSys(false),
	m_isClickedParticleRect(false),
	m_isShowHelpText(true),
	m_keyFrameSlider(NULL),
	m_particlePlayer(NULL),
	m_selectedRectWidth(1.0f),
	m_selectedRectHeight(1.0f),
	m_showEmitAngleHelper(false),
	m_isShowTexture(true),
	m_isShowParticleModel(false),
	m_particleModel(NULL),
	m_showHelperEmitter(NULL)
{
	m_lastTime = Milliseconds();

	m_renderCamera.setClearColor(Rgba(0, 0, 0));

	m_tool = new ParticleTool();
	m_tool->doBindCamera(&m_renderCamera);

	//m_material = new Material("_frond");
	//m_texQuad->setMaterial(m_material);

	m_texQuad = NULL;
	m_material = NULL;

	m_isDrawAxis = false;

	m_renderWorld->getOutdoorEnv()->setHaveOcean(false);
	m_renderWorld->getOutdoorEnv()->setHaveFarSky(false);

	m_backColor = Rgba::MdGrey;

	m_emitterHelper[0].minLineID = 1539999;
	m_emitterHelper[0].maxLineID = m_emitterHelper[0].minLineID + 1;
	m_emitterHelper[1].minLineID = m_emitterHelper[0].minLineID + 2;
	m_emitterHelper[1].maxLineID = m_emitterHelper[0].minLineID + 3;

	TypeZeroArray(m_centerQuadLine);
	centerQuadLineNum = 22;
	initCenterQuadLine();
}

ParticlePreviewWidget::~ParticlePreviewWidget()
{
	if (m_modelList.size() > 0)
	{
		for (int i=0; i<m_modelList.size(); ++i)
		{
			m_renderWorld->removeActor(m_modelList[i]);
			m_modelList[i]->setInstance(NULL);

			SAFEDELETE(m_modelList[m_modelList.size()-1]);	// 记得删除
			m_modelList.pop_back();
		}
	}

	SafeDelete(m_tool);
	SafeDelete(m_texQuad);
	SafeDelete(m_particleModel);

	for (int i = 0;i < 202;++ i)
	{
		SAFEDELETE(m_centerQuadLine[i]);
	}
	//FreeAsset_(m_material);
}

void ParticlePreviewWidget::initParticleModel(const String &modelFile)
{
	if (m_particleModel)
	{
		SAFEDELETE(m_particleModel);
	}

	m_particleModel = new Render::Model(modelFile);
}

void ParticlePreviewWidget::initTexQuad(const String &materialName,Vector2 startTexUV,float uvOffset)
{
	if (!m_texQuad)
	{
		m_texQuad = new Render::Mesh(Render::Primitive::Static);
		m_texQuad->initialize(4,6);
	}

	if (m_material)
	{
		if (materialName == m_materialName)
		{}
		else
		{
		    m_material->release();
			m_material = Material::loadUnique(materialName);
		}
	}
	else
	{
		m_material = Material::loadUnique(materialName);
	}

	Vertex *vertexData = m_texQuad->lockVertexes();

	vertexData[0].st = startTexUV;
	vertexData[1].st = Vector2(startTexUV.x,startTexUV.y + uvOffset);
	vertexData[2].st = Vector2(startTexUV.x + uvOffset,startTexUV.y + uvOffset);
	vertexData[3].st = Vector2(startTexUV.x + uvOffset,startTexUV.y);

	m_texQuad->unlockVertexes();

	m_texQuad->setMaterial(m_material);
	m_materialName = materialName;
}

void ParticlePreviewWidget::drawTexQuadOrParticleModel()
{
	Rect rect = m_renderCamera.getViewRect();
	float scale = (float)rect.height / (float)rect.width;
	Matrix3 axis = m_renderCamera.getViewAxis();
	Vector3 pos  = m_renderCamera.getOrigin();
	pos += axis[0] * 15.0f;
	Vector3 widthVec   = axis[1];
	Vector3 heightVec  = axis[2];
	Vector3 drawPos = pos - widthVec * 13 + heightVec * 11 * scale;

	Vector3 wpos = m_renderCamera.worldToScreen(drawPos);
	Vector3 upos = drawPos + m_renderCamera.getViewAxis()[2];
	upos = m_renderCamera.worldToScreen(upos);
	float dist = (upos - wpos).getLength();

	float m_distScale = 1.0f / dist;

	float length = m_distScale * 100;
	float halfLength = length / 2.0f;

	if (m_isShowTexture && m_texQuad)
	{
		Vertex *vertexData = m_texQuad->lockVertexes();

		vertexData[0].xyz = drawPos - widthVec * halfLength + heightVec * halfLength;
		vertexData[0].rgba = Rgba(255,255,255,255);
		vertexData[1].xyz = drawPos + widthVec * halfLength + heightVec * halfLength;
		vertexData[1].rgba = Rgba(255,255,255,255);
		vertexData[2].xyz = drawPos + widthVec * halfLength - heightVec * halfLength;
		vertexData[2].rgba = Rgba(255,255,255,255);
		vertexData[3].xyz = drawPos - widthVec * halfLength - heightVec * halfLength;
		vertexData[3].rgba = Rgba(255,255,255,255);

		m_texQuad->unlockVertexes();

		int *indexData = m_texQuad->lockIndexes();

		int index[6] = {2,1,0,0,3,2};

		memcpy(indexData,index,6 * sizeof(int));

		m_texQuad->unlockIndexes();

		gRenderSystem->addToScene(m_texQuad);
	}

	if (m_isShowParticleModel && m_particleModel)
	{
		//m_particleModel->setOrigin(drawPos);
		m_particleModel->setOrigin(Vector3(0.0f,0.0f,0.0f));
		//float modellength = (m_particleModel->getBoundingBox().high - m_particleModel->getBoundingBox().low).getLength();
		//scale = length / modellength;
		//m_particleModel->setAxis(Angles(0.0f,0.0f,0.0f),scale);
		//Render::PrimitiveSeq meshPrimitive = m_particleModel->getAllPrimitives();
		//Render::PrimitiveSeq::iterator it;

		//for (it = meshPrimitive.begin();it != meshPrimitive.end();++it)
		  //  gRenderSystem->addToScene((*it));
		gRenderSystem->addToScene(m_particleModel);
	}
}

void ParticlePreviewWidget::initCenterQuadLine()
{
	float diff = 2.0f;
	float value = 2.0f * (centerQuadLineNum / 2 - 1) / 2;
	Vector3 leftStartPos = Vector3(-value,-value,0.0f);
	Vector3 RightEndPos = Vector3(-value,value,0.0f);
	Vector3 topStartPos = Vector3(value,-value,0.0f);
	Vector3 bottomEndPos = Vector3(-value,-value,0.0f);
	Rgba color = Rgba(200,200,200,255);

	for (int i = 0;i < centerQuadLineNum;i += 2)
	{
		if (leftStartPos.x == 0.0f || topStartPos.y == 0.0f)
			color = Rgba(255,0,0,255);
		else
			color = Rgba(200,200,200,255);

		m_centerQuadLine[i] = Render::Line::createLine(Render::Primitive::Static,leftStartPos,RightEndPos,color);
		m_centerQuadLine[i + 1] = Render::Line::createLine(Render::Primitive::Static,topStartPos,bottomEndPos,color);

		leftStartPos.x += diff;
		RightEndPos.x += diff;
		topStartPos.y += diff;
		bottomEndPos.y += diff;
	}
}

void ParticlePreviewWidget::drawCenterQuadLine()
{
	for (int i = 0;i < centerQuadLineNum;++ i)
	{
        gRenderSystem->addToScene(m_centerQuadLine[i]);
	}
}

void ParticlePreviewWidget::updateParticleEffects(float elapsedTime)
{
	if (m_particlePlayer == NULL)
	{
		for (int i=0; i<m_parEffectList->size(); ++i)
		{
			(*m_parEffectList)[i]->update(elapsedTime);
		}

		return ;
	}

	// reset the player if current time reach the total time
	if (m_particlePlayer->getCurrenTime() >= m_particlePlayer->getTotalTime())
	{
		if (m_particlePlayer->isCycled() == true && m_particlePlayer->isPaused() == false)
		{
			m_particlePlayer->getOwner()->reInitParticleEffects();
			m_particlePlayer->setCurrenTime(0.0f);

			//m_timeSlider->setValue(* m_timeAdjustFactor);
			m_keyFrameSlider->setValue(m_particlePlayer->getCurrenTime());

			return ;
		}
		else
		{
			m_particlePlayer->setPaused(true);

			m_particleEditor->refreshButtons();
		}
	}

	// update current time of the player
	if (m_particlePlayer->isPaused() == false)
	{
		
		float playerTime = m_particlePlayer->getCurrenTime() + elapsedTime;

		if (playerTime > m_particlePlayer->getTotalTime())
		{
			m_particlePlayer->setCurrenTime(m_particlePlayer->getTotalTime());
		}
		else
		{
			m_particlePlayer->setCurrenTime(playerTime);
		}

		m_keyFrameSlider->setValue(m_particlePlayer->getCurrenTime());
		//m_timeSlider->setValue(m_particlePlayer->getCurrenTime() * m_timeAdjustFactor);
	}
	else
	{
		float keySliderTime = m_particlePlayer->getKeySliderValueTime();

		if (keySliderTime != -1.0f)
		    m_particlePlayer->setCurrenTime(keySliderTime);
	}
}

void ParticlePreviewWidget::setParticleEditor(ParticleEditor *particleEditor)
{
	AX_ASSERT(particleEditor != NULL);

	m_particleEditor = particleEditor;
}

float playerLastTime = 0;
void ParticlePreviewWidget::doRender()
{
	PreviewWidget::doRender();

	m_renderCamera.setClearColor(m_backColor);
	
	static bool initTime = false;
	static bool setPalyer = false;

	if (m_renderWorldAdded == true && m_parEffectList != NULL && m_parEffectList->size() > 0)
	{
		// update the bounding box of the moedels
		for (int i=0; i<m_modelList.size(); ++i)
		{
			m_renderWorld->addActor(m_modelList[i]);
		}

		float curTime = Milliseconds();;
		if (!initTime)
		{
			m_lastTime = Milliseconds();
			curTime = Milliseconds();
			initTime = true;
		}

		float elapsedTime = (curTime - m_lastTime) / 1000.0f;

		if (m_particlePlayer == NULL)	// 没有播放器(如编辑器右侧预览窗口)
		{
			updateParticleEffects(elapsedTime);
			m_lastTime = Milliseconds();
		}
		else
		{
			if (!setPalyer)
			{
                elapsedTime = 0.0f;
				setPalyer = true;
			}

			// 是否处于播放状态
			if (m_particlePlayer->isPaused() == false)	
			{
				updateParticleEffects(elapsedTime);
				playerLastTime = m_particlePlayer->getCurrenTime();
			}
			else
			{
				if (playerLastTime != m_particlePlayer->getKeySliderValueTime())
				{
					updateParticleEffects(elapsedTime);

					playerLastTime = m_particlePlayer->getKeySliderValueTime();
				}

			}

			m_lastTime = Milliseconds();

			ParticleEffectList *effectList = m_particlePlayer->getOwner()->getParticleEffectList();
			ParticleEffectList::iterator it;
			ParticleEffect::ParticleSystemList *systemList;
			ParticleEffect::ParticleSystemList::iterator it_sym;

			if (m_isRenderSelectRect)
			{
				for (it = effectList->begin();it != effectList->end();++ it)
				{
					if (!m_pickedByParticleSys)
					{
						drawParticleAxis((*it));
					}
					else
					{
						systemList = (*it)->getParticleSystemList();

						for (it_sym = systemList->begin();it_sym != systemList->end();++ it_sym)
							drawParticleAxis((*it_sym));
					}
				}
			}

			ParticleAxis *selectedParticle = getSelectedParticleEffect();

			if (selectedParticle)
			{
				if (selectedParticle->m_isRenderEmitterLine)
					drawEmitter(selectedParticle->particle);
			}

			if (m_particleEditor->getUI()->actionEdit_Emit_Angles->isChecked())
			{
				m_showEmitAngleHelper = true;
			}
			else
			{
				m_showEmitAngleHelper = false;
			}
			
			if (m_showEmitAngleHelper == true && m_showHelperEmitter != NULL)
			{
				Quaternion qua;
				Vector3 vec;
				float lineWidth = 5;

				Vector3 particlePos;

				// 调整绘制长度
				Vector3 wpos = m_renderCamera.worldToScreen(m_showHelperEmitter->getPosition());
				Vector3 upos =  m_showHelperEmitter->getPosition() + m_renderCamera.getViewAxis()[2];
				upos = m_renderCamera.worldToScreen(upos);
				float dist = (upos - wpos).getLength();

				float m_distScale = 1.0f / dist;

				m_emitterHelperLength  = 100.0f * m_distScale;

				/*if (m_emitterHelper[0].circle != NULL)
				{
					m_emitterHelper[0].circle->setLineWidth(lineWidth);
					m_emitterHelper[0].angleMinLine->setLineWidth(lineWidth);
					m_emitterHelper[0].angleMaxLine->setLineWidth(lineWidth);
					m_emitterHelper[1].circle->setLineWidth(lineWidth);
					m_emitterHelper[1].angleMinLine->setLineWidth(lineWidth);
					m_emitterHelper[1].angleMaxLine->setLineWidth(lineWidth);
				}*/
				
				// horizontal

				Rgba hLineColor(255, 0, 0);
				Rgba hSelecColor(255, 255, 255);
				Rgba hFanColor(255, 0, 0, 50);

				float hMinAngle(0), hMaxAngle(0);

				ParticleEmitterDlg *emitterDlg = NULL;
				IParticleObjectDlg *parDlg = m_particleEditor->getParticleDialogManager()->getActiveParticleDlg();

				if (parDlg != NULL && parDlg->getDlgType() == "ParticleEmitter")
				{
					emitterDlg = (ParticleEmitterDlg*) parDlg;
				}

				if (emitterDlg != NULL && emitterDlg->isKeyFrameEnabled() == false)
				{
					hMinAngle = m_showHelperEmitter->getMinHorizontalAngle();
					hMaxAngle = m_showHelperEmitter->getMaxHorizontalAngle();
				}
				else if (emitterDlg != NULL && emitterDlg->isKeyFrameEnabled() == true)
				{
					hMinAngle = emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_horizontalAngleMin;
					hMaxAngle = emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_horizontalAngleMax;
				}
				
				Line::setupCircle(m_emitterHelper[0].circle, m_showHelperEmitter->getPosition(), Vector3(0, m_emitterHelperLength, 0), Vector3(m_emitterHelperLength, 0, 0), hLineColor, 32);
				
				gRenderSystem->addToScene(m_emitterHelper[0].circle);

				qua.fromAxisAngle(Vector3(0, 0, 1), hMinAngle);
				vec = qua * Vector3(m_emitterHelperLength, 0, 0);

				if (m_emitterHelper[0].angleMinLine == selectedLine)
				{
					Line::setupLine(m_emitterHelper[0].angleMinLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, hSelecColor);
				}
				else
				{
					Line::setupLine(m_emitterHelper[0].angleMinLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, hLineColor);
				}
				
				gRenderSystem->addToScene(m_emitterHelper[0].angleMinLine);

				qua.fromAxisAngle(Vector3(0, 0, 1), hMaxAngle);
				vec = qua * Vector3(m_emitterHelperLength, 0, 0);

				if (m_emitterHelper[0].angleMaxLine == selectedLine)
				{
					Line::setupLine(m_emitterHelper[0].angleMaxLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, hSelecColor);
				}
				else
				{
					Line::setupLine(m_emitterHelper[0].angleMaxLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, hLineColor);
				}
				
				gRenderSystem->addToScene(m_emitterHelper[0].angleMaxLine);

				Render::Mesh::setupFan(m_emitterHelper[0].fan, m_showHelperEmitter->getPosition(), Vector3(m_emitterHelperLength, 0, 0), Vector3(0, m_emitterHelperLength, 0), hMaxAngle*3.14159/180.0f, hMinAngle*3.14159/180.0f, hFanColor, 32); 
				gRenderSystem->addToScene(m_emitterHelper[0].fan);

				Render::Mesh::setupFan(m_emitterHelper[0].fanReversed, m_showHelperEmitter->getPosition(), Vector3(m_emitterHelperLength, 0, 0), Vector3(0, m_emitterHelperLength, 0), hMinAngle*3.14159/180.0f, hMaxAngle*3.14159/180.0f, hFanColor, 32); 
				gRenderSystem->addToScene(m_emitterHelper[0].fanReversed);

				// vertical

				Rgba vLineColor(0, 255, 0);
				Rgba vSelecColor(hSelecColor);
				Rgba vFanColor(0, 255, 0, 50);

				float vMinAngle(0), vMaxAngle(0);

				if (emitterDlg != NULL && emitterDlg->isKeyFrameEnabled() == false)
				{
					vMinAngle = m_showHelperEmitter->getMinVerticalAngle();
					vMaxAngle = m_showHelperEmitter->getMaxVerticalAngle();
				}
				else if (emitterDlg != NULL && emitterDlg->isKeyFrameEnabled() == true)
				{
					vMinAngle = emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_verticalAngleMin;
					vMaxAngle = emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_verticalAngleMax;
				}
				
				float axAngle = (hMinAngle + hMaxAngle) / 2.0f;
				Quaternion vQua;
				vQua.fromAxisAngle(Vector3(0, 0, 1), axAngle);
				Vector3 vVec = vQua * Vector3(m_emitterHelperLength, 0, 0);
				
				Line::setupCircle(m_emitterHelper[1].circle, m_showHelperEmitter->getPosition(), vVec, Vector3(0, 0, m_emitterHelperLength), vLineColor, 32);

				gRenderSystem->addToScene(m_emitterHelper[1].circle);

				Vector3 verticalVec = vVec ^ Vector3(0, 0, m_emitterHelperLength);
				verticalVec.normalize();

				qua.fromAxisAngle(verticalVec, vMinAngle);
				vec = qua * vVec;

				if (m_emitterHelper[1].angleMinLine == selectedLine)
				{
					Line::setupLine(m_emitterHelper[1].angleMinLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, vSelecColor);
				}
				else
				{
					Line::setupLine(m_emitterHelper[1].angleMinLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, vLineColor);
				}
				
				gRenderSystem->addToScene(m_emitterHelper[1].angleMinLine);

				qua.fromAxisAngle(verticalVec, vMaxAngle);
				vec = qua * vVec;
				if (m_emitterHelper[1].angleMaxLine == selectedLine)
				{
					Line::setupLine(m_emitterHelper[1].angleMaxLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, vSelecColor);
				}
				else
				{
					Line::setupLine(m_emitterHelper[1].angleMaxLine, m_showHelperEmitter->getPosition(), m_showHelperEmitter->getPosition()+vec, vLineColor);
				}
				
				gRenderSystem->addToScene(m_emitterHelper[1].angleMaxLine);

				Render::Mesh::setupFan(m_emitterHelper[1].fan, m_showHelperEmitter->getPosition(), vVec, Vector3(0, 0, m_emitterHelperLength), vMinAngle*3.14159/180.0f, vMaxAngle*3.14159/180.0f, vFanColor, 32); 
				gRenderSystem->addToScene(m_emitterHelper[1].fan);

				Render::Mesh::setupFan(m_emitterHelper[1].fanReversed, m_showHelperEmitter->getPosition(),vVec,  Vector3(0, 0, m_emitterHelperLength), vMaxAngle*3.14159/180.0f, vMinAngle*3.14159/180.0f, vFanColor, 32); 
				gRenderSystem->addToScene(m_emitterHelper[1].fanReversed);
				
			}
			
			resetLineData();

			if (m_isRenderAxis)
				m_tool->doRender();
		}
	}

	if (m_isShowHelpText && m_particlePlayer != NULL)
		showHelpText();

	if (m_particlePlayer != NULL)
	{
		if (m_isShowTexture || m_isShowParticleModel)
			drawTexQuadOrParticleModel();
		//gRenderSystem->addToScene(m_texQuad);
		drawCenterQuadLine();
        drawStaticAxis();
	}
}

void ParticlePreviewWidget::drawStaticAxis()
{
	Rect rect = m_renderCamera.getViewRect();
	float scale = (float)rect.height / (float)rect.width;
	Matrix3 axis = m_renderCamera.getViewAxis();
	Vector3 pos  = m_renderCamera.getOrigin();
	pos += axis[0] * 15.0f;
	Vector3 widthVec   = -axis[1];
	Vector3 heightVec  = axis[2];
	Vector3 drawPos = pos - widthVec * 14.2 - heightVec * 14 * scale;

	Vector3 wpos = m_renderCamera.worldToScreen(drawPos);
	Vector3 upos = drawPos + m_renderCamera.getViewAxis()[2];
	upos = m_renderCamera.worldToScreen(upos);
	float dist = (upos - wpos).getLength();

	float m_distScale = 1.0f / dist;
    
	float length = m_distScale * 20;

	Matrix3 axisMat(-1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f);
	Render::Line *axisLine = Render::Line::createAxis(Render::Primitive::OneFrame,drawPos,axisMat,length);
	gRenderSystem->addToScene(axisLine);
}

void ParticlePreviewWidget::setParticleEffectList(ParticleEffectList *parEffectList)
{
	AX_ASSERT(parEffectList != NULL);

	m_parEffectList = parEffectList;

	refresh();
}

void ParticlePreviewWidget::refresh()
{
	if (m_parEffectList == NULL)
	{
		return ;
	}
	
	m_renderWorldAdded = false;

	// 清空之前加入RenderWorld的Model
	for (int i=0; i<m_modelList.size(); ++i)
	{
		m_renderWorld->removeActor(m_modelList[i]);
		ParticleModelInstance *parModelIns = (ParticleModelInstance*)m_modelList[i]->getInstance();
		parModelIns->setParticleEffect(NULL);
		//m_modelList[i]->setInstance(NULL);
	}

	// 调整m_parModelList的大小,以使它与m_parEffectList的大小一样
	if (m_modelList.size() < m_parEffectList->size())
	{
		Render::Model *parModel = NULL;
		ParticleModelInstance *parInstance = NULL;

		for (int i=0; i<m_parEffectList->size() - m_modelList.size(); ++i)
		{
			parModel = new Render::Model();
			parInstance = new ParticleModelInstance(NULL);
			parModel->setInstance(parInstance);

			m_modelList.push_back(parModel);
		}
	}
	else if (m_modelList.size() > m_parEffectList->size())
	{
		size_t numModel = m_modelList.size() - m_parEffectList->size();

		for (int i=0; i<numModel; ++i)
		{
			SAFEDELETE(m_modelList[m_modelList.size()-1]);	// 记得删除
			m_modelList.pop_back();
		}
	}

	// 将ParticleEffect加入Model,并将Model加入RenderWorld中
	for (int i=0; i<m_modelList.size(); ++i)
	{
		ParticleModelInstance *parModelIns = (ParticleModelInstance*) m_modelList[i]->getInstance();
		parModelIns->setParticleEffect((*m_parEffectList)[i]);

		m_renderWorld->addActor(m_modelList[i]);
	}
	
	m_renderWorldAdded = true;
}

void ParticlePreviewWidget::addParticleEffect(ParticleEffect *parEffect)
{
	m_renderWorldAdded = false;

	AX_ASSERT(parEffect != NULL);

	if (m_parEffectList == NULL)
	{
		m_parEffectList = new ParticleEffectList();
	}
	
	m_parEffectList->push_back(parEffect);

	refresh();
}

void ParticlePreviewWidget::clearParticleEffectList(bool destroyParticleEffects)
{
	m_renderWorldAdded = false;

	if (m_parEffectList == NULL)
	{
		return;
	}
	
	if (destroyParticleEffects == true)
	{
		for (int i=0; i<m_parEffectList->size(); ++i)
		{
			DestroyParticleModule((*m_parEffectList)[i]);
		}

		m_parEffectList->clear();

		refresh();
	}
	else
	{
        setStopUpdate(true);
	}
	//m_parEffectList->clear();
}

void ParticlePreviewWidget::setParticlePlayer(ParticlePlayer *particlePlayer)
{
	AX_ASSERT(particlePlayer != NULL);

	m_particlePlayer = particlePlayer;
}

void ParticlePreviewWidget::setKeyFrameSlider(KeyFrameSlider *keyFrameSlider)
{
	AX_ASSERT(keyFrameSlider != NULL);

	m_keyFrameSlider = keyFrameSlider;
}

bool ParticlePreviewWidget::hasParticleEffectHolded()
{
	for (int i = 0;i < 1000;i ++)
	{
		if (m_axis[i].isHold)
			return true;
	}

	return false;
}

/*bool ParticlePreviewWidget::clipTest(int x, int y, Plane *clickPlane, float &result) 
{
	Vector3 start = m_renderCamera.getOrigin();
	Vector3 end(x, y, 1);

	end = m_renderCamera.screenToWorld(end);
	Vector3 dir = (end - start).getNormalized();

	float scale;
	bool v = clickPlane.rayIntersection(start, dir, scale);

	Vector3 intersected = start + dir * scale;
	dir = intersected - m_showHelperEmitter->getPosition();

	if (dir.normalize() < 1e-5)
		return false;

	float dotleft = dir | m_clipLeft;
	float dotup = dir | m_clipUp;

	result = acosf(dotleft);
	if (dotup < 0) {
		result = AX_PI * 2.0f - result;
	}

	return v;
	

	return false;
}*/

void ParticlePreviewWidget::mouseMoveEvent(QMouseEvent * e)
{
	PreviewWidget::mouseMoveEvent(e);

	QPoint pos = e->pos();

	if (m_showEmitAngleHelper == true && m_showHelperEmitter != NULL && emitterHelperHandling == false)
	{
		int x = pos.x();
		int y = pos.y();
		int size = 2;

		Rect r(x-size, y-size, size*2, size*2);

		Render::RenderCamera cam = m_renderCamera.createSelectionCamera(r);

		gRenderSystem->beginSelect(cam);

		gRenderSystem->loadSelectId(m_emitterHelper[0].minLineID);
		gRenderSystem->testPrimitive(m_emitterHelper[0].angleMinLine);

		gRenderSystem->loadSelectId(m_emitterHelper[0].maxLineID);
		gRenderSystem->testPrimitive(m_emitterHelper[0].angleMaxLine);

		gRenderSystem->loadSelectId(m_emitterHelper[1].minLineID);
		gRenderSystem->testPrimitive(m_emitterHelper[1].angleMinLine);

		gRenderSystem->loadSelectId(m_emitterHelper[1].maxLineID);
		gRenderSystem->testPrimitive(m_emitterHelper[1].angleMaxLine);

		Render::SelectRecordSeq records = gRenderSystem->endSelect();

		if (records.empty())
		{
			selectedLine = NULL;
		}
		else
		{
			float minz = 1.0f;
			int nearest = -1;

			for (size_t i = 0; i < records.size(); i++) 
			{
				int id = records[i].name;

				if (records[i].minz < minz) {
					minz = records[i].minz;
					nearest = id;
				}
			}

			if (nearest == m_emitterHelper[0].minLineID)
			{
				selectedLine = m_emitterHelper[0].angleMinLine;
			}
			else if (nearest == m_emitterHelper[0].maxLineID)
			{
				selectedLine = m_emitterHelper[0].angleMaxLine;
			}
			else if (nearest == m_emitterHelper[1].minLineID)
			{
				selectedLine = m_emitterHelper[1].angleMinLine;
			}
			else if (nearest == m_emitterHelper[1].maxLineID)
			{
				selectedLine = m_emitterHelper[1].angleMaxLine;
			}
		}
	}
	else if (m_showEmitAngleHelper == true && m_showHelperEmitter != NULL && emitterHelperHandling == true && selectedLine != NULL)
	{
		Vector3 start = m_renderCamera.getOrigin();
		Vector3 end(pos.x(), pos.y(), 1);

		Plane clickPlane;
		Vector3 clipLeft, clipUp;

		if (selectedLine == m_emitterHelper[0].angleMaxLine || selectedLine == m_emitterHelper[0].angleMinLine)
		{
			clickPlane = Plane(m_showHelperEmitter->getPosition(), Vector3(0, 0, 1));

			clipLeft = Vector3(1, 0, 0);
			clipUp = Vector3(0, 1, 0);
		}
		else if (selectedLine == m_emitterHelper[1].angleMaxLine || selectedLine == m_emitterHelper[1].angleMinLine)
		{
			float axAngle = (m_showHelperEmitter->getMinHorizontalAngle() + m_showHelperEmitter->getMaxHorizontalAngle()) / 2.0f;

			Quaternion vQua;
			vQua.fromAxisAngle(Vector3(0, 0, 1), axAngle);
			Vector3 vVec = vQua * Vector3(1, 0, 0);

			Vector3 verticalVec = vVec ^ Vector3(0, 0, 1);
			verticalVec.normalize();

			clickPlane = Plane(m_showHelperEmitter->getPosition(), verticalVec);

			clipLeft = vVec;
			clipUp = Vector3(0, 0, 1);
		}

		end = m_renderCamera.screenToWorld(end);
		Vector3 dir = (end - start).getNormalized();

		float scale;
		bool v = clickPlane.rayIntersection(start, dir, scale);

		Vector3 intersected = start + dir * scale;
		dir = intersected - m_showHelperEmitter->getPosition();

		if (dir.normalize() >= 1e-5)
		{
			float dotleft = dir | clipLeft;
			float dotup = dir | clipUp;

			float result = acosf(dotleft);
			static float lastResult = result;
			
			if (dotup < 0) 
			{
				float realResult = AX_PI * 2 - result;
				if ((realResult - lastResult) > 0)
				{
				    result =  realResult;
					lastResult = result;
				}
				else
				{
					result = -result;
				}

				lastResult = realResult;
			}

			//lastResult = result;
			result = result * 180.0f / AX_PI;

			IParticleObjectDlg *parDlg = m_particleEditor->getParticleDialogManager()->getActiveParticleDlg();

			if (parDlg->getDlgType() == "ParticleEmitter")
			{
				ParticleEmitterDlg *emitterDlg = (ParticleEmitterDlg*) parDlg;

				if (emitterDlg->isKeyFrameEnabled() == false)
				{
					if (selectedLine == m_emitterHelper[0].angleMinLine)
					{
						m_showHelperEmitter->setMinHorizontalAngle(result);
					}
					else if (selectedLine == m_emitterHelper[0].angleMaxLine)
					{
						m_showHelperEmitter->setMaxHorizontalAngle(result);
					}
					else if (selectedLine == m_emitterHelper[1].angleMinLine)
					{
						m_showHelperEmitter->setMinVerticalAngle(result);
					}
					else if (selectedLine == m_emitterHelper[1].angleMaxLine)
					{
						m_showHelperEmitter->setMaxVerticalAngle(result);
					}
				}
				else
				{
					if (selectedLine == m_emitterHelper[0].angleMinLine)
					{
						emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_horizontalAngleMin = result;
					}
					else if (selectedLine == m_emitterHelper[0].angleMaxLine)
					{
						emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_horizontalAngleMax = result;
					}
					else if (selectedLine == m_emitterHelper[1].angleMinLine)
					{
						emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_verticalAngleMin = result;
					}
					else if (selectedLine == m_emitterHelper[1].angleMaxLine)
					{
						emitterDlg->getCurrentPointEmitterFrame()->m_baseData.m_verticalAngleMax = result;
					}
				}

				emitterDlg->refreshDlg();
			}

			return ;
		}
	}
	
	if (!m_tool->isSelectAxis())
	    m_tool->heightLightSelect(pos.x(), pos.y());

	if (!m_mouseLeftButtonHold)
		return;
	
	Vector2 holdPos = m_tool->getHoldPos();

    m_tool->doMove(pos.x(),pos.y());	

	ParticleObject *effect = m_tool->getSelectedParticleEffect();

	if (effect)
	{
		Vector3 pos;
		GET_PARTICLE_POS(effect,pos);

		if (effect->getTopChildrenType() == "ParticleEffect")
		    m_particleEditor->selectedParticleEffectPosChange(pos);
		else
			m_particleEditor->selectedParticleEffectPosChange(pos,false);
	}

	if (!m_isClickedParticleRect && !m_tool->isSelectAxis())
	{
		float xMove = (float)(pos.x() - holdPos.x);
		float yMove = (float)(pos.y() - holdPos.y);

		Matrix3 mat;
		Quaternion q;
		Vector3 pos = m_renderCamera.getOrigin();
		Matrix3 axis = m_renderCamera.getViewAxis();

		Vector3 rightVec = axis[1];

		float angle;

		if (fabs(xMove) > fabs(yMove))
		{
			angle = xMove * m_tool->getMoveSpeed() * 0.2f;
            q.fromAxisAngle(UP_VECTOR,-angle);
		}
		else
		{
			angle = yMove * m_tool->getMoveSpeed() * 0.2f;
			q.fromAxisAngle(axis[1],angle);
		}

		mat.fromQuaternion(q);

		Vector3 newPos = pos;
		if (m_isLocked)
		{
			ParticleAxis *axis = getSelectedParticleEffect();
			Vector3 parPos;

			GET_PARTICLE_POS(axis->particle,parPos);

			newPos = pos - parPos;

			m_viewOrg = q * newPos;

			m_viewOrg += parPos;
		}
		else
		    m_viewOrg = q * pos;

		axis = mat * axis;

		m_viewAngles = axis.toAngles();

	}
}

void ParticlePreviewWidget::mouseReleaseEvent(QMouseEvent * e)
{
	PreviewWidget::mouseReleaseEvent(e);

	for (int i = 0;i < 1000;i ++)
	{
		m_axis[i].isHold    = false;
		m_axis[i].isRender  = true;
	}

	m_mouseLeftButtonHold = false;

	emitterHelperHandling = false;

	m_tool->doPress(-1,-1,0,0);
}

void ParticlePreviewWidget::keyPressEvent(QKeyEvent *e)
{
	PreviewWidget::keyPressEvent(e);

	if (e->key() == Qt::Key_Z)
	{
		ParticleEffectList *effectList = m_particleEditor->getParticleContext()->getParticleEffectList();
		ParticleEffectList::iterator it;
		Vector3 posAll = Vector3(0.0f,0.0f,0.0f);

		for (it = effectList->begin();it != effectList->end();++ it)
		{
			posAll += (*it)->getCenterPos();
		} 

		float scale = (float)effectList->size();
		Vector3 allParCenterPos = Vector3(0.0f,0.0f,0.0f);

		if (scale)
			allParCenterPos = posAll / scale;

		m_viewOrg = Vector3(allParCenterPos.x + 15.0f,allParCenterPos.y,allParCenterPos.z + 1.0f);
		Matrix3 mat(-1.0f,0.0f,0.0f,0.0f,-1.0f,0.0f,0.0f,0.0f,1.0f);
		m_viewAngles = mat.toAngles();
	}

	if (e->key() == Qt::Key_Shift)
	{
		ParticleAxis *axis = getSelectedParticleEffect();

		if (!axis)
			return;

		Vector3 pos;

		GET_PARTICLE_POS(axis->particle,pos);

		m_angleBeforLocked = m_viewAngles;
		m_posBeforLocked   = m_viewOrg;

		m_viewOrg = Vector3(pos.x + 10.0f,pos.y,pos.z + 0.0f);
		Matrix3 mat(-1.0f,0.0f,0.0f,0.0f,-1.0f,0.0f,0.0f,0.0f,1.0f);
		m_viewAngles = mat.toAngles();
		m_isLocked = true;
	}
}

void ParticlePreviewWidget::keyReleaseEvent(QKeyEvent *e)
{
	PreviewWidget::keyReleaseEvent(e);

	if (m_isLocked)
	{
        m_isLocked = false;
		m_viewOrg = m_posBeforLocked;
		m_viewAngles = m_angleBeforLocked;
	}
}

void ParticlePreviewWidget::updateSelectParticleEffect(ParticleAxis *axis)
{
    for (int i = 0;i < 1000;i ++)
	{
		if ((&m_axis[i]) == axis)
		{
			ParticleAxis *curSelected = getSelectedParticleEffect();

			if (curSelected)
				m_axis[i].m_isRenderEmitterLine = curSelected->m_isRenderEmitterLine;
			m_axis[i].isSelected = true;
			continue;
		}

		m_axis[i].isSelected = false;
		//m_axis[i].m_isRenderEmitterLine = false;
	}
}

ParticleAxis *ParticlePreviewWidget::getSelectedParticleEffect()
{
	for (int i = 0;i < 1000;i ++)
	{
		if (m_axis[i].isSelected)
			return &m_axis[i];
	}

	return NULL;
}

void ParticlePreviewWidget::mousePressEvent(QMouseEvent * e)
{
	PreviewWidget::mousePressEvent(e);

	if (m_showHelperEmitter != NULL)
	{
		emitterHelperHandling = true;
	}
	 
	if (m_particlePlayer != NULL)
	{
		m_particleEditor->getPreviewWidget()->clearParticleEffectList();
	}
	else
	{
		setStopUpdate(false);

		m_renderWorldAdded = true;

		return;
	}
    
	QPoint pos  = e->pos();

	if (e->button() == Qt::LeftButton)
	{
		m_mouseLeftButtonHold = true;

		m_tool->doPress(pos.x(),pos.y(),0,0);
		m_tool->setHoldPos(Vector2(pos.x(),pos.y()));

		int x = pos.x();
		int y = pos.y();
		int size = 1;

		Rect r(x-size, y-size, size*2+1, size*2+1);

		Render::RenderCamera cam = m_renderCamera.createSelectionCamera(r);

		gRenderSystem->beginSelect(cam);

		for (int i = 0;i < 1000;i ++)
		{
			if (m_axis[i].plane)
			{
				gRenderSystem->loadSelectId(m_axis[i].id);
				gRenderSystem->testPrimitive(m_axis[i].plane);
			}
		}

		Render::SelectRecordSeq records = gRenderSystem->endSelect();

		if (records.empty())
		{
			m_isClickedParticleRect = false;	
			return;
		}

		float minz = 1.0f;
		int nearest = -1;
		for (size_t i = 0; i < records.size(); i++) {
			int id = records[i].name;

			if (records[i].minz < minz) {
				minz = records[i].minz;
				nearest = id;
			}
		}

		for (int i = 0;i < 1000;i ++)
		{
			if (m_axis[i].id == nearest)
			{
				m_axis[i].isHold = true;
				m_axis[i].isRender = false;
				m_axis[i].holdPos = pos;

				updateSelectParticleEffect(&m_axis[i]);
				m_tool->setSelectedParticleEffect(m_axis[i].particle);
				m_tool->setHoldPos(Vector2(pos.x(),pos.y()));

				m_particleEditor->setSelectedParticleEffect(m_axis[i].particle);
				m_particleEditor->getParticleGlobalSettingDlg()->setRenderSelectParticleEmitterLineCheck(m_axis[i].m_isRenderEmitterLine);

				Vector3 pos;
				GET_PARTICLE_POS(m_axis[i].particle,pos);

				if (m_axis[i].particle->getTopChildrenType() == "ParticleEffect")
				    m_particleEditor->selectedParticleEffectPosChange(pos);
				else
					m_particleEditor->selectedParticleEffectPosChange(pos,false);

				m_isRenderAxis = true;
				m_isClickedParticleRect = true;

				m_particleEditor->getParticleGlobalSettingDlg()->setRenderSelectParticleAxisCheck(m_isRenderAxis);

				return;
			}

			m_axis[i].isRender = true;
		}
     
	}
	
}

void ParticlePreviewWidget::setupMeshLine(Render::Line*& line,const Vector3 &pos,Render::Model*& model,const Vector3 &scale,const Vector3 &rotate,const Vector3 &translate)
{
	Render::Primitives primitives = model->getAllPrimitives();
	Render::Primitives::iterator it;
	int vertexNum = 0,indexNum  = 0;
	int startVertex = 0;

	for (it = primitives.begin();it != primitives.end();++ it)
	{
		Render::Mesh *mesh = static_cast<Render::Mesh*>((*it));
		vertexNum += mesh->getNumVertexes();
		indexNum += mesh->getNumIndexes();
	}

	if (line)
	{
		if (line->getNumIndexes() == indexNum && line->getNumVertexes())
		{

		}
		else
		{
			line->initialize(vertexNum,indexNum);
		}
	}
	else
	{
		line = new Render::Line(Render::Primitive::Dynamic);
		line->initialize(vertexNum,indexNum);
	}

	Vertex *meshVertexes;
	int meshVertexNum;
	int *meshIndexes;
	int meshIndexNum;
	AffineMat mat;
	mat.setIdentity();
	mat.translateSelf(translate.x,translate.y,translate.z);
	mat.scaleSelf(scale.x,scale.y,scale.z);
	mat.rotateSelf(rotate.x,rotate.y,rotate.z);
	DebugVertex *lineVertexes = line->lockVertexes();
	int vexIndex = 0;
	int idxIndex = 0;
	int *lineIndexes = line->lockIndexes();
	for (it = primitives.begin();it != primitives.end();++ it)
	{
		Render::Mesh *mesh = static_cast<Render::Mesh*>((*it));
	    meshVertexes = mesh->lockVertexes();
		meshIndexes = mesh->lockIndexes();
		meshVertexNum = mesh->getNumVertexes();
		meshIndexNum = mesh->getNumIndexes();

		for (int i = 0;i < meshVertexNum;++ i)
		{
			lineVertexes[vexIndex].xyz = mat.transformPoint(meshVertexes[i].xyz) + pos;
			lineVertexes[vexIndex].rgba = Rgba(255,0,0,255);
			vexIndex ++;
		}

		for (int i = 0;i < meshIndexNum;++ i)
		{
			lineIndexes[idxIndex] = meshIndexes[i] + startVertex;
			idxIndex ++;
		}

		startVertex = vexIndex;
		mesh->unlockVertexes();
		mesh->unlockIndexes();
	}
	line->unlockVertexes();
	line->unlockIndexes();
}

void ParticlePreviewWidget::drawMeshLine(const String &modelFile,const Vector3 &pos,const Vector3 &scale,const Vector3 &rotate,const Vector3 &translate)
{
    static String lastModelFile = modelFile;
	static Render::Model *model = NULL;

	if (lastModelFile != modelFile || model == NULL)
	{
		SAFEDELETE(model);
		model = new Render::Model(modelFile);
	}

	EmitterLine *line = getFreeMeshLine();

	setupMeshLine(line->line,pos,model,scale,rotate,translate);
	gRenderSystem->addToScene(line->line);
}

void ParticlePreviewWidget::doDrawEmitter(ParticleControler::ParticleEmitterList *emitterList)
{
	ParticleControler::ParticleEmitterList::iterator it_em;

	for (it_em = emitterList->begin();it_em != emitterList->end();++ it_em)
	{
		if ((*it_em)->getEmitterType() == "BoxEmitter")
		{
			BoxEmitter *emitter = static_cast<BoxEmitter*>(*it_em);
			drawBox(emitter->getCurrentEmitterData().m_position,emitter->getCurrentInnerWidth(),emitter->getCurrentInnerHeight(),emitter->getCurrentInnerDepth());
			drawBox(emitter->getCurrentEmitterData().m_position,emitter->getCurrentOuterWidth(),emitter->getCurrentOuterHeight(),emitter->getCurrentOuterDepth());
		}
		else if ((*it_em)->getEmitterType() == "SphereEmitter")
		{
			SphereEmitter *emitter = static_cast<SphereEmitter*>(*it_em);
			drawEllipse(emitter->getCurrentEmitterData().m_position,emitter->getCurrentInnerWidth(),emitter->getCurrentInnerHeight(),emitter->getCurrentInnerDepth());
			drawEllipse(emitter->getCurrentEmitterData().m_position,emitter->getCurrentOuterWidth(),emitter->getCurrentOuterHeight(),emitter->getCurrentOuterDepth());
		}
		else if ((*it_em)->getEmitterType() == "MeshEmitter")
		{
            MeshEmitter *emitter = static_cast<MeshEmitter*>(*it_em);
			drawMeshLine(emitter->getModelName(),emitter->getCenterPos(),emitter->getMeshScale(),emitter->getMeshRotate(),emitter->getMeshTranslate());
		}
	}
}

void ParticlePreviewWidget::drawEmitter(ParticleObject *particle)
{
	ParticleControler::ParticleEmitterList *emitterList;
	ParticleEffect::ParticleSystemList::iterator it;
	ParticleControler::ParticleEmitterList::iterator it_em;

	if (particle->getTopChildrenType() == "ParticleEffect")
	{
		ParticleEffect *effect = static_cast<ParticleEffect*>(particle);
		ParticleEffect::ParticleSystemList *particleList = effect->getParticleSystemList();
		for (it = particleList->begin();it != particleList->end();++ it)
		{
			emitterList = (*it)->getParticleEmitterList();
            doDrawEmitter(emitterList);
		}
	}
	else
	{
		ParticleSystem *system = static_cast<ParticleSystem*>(particle);

		doDrawEmitter(system->getParticleEmitterList());
	}
}

EmitterLine *ParticlePreviewWidget::getFreeMeshLine()
{
	for (int i = 0;i < 1000;++ i)
	{
		if (!m_mesh[i].isUse)
		{
			m_mesh[i].isUse = true;
			return &m_mesh[i];
		}
	}

	return NULL;
}

EmitterLine *ParticlePreviewWidget::getFreeEllipseLine()
{
    for (int i = 0;i < 1000;++ i)
	{
		if (!m_ellipse[i].isUse)
		{
			m_ellipse[i].isUse = true;
			return &m_ellipse[i];
		}
	}

	return NULL;
}

void ParticlePreviewWidget::drawEllipse(Vector3 &center,float width,float height,float depth)
{
	EmitterLine *verticalCircle = getFreeEllipseLine();
	EmitterLine *horizontalCircle = getFreeEllipseLine();

	Vector3 upAxis    = Vector3(0.0f,0.0f,height);
	Vector3 rightAxis = Vector3(0.0f,width,0.0f);
	Vector3 depthAxis = Vector3(depth,0.0f,0.0f);

	if (height != 0.0f && width != 0.0f)
	{
		Line::setupCircle(verticalCircle->line,center,upAxis,rightAxis,Rgba(255,0,0,255),64);
		gRenderSystem->addToScene(verticalCircle->line);
		verticalCircle->isUse = true;
	}

	if (width != 0.0f && depth != 0.0f)
	{
		Line::setupCircle(horizontalCircle->line,center,depthAxis,rightAxis,Rgba(0,255,0,255),64);
		gRenderSystem->addToScene(horizontalCircle->line);
		horizontalCircle->isUse = true;
	}
}

void ParticlePreviewWidget::setPickedByParticleSystem(bool state)
{
    m_pickedByParticleSys = state;

	m_isRenderAxis = false;
	m_particleEditor->getParticleGlobalSettingDlg()->setRenderSelectParticleAxisCheck(m_isRenderAxis);
}

EmitterLine *ParticlePreviewWidget::getFreeBoxLine()
{
	for (int i = 0;i < 1000;++ i)
	{
		if (!m_box[i].isUse)
		{
			return &m_box[i];
		}
	}

	return NULL;
}

void ParticlePreviewWidget::drawBox(Vector3 &center,float width,float height,float depth)
{
	EmitterLine *boxLine = getFreeBoxLine();

	Matrix3 axis;
	axis.setIdentity();
	BoundingBox box(Vector3(-depth,-width,-height),Vector3(depth,width,height));

	Line::setupBoundingBox(boxLine->line, center, axis, box, 1.0f);
	gRenderSystem->addToScene(boxLine->line);
	boxLine->isUse = true;
}

void ParticlePreviewWidget::resetLineData()
{
	for (int i = 0;i < 1000;++ i)
	{
		m_box[i].isUse = false;
		m_ellipse[i].isUse = false;
		m_axis[i].isUse = false;
		m_mesh[i].isUse = false;
	}

}

ParticleAxis *ParticlePreviewWidget::getFreeParticleAxis()
{
	for (int i = 0;i < 1000;++ i)
	{
		if (!m_axis[i].isUse)
		{
			return &m_axis[i];
		}
	}

	return NULL;
}

void ParticlePreviewWidget::setupParticleAxis(ParticleAxis*& axis,const Vector3 &pos,float length,Rgba color /* = Rgba */)
{
	if (!axis->plane)
	{
		axis->plane = new Render::Mesh(Render::Primitive::Dynamic);

		axis->plane->initialize(4,6);
	}

	float width  = 0.0f;
	float height = 0.0f;
	if (axis->particle)
	{
		Vector3 particlePos;
	    GET_PARTICLE_POS(axis->particle,particlePos);

		Vector3 wpos = m_renderCamera.worldToScreen(particlePos);
		Vector3 upos = particlePos + m_renderCamera.getViewAxis()[2];
		upos = m_renderCamera.worldToScreen(upos);
		float dist = (upos - wpos).getLength();

		float m_distScale = 1.0f / dist;

		width  = 15.0f * m_distScale;
		height = 15.0f * m_distScale;
	}

	Matrix3 viewAxis = m_renderCamera.getViewAxis();
	Vector3 widthVec  = -viewAxis[1];
	Vector3 heightVec = viewAxis[2];
	widthVec.normalize();
	heightVec.normalize();
	
	Vertex *vertexData = axis->plane->lockVertexes();

	vertexData[0].xyz = pos - (widthVec * (width / 2)) + (heightVec * (height / 2));// Vector3(pos.x,pos.y - width / 2,pos.z + height / 2);
	vertexData[0].rgba = color;
	vertexData[1].xyz = pos + widthVec * (width / 2) + heightVec * (height / 2);//Vector3(pos.x,pos.y + width / 2,pos.z + height / 2);
	vertexData[1].rgba = color;
	vertexData[2].xyz = pos + widthVec * (width / 2) - heightVec * (height / 2);//Vector3(pos.x,pos.y + width / 2,pos.z - height / 2);
	vertexData[2].rgba = color;
	vertexData[3].xyz = pos - widthVec * (width / 2) - heightVec * (height / 2);//Vector3(pos.x,pos.y - width / 2,pos.z - height / 2);
	vertexData[3].rgba = color;

	axis->plane->unlockVertexes();

	int *indexData = axis->plane->lockIndexes();

	int index[6] = {0,1,2,2,3,0};

	memcpy(indexData,index,6 * sizeof(int));

	axis->plane->unlockIndexes();

#if 0
	Vector3 start,end;

	Line::createScreenRect()
	start = Vector3(pos.x,pos.y - m_selectedRectWidth / 2,pos.z + m_selectedRectHeight / 2);
	end = Vector3(pos.x,pos.y + m_selectedRectWidth / 2,pos.z + m_selectedRectHeight / 2);
	Line::setupLine(axis->up,start,end,Rgba(0,255,0,255));

	start = Vector3(pos.x,pos.y - m_selectedRectWidth / 2,pos.z - m_selectedRectHeight / 2);
	end = Vector3(pos.x,pos.y + m_selectedRectWidth / 2,pos.z - m_selectedRectHeight / 2);
	Line::setupLine(axis->depth,start,end,Rgba(0,255,0,255));

	start = Vector3(pos.x,pos.y + m_selectedRectWidth / 2,pos.z + m_selectedRectHeight / 2);
	end = Vector3(pos.x,pos.y + m_selectedRectWidth / 2,pos.z - m_selectedRectHeight / 2);
	Line::setupLine(axis->right,start,end,Rgba(0,255,0,255));

	start = Vector3(pos.x,pos.y - m_selectedRectWidth / 2,pos.z + m_selectedRectHeight / 2);
	end = Vector3(pos.x,pos.y - m_selectedRectWidth / 2,pos.z - m_selectedRectHeight / 2);
	Line::setupLine(axis->left,start,end,Rgba(0,255,0,255));
#endif

}

void ParticlePreviewWidget::drawParticleAxis(PARTICLESYSTEM::ParticleObject *particle)
{
    ParticleAxis *axis = getFreeParticleAxis(); 
	Vector3 pos;
	GET_PARTICLE_POS(particle,pos);

	if (particle->getTopChildrenType() == "ParticleEffect")
	    setupParticleAxis(axis,pos,1.0f,Rgba(255,0,0,255));
	else
		setupParticleAxis(axis,pos,1.0f);

	if (axis->isRender)
	{
	    g_renderSystem->addToScene(axis->plane);
	}

	axis->isUse = true;
	axis->particle = particle;
}

void ParticlePreviewWidget::setRenderEmitterLineOfSelectedParticle(bool state)
{
    ParticleAxis *selectedParticle = getSelectedParticleEffect();

	if (selectedParticle)
	    selectedParticle->m_isRenderEmitterLine = state;
}

void ParticlePreviewWidget::setSelectedParticle(ParticleObject *particle)
{
	for (int i = 0;i < 1000;++ i)
	{
		if (m_axis[i].particle == particle)
		{
			m_axis[i].isHold = true;
			m_axis[i].isRender = true;

			updateSelectParticleEffect(&m_axis[i]);
			m_tool->setSelectedParticleEffect(m_axis[i].particle);
			m_tool->setStartMove(true);

			Vector3 pos;
			GET_PARTICLE_POS(m_axis[i].particle,pos);
			m_particleEditor->selectedParticleEffectPosChange(pos);
			m_particleEditor->getParticleGlobalSettingDlg()->setRenderSelectParticleEmitterLineCheck(m_axis[i].m_isRenderEmitterLine);

			m_isRenderAxis = true;
			m_particleEditor->getParticleGlobalSettingDlg()->setRenderSelectParticleAxisCheck(m_isRenderAxis);
		}
	}
}

size_t ParticlePreviewWidget::stasticsNumActiveParticle()
{
	ParticleEffectList::iterator it_par;
	ParticleEffect::ParticleSystemList *sysList;
	ParticleEffect::ParticleSystemList::iterator it_sys;
	size_t numActiveParticle = 0;
	size_t curSysActivePar = 0;

    for (it_par = m_parEffectList->begin();it_par != m_parEffectList->end();++ it_par)
	{
		sysList = (*it_par)->getParticleSystemList();
        for (it_sys = sysList->begin();it_sys != sysList->end();++ it_sys)
		{
			if (!(*it_sys)->isDisableStateObjectType("ParticleRenderer"))
			{
				numActiveParticle += (*it_sys)->getParticleData()->getNumActiveParticle();
			}
		}
	}

	return numActiveParticle;
}

void ParticlePreviewWidget::showHelpText()
{
	static float fLastTime = milliseconds();
	float  fCurrentTime = milliseconds();

	static size_t frameNum = 0;
	static float frameRate;

	if (frameNum % 10 == 0)
	{
		frameRate = frameNum / (fCurrentTime / 1000 - fLastTime / 1000);
		fLastTime = fCurrentTime;
		frameNum = 0;
	}

	Rect rect = m_renderCamera.getViewRect();
	float scale = (float)rect.height / (float)rect.width;
	QString qStr;
	qStr.sprintf("FPS: %0.1f",frameRate);
	std::string ufpsStr = q2u(qStr);
	Matrix3 axis = m_renderCamera.getViewAxis();
	Vector3 pos  = m_renderCamera.getOrigin();
	pos += axis[0] * 15.0f;
	Vector3 widthVec   = -axis[1];
	Vector3 heightVec  = axis[2];
	Vector3 drawPos = pos - widthVec * 13 + heightVec * 14 * scale;

	frameNum ++;
	Render::Text *tex = Render::Text::createSimpleText(Render::Primitive::OneFrame,drawPos,Rgba(255,0,0,255),ufpsStr);
	g_renderSystem->addToScene(tex);

	size_t numActivePar = stasticsNumActiveParticle();
	qStr.sprintf("particles: %d",numActivePar);
	std::string uNumParStr = q2u(qStr);
	drawPos -= heightVec;
	drawPos += widthVec * 0.7f;
	Render::Text *numParTex = Render::Text::createSimpleText(Render::Primitive::OneFrame,drawPos,Rgba(255,0,0,255),uNumParStr);
	g_renderSystem->addToScene(numParTex);

}

void ParticlePreviewWidget::showEnv(bool show)
{
    m_renderWorld->getOutdoorEnv()->setHaveOcean(show);
	m_renderWorld->getOutdoorEnv()->setHaveFarSky(show);
}

void ParticlePreviewWidget::focusInEvent(QFocusEvent *e)
{
}

void ParticlePreviewWidget::focusOutEvent(QFocusEvent *e)
{
}

// ================= timlly add end ======================>>>>>>> .r351
#endif