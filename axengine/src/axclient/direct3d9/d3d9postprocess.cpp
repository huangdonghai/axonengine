/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

AX_BEGIN_NAMESPACE

PostMesh::PostMesh()
{
	m_numVertices = 0;
	m_vertices = nullptr;
	m_numIndices = 0;
	m_indices = nullptr;
}

PostMesh::~PostMesh()
{
	TypeFree(m_vertices);
	TypeFree(m_indices);
}

void PostMesh::init(int num_vertices, int num_indices)
{
	if (m_numVertices || m_numIndices) {
		Errorf("mesh already initilized");
	}

	m_numVertices = num_vertices;
	m_vertices = TypeAlloc<Vector3>(m_numVertices);
	m_numIndices = num_indices;
	m_indices = TypeAlloc<ushort_t>(m_numIndices);
}

bool PostMesh::setupScreenQuad(PostMesh*& mesh, const Rect &rect)
{
	bool result = false;

	if (!mesh) {
		mesh = new PostMesh();
		mesh->init(4, 6);
		mesh->m_indices[0] = 0;
		mesh->m_indices[1] = 1;
		mesh->m_indices[2] = 2;
		mesh->m_indices[3] = 2;
		mesh->m_indices[4] = 1;
		mesh->m_indices[5] = 3;
	}

	mesh->m_vertices[0].set(rect.x, rect.y, 0.0f);
	mesh->m_vertices[1].set(rect.x + rect.width, rect.y, 0.0f);
	mesh->m_vertices[2].set(rect.x, rect.y + rect.height, 0.0f);
	mesh->m_vertices[3].set(rect.x + rect.width, rect.y + rect.height, 0.0f);

	return result;
}

bool PostMesh::setupHexahedron(PostMesh*& mesh, Vector3 vertes[8])
{
	bool result = false;
	int numverts = 8;
	int numindexes = 6 * 2 * 3;

	if (!mesh) {
		result = true;
		mesh = new PostMesh();
		mesh->init(numverts, numindexes);

		// triangles
		static ushort_t s_idxes[] = {
			0, 2, 1, 1, 2, 3,
			2, 6, 3, 3, 6, 7,
			6, 4, 7, 7, 4, 5,
			4, 0, 5, 5, 0, 1,
			1, 3, 5, 5, 3, 7,
			0, 4, 2, 2, 4, 6
		};

		memcpy(mesh->m_indices, s_idxes, numindexes * sizeof(ushort_t));
	}

	memcpy(mesh->m_vertices, vertes, 8 * sizeof(Vector3));

	return result;
}

bool PostMesh::setupBoundingBox( PostMesh*& mesh, const BoundingBox &bbox )
{
	bool result = false;
	int numverts = 8;
	int numindexes = 6 * 2 * 3;

	if (!mesh) {
		result = true;
		mesh = new PostMesh();
		mesh->init(numverts, numindexes);

		// triangles
		static ushort_t s_idxes[] = {
			0, 2, 1, 1, 2, 3,
			2, 6, 3, 3, 6, 7,
			6, 4, 7, 7, 4, 5,
			4, 0, 5, 5, 0, 1,
			1, 3, 5, 5, 3, 7,
			0, 4, 2, 2, 4, 6
		};

		memcpy(mesh->m_indices, s_idxes, numindexes * sizeof(ushort_t));
	}

	Vector3 *verts = mesh->m_vertices;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				verts[i*4+j*2+k].x = i == 0 ? bbox.min.x : bbox.max.x; 
				verts[i*4+j*2+k].y = j == 0 ? bbox.min.y : bbox.max.y; 
				verts[i*4+j*2+k].z = k == 0 ? bbox.min.z : bbox.max.z; 
			}
		}
	}

	return result;
}


static Vector2 	sSampleOffsets[32];
static float 	sSampleWeights[32];

//-----------------------------------------------------------------------------
// Name: GaussianDistribution
// Desc: Helper function for GetSampleOffsets function to compute the 
//       2 parameter Gaussian distribution using the given standard deviation
//       sigma
//-----------------------------------------------------------------------------
static float gaussianDistribution(float x, float y, float sigma) {
	float g = 1.0f / sqrtf(2.0f * AX_PI * sigma * sigma);
	g *= expf(-(x*x + y*y)/(2*sigma*sigma));

	return g;
}

//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_GaussBlur5x5
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.
//-----------------------------------------------------------------------------
static void getSampleOffsets_GaussBlur5x5(int width, int height, Vector2 *offsets, Vector4 *weights, float factor=1.0f) {
	float tu = 1 / (float)width ;
	float tv = 1 / (float)height;

	Vector4 vWhite(1.0f, 1.0f, 1.0f, 1.0f);

	float totalWeight = 0.0f;
	int index=0;
	for (int x = -2; x <= 2; x++) {
		for (int y = -2; y <= 2; y++) {
			// Exclude pixels with a block distance greater than 2. This will
			// create a kernel which approximates a 5x5 kernel using only 13
			// sample points instead of 25; this is necessary since 2.0 shaders
			// only support 16 texture grabs.
			if (abs(x) + abs(y) > 2)
				continue;

			// Get the unscaled Gaussian intensity for this offset
			offsets[index] = Vector2(x * tu, y * tv);
			weights[index] = vWhite * gaussianDistribution((float)x, (float)y, 1.0f);
			totalWeight += weights[index].x;

			index++;
		}
	}

	// Divide the current weight by the total weight of all the samples; 
	// Gaussian blur kernels add to 1.0f to ensure that the intensity of
	// the image isn't changed when the blur occurs. An optional
	// multiplier variable is used to add or remove image intensity during
	// the blur.
	for (int i=0; i < index; i++) {
		weights[i] /= totalWeight;
		weights[i] *= factor;
	}
}

static int getSamplerOffsets_Gauss1D(int texwidth, int texheight, int len, Vector2 offsets[32], float weights[32], bool du) {
	len = Math::clamp(len, 1, 15);

	float sigma = len / 2.0f;
	//		float sigma = 1;
	float tu = 1 / (float)texwidth ;
	float tv = 1 / (float)texheight ;

	float totalWeight = 0.0f;
	int count = 0;

	for (int i = -len; i <= len; i++) {
		if (du) {
			offsets[count].set(i * tu, 0);
		} else {
			offsets[count].set(0, i * tv);
		}

		weights[count] = gaussianDistribution((float)i, 0, sigma);
		//			weights[count] = 1;
		totalWeight += weights[count];
		count++;
	}

	for (int i=0; i < count; i++) {
		weights[i] /= totalWeight;
	}

	return count;
}


D3D9postprocess::D3D9postprocess() {
	m_mtrDrawQuad = Material::load("drawquad");
	m_mtrMaskVolume = Material::load("maskvolume");

	m_mtrPointLight =  Material::load("_pointlight");
	m_mtrGlobalLight =  Material::load("_globallight");

	m_shaderDrawQuad = d3d9ShaderManager->findShaderDX("drawquad");
	m_shaderHistogram = d3d9ShaderManager->findShaderDX("_histogram");
	m_shaderDownscale4x4 = d3d9ShaderManager->findShaderDX("_downscale4x4");
	m_shaderQuery = d3d9ShaderManager->findShaderDX("_query");

	m_screenQuad = nullptr;
	m_hexahedron = nullptr;
}

D3D9postprocess::~D3D9postprocess() {
}

void D3D9postprocess::process(Type type, D3D9texture *texture) {
}

void D3D9postprocess::drawQuad(D3D9texture *texture) {
#if 0
	texture->setFilterMode(Texture::Nearest);

	m_matDrawQuad->setTexture(SamplerType::Diffuse, texture);
	m_screenQuad->setMaterial(m_matDrawQuad);

	const Rect &r = d3d9Camera->getViewRect();
	updateScreenQuad(r);

	d3d9Thread->drawPrimitive(m_screenQuad->getCachedId());
#else
	//		texture->setFilterMode(Texture::Nearest);

	m_shaderDrawQuad->setSystemMap(SamplerType::Diffuse, texture);
#if 0
	m_shaderDrawQuad->setSU();
#endif
	const Rect &r = d3d9Camera->getViewRect();
	PostMesh::setupScreenQuad(m_screenQuad, r);
#endif
}

void D3D9postprocess::measureHistogram(D3D9texture *tex, int index) {
	tex->setFilterMode(Texture::FM_Nearest);

	float w = 1.0f / RenderWorld::HISTOGRAM_WIDTH;
	Vector4 param;
	param.x = w * index;
	param.y = w * (index+1);
	param.z = 0;
	param.w = 1;

	AX_SU(g_instanceParam, param);
	m_shaderHistogram->setSystemMap(SamplerType::Diffuse, tex);
#if 0
	m_shaderHistogram->setSU();
#endif

	const Rect &r = d3d9Camera->getViewRect();
	PostMesh::setupScreenQuad(m_screenQuad, r);
}

void D3D9postprocess::maskVolume(Vector3 volume[8]) {
	d3d9StateManager->SetRenderState(D3DRS_STENCILWRITEMASK, 255);
	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	d3d9StateManager->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);
	d3d9StateManager->SetRenderState(D3DRS_STENCILREF, 1);
	d3d9StateManager->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_REPLACE);
	d3d9StateManager->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO);

	PostMesh::setupHexahedron(m_hexahedron, volume);
	m_mtrMaskVolume->setFeature(0, false);
	d3d9Draw->drawPostUP(m_mtrMaskVolume, m_hexahedron);

	d3d9StateManager->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	d3d9StateManager->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	d3d9StateManager->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
	d3d9StateManager->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_EQUAL);
	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
}

void D3D9postprocess::maskVolumeTwoSides(Vector3 volume[8])
{
	D3D9clearer clearer;
	clearer.clearStencil(true,127);
	clearer.doClear();

	d3d9StateManager->SetRenderState(D3DRS_STENCILWRITEMASK, 255);
	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	d3d9StateManager->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
	d3d9StateManager->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	d3d9StateManager->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR);
	d3d9StateManager->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_DECR);

	m_mtrMaskVolume->setFeature(0, true);
	PostMesh::setupHexahedron(m_hexahedron, volume);
	d3d9Draw->drawPostUP(m_mtrMaskVolume, m_hexahedron);

	d3d9StateManager->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	d3d9StateManager->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	d3d9StateManager->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
	d3d9StateManager->SetRenderState(D3DRS_STENCILREF, 127);
	d3d9StateManager->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_NOTEQUAL);
	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
}

#if 0
void D3D9postprocess::maskShadow(Vector3 volume[8], const Matrix4 &matrix, D3D9texture *tex, bool front) {
	PostMesh::setupHexahedron(m_hexahedron, volume);

	float range = r_csmRange->getFloat();
	Vector2 zrange(range * 0.5f, range);

	int width, height;
	tex->getSize(width, height);
	m_mtrShadowMask->setPixelToTexel(width, height);

#if 0
	m_mtrShadowMask->setTexture(SamplerType::Diffuse, tex);
	m_mtrShadowMask->setParameter("s_shadowMatrix", 16, matrix.getTranspose());
#else
	AX_SU(g_shadowMap, tex);
	AX_SU(g_shadowMatrix, matrix);
#endif
	m_mtrShadowMask->setParameter("s_shadowRange", 2, zrange.toFloatPointer());

	m_mtrShadowMask->setFeature(0, front);
	m_mtrShadowMask->setFeature(1, false);

	d3d9Draw->drawPostUP(m_mtrShadowMask, m_hexahedron);
}

void D3D9postprocess::maskShadow(Vector3 volume[8], const Matrix4 &matrix, D3D9texture *tex, const Vector4 &minrange, const Vector4 &maxrange, const Matrix4 &scaleoffset, bool front /*= false */)
{
	PostMesh::setupHexahedron(m_hexahedron, volume);

	float range = r_csmRange->getFloat();
	Vector2 zrange(range * 0.5f, range);

	int width, height;
	tex->getSize(width, height);
	m_mtrShadowMask->setPixelToTexel(width, height);

#if 0
	m_mtrShadowMask->setTexture(SamplerType::Diffuse, tex);
	m_mtrShadowMask->setParameter("s_shadowMatrix", 16, matrix.getTranspose());

	m_mtrShadowMask->setParameter("s_minRange", 4, minrange);
	m_mtrShadowMask->setParameter("s_maxRange", 4, maxrange);
#else
	AX_SU(g_shadowMap, tex);
	AX_SU(g_shadowMatrix, matrix);
#endif

	Matrix4 fixmtx = scaleoffset;
	fixmtx[0][2] += 0.5f / width;
	fixmtx[1][2] += 0.5f / width;
	fixmtx[2][2] += 0.5f / width;
	fixmtx[3][2] += 0.5f / width;
	fixmtx[0][3] = 1.0f - fixmtx[0][1] - fixmtx[0][3] + 0.5f / height;
	fixmtx[1][3] = 1.0f - fixmtx[1][1] - fixmtx[1][3] + 0.5f / height;
	fixmtx[2][3] = 1.0f - fixmtx[2][1] - fixmtx[2][3] + 0.5f / height;
	fixmtx[3][3] = 1.0f - fixmtx[3][1] - fixmtx[3][3] + 0.5f / height;
#if 0
	m_mtrShadowMask->setParameter("s_offsetScales", 16, fixmtx);
#else
	AX_SU(g_csmOffsetScales, fixmtx);
#endif
	m_mtrShadowMask->setParameter("s_shadowRange", 2, zrange.toFloatPointer());
	m_mtrShadowMask->setFeature(0, front);
	m_mtrShadowMask->setFeature(1, true);

	d3d9Draw->drawPostUP(m_mtrShadowMask, m_hexahedron);
}

void D3D9postprocess::shadowBlur(D3D9texture *texture, bool is_du) {
	//		cgGLSetTextureParameter(g_shadowMask, texture->getObject());

	const Vector4 &viewport = d3d9Camera->getViewPort();
	//		getSampleOffsets_GaussBlur5x5(viewport.z, viewport.w, sSampleOffsets, sSampleWeights);
	getSamplerOffsets_Gauss1D(viewport.z, viewport.w, 5, sSampleOffsets, sSampleWeights, is_du);
	m_mtrShadowBlur->setTexture(SamplerType::Diffuse, texture);
	m_mtrShadowBlur->setParameter("g_sampleOffsets", 32 * 2, sSampleOffsets[0].toFloatPointer());
	m_mtrShadowBlur->setParameter("g_sampleWeights", 32, sSampleWeights);

	const Rect &r = d3d9Camera->getViewRect();
	PostMesh::setupScreenQuad(m_screenQuad, r);
}
#endif

void D3D9postprocess::downscale4x4(D3D9texture *tex, const Rect &rect) {
	tex->setFilterMode(Texture::FM_Linear);
	tex->setClampMode(Texture::CM_ClampToEdge);

	m_shaderDownscale4x4->setSystemMap(SamplerType::Diffuse, tex);
#if 0
	m_shaderDownscale4x4->setSU();
#endif
	Vector2 param[4];
	float uscale = 1.0f / rect.width;
	float vscale = 1.0f / rect.height;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			param[i*2+j].x = uscale * i;
			param[i*2+j].y = vscale * j;
		}
	}

//		m_shaderDownscale4x4->setParameter("g_sampleOffsets", param, 4);

	const Rect &r = d3d9Camera->getViewRect();
	PostMesh::setupScreenQuad(m_screenQuad, r);
}

D3D9shader *D3D9postprocess::getShader(const String &name) {
	Dict<String, D3D9shader*>::iterator it = m_genericShaders.find(name);

	if (it != m_genericShaders.end()) {
		return it->second;
	}

#if 0
	D3D9shader *shader = FindAsset_<D3D9shader>(name);
#else
	D3D9shader *shader = d3d9ShaderManager->findShaderDX(name);
#endif
	m_genericShaders[name] = shader;
	return shader;
}

void D3D9postprocess::genericPP(const String &shadername, D3D9texture *src) {
	D3D9shader *shader = getShader(shadername);

	src->setFilterMode(Texture::FM_Nearest);
	src->setClampMode(Texture::CM_ClampToEdge);

	shader->setSystemMap(SamplerType::Diffuse, src);
#if 0
	shader->setSU();
#endif
	int width, height;
	src->getSize(width, height);
	shader->setPixelToTexel(width, height);

	const Rect &r = Rect(0, 0, width, height);
	PostMesh::setupScreenQuad(m_screenQuad, r);
}

void D3D9postprocess::genericPP(const String &shadername, RenderTarget *target, D3D9texture *src) {
	D3D9shader *shader = getShader(shadername);
	RenderCamera camera;
	camera.setTarget(target);
	camera.setOverlay(target->getRect());

	d3d9Thread->setupScene(0, 0, 0, &camera);

	src->setFilterMode(Texture::FM_Nearest);
	src->setClampMode(Texture::CM_ClampToEdge);

	shader->setSystemMap(SamplerType::Diffuse, src);
#if 0
	shader->setSU();
#endif
//		int width, height;
//		src->getSize(width, height);

//		shader->setParameter("s_invTextureSize", Vector2(1.0f/width, 1.0f/height));

	const Rect &r = camera.getViewRect();
	PostMesh::setupScreenQuad(m_screenQuad, r);

	d3d9Thread->unsetScene(0, 0, 0, &camera);
}

void D3D9postprocess::genericPP(const String &shadername, D3D9texture *src1, D3D9texture *src2) {
	genericPP(shadername, nullptr, src1, src2);
}

void D3D9postprocess::genericPP(const String &shadername, RenderTarget *target, D3D9texture *src1, D3D9texture *src2) {
	D3D9shader *shader = getShader(shadername);

	RenderCamera camera;
	if (target) {
		camera.setTarget(target);
		camera.setOverlay(target->getRect());

		d3d9Thread->setupScene(0, 0, 0, &camera);
	}

	shader->setSystemMap(SamplerType::Diffuse, src1);
	shader->setSystemMap(SamplerType::Specular, src2);
#if 0
	shader->setSU();
#endif
//		int width, height;
//		src1->getSize(width, height);

//		shader->setParameter("s_invTextureSize", Vector2(1.0f/width, 1.0f/height));

	Rect r;

	if (target) {
		r = camera.getViewRect();
	} else {
		r = d3d9Camera->getViewRect();
	}
	PostMesh::setupScreenQuad(m_screenQuad, r);

//		m_screenQuadGeo->bindVertexBuffer();
//		D3D9draw::draw(shader, Technique::Main, m_screenQuadGeo);

	if (target) {
		d3d9Thread->unsetScene(0, 0, 0, &camera);
	}
}

#define F_MASKFRONT 0
#define F_SPECULAR 1
#define F_PROJECTOR 2
#define F_SHADOWED 3
#define F_SPOTLIGHT 4
#define F_BOXFALLOFF 5

void D3D9postprocess::drawLight(Vector3 volume[8], QueuedLight *light)
{
	PostMesh::setupHexahedron(m_hexahedron, volume);

	Vector4 lightpos = light->matrix.origin;
	lightpos.w = 1.0f / light->radius;

//		m_mtrPointLight->clearParameters();
	m_mtrPointLight->clearFeatures();
	m_mtrPointLight->clearLiterals();

	m_mtrPointLight->setFeature(F_SPOTLIGHT, light->type == RenderLight::kSpot);

	m_mtrPointLight->setParameter("s_lightColor", 4, light->color);
	m_mtrPointLight->setParameter("s_lightPos", 4, lightpos);
	m_mtrPointLight->setParameter("s_lightMatrix", 16, light->projMatrix.getTranspose());

	d3d9Draw->drawPostUP(m_mtrPointLight, m_hexahedron);
}

void D3D9postprocess::drawLightShadowed(Vector3 volume[8], QueuedLight *light, const RenderCamera &shadowCamera)
{
	PostMesh::setupHexahedron(m_hexahedron, volume);

	Vector4 lightpos = light->matrix.origin;
	lightpos.w = 1.0f / light->radius;

	Matrix4 matrix = shadowCamera.getViewProjMatrix();
	D3D9texture *tex = (D3D9texture*)shadowCamera.getTarget()->getTexture();
	int width, height;
	tex->getSize(width, height);
	matrix.scale(0.5f, -0.5f, 0.5f);
	matrix.translate(0.5f+0.5f/width, 0.5f+0.5f/height, 0.5f);

//		m_mtrPointLight->clearParameters();
	m_mtrPointLight->clearFeatures();
	m_mtrPointLight->clearLiterals();

	m_mtrPointLight->setFeature(F_SHADOWED, true);
	m_mtrPointLight->setFeature(F_SPOTLIGHT, light->type == RenderLight::kSpot);

	m_mtrPointLight->setParameter("s_lightColor", 4, light->color);
	m_mtrPointLight->setParameter("s_lightPos", 4, lightpos);
	m_mtrPointLight->setParameter("s_lightMatrix", 16, light->projMatrix.getTranspose());

#if 0
	m_mtrPointLight->setParameter("s_shadowMatrix", 16, matrix.getTranspose());
	m_mtrPointLight->setTexture(SamplerType::Diffuse, tex);
#else
	AX_SU(g_shadowMap,tex);
	AX_SU(g_shadowMatrix,matrix);
#endif
	m_mtrPointLight->setPixelToTexel(width, height);

	d3d9Draw->drawPostUP(m_mtrPointLight, m_hexahedron);
}
#undef F_MASKFRONT
#undef F_SPECULAR
#undef F_PROJECTOR
#undef F_SHADOWED
#undef F_SPOTLIGHT
#undef F_BOXFALLOFF

#define F_SHADOWED 0
#define F_DIRECTION_LIGHT 1
#define F_SKY_LIGHT 2
#define F_ENV_LIGHT 3
void D3D9postprocess::drawGlobalLight( Vector3 volume[8], QueuedLight *light )
{
	QueuedShadow *qshadow = light->shadowInfo;

	if (qshadow) {
		D3D9texture *tex = (D3D9texture*)qshadow->splitCameras[0].getTarget()->getTexture();
		Matrix4 matrix = qshadow->splitCameras[0].getViewProjMatrix();
		matrix.scale(0.5f, -0.5f, 0.5f);
		matrix.translate(0.5f, 0.5f, 0.5f);

		AX_SU(g_shadowMap, tex);
		AX_SU(g_shadowMatrix, matrix);

		int width, height;
		tex->getSize(width, height);
		Matrix4 fixmtx = *(Matrix4*)qshadow->splitScaleOffsets;

		float fixWidth = 0.5f / width;
		float fixHeight = 0.5f / height;

		fixmtx[0][2] += fixWidth;
		fixmtx[1][2] += fixWidth;
		fixmtx[2][2] += fixWidth;
		fixmtx[3][2] += fixWidth;
		fixmtx[0][3] = 1.0f - fixmtx[0][1] - fixmtx[0][3] + fixHeight;
		fixmtx[1][3] = 1.0f - fixmtx[1][1] - fixmtx[1][3] + fixHeight;
		fixmtx[2][3] = 1.0f - fixmtx[2][1] - fixmtx[2][3] + fixHeight;
		fixmtx[3][3] = 1.0f - fixmtx[3][1] - fixmtx[3][3] + fixHeight;
		AX_SU(g_csmOffsetScales, fixmtx.getTranspose());

		fixWidth *= 2;
		fixHeight *= 2;

		Matrix4 g_splitRanges = Matrix4(
			0,		0.5,	0.5,	1,
			0.5,	0.5,	1,		1,
			0,		0,		0.5,	0.5,
			0.5,	0,		1,		0.5
		);

		for (int i = 0; i < 4; i++) {
			g_splitRanges[i].xy() += Vector2(fixWidth, fixHeight);
			g_splitRanges[i].zw() -= Vector2(fixWidth*2, fixHeight*2);
			g_splitRanges[i] = (g_splitRanges[i] - fixmtx[i].zw()) / fixmtx[i].xy();
		}
#if 0
		g_splitRanges[0] = (g_splitRanges[0] - fixmtx[0].zw()) / fixmtx[0].xy();
		g_splitRanges[1] = (g_splitRanges[1] - fixmtx[1].zw()) / fixmtx[1].xy();
		g_splitRanges[2] = (g_splitRanges[2] - fixmtx[2].zw()) / fixmtx[2].xy();
		g_splitRanges[3] = (g_splitRanges[3] - fixmtx[3].zw()) / fixmtx[3].xy();
#endif
		m_mtrGlobalLight->setParameter("s_splitRanges", 16, g_splitRanges.getTranspose());

		m_mtrGlobalLight->setPixelToTexel(width, height);
	}

	PostMesh::setupHexahedron(m_hexahedron, volume);

	Vector4 lightpos = light->matrix.origin;
	lightpos.xyz().normalize();

	m_mtrGlobalLight->clearFeatures();
	m_mtrGlobalLight->setFeature(F_SHADOWED, light->shadowInfo != 0);
	m_mtrGlobalLight->setFeature(F_DIRECTION_LIGHT, !light->color.xyz().isZero());
	m_mtrGlobalLight->setFeature(F_SKY_LIGHT, !light->skyColor.isZero());
	m_mtrGlobalLight->setFeature(F_ENV_LIGHT, !light->envColor.isZero());
	m_mtrGlobalLight->setParameter("s_lightColor", 4, light->color);
	m_mtrGlobalLight->setParameter("s_skyColor", 3, light->skyColor);
	m_mtrGlobalLight->setParameter("s_envColor", 3, light->envColor);
	m_mtrGlobalLight->setParameter("s_lightPos", 4, lightpos);

	d3d9Draw->drawPostUP(m_mtrGlobalLight, m_hexahedron);
}
#undef F_SHADOWED
#undef F_DIRECTION_LIGHT
#undef F_SKY_LIGHT
#undef F_ENV_LIGHT

void D3D9postprocess::issueBboxQuery( const BoundingBox &bbox )
{
	PostMesh::setupBoundingBox(m_hexahedron, bbox);
	d3d9Draw->drawPostUP(m_shaderQuery, m_hexahedron);
}

void D3D9postprocess::issueQueryList( const List<D3D9querymanager::ActiveQuery*>& querylist )
{
	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	d3d9StateManager->setVertexDeclaration(d3d9Draw->m_postVertDecl->getObject());

	int count = 0;
	UINT cPasses = m_shaderQuery->begin(Technique::Main);
	for (UINT i = 0; i < cPasses; i++) {
		m_shaderQuery->beginPass(i);

		List<D3D9querymanager::ActiveQuery*>::const_iterator it = querylist.begin();
		for (; it != querylist.end(); ++it) {
			D3D9querymanager::ActiveQuery *active = *it;

			// have issued
			if (active->issued)
				continue;

			if (active->frameTarget != d3d9FrameWnd)
				continue;

			count++;

			D3D9query *query = active->query;
			if (!query->beginQuery())
				continue;

			PostMesh::setupBoundingBox(m_hexahedron, active->bbox);
			d3d9Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_hexahedron->m_numVertices, m_hexahedron->m_numIndices/3, m_hexahedron->m_indices, D3DFMT_INDEX16, m_hexahedron->m_vertices, sizeof(Vector3));

			query->endQuery();
			active->issued = true;
		}

		m_shaderQuery->endPass();
	}
	m_shaderQuery->end();

	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
	g_statistic->setValue(stat_numVisQuery, count);
}

AX_END_NAMESPACE
