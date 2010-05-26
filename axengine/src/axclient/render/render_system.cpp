/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

//#define USE_SELF_SELETION 1

AX_BEGIN_NAMESPACE

static const char *glname = "axopengl.driver";
static const char *d3d9name = "axdirect3d9.driver";

AX_BEGIN_COMMAND_MAP(RenderSystem)
	AX_COMMAND_ENTRY("texlist",	texlist_f)
	AX_COMMAND_ENTRY("matlist",	matlist_f)
AX_END_COMMAND_MAP()

RenderSystem::RenderSystem()
	: m_initialized(false)
	, m_isSelectMode(false)
	, m_frameNum(0)
{
	g_cmdSystem->registerHandler(this);
}

RenderSystem::~RenderSystem()
{
	g_cmdSystem->removeHandler(this);
}

void RenderSystem::initialize()
{
	if (m_initialized)
		return;

	Printf("Initializing RenderSystem...\n");

	g_queues[0] = new RenderQueue;
	g_queues[0]->initialize();
	g_queues[1] = new RenderQueue;
	g_queues[1]->initialize();

	g_renderQueue = g_queues[m_frameNum];

#if 0
	gRenderDriver = (IDriver*)(gClassFactory->createInstanceByAlias(ClassName_RenderDriver));
#else
	String rd = r_driver.getString();
	String drivername = d3d9name;

	if (rd == "gl") {
		drivername = glname;
	} else if (rd == "d3d9") {
		drivername = d3d9name;
	}

	g_renderDriver = (IRenderDriver*)(g_classFactory->createInstance(drivername));
#endif
	g_renderDriver->initialize();

	getShaderQuality();

	set_objectName("gRenderSystem");

	m_isMTrendering = r_multiThread.getBool();

	m_selection = new Selection();

	Printf("Initialized RenderSystem\n");
}

void RenderSystem::finalize()
{
	g_renderDriver->finalize();
	SafeDelete(g_renderDriver);
}

ShaderQuality RenderSystem::getShaderQuality()
{
	if (r_shaderQuality.isModified()) {
		m_shaderQuality = std::min<int>(r_shaderQuality.getInteger(), g_renderDriver->getDriverInfo()->highestQualitySupport);
		r_shaderQuality.clearModifiedFlag();
	}
	return m_shaderQuality;
}


void RenderSystem::screenShot(const String &name, const Rect &rect) {
}

void RenderSystem::info()
{
	const RenderDriverInfo *info = g_renderDriver->getDriverInfo();

	Printf("------- Render Driver Caps -------\n");

	Printf("max 2D texture size      : %d\n", info->maxTextureSize);
	Printf("max 3D texture size      : %d\n", info->max3DTextureSize);
	Printf("max cubemap texture size : %d\n", info->maxCubeMapTextureSize);
	Printf("max texture units        : %d\n", info->maxTextureUnits);
	Printf("max texture coords       : %d\n", info->maxTextureCoords);
	Printf("max texture image units  : %d\n", info->maxTextureImageUnits);

	Printf("------- OpenGL Driver Info -------\n");

	Printf("GL_VENDOR    : %s\n", info->vendor.c_str());
	Printf("GL_RENDERER  : %s\n", info->renderer.c_str());
	Printf("GL_VERSION   : %s\n", info->version.c_str());

	StringSeq exts = StringUtil::tokenizeSeq(info->extension.c_str(), L',');

	if (exts.size() == 0)
		return;

	Printf("GL_EXTENSIONS: %s\n", exts[0].c_str());

	size_t i;
	for (i=1; i<exts.size(); i++) {
		Printf("               %s\n", exts[i].c_str());
	}
}

void RenderSystem::beginFrame(RenderTarget *target)
{
#if 0
	g_assetManager->runFrame();
#endif
	m_curTarget = target;
}


void RenderSystem::beginScene(const RenderCamera &camera)
{
	if (m_curScene)
		Errorf("already in a camera");

	if (camera.getTarget() != m_curTarget)
		Errorf("camera's target != current target");

	ScenePtr sourceview(new RenderScene);

	m_curScene = sourceview;

	m_sceneSeq.push_back(m_curScene);

	m_curScene->camera = camera;
	m_curScene->world = NULL;
	m_curScene->primitives.reserve(256);
	m_curScene->primitives.reserve(256);
}

void RenderSystem::addToScene(RenderWorld *world)
{
	if (m_curScene->world) {
		Errorf("RenderSystem::addToScene: renderWorld has already added.");
	}
	m_curScene->world = world;
}


void RenderSystem::addToScene(Primitive *primitive)
{
	m_curScene->primitives.push_back(primitive);
}

void RenderSystem::addToOverlay(Primitive *primitive)
{
	m_curScene->overlays.push_back(primitive);
}

void RenderSystem::endScene()
{
	m_curScene.reset();
}

void RenderSystem::endFrame()
{
	double frontEndStart = OsUtil::seconds();

	int shaderdebug = r_shaderDebug.getInteger();
	g_shaderMacro.setMacro(ShaderMacro::G_DEBUG, shaderdebug);

	int show_performer = r_showStat.getInteger();
	bool show_memoryinfo = r_showMemoryInfo.getBool();

	if (show_performer || show_memoryinfo) {
		RenderCamera camera;

		Rect screen_rect = m_curTarget->getRect();
		camera.setTarget(m_curTarget);
		camera.setOverlay(screen_rect);

		this->beginScene(camera);

		const int line_height = 12;

		if (show_performer) {
			const List<Stat *> &stats = g_statistic->getGroup("Client");

			Rect rect(0,0,120,12);

			List<Stat *>::const_iterator it = stats.begin();

			String msg;
			for (; it != stats.end(); ++it) {
				Stat *index = *it;
				const String &name = index->getName();
				int value = index->getInt();

				StringUtil::sprintf(msg, "%32s: %d", name.c_str(), value);

				if (rect.y > screen_rect.height - 2 * line_height)
				{
					rect.y = 0;
					rect.x += 300;
				}

				TextPrim *text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left);
				addToScene(text);

				rect.y += line_height;
			}
		}

		if (show_memoryinfo) {
			Rect rect(400,0,120,12);

			String msg;
			TextPrim *text;

			StringUtil::sprintf(msg, "%32s: %d", "Total Allocated", gMemoryInfo.totalAllocated);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Small Allocated", gMemoryInfo.smallAllocated);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Medium Allocated", gMemoryInfo.mediumAllocated);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Large Allocated", gMemoryInfo.largeAllocated);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;

			StringUtil::sprintf(msg, "%32s: %d", "Total Blocks", gMemoryInfo.totalBlocks);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Small Blocks", gMemoryInfo.smallBlocks);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Medium Blocks", gMemoryInfo.mediumBlocks);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Large Blocks", gMemoryInfo.largeBlocks);
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;

			StringUtil::sprintf(msg, "%32s: %d", "Frame Total Allocated", gMemoryInfo.frameTotalAllocated); gMemoryInfo.frameTotalAllocated = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Frame Small Allocated", gMemoryInfo.frameSmallAllocated); gMemoryInfo.frameSmallAllocated = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Frame Medium Allocated", gMemoryInfo.frameMediumAllocated); gMemoryInfo.frameMediumAllocated = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Frame Large Allocated", gMemoryInfo.frameLargeAllocated); gMemoryInfo.frameLargeAllocated = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;

			StringUtil::sprintf(msg, "%32s: %d", "Frame Total Blocks", gMemoryInfo.frameTotalBlocks); gMemoryInfo.frameTotalBlocks = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Frame Small Blocks", gMemoryInfo.frameSmallBlocks); gMemoryInfo.frameSmallBlocks = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Frame Medium Blocks", gMemoryInfo.frameMediumBlocks); gMemoryInfo.frameMediumBlocks = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
			StringUtil::sprintf(msg, "%32s: %d", "Frame Large Blocks", gMemoryInfo.frameLargeBlocks); gMemoryInfo.frameLargeBlocks = 0;
			text = TextPrim::createText(Primitive::HintFrame, rect, g_miniFont.get(), msg, Rgba::White, TextPrim::Left); addToScene(text); rect.y += line_height;
		}

		this->endScene();
	}

	g_renderQueue->beginProviding();
	g_renderQueue->setTarget(m_curTarget);

	// add to render queue
	// gQueryManager->runFrame();
	for (size_t i = 0; i < m_sceneSeq.size(); i++) {
		RenderScene *s_view = m_sceneSeq[i].get();

		QueuedScene *queued = g_renderQueue->allocQueuedScene();
		memset(queued, 0, sizeof(QueuedScene));
		queued->source = s_view;
		queued->camera = s_view->camera;

		if (s_view->world) {
			queued->sceneType = QueuedScene::WorldMain;
			s_view->world->renderTo(queued);
		}

		g_renderQueue->addScene(queued);
	}

	float frontEndTime = OsUtil::seconds() - frontEndStart;

	//g_statistic->setValue(stat_frontendTime, frontEndTime * 1000);
	stat_frontendTime.setInt(frontEndTime * 1000);

	if (m_isMTrendering) {
		g_renderQueue->endProviding();
	} else {
//		g_renderDriver->runFrame();
		g_renderQueue->endConsuming();
	}

	m_sceneSeq.clear();
	m_curScene.reset();

	m_frameNum++;
	g_renderQueue = g_queues[m_frameNum%2];
}

void RenderSystem::beginHitTest(const RenderCamera &view)
{
	m_selectionCamera = view;

	m_selection->beginSelect(view);
}

void RenderSystem::loadHitId(int id)
{
	m_selection->loadSelectId(id);
}


void RenderSystem::hitTest(RenderEntity *re)
{
	m_selection->testEntity(re);
}

void RenderSystem::hitTest(Primitive *prim)
{
	m_selection->testPrimitive(prim);
}

void RenderSystem::hitTest(Primitive *prim, const Matrix &matrix)
{
	m_selection->testPrimitive(prim, matrix);
}

HitRecords RenderSystem::endHitTest()
{
	return m_selection->endSelect();
}

#if 0
RenderTarget *RenderSystem::createWindowTarget(Handle wndId, const String &name)
{
	return g_renderDriver->createWindowTarget(wndId, name);
}
#endif

void RenderSystem::addEntityManager(IEntityManager *manager)
{
	m_entityManagers.push_back(manager);
}

void RenderSystem::removeEntityManager(IEntityManager *manager)
{
	Sequence<IEntityManager*>::iterator it = m_entityManagers.begin();

	for (; it != m_entityManagers.end(); ++it) {
		if (*it == manager) {
			m_entityManagers.erase(it);
			return;
		}
	}
}

int RenderSystem::getNumEntityManager() const
{
	return s2i(m_entityManagers.size());
}

IEntityManager *RenderSystem::getEntityManager(int index) const
{
	return m_entityManagers[index];
}

const RenderDriverInfo *RenderSystem::getDriverInfo()
{
	return g_renderDriver->getDriverInfo();
}

void RenderSystem::texlist_f( const CmdArgs &args )
{
//	Texture::texlist_f(args);
}

void RenderSystem::matlist_f( const CmdArgs &args )
{
	Material::matlist_f(args);
}

int RenderSystem::testArgs( int arg0, float arg1, const Vector3 &arg2, const Color3 &arg3, const Rect &arg4 )
{
	return 1975;
}

Rect RenderSystem::getWindowRect( Handle hwnd )
{
	RECT r;
	BOOL v = ::GetClientRect(hwnd.to<HWND>(), &r);

	if (!v) {
		DWORD error = ::GetLastError();
		Errorf("error wndId");
	}

	return Rect(0, 0, r.right, r.bottom);
}

SamplerStatePtr RenderSystem::findSamplerState( const SamplerStateDesc *desc )
{
	if (!desc)
		desc = &m_defaultSamplerStateDesc;

	SamplerStateDict::const_iterator it = m_samplerStateDict.find(*desc);
	if (it != m_samplerStateDict.end())
		return it->second;

	SamplerState *result = new SamplerState(*desc);
	m_samplerStateDict[*desc] = result;

	return result;
}

ReflectionMap * RenderSystem::findReflection(RenderWorld *world, RenderEntity *actor, Primitive *prim, int width, int height)
{
	return 0;
}

TextureWrapPtr RenderSystem::findTexture(const String &name, int flags)
{
	return TextureWrapPtr();
}

TextureWrapPtr RenderSystem::createTexture(const String &debugname, TexFormat format, int width, int height, int flags)
{
	return TextureWrapPtr();
}

BlendStatePtr RenderSystem::findBlendState(const BlendStateDesc *desc)
{
	return BlendStatePtr();
}

RasterizerStatePtr RenderSystem::findRasterizerState(const RasterizerStateDesc *desc)
{
	return RasterizerStatePtr();
}

DepthStencilStatePtr RenderSystem::findDepthStencilState(const DepthStencilStateDesc *desc)
{
	return DepthStencilStatePtr();
}


AX_END_NAMESPACE

