#include "../private.h"

AX_BEGIN_NAMESPACE

Dict<int, std::list<RenderTarget *>> RenderLight::ShadowGenerator::ShadowMap::ms_shadowMapPool[TexType::MaxType];

RenderLight::ShadowGenerator::ShadowGenerator(RenderLight *light, int numSplits, int shadowSize)
{
	m_light = light;
	m_shadowMapSize = Math::nearestPowerOfTwo(shadowSize);

	if (m_light->isGlobal()) {
		int maxsize = g_renderDriverInfo.maxTextureSize;

		if (m_shadowMapSize > maxsize / 2) {
			m_shadowMapSize = maxsize / 2;
		}
		int csmSize = m_shadowMapSize * 2;

		m_shadowMap = new ShadowMap(TexType::_2D, csmSize);
		m_shadowMap->allocReal();
	} else if (m_light->isPoint()) {
		m_shadowMapSize = std::min(m_shadowMapSize, g_renderDriverInfo.maxCubeMapTextureSize);
		m_shadowMap = new ShadowMap(TexType::CUBE, m_shadowMapSize);
	} else if (m_light->isSpot()) {
		m_shadowMapSize = std::min(m_shadowMapSize, g_renderDriverInfo.maxTextureSize);
		m_shadowMap = new ShadowMap(TexType::_2D, m_shadowMapSize);
	} else {
		m_shadowMap = 0;
	}

	TypeZeroArray(m_splits);
	m_numSplits = numSplits;
	for (int i = 0; i < m_numSplits; i++) {
		m_splits[i] = new SplitInfo(this, shadowSize);
	}
	m_updateFrame = -1;
	light->m_shadowMemoryUsed = m_numSplits * m_shadowMapSize * m_shadowMapSize * 2;

	m_origin.clear();
	m_axis.clear();
	m_radius = 0;

	init();
}

RenderLight::ShadowGenerator::~ShadowGenerator()
{
	for (int i = 0; i < m_numSplits; i++) {
		SafeDelete(m_splits[i]);
	}
}

bool RenderLight::ShadowGenerator::init()
{
	Type t = m_light->lightType();

	if (t == RenderLight::kGlobal) {
		return initGlobal();
	} else if (t == RenderLight::kPoint) {
		return initPoint();
	} else if (t == RenderLight::kSpot) {
		return initSpot();
	}

	return false;
}

void RenderLight::ShadowGenerator::update( RenderScene *qscene )
{
	switch (m_light->lightType()) {
	case RenderLight::kGlobal:
		updateGlobal(qscene);
		break;
	case RenderLight::kPoint:
		updatePoint(qscene);
		break;
	case RenderLight::kSpot:
		updateSpot(qscene);
		break;
	}
}

#if 0
void RenderLight::ShadowGenerator::issueQueuedShadow( RenderScene *qscene )
{
	if (m_updateFrame < 0)
		return;

	ShadowData *qshadow = g_renderFrame->allocType<ShadowData>(1);
	m_light->m_shadowData = qshadow;

	if (m_light->isGlobal()) {
		qshadow->numSplitCamera = m_numCsmSplits;
		memcpy(qshadow->splitScaleOffsets, m_splitScaleOffsets, sizeof(qshadow->splitScaleOffsets));
	} else {
		qshadow->numSplitCamera = m_numSplits;
	}

	for (int i = 0; i < qshadow->numSplitCamera; i++) {
		SplitInfo *si = m_splits[i];
		qshadow->splitCameras[i] = si->m_camera;
		qshadow->splitCameras[i].setTarget(m_shadowMap->m_renderTarget);
		memcpy(qshadow->splitVolumes[i], si->m_volume, sizeof(qshadow->splitVolumes[i]));
	}
}
#endif

void RenderLight::ShadowGenerator::updatePoint(RenderScene *qscene)
{
	bool needRegen = initPoint();

	if (!needRegen && m_light->m_linkedNode->lastUpdateFrame < m_updateFrame)
		return;

	useShadowMap();

	int oldestFrame = -1;
	for (int i = 0; i < m_numSplits; i++) {
		SplitInfo *si = m_splits[i];

		RenderScene *subscene = qscene->addSubScene();

		if (!subscene)
			break;

		stat_shadowPoolUpdate.inc();
		subscene->sceneType = RenderScene::ShadowGen;

		subscene->camera = si->m_camera;
		subscene->sourceLight = m_light;
		subscene->splitIndex = i;

		si->m_updateFrame = m_light->getWorld()->getVisFrameId();

		if (i == 0) {
			oldestFrame = si->m_updateFrame;
		} else {
			oldestFrame = std::min(oldestFrame, si->m_updateFrame);
		}
	}

	m_updateFrame = oldestFrame;
}

void RenderLight::ShadowGenerator::updateSpot(RenderScene *qscene)
{
	bool needRegen = initSpot();

	if (!needRegen && m_light->m_linkedNode->lastUpdateFrame < m_updateFrame)
		return;

	useShadowMap();

	SplitInfo *si = m_splits[0];

	RenderScene *subscene = qscene->addSubScene();

	//g_statistic->incValue(stat_shadowPoolUpdate);
	stat_shadowPoolUpdate.inc();

	subscene->sceneType = RenderScene::ShadowGen;

	subscene->camera = si->m_camera;
	subscene->sourceLight = m_light;
	subscene->splitIndex = 0;

	m_updateFrame = m_light->getWorld()->getVisFrameId();
}

// updateSplitDist computes the near and far distances for every frustum slice
// in camera eye space - that is, at what distance does a slice start and end
int RenderLight::ShadowGenerator::updateSplitDist(float f[RenderLight::MAX_CSM_SPLITS+1], float nd, float fd)
{
	float lambda = r_csmLambda.getFloat();
	float ratio = fd/nd;
	int numsplits = r_csmSplits.getInteger();

	numsplits = Math::clamp<int>(numsplits, 1, RenderLight::MAX_CSM_SPLITS);

	f[0] = nd;

	for (int i=1; i<numsplits; i++) {
		float si = i / (float)numsplits;

		f[i] = lambda*(nd*powf(ratio, si)) + (1-lambda)*(nd + (fd - nd)*si);
	}
	f[numsplits] = fd;

	if (lambda != m_csmLambda || numsplits != m_numCsmSplits || fd != m_csmRange) {
		m_csmLambda = lambda;
		m_numCsmSplits = numsplits;
		m_csmRange = fd;

		for (int i = 0; i < m_numCsmSplits; i++) {
			m_splits[i]->m_csmExtend = 0;
		}
	}

	return numsplits;
}

bool RenderLight::ShadowGenerator::checkIfNeedUpdateSplit(int index)
{
	//static int d[] = {1,3,8,23,16,32};
	static int d[] = {1,1,1,1,1,1};

	SplitInfo *si = m_splits[index];

	if (si->m_updateFrame < 0)
		return true;

	if (m_light->getWorld()->getVisFrameId() - si->m_updateFrame >= d[index])
		return true;
	else
		return false;
}

void RenderLight::ShadowGenerator::updateGlobalSplit(RenderScene *scene, int index, float *f)
{
	RenderScene *subscene = scene->addSubScene();
	if (!subscene)
		return;

	SplitInfo *si = m_splits[index];

	Vector3 *points = si->m_volume;
	BoundingBox &bbox = si->m_csmBbox;

	const RenderCamera &visCamera = scene->camera;

	visCamera.calcPointsAlongZdist(points, f[index]);
	visCamera.calcPointsAlongZdist(points + 4, f[index+1]);

	bbox.clear();

	si->m_camera.setTime(visCamera.time());

	const Matrix4 &lightmatrix = si->m_camera.getViewMatrix();

	for (int j = 0; j < 8; j++) {
		Vector3 lightspace = lightmatrix * points[j];
		bbox.expandBy(lightspace);
	}

	float zfar = -bbox.min.z;
	float znear = zfar - 4000;

	bbox.max.z = zfar;
	bbox.min.z = znear;

	Vector3 extends = bbox.getExtends();

	// fix sub-pixel correct
	if (si->m_csmExtend < std::max(extends.x, extends.y)) {
		si->m_csmExtend = std::max(extends.x, extends.y);
	}

	float pixeldist = si->m_csmExtend / 512.0f;

	bbox.min.x = floor(bbox.min.x / pixeldist) * pixeldist;
	bbox.max.x = bbox.min.x + si->m_csmExtend;

	bbox.min.y = floor(bbox.min.y / pixeldist) * pixeldist;
	bbox.max.y = bbox.min.y + si->m_csmExtend;

	si->m_camera.setOrtho(bbox.min.x, bbox.max.x, bbox.min.y, bbox.max.y, m_csmZfar - 512, m_csmZfar);

	subscene->sceneType = RenderScene::ShadowGen;

	// setup atlas info
	static Point s_viewOffsets[4] = { Point(0,1), Point(1,1), Point(0,0), Point(1,0) };

	subscene->camera = si->m_camera;
	subscene->camera.setViewRect(Rect(s_viewOffsets[index].x * m_shadowMapSize, s_viewOffsets[index].y * m_shadowMapSize, m_shadowMapSize, m_shadowMapSize));
	subscene->sourceLight = m_light;
	subscene->splitIndex = index;

	si->m_updateFrame = m_light->getWorld()->getVisFrameId();
}

void RenderLight::ShadowGenerator::updateZfar(RenderScene *scene, float *f)
{
	SplitInfo *si = m_splits[m_numCsmSplits-1];
	if (si->m_updateFrame > 0) {
		m_csmZfar = si->m_csmBbox.max.z + 16;
		return;
	}

	Vector3 *points = si->m_volume;
	BoundingBox &bbox = si->m_csmBbox;

	const RenderCamera &visCamera = scene->camera;

	visCamera.calcPointsAlongZdist(points, f[m_numCsmSplits-1]);
	visCamera.calcPointsAlongZdist(points + 4, f[m_numCsmSplits]);

	bbox.clear();

	si->m_camera.setTime(visCamera.time());

	const Matrix4 &lightmatrix = si->m_camera.getViewMatrix();

	for (int j = 0; j < 8; j++) {
		Vector3 lightspace = lightmatrix * points[j];
		bbox.expandBy(lightspace);
	}

	float zfar = -bbox.min.z;
	float znear = zfar - 4000;

	m_csmZfar = zfar + 16;
}

void RenderLight::ShadowGenerator::updateGlobal(RenderScene *scene)
{
	if (!r_shadowGen.getBool())
		return;

	initGlobal();

	float neard = scene->camera.znear();
	neard = std::max(neard, 1.0f);
	float fard = r_csmRange.getFloat();

	float f[RenderLight::MAX_CSM_SPLITS+1];

	// calculate split dist
	m_numCsmSplits = updateSplitDist(f, neard, fard);

	Stat *statindexes[4] = {
		&stat_csmSplit0Dist, &stat_csmSplit1Dist, &stat_csmSplit2Dist, &stat_csmSplit3Dist,
	};

	for (int i = 0; i < m_numCsmSplits; i++) {
		statindexes[i]->setInt(f[i+1]);
	}

	checkCsmTarget();

	bool allupdate = true;
	for (int i = 0; i < m_numCsmSplits; i++) {
		if (!checkIfNeedUpdateSplit(i)) {
			allupdate = false;
			break;
		}
	}

	if (allupdate) {
		updateZfar(scene, f);
	}

	for (int i = 0; i < m_numCsmSplits; i++) {
		if (checkIfNeedUpdateSplit(i)) {
			updateGlobalSplit(scene, i, f);
		}
	}

	// setup atlas info
	static const Vector3 s_offsets[4] = { Vector3(0,0,0), Vector3(0.5,0,0), Vector3(0,0.5,0), Vector3(0.5,0.5,0) };

	int oldestFrame = -1;
	for (int i = 0; i < m_numCsmSplits; i++) {
		if (i == 0) {
			oldestFrame = m_splits[i]->m_updateFrame;
			m_splitScaleOffsets[0].set(0.5, 0.5, 0, 0);
		} else {
			const BoundingBox &bbox0 = m_splits[0]->m_csmBbox;
			const BoundingBox &bbox = m_splits[i]->m_csmBbox;
			Vector3 scale = bbox0.getExtends() / bbox.getExtends() * 0.5f;
			Vector3 offset =(bbox0.min - bbox.min) / bbox0.getExtends() * scale + s_offsets[i];
			m_splitScaleOffsets[i].set(scale.x, scale.y, offset.x, offset.y);
			oldestFrame = std::min(oldestFrame, m_splits[i]->m_updateFrame);
		}
	}

	m_updateFrame = oldestFrame;
}

bool RenderLight::ShadowGenerator::initGlobal()
{
	Vector3 origin = m_light->lightDirection();

	if (m_origin == origin)
		return false;

	m_origin = origin;

	Vector3 forward = -m_light->lightDirection();
	Vector3 up(0, 0, 1);

	if (forward.x == 0.0f) {
		up.set(1, 0, 0);
	}

	Vector3 left = up ^ forward;
	up = forward ^ left;

	left.normalize();
	up.normalize();

	for (int i = 0; i < 4; i++) {
		SplitInfo *si = m_splits[i];
		si->m_camera.setTarget(m_shadowMap->m_renderTarget);
		si->m_camera.setOrigin(origin);
		si->m_camera.setViewAxis(Matrix3(forward, left, up));
		si->m_updateFrame = -1;
	}

	return true;
}

void RenderLight::ShadowGenerator::checkCsmTarget()
{
	int maxsize = g_renderDriverInfo.maxTextureSize;

	int desiredSize = r_shadowMapSize.getInteger();
	if (desiredSize > maxsize / 2) {
		desiredSize = maxsize / 2;
	}
	int csmSize = desiredSize * 2;

	if (desiredSize != m_shadowMapSize) {
		SafeDelete(m_shadowMap);
		m_shadowMap = new ShadowMap(TexType::_2D, csmSize);

		for (int i = 0; i < 4; i++) {
			SplitInfo *si = m_splits[i];
			si->m_camera.setTarget(m_shadowMap->m_renderTarget);
		}
	}

	if (!m_shadowMap->m_renderTarget) {
		m_shadowMap->allocReal();
		for (int i = 0; i < 4; i++) {
			m_splits[i]->m_updateFrame = -1;
		}
	}
}

bool RenderLight::ShadowGenerator::initPoint()
{
	const Matrix &mtx = m_light->getMatrix();

	if (m_origin == mtx.origin && m_radius == m_light->radius()) {
		return false;
	}

	m_origin = mtx.origin;
	m_radius = m_light->radius();

	// create camera
	RenderCamera cameras[6];
	RenderCamera::createCubemapCameras(mtx, cameras, 0.5f, m_light->radius());

	// calculate volumes
	for (int i=0; i<6; i++) {
		SplitInfo *si = m_splits[i];

		si->m_volume[0] = mtx.origin;
		si->m_volume[1] = mtx.origin;
		si->m_volume[2] = mtx.origin;
		si->m_volume[3] = mtx.origin;

		// TODO: fix this
		si->m_camera = cameras[i];
		si->m_camera.calcPointsAlongZdist(&si->m_volume[4], m_light->radius());
		si->m_camera.setTarget(m_shadowMap->m_renderTarget);
		si->m_camera.setTargetSlice(i);
		si->m_camera.setViewRect(Rect(0,0,m_shadowMapSize,m_shadowMapSize));
	}

	return true;
}

bool RenderLight::ShadowGenerator::initSpot()
{
	const Matrix &mtx = m_light->getMatrix();

	if (m_origin == mtx.origin && m_axis == mtx.axis && m_radius == m_light->radius()) {
		return false;
	}

	m_origin = mtx.origin;
	m_axis = mtx.axis;
	m_radius = m_light->radius();

	RenderCamera &camera = m_splits[0]->m_camera;
	SplitInfo *si = m_splits[0];

	camera.setOrigin(mtx.origin);
	const Vector3 forward = -mtx.axis[2];
	const Vector3 left = mtx.axis[0];
	const Vector3 up = -mtx.axis[1];
	Matrix3 axis(forward,left,up);
	camera.setViewAxis(axis);
	camera.setFov(m_light->spotAngle(), m_light->spotAngle(), 0.5f, m_light->radius());

	// calculate volumes
	si->m_volume[0] = mtx.origin;
	si->m_volume[1] = mtx.origin;
	si->m_volume[2] = mtx.origin;
	si->m_volume[3] = mtx.origin;

	// TODO: fix this
	camera.calcPointsAlongZdist(&si->m_volume[4], m_light->radius());

	si->m_camera.setTarget(m_shadowMap->m_renderTarget);
	si->m_camera.setViewRect(Rect(0,0,m_shadowMapSize,m_shadowMapSize));

	return true;
}

void RenderLight::ShadowGenerator::useShadowMap()
{
	m_shadowMap->allocReal();
	for (int i = 0; i < m_numSplits; i++) {
		m_splits[i]->m_camera.setTarget(m_shadowMap->m_renderTarget);
	}
}

void RenderLight::ShadowGenerator::unuseShadowMap()
{
	m_shadowMap->freeReal();
	for (int i = 0; i < m_numSplits; i++) {
		m_splits[i]->m_updateFrame = -1;
		m_updateFrame = -1;
	}
}

AX_END_NAMESPACE
