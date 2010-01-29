/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

GameWorld::GameWorld()
{
	m_onlyClient = false;
	m_onlyServer = false;
	m_multiPlayer = false;
	TypeZeroArray(m_actors);
	TypeZeroArray(m_spawnIds);

	m_numEntities = ActorNum::MAX_CLIENTS;
	m_firstFreeEntity = ActorNum::MAX_CLIENTS;
	m_numClients = 0;

	Landscape *landscape = new Landscape(this);
	m_actors[ActorNum::LANDSCAPE] = landscape;

	m_renderWorld = new RenderWorld();
	m_renderWorld->initialize(2048);
	m_outdoorEnv = m_renderWorld->getOutdoorEnv();

	m_physicsWorld = new PhysicsWorld();
	m_physicsWorld->attachObserver(this);

	m_soundWorld = new SoundWorld();

	m_mapEnvDef = new MapEnvDef();

	m_lasttime = OsUtil::getTime();
	m_frametime = 0;

	g_gameSystem->setGameWorld(this);
}

GameWorld::~GameWorld()
{
	g_gameSystem->setGameWorld(0);

	m_physicsWorld->detachObserver(this);
	SafeDelete(m_physicsWorld);
	SafeDelete(m_renderWorld);
}

void GameWorld::runFrame(int what, int frametime)
{
	m_frametime = frametime;
	m_lasttime += m_frametime;

	if (what & Think_physics) {
		m_physicsWorld->step(m_frametime);
	}

	// tick script thread

	for (int i = 0; i < m_numEntities; i++) {
		if (!m_actors[i]) {
			continue;
		}

		m_actors[i]->doThink();
	}
}

void GameWorld::drawFrame()
{
	m_lastCamera.setTime(m_lasttime);

	if (m_actors[0]) {
		Player *player = static_cast<Player*>(m_actors[0]);
		Matrix player3rd = player->getThirdPersonMatrix();
		m_lastCamera.setOrigin(player3rd.origin);
		m_lastCamera.setViewAxis(player3rd.axis);
	}

	g_renderSystem->beginFrame(m_lastCamera.getTarget());
	drawScene(m_lastCamera);
	g_renderSystem->endFrame();
}

void GameWorld::drawScene(const RenderCamera &camera)
{
	m_lastCamera = camera;

	m_soundWorld->setListener(Matrix(camera.getViewAxis(), camera.getOrigin()), Vector3(0,0,0));
	g_soundSystem->setWorld(m_soundWorld);

	if (m_actors[0] && !g_gameSystem->isRunning()) {
		Player *player = static_cast<Player*>(m_actors[0]);
		player->setMatrix(Matrix(camera.getViewAxis(), camera.getOrigin()));
	}

	g_renderSystem->beginScene(camera);
	g_renderSystem->addToScene(m_renderWorld);
	g_renderSystem->endScene();
}

void GameWorld::addActor(GameActor *actor)
{
	int start = m_firstFreeEntity;
	int end = ActorNum::MAX_NORMAL;

	if (actor->isPlayer()) {
		start = m_numClients;
		end = ActorNum::MAX_CLIENTS;
	}

	int i = start;
	while (i < end) {
		if (!m_actors[i])
			break;

		i++;
	}

	if (i == end) {
		Errorf("no free actor's slot to add actor");
		return;
	}

	m_actors[i] = actor;
	actor->m_actorNum = i;
	actor->m_world = this;
	actor->doSpawn();

	if (actor->isPlayer()) {
		m_numClients = std::max(i+1, m_numClients);
	} else {
		m_numEntities = std::max(i+1, m_numEntities);
		m_firstFreeEntity = m_numEntities;
	}
}

void GameWorld::removeActor(GameActor *actor)
{
	int num = actor->m_actorNum;
	if (num < 0) {
		Errorf("not a valid actor number");
		return;
	}
	AX_ASSERT(num >= 0 && num < ActorNum::MAX_ACTORS);

	actor->doRemove();

	m_actors[num] = nullptr;
	actor->m_actorNum = -1;
	actor->m_world = nullptr;

	m_firstFreeEntity = num;
}

GameActor *GameWorld::createActor(const char *clsname)
{
	Object *obj = g_scriptSystem->createObject(clsname);

	if (!obj) {
		return nullptr;
	}

	GameActor *ent = object_cast<GameActor*>(obj);

	if (!ent) {
		delete obj;
		return nullptr;
	}
#if 0
	String objname = clsname;
	StringUtil::strlwr(&objname[0]);
	objname = gScriptSystem->generateObjectName(objname);

	ent->set_objectName(objname);
#endif
	return ent;
}

void GameWorld::addFixed(Fixed *fixed)
{
	getLandscape()->addFixed(fixed);
}

void GameWorld::removeFixed(Fixed *fixed)
{
	getLandscape()->removeFixed(fixed);
}

void GameWorld::doNotify(IObservable *subject, int arg)
{
}

void GameWorld::updateEnvdef()
{
	m_outdoorEnv->setHaveFarSky(m_mapEnvDef->m_haveSkyBox);
	m_outdoorEnv->setHaveOcean(m_mapEnvDef->m_haveOcean);
	m_outdoorEnv->setFog(m_mapEnvDef->m_fogColor.toVector3(), m_mapEnvDef->m_fogDensity);
	m_outdoorEnv->setOceanFog(m_mapEnvDef->m_oceanFogColor.toVector3(), m_mapEnvDef->m_oceanFogDensity);
	m_outdoorEnv->setHaveGlobalLight(m_mapEnvDef->m_haveGlobalLight);
	m_outdoorEnv->setSunColor(m_mapEnvDef->m_sunColor, m_mapEnvDef->m_sunColorX, 1);
	m_outdoorEnv->setSkyColor(m_mapEnvDef->m_skyColor, m_mapEnvDef->m_skyColorX);
	m_outdoorEnv->setEnvColor(m_mapEnvDef->m_envColor, m_mapEnvDef->m_envColorX);
	m_outdoorEnv->setCastShadow(m_mapEnvDef->m_castShadow);
	m_outdoorEnv->setSkyBoxTexture(m_mapEnvDef->m_textureSkyBox);

	float alpha = Math::d2r(m_mapEnvDef->m_dayTime / 24 * 360 - 90);
	float beta = Math::d2r(Math::clamp(m_mapEnvDef->m_latitude,-90.0f, 90.0f));

	Vector3 sundir;
	float bc;
	Math::sincos(alpha, sundir.z, sundir.x);
	Math::sincos(-beta, sundir.y, bc);

	sundir.x *= bc;
	sundir.z *= bc;

	m_outdoorEnv->setSunDir(sundir);
}

void GameWorld::setWindow(RenderTarget *targetWin)
{
	m_targetWindow = targetWin;
}

void GameWorld::restoreActors()
{
	for (int i = ActorNum::MAX_CLIENTS; i < m_numEntities; i++) {
		if (m_actors[i]) {
			m_actors[i]->doPropertyChanged();
		}
	}

}

void GameWorld::reset()
{

}

void GameWorld::addObject(GameObject *node)
{
	if (node->isFixed())
		addFixed((Fixed*)node);
	else
		addActor((GameActor*)node);
}

void GameWorld::removeObject(GameObject *node)
{
	if (node->isFixed())
		removeFixed((Fixed*)node);
	else
		removeActor((GameActor*)node);
}

Matrix GameWorld::getLastViewMatrix() const
{
	return Matrix(m_lastCamera.getViewAxis(), m_lastCamera.getOrigin());
}

AX_END_NAMESPACE

