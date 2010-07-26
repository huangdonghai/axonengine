/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

AX_BEGIN_NAMESPACE

static Vector2 	sSampleOffsets[32];
static float 	sSampleWeights[32];

//-----------------------------------------------------------------------------
// Name: GaussianDistribution
// Desc: Helper function for GetSampleOffsets function to compute the 
//       2 parameter Gaussian distribution using the given standard deviation
//       sigma
//-----------------------------------------------------------------------------
float gaussianDistribution(float x, float y, float sigma) {
	float g = 1.0f / sqrtf(2.0f * AX_PI * sigma * sigma);
	g *= expf(-(x*x + y*y)/(2*sigma*sigma));

	return g;
}

//-----------------------------------------------------------------------------
// Name: GetSampleOffsets_GaussBlur5x5
// Desc: Get the texture coordinate offsets to be used inside the GaussBlur5x5
//       pixel shader.
//-----------------------------------------------------------------------------
void getSampleOffsets_GaussBlur5x5(int width, int height, Vector2 *offsets, Vector4 *weights, float factor=1.0f) {
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

int getSamplerOffsets_Gauss1D(int texwidth, int texheight, int len, Vector2 offsets[32], float weights[32], bool du) {
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


GLpostprocess::GLpostprocess() {
#if 1
	m_screenQuad = new MeshPrim(Primitive::HintStatic);

	Rgba color(255, 255, 255, 255);

	m_screenQuad->init(4, 6);
	MeshVertex *verts = m_screenQuad->lockVertexes();
//		verts[0].xyz = Vector3(rect.x, rect.y, 0.0f);
	verts[0].st.x = 0;
	verts[0].st.y = 1;
	verts[0].rgba = color;
	verts[0].st2 = Vector2(0, 0);
	verts[0].normal = Vector3(0, 0, 1);
	verts[0].tangent = Vector3(1, 0, 0);
	verts[0].binormal = Vector3(0, 1, 0);

//		verts[1].xyz = Vector3(rect.x + rect.width, rect.y, 0.0f);
	verts[1].st.x = 1;
	verts[1].st.y = 1;
	verts[1].rgba = color;
	verts[1].st2 = Vector2(0, 0);
	verts[1].normal = Vector3(0, 0, 1);
	verts[1].tangent = Vector3(1, 0, 0);
	verts[1].binormal = Vector3(0, 1, 0);

//		verts[2].xyz = Vector3(rect.x, rect.y + rect.height, 0.0f);
	verts[2].st.x = 0;
	verts[2].st.y = 0;
	verts[2].rgba = color;
	verts[2].st2 = Vector2(0, 0);
	verts[2].normal = Vector3(0, 0, 1);
	verts[2].tangent = Vector3(1, 0, 0);
	verts[2].binormal = Vector3(0, 1, 0);

//		verts[3].xyz = Vector3(rect.x + rect.width, rect.y + rect.height, 0.0f);
	verts[3].st.x = 1;
	verts[3].st.y = 0;
	verts[3].rgba = color;
	verts[3].st2 = Vector2(0, 0);
	verts[3].normal = Vector3(0, 0, 1);
	verts[3].tangent = Vector3(1, 0, 0);
	verts[3].binormal = Vector3(0, 1, 0);
	m_screenQuad->unlockVertexes();

	ushort_t *indexes = m_screenQuad->lockIndexes();
	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 2;
	indexes[4] = 1;
	indexes[5] = 3;
	m_screenQuad->unlockIndexes();
#else
	m_screenQuad = MeshPrim::createScreenQuad(Primitive::HintStatic, Rect(), Rgba::White);
#endif

	m_boxVolume = nullptr;

	m_matDrawQuad = new Material("drawquad");
	m_matMaskVolume = new Material("maskvolume");
	m_matShadowMask = new Material("shadowmask");
	m_matShadowBlur = new Material("shadowblur");

#if 0
	m_shaderDrawQuad = FindAsset_<GLshader>("drawquad");
	m_shaderHistogram = FindAsset_<GLshader>("_histogram");
	m_shaderDownscale4x4 = FindAsset_<GLshader>("_downscale4x4");
#else
	m_shaderDrawQuad = glShaderManager->findShaderGL("drawquad");
	m_shaderHistogram = glShaderManager->findShaderGL("_histogram");
	m_shaderDownscale4x4 = glShaderManager->findShaderGL("_downscale4x4");
#endif
	glPrimitiveManager->cachePrimitive(m_screenQuad);
	GLgeometry *m_screenQuadGeo = dynamic_cast<GLgeometry*>(glPrimitiveManager->getPrimitive(m_screenQuad->getCachedId()));
}

GLpostprocess::~GLpostprocess() {
	delete m_screenQuad;
}

void GLpostprocess::process(Type type, GLtexture *texture) {
}

void GLpostprocess::updateScreenQuad(const Rect &rect) {
	MeshVertex *verts = m_screenQuad->lockVertexes();
	verts[0].xyz = Vector3(rect.x, rect.y, 0.0f);
	verts[1].xyz = Vector3(rect.x + rect.width, rect.y, 0.0f);
	verts[2].xyz = Vector3(rect.x, rect.y + rect.height, 0.0f);
	verts[3].xyz = Vector3(rect.x + rect.width, rect.y + rect.height, 0.0f);
	m_screenQuad->unlockVertexes();

	GLprimitive *glprim = glPrimitiveManager->getPrimitive(m_screenQuad->getCachedId());
	glprim->update();
	m_screenQuadGeo = dynamic_cast<GLgeometry*>(glprim);
}

void GLpostprocess::drawQuad(GLtexture *texture) {
#if 0
	texture->setFilterMode(Texture::Nearest);

	m_matDrawQuad->setTexture(SamplerType::Diffuse, texture);
	m_screenQuad->setMaterial(m_matDrawQuad);

	const Rect &r = gCamera->getViewRect();
	updateScreenQuad(r);

	glThread->drawPrimitive(m_screenQuad->getCachedId());
#else
//		texture->setFilterMode(Texture::Nearest);

	m_shaderDrawQuad->setSystemMap(MaterialTextureId::Diffuse, texture);
	m_shaderDrawQuad->setSU();

	const Rect &r = gCamera->getViewRect();
	updateScreenQuad(r);

	m_screenQuadGeo->bindVertexBuffer();
	GLrender::draw(m_shaderDrawQuad, Technique::Main, m_screenQuadGeo);
#endif
}

void GLpostprocess::measureHistogram(GLtexture *tex, int index) {
	tex->setFilterMode(Texture::FM_Nearest);

	float w = 1.0f / RenderWorld::HISTOGRAM_WIDTH;
	Vector4 param;
	param.x = w * index;
	param.y = w * (index+1);
	param.z = 0;
	param.w = 1;

	AX_SU(g_instanceParam, param);
	m_shaderHistogram->setSystemMap(MaterialTextureId::Diffuse, tex);
	m_shaderHistogram->setSU();

	const Rect &r = gCamera->getViewRect();
	updateScreenQuad(r);

	m_screenQuadGeo->bindVertexBuffer();
	GLrender::draw(m_shaderHistogram, Technique::Main, m_screenQuadGeo);
}

void GLpostprocess::maskVolume(Vector3 volume[8]) {
	MeshPrim::setupHexahedron(m_boxVolume, volume);
	m_boxVolume->setMaterial(m_matMaskVolume.get());
	glPrimitiveManager->cachePrimitive(m_boxVolume);
	GLprimitive *glprim = glPrimitiveManager->getPrimitive(m_boxVolume->getCachedId());
	glprim->update();

	glThread->drawPrimitive(m_boxVolume->getCachedId());
}

void GLpostprocess::maskShadow(Vector3 volume[8], const Matrix4 &matrix, GLtexture *tex, bool front) {
	MeshPrim::setupHexahedron(m_boxVolume, volume);

	float range = r_csmRange.getFloat();
	Vector2 zrange(range * 0.5f, range);

	int width, height;
	tex->getSize(width, height);
	m_matShadowMask->setPixelToTexel(width, height);

	m_matShadowMask->setTexture(MaterialTextureId::Diffuse, tex);
	m_matShadowMask->setParameter("s_shadowMatrix", 16, matrix.c_ptr());
	m_matShadowMask->setParameter("s_shadowRange", 2, zrange.c_ptr());

	m_matShadowMask->setFeature(0, front);
	m_matShadowMask->setFeature(1, false);

	m_boxVolume->setMaterial(m_matShadowMask.get());
	glPrimitiveManager->cachePrimitive(m_boxVolume);
	GLprimitive *glprim = glPrimitiveManager->getPrimitive(m_boxVolume->getCachedId());
	glprim->update();

	glThread->drawPrimitive(m_boxVolume->getCachedId());
}

void GLpostprocess::maskShadow(Vector3 volume[8], const Matrix4 &matrix, GLtexture *tex, const Vector4 &minrange, const Vector4 &maxrange, const Matrix4 &scaleoffset, bool front /*= false */)
{
	MeshPrim::setupHexahedron(m_boxVolume, volume);

	float range = r_csmRange.getFloat();
	Vector2 zrange(range * 0.5f, range);

	int width, height;
	tex->getSize(width, height);
	m_matShadowMask->setPixelToTexel(width, height);

	m_matShadowMask->setTexture(MaterialTextureId::Diffuse, tex);
	m_matShadowMask->setParameter("s_shadowMatrix", 16, matrix.c_ptr());
	m_matShadowMask->setParameter("s_shadowRange", 2, zrange.c_ptr());

	m_matShadowMask->setParameter("s_minRange", 4, minrange.c_ptr());
	m_matShadowMask->setParameter("s_maxRange", 4, maxrange.c_ptr());
	m_matShadowMask->setParameter("s_offsetScales", 16, scaleoffset.c_ptr());

	m_matShadowMask->setFeature(0, front);
	m_matShadowMask->setFeature(1, true);

	m_boxVolume->setMaterial(m_matShadowMask.get());
	glPrimitiveManager->cachePrimitive(m_boxVolume);
	GLprimitive *glprim = glPrimitiveManager->getPrimitive(m_boxVolume->getCachedId());
	glprim->update();

	glThread->drawPrimitive(m_boxVolume->getCachedId());
}

void GLpostprocess::shadowBlur(GLtexture *texture, bool is_du) {
//		cgGLSetTextureParameter(g_shadowMask, texture->getObject());

	const Vector4 &viewport = gCamera->getViewPort();
//		getSampleOffsets_GaussBlur5x5(viewport.z, viewport.w, sSampleOffsets, sSampleWeights);
	getSamplerOffsets_Gauss1D(viewport.z, viewport.w, 5, sSampleOffsets, sSampleWeights, is_du);
	m_matShadowBlur->setTexture(MaterialTextureId::Diffuse, texture);
	m_matShadowBlur->setParameter("g_sampleOffsets", 32 * 2, sSampleOffsets[0].c_ptr());
	m_matShadowBlur->setParameter("g_sampleWeights", 32, sSampleWeights);

	m_screenQuad->setMaterial(m_matShadowBlur.get());

	const Rect &r = gCamera->getViewRect();
	updateScreenQuad(r);

	glThread->drawPrimitive(m_screenQuad->getCachedId());
}

void GLpostprocess::downscale4x4(GLtexture *tex, const Rect &rect) {
	tex->setFilterMode(Texture::FM_Linear);
	tex->setClampMode(Texture::CM_ClampToEdge);

	m_shaderDownscale4x4->setSystemMap(MaterialTextureId::Diffuse, tex);
	m_shaderDownscale4x4->setSU();

	Vector2 param[4];
	float uscale = 1.0f / rect.width;
	float vscale = 1.0f / rect.height;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			param[i*2+j].x = uscale * i;
			param[i*2+j].y = vscale * j;
		}
	}

	m_shaderDownscale4x4->setParameter("g_sampleOffsets", param, 4);

	const Rect &r = gCamera->getViewRect();
	updateScreenQuad(r);

	m_screenQuadGeo->bindVertexBuffer();
	GLrender::draw(m_shaderDownscale4x4, Technique::Main, m_screenQuadGeo);
}

GLshader *GLpostprocess::getShader(const String &name) {
	Dict<String, GLshader*>::iterator it = m_genericShaders.find(name);

	if (it != m_genericShaders.end()) {
		return it->second;
	}

#if 0
	GLshader *shader = FindAsset_<GLshader>(name);
#else
	GLshader *shader = glShaderManager->findShaderGL(name);
#endif
	m_genericShaders[name] = shader;
	return shader;
}

void GLpostprocess::genericPP(const String &shadername, GLtexture *src) {
	GLshader *shader = getShader(shadername);

	src->setFilterMode(Texture::FM_Nearest);
	src->setClampMode(Texture::CM_ClampToEdge);

	shader->setSystemMap(MaterialTextureId::Diffuse, src);
	shader->setSU();

	int width, height;
	src->getSize(width, height);

	shader->setParameter("s_invTextureSize", Vector2(1.0f/width, 1.0f/height));

	const Rect &r = Rect(0, 0, width, height);
	updateScreenQuad(r);

	m_screenQuadGeo->bindVertexBuffer();
	GLrender::draw(shader, Technique::Main, m_screenQuadGeo);
}

void GLpostprocess::genericPP(const String &shadername, RenderTarget *target, GLtexture *src) {
	GLshader *shader = getShader(shadername);
	RenderCamera camera;
	camera.setTarget(target);
	camera.setOverlay(target->getRect());

	glThread->setupScene(0, 0, 0, &camera);

	src->setFilterMode(Texture::FM_Nearest);
	src->setClampMode(Texture::CM_ClampToEdge);

	shader->setSystemMap(MaterialTextureId::Diffuse, src);
	shader->setSU();

	int width, height;
	src->getSize(width, height);

	shader->setParameter("s_invTextureSize", Vector2(1.0f/width, 1.0f/height));

	const Rect &r = camera.getViewRect();
	updateScreenQuad(r);

	m_screenQuadGeo->bindVertexBuffer();
	GLrender::draw(shader, Technique::Main, m_screenQuadGeo);

	glThread->unsetScene(0, 0, 0, &camera);
}

void GLpostprocess::genericPP(const String &shadername, GLtexture *src1, GLtexture *src2) {
	genericPP(shadername, nullptr, src1, src2);
}

void GLpostprocess::genericPP(const String &shadername, RenderTarget *target, GLtexture *src1, GLtexture *src2) {
	GLshader *shader = getShader(shadername);

	RenderCamera camera;
	if (target) {
		camera.setTarget(target);
		camera.setOverlay(target->getRect());

		glThread->setupScene(0, 0, 0, &camera);
	}

	shader->setSystemMap(MaterialTextureId::Diffuse, src1);
	shader->setSystemMap(MaterialTextureId::Specular, src2);
	shader->setSU();

	int width, height;
	src1->getSize(width, height);

	shader->setParameter("s_invTextureSize", Vector2(1.0f/width, 1.0f/height));

	Rect r;

	if (target) {
		r = camera.getViewRect();
	} else {
		r = gCamera->getViewRect();
	}
	updateScreenQuad(r);

	m_screenQuadGeo->bindVertexBuffer();
	GLrender::draw(shader, Technique::Main, m_screenQuadGeo);

	if (target) {
		glThread->unsetScene(0, 0, 0, &camera);
	}
}

AX_END_NAMESPACE

