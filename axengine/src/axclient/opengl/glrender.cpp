/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"


AX_BEGIN_NAMESPACE

static Material *nullmaterial = nullptr;

static inline void setTextureParameter(CGparameter param, GLtexture *tex) {
	if (tex == nullptr) {
//			cgGLSetTextureParameter(param, 0);
		return;
	}

	cgGLSetTextureParameter(param, tex->getObject());
}

void GLrender::setMaterialParameter(Material *mat)
{
	// set texgen parameters
	if (mat->isBaseTcAnim()) {
		const Matrix4 *matrix = mat->getBaseTcMatrix();
		if (matrix) {
			AX_SU(g_baseTcMatrix, *matrix);
		}
	}
}

void GLrender::draw(Material *mat, Technique tech, GLgeometry *prim)
{
	if (!mat) {
		if (!nullmaterial) {
			nullmaterial = Material::load("null");
		}
		mat = nullmaterial.get();
		AX_ASSERT(mat);
	}

	setMaterialParameter(mat);

	ShaderMacro macro = g_shaderMacro;

	// bind sample
	GLtexture *diffuse = (GLtexture*)(mat->getTexture(SamplerType::Diffuse));
	GLtexture *normalmap = (GLtexture*)mat->getTexture(SamplerType::Normal);
	GLtexture *specular = (GLtexture*)mat->getTexture(SamplerType::Specular);
	GLtexture *terrcolor = (GLtexture*)mat->getTexture(SamplerType::TerrainColor);
	GLtexture *terrnormal = (GLtexture*)mat->getTexture(SamplerType::TerrainNormal);
	GLtexture *layeralpha = (GLtexture*)mat->getTexture(SamplerType::LayerAlpha);

	const ShaderMacro &matmacro = mat->getShaderMacro();
	macro.mergeFrom(&matmacro);

#if 0
	// set light parameter
	Interaction *ia = gCurInteraction;
	if (ia &&(tech == Technique::Main || tech == Technique::Layer)) {
		macro.setMacro(ShaderMacro::G_LT_NUMBER, ia->numLights);
		bool haveshadow = false;
		for (int i = 0; i < ia->numLights; i++) {
			macro.setMacro(ShaderMacro::Flag(ShaderMacro::G_LT_TYPE0 + i), ia->qlights[i]->type);
			if (ia->qlights[i]->shadowInfo) {
				haveshadow = true;
			}
		}

		if (haveshadow) {
			macro.setMacro(ShaderMacro::G_LT_HAVESHADOW);
		}
	}
#endif

#if 1
	AX_SU(g_lightMap,  prim->m_lightmap);
	if (prim->m_lightmap && r_lightmap.getBool()) {
		macro.setMacro(ShaderMacro::G_HAVE_LIGHTMAP);
	} else {
		macro.resetMacro(ShaderMacro::G_HAVE_LIGHTMAP);
	}
#endif
	// set material parameter
	AX_SU(g_matDiffuse, mat->getMatDiffuse());
	AX_SU(g_matSpecular, mat->getMatSpecular());
	AX_SU(g_matShiness, mat->getMatShiness());

	double begin = OsUtil::seconds();
#if 0
	GLshader *shader = (GLshader*)FindAsset_<Shader>(mat->getShaderName(), (intptr_t)&macro);
#else
	GLshader *shader = glShaderManager->findShaderGL(mat->getShaderName(), macro);
#endif
	double end = OsUtil::seconds();
	//g_statistic->addValue(stat_findShaderTime, end-begin);
	stat_findShaderTime.add(end - begin);

	if (!shader->haveTechnique(tech)) {
		return;
	}

	shader->setSystemMap(SamplerType::Diffuse, diffuse);
	shader->setSystemMap(SamplerType::Normal, normalmap);
	shader->setSystemMap(SamplerType::Specular, specular);
	shader->setSystemMap(SamplerType::TerrainColor, terrcolor);
	shader->setSystemMap(SamplerType::TerrainNormal, terrnormal);
	shader->setSystemMap(SamplerType::LayerAlpha, layeralpha);

	shader->setSU();
	shader->setInteractionMap();
	shader->setInteractionParameter(mat->getParameters());

	//g_statistic->incValue(stat_numDrawElements);
	stat_numDrawElements.inc();

	if (mat->isPixelToTexelEnabled()) {
		int width = mat->getPixelToTexelWidth();
		int height = mat->getPixelToTexelHeight();
		shader->setPixelToTexel(width, height);
	}

	draw(shader, tech, prim);
}

void GLrender::draw(GLshader *shader, Technique tech, GLgeometry *prim) {
	if (r_nulldraw.getBool()) {
		return;
	}


	CGpass pass = shader->getFirstPass(tech);
	while (pass) {
		cgSetPassState(pass);
		prim->drawElements();
		cgResetPassState(pass);
		pass = cgGetNextPass(pass);
	}

}

static void GetVertexDefInfo(VertexDef vdt, int &numcomponents, GLenum &datatype, int &bytes) {
	switch (vdt) {
	case VDF_position:
	case VDF_normal:
	case VDF_tangent:
	case VDF_binormal:
		numcomponents = 3;
		datatype = GL_FLOAT;
		bytes = numcomponents * sizeof(float);
		break;
	case VDF_baseTc:
	case VDF_lightmapTc:
		numcomponents = 2;
		datatype = GL_FLOAT;
		bytes = numcomponents * sizeof(float);
		break;
	case VDF_color:
		numcomponents = 4;
		datatype = GL_UNSIGNED_BYTE;
		bytes = numcomponents * sizeof(char);
		break;
	default:
		Errorf("GetVertexDefInfo: bad enum");
	}
}

void GLrender::bindVertexBuffer(const VertexDefSeq &defs, GLsizei stride, GLenum bufId, GLuint offset) {
	bool position_have = false;
	bool normal_have = false;
	bool color_have = false;
	bool basetc_have = false;
	bool lightmaptc_have = false;
	bool tangent_have = false;
	bool binormal_have = false;

	glBindBuffer(GL_ARRAY_BUFFER, bufId);
	for (size_t i = 0; i < defs.size(); i++) {
		int num_c, bytes;
		GLenum type;

		GetVertexDefInfo(defs[i], num_c, type, bytes);

		switch (defs[i]) {
		case VDF_position:
			glVertexPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			position_have = true;
			break;
		case VDF_normal:
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(type, stride, BUFFER_OFFSET(offset));
			normal_have = true;
			break;
		case VDF_color:
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			color_have = true;
			break;
		case VDF_baseTc:
			glClientActiveTexture(GL_TEXTURE0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			basetc_have = true;
			break;
		case VDF_lightmapTc:
			glClientActiveTexture(GL_TEXTURE1);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			lightmaptc_have = true;
			break;
		case VDF_tangent:
			glClientActiveTexture(GL_TEXTURE6);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			tangent_have = true;
			break;
		case VDF_binormal:
			glClientActiveTexture(GL_TEXTURE7);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			binormal_have = true;
			break;
		default:
			Errorf("GLrender::bindVertexBuffer: bad enum");
			break;
		}

		offset += bytes;
	}

	if (!normal_have) {
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if (!color_have) {
		glDisableClientState(GL_COLOR_ARRAY);
		glColor4ub(255, 255, 255, 255);
	}

	if (!basetc_have) {
		glClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (!lightmaptc_have) {
		glClientActiveTexture(GL_TEXTURE1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (!tangent_have) {
		glClientActiveTexture(GL_TEXTURE6);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (!binormal_have) {
		glClientActiveTexture(GL_TEXTURE7);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void GLrender::bindVertexBuffer(const VertexDef *defs, GLsizei stride, GLenum bufId, GLuint offset) {
	bool position_have = false;
	bool normal_have = false;
	bool color_have = false;
	bool basetc_have = false;
	bool lightmaptc_have = false;
	bool tangent_have = false;
	bool binormal_have = false;

	glBindBuffer(GL_ARRAY_BUFFER, bufId);
	while (*defs != -1) {
		VertexDef def = *defs++;
		int num_c, bytes;
		GLenum type;

		GetVertexDefInfo(def, num_c, type, bytes);

		switch (def) {
		case VDF_position:
			glVertexPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			position_have = true;
			break;
		case VDF_normal:
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(type, stride, BUFFER_OFFSET(offset));
			normal_have = true;
			break;
		case VDF_color:
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			color_have = true;
			break;
		case VDF_baseTc:
			glClientActiveTexture(GL_TEXTURE0);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			basetc_have = true;
			break;
		case VDF_lightmapTc:
			glClientActiveTexture(GL_TEXTURE1);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			lightmaptc_have = true;
			break;
		case VDF_tangent:
			glClientActiveTexture(GL_TEXTURE6);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			tangent_have = true;
			break;
		case VDF_binormal:
			glClientActiveTexture(GL_TEXTURE7);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(num_c, type, stride, BUFFER_OFFSET(offset));
			binormal_have = true;
			break;
		default:
			Errorf("GLrender::bindVertexBuffer: bad enum");
			break;
		}

		offset += bytes;
	}

	if (!normal_have) {
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if (!color_have) {
		glDisableClientState(GL_COLOR_ARRAY);
		glColor4ub(255, 255, 255, 255);
	}

	if (!basetc_have) {
		glClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (!lightmaptc_have) {
		glClientActiveTexture(GL_TEXTURE1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (!tangent_have) {
		glClientActiveTexture(GL_TEXTURE6);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (!binormal_have) {
		glClientActiveTexture(GL_TEXTURE7);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	double begin = OsUtil::seconds();
	double end = OsUtil::seconds();
	//g_statistic->addValue(stat_findShaderTime, end-begin);
	stat_findShaderTime.add(end - begin);
}

static VertexDef sGeneric[] = {
	VDF_position, // 3, GL_FLOAT, 0,
	VDF_baseTc, // 2, GL_FLOAT, 12,
	VDF_color, // 4, GL_UNSIGNED_BYTE, 20,
	VDF_lightmapTc, // 2, GL_FLOAT, 24,
	VDF_normal, // 3, GL_FLOAT, 32,
	VDF_tangent, // 3, GL_FLOAT, 44,
	VDF_binormal, // 3, GL_FLOAT, 56

	VDF_invalid
};

static VertexDef sDebugVertex[] = {
	VDF_position, // 3, GL_FLOAT, 0,
	VDF_color, // 4, GL_UNSIGNED_BYTE, 12,

	VDF_invalid
};

static VertexDef sBlendVertex[] = {
	VDF_position, // 3, GL_FLOAT, 0,
	VDF_baseTc, // 2, GL_FLOAT, 12,
	VDF_color, // 4, GL_UNSIGNED_BYTE, 20,

	VDF_invalid
};

static VertexDef sChunkVertex[] = {
	VDF_position, // 3, GL_FLOAT, 0,

	VDF_invalid
};

static VertexDef *sAllDef[] = {
	sGeneric, sDebugVertex, sBlendVertex, sChunkVertex
};

static int sStrides[] = {
	sizeof(MeshVertex), sizeof(DebugVertex), sizeof(BlendVertex), sizeof(ChunkVertex)
};

void GLrender::bindVertexBuffer(VertexType vt, GLenum bufId, GLuint offset) {
	bindVertexBuffer(sAllDef[vt], sStrides[vt], bufId, offset);
}

void GLrender::checkForCgError() {
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error == CG_NO_ERROR)
		return;

	if (error == CG_COMPILER_ERROR) {
		Errorf("Error: %s\n\n" "Cg compiler output...\n%s", string, cgGetLastListing(glCgContext));
	} else {
		Errorf("Cg Error: %s", string);
	}
}

void GLrender::checkForCgError(const char *shaderfile) {
#if 0
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error == CG_NO_ERROR)
		return;

	if (error == CG_COMPILER_ERROR) {
		Errorf("Error: %s\n\n" "Cg compiler output...\n%s", string, cgGetLastListing(glCgContext));
	} else {
		Errorf("Cg Error: %s", string);
	}
#else
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error == CG_NO_ERROR)
		return;

	if (error == CG_COMPILER_ERROR) {
		Errorf("%s%s", shaderfile, cgGetLastListing(glCgContext));
	} else {
		Errorf("Cg Error: %s", string);
	}

#endif
}

void GLrender::setViewport(const Rect &rect) {
	glViewport(rect.x, rect.y, rect.width, rect.height);
	glScissor(rect.x, rect.y, rect.width, rect.height);
}


void GLrender::checkErrors() {
#if !defined(DEBUG) || !defined(_DEBUG)
	return;
#endif

	GLenum error;
	char *error_string;

	error = glGetError();
	if (error != GL_NO_ERROR) {
		switch (error) {
		case GL_INVALID_ENUM:
			error_string = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_string = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error_string = "GL_INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			error_string = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error_string = "GL_STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			error_string = "GL_OUT_OF_MEMORY";
			break;
		default:
			Errorf("GLrender::checkErrors: %d", error);
			return;
		}
		Errorf("GLrender::checkErrors: %s", error_string);
	}
}

GLfontrender::GLfontrender()
{

}

GLfontrender::~GLfontrender()
{

}

void GLfontrender::initialize() {
	m_vertexBuffer.setData(nullptr, NumCharsPerBatch * 4 * sizeof(BlendVertex), Primitive::HintDynamic);
#if 0
	m_shader = (GLshader*)FindAsset_<Shader>("font");
#else
	m_shader = glShaderManager->findShaderGL("font", g_shaderMacro);
#endif
	m_posOffset = (float)(Font::ATLAS_PAD) / Font::TEXTURE_SIZE;
#if 0
	m_indexBuffer.setData(nullptr, MAX_CHARS * 2 * 3, Primitive::Static, GL_UNSIGNED_SHORT);
	ushort_t *pidx = (ushort_t*)m_indexBuffer.mapBuffer();
	for (int i = 0; i < MAX_CHARS; i++) {
		pidx[0] = i * 4 + 0;
		pidx[1] = i * 4 + 1;
		pidx[2] = i * 4 + 2;
		pidx[3] = i * 4 + 2;
		pidx[4] = i * 4 + 1;
		pidx[5] = i * 4 + 3;
		pidx += 6;
	}
	m_indexBuffer.unmapBuffer();
#endif
}

void GLfontrender::finalize() {

}

Vector2 GLfontrender::drawString(Font *font, Rgba color, const TextQuad &tq, const Vector2 &offset, const wchar_t *str, size_t len, const Vector2 &scale, bool italic /*= false */) {
	size_t i;
	uint_t count;
	Texture *tex = nullptr;
	Texture *newtex = nullptr;
	Vector4 tc;
	float fontheight = font->getHeight();
	float fontitalic;
	Vector2 pos = offset;

	if (italic) {
		fontitalic = fontheight * 0.2f;
	} else {
		fontitalic = 0;
	}

	font->newFrame();
	m_vertexBuffer.bind();
	GLrender::bindVertexBuffer(VertexType::kBlend, m_vertexBuffer.getObject(), 0);

	BlendVertex *data = (BlendVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	count = 0;
	font->getCharInfo(str[0], tex, tc);

	for (i = 0; i < len; i++) {
		font->getCharInfo(str[i], newtex, tc);

		// check if need render
		if ((tex && newtex != tex) || (count == NumCharsPerBatch)) {
//				m_vertexCount = count * 4;
//				activeCount = count * 6;

			m_shader->setSystemMap(SamplerType::Diffuse, (GLtexture*)tex);

			glUnmapBuffer(GL_ARRAY_BUFFER);
			draw(count);

			count = 0;
			tex = newtex;
			data = (BlendVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		}

		// get glyph info
		const GlyphInfo &glyphinfo = font->getGlyphInfo(str[i]);

		// adjust tc to char glyph width
		tc[2] = tc[0] +(tc[2] - tc[0]) * (float)(glyphinfo.width + 4 * Font::ATLAS_PAD) / font->getWidth();

		// set vertex buffer
		data[count*4+0].xyz = tq.getPos(pos.x + glyphinfo.bearing - 1 * Font::ATLAS_PAD, pos.y + fontheight, scale);
		data[count*4+0].st = Vector2(tc[0], tc[3]);
		data[count*4+0].rgba = color;

		data[count*4+1].xyz = tq.getPos(pos.x + glyphinfo.bearing - 1 * Font::ATLAS_PAD + fontitalic, pos.y, scale);
		data[count*4+1].st = Vector2(tc[0], tc[1]);
		data[count*4+1].rgba = color;

		data[count*4+3].xyz = tq.getPos(pos.x +(glyphinfo.bearing + glyphinfo.width + 3 * Font::ATLAS_PAD), pos.y + fontheight, scale);
		data[count*4+3].st = Vector2(tc[2], tc[3]);
		data[count*4+3].rgba = color;

		data[count*4+2].xyz = tq.getPos(pos.x +(glyphinfo.bearing + glyphinfo.width + 3 * Font::ATLAS_PAD) + fontitalic, pos.y, scale);
		data[count*4+2].st = Vector2(tc[2], tc[1]);
		data[count*4+2].rgba = color;
		count++;

		pos.x += glyphinfo.advance;
	}

//		m_vertexCount = count * 4;
//		activeCount = count * 6;

	m_shader->setSystemMap(SamplerType::Diffuse, (GLtexture*)tex);

	glUnmapBuffer(GL_ARRAY_BUFFER);

	draw(count);

	return pos - offset;
}

void GLfontrender::draw(int count) {
	if (r_nulldraw.getBool()) {
		return;
	}


	m_shader->setSU();

	CGpass pass = m_shader->getFirstPass(Technique::Main);
	while (pass) {
		cgSetPassState(pass);
#if 1
		glDrawArrays(GL_QUADS, 0, count * 4);
#else
		m_indexBuffer.setActiveCount(count * 6);
		m_indexBuffer.drawElements(GL_TRIANGLES);
#endif
		cgResetPassState(pass);
		pass = cgGetNextPass(pass);
	}
}

AX_END_NAMESPACE

