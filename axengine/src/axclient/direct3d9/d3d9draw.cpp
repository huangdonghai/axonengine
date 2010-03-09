/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "d3d9private.h"

AX_BEGIN_NAMESPACE

static void setMaterialUniforms(Material *mat) {
	if (!mat) {
		AX_SU(g_matDiffuse, Vector3(1,1,1));
		AX_SU(g_matSpecular, Vector3(0,0,0));
		AX_SU(g_matShiness, 10);
		return;
	}

	// set texgen parameters
	if (mat->isBaseTcAnim()) {
		const Matrix4 *matrix = mat->getBaseTcMatrix();
		if (matrix) {
			AX_SU(g_baseTcMatrix, *matrix);
		}
	}

	// set material parameter
	AX_SU(g_matDiffuse, mat->getMatDiffuse());
	AX_SU(g_matSpecular, mat->getMatSpecular());
	AX_SU(g_matShiness, mat->getMatShiness());

	if (mat->haveDetail()) {
		float scale = mat->getDetailScale();
		Vector2 scale2(scale,scale);
		AX_SU(g_layerScale, scale2);
	}
}

void D3D9draw::draw(Material *mat, Technique tech, D3D9geometry *prim)
{
	if (!mat) {
		static D3D9shader *debugshader = d3d9ShaderManager->findShaderDX("_debug");

		if (tech != Technique::Main) {
			return;
		}

		setMaterialUniforms(0);
#if 0
		debugshader->setSU();
#endif
		draw(debugshader, tech, prim);

		return;
	}

	if (!mat->getShaderTemplate()->haveTechnique(tech))
		return;

	setMaterialUniforms(mat);

	ShaderMacro macro = g_shaderMacro;

	const ShaderMacro &matmacro = mat->getShaderMacro();

	macro.mergeFrom(&matmacro);
	if (!r_detail.getBool() && mat->haveDetail()) {
		macro.resetMacro(ShaderMacro::G_HAVE_DETAIL);
		macro.resetMacro(ShaderMacro::G_HAVE_DETAIL_NORMAL);
	}

	if (!r_bumpmap.getBool()) {
		g_shaderMacro.resetMacro(ShaderMacro::G_HAVE_NORMAL);
	}


#if 1
	AX_SU(g_lightMap,  prim->m_lightmap);
	if (prim->m_lightmap && r_lightmap.getBool()) {
		macro.setMacro(ShaderMacro::G_HAVE_LIGHTMAP);
	} else {
		macro.resetMacro(ShaderMacro::G_HAVE_LIGHTMAP);
	}
#endif

	D3D9shader *shader = (D3D9shader*)d3d9ShaderManager->findShader(mat->getShaderNameId(), macro);

	if (!shader->haveTechnique(tech)) {
		return;
	}

	shader->setCoupled(mat);
#if 0
	g_statistic->incValue(stat_numDrawElements);
#else
	stat_numDrawElements.inc();
#endif
	if (mat->isWireframe() & !d3d9ForceWireframe) {
		d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	draw(shader, tech, prim);

	if (mat->isWireframe()  & !d3d9ForceWireframe) {
		d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}

void D3D9draw::draw(D3D9shader *shader, Technique tech, D3D9geometry *prim)
{
	if (r_nulldraw.getBool()) {
		return;
	}

	UINT cPasses = shader->begin(tech);
	for (UINT i = 0; i < cPasses; i++) {
		shader->beginPass(i);
		prim->drawElements();
		shader->endPass();
	}
	shader->end();
}

D3D9draw::D3D9draw()
{
	m_fontShader = d3d9ShaderManager->findShaderDX("font", g_shaderMacro);
	m_vertDecl = d3d9VertexBufferManager->getVertDecl(D3D9vertexobject::VertexBlend);
	m_postVertDecl = d3d9VertexBufferManager->getVertDecl(D3D9vertexobject::VertexChunk);
}

D3D9draw::~D3D9draw()
{

}

Vector2 D3D9draw::drawString(Font *font, Rgba color, const TextQuad &tq, const Vector2 &offset, const wchar_t *str, size_t len, const Vector2 &scale, bool italic /*= false */)
{
//	ulonglong_t t0 = OsUtil::microseconds();
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

	count = 0;
	font->getCharInfo(str[0], tex, tc);

	BlendVertex *data = m_fontVerts;

//	ulonglong_t t1 = OsUtil::microseconds();
	for (i = 0; i < len; i++) {
		font->getCharInfo(str[i], newtex, tc);

		// check if need render
		if ((tex && newtex != tex) || (count == NUM_CHARS_PER_BATCH)) {
			m_fontShader->setSystemMap(SamplerType::Diffuse, (D3D9texture*)tex);

			drawChars(count);

			count = 0;
			tex = newtex;
			data = m_fontVerts;
		}

		// get glyph info
		const GlyphInfo &glyphinfo = font->getGlyphInfo(str[i]);

		// adjust tc to char glyph width
		tc[2] = tc[0] +(tc[2] - tc[0]) * (float)(glyphinfo.width + 4 * Font::ATLAS_PAD) / font->getWidth();

		// set vertex buffer
		// set vertex buffer
		data[count*6+0].xyz = tq.getPos(pos.x + glyphinfo.bearing - 1 * Font::ATLAS_PAD, pos.y + fontheight, scale);
		data[count*6+0].st.set(tc[0], tc[3]);
		data[count*6+0].rgba = color;

		data[count*6+1].xyz = tq.getPos(pos.x + glyphinfo.bearing - 1 * Font::ATLAS_PAD + fontitalic, pos.y, scale);
		data[count*6+1].st.set(tc[0], tc[1]);
		data[count*6+1].rgba = color;

		data[count*6+2].xyz = tq.getPos(pos.x +(glyphinfo.bearing + glyphinfo.width + 3 * Font::ATLAS_PAD) + fontitalic, pos.y, scale);
		data[count*6+2].st.set(tc[2], tc[1]);
		data[count*6+2].rgba = color;

		data[count*6+3] = data[count*6+2];
#if 0
		data[count*6+3].xyz = tq.getPos(pos.x +(glyphinfo.bearing + glyphinfo.width + 3 * Font::ATLAS_PAD) + fontitalic, pos.y, scale);
		data[count*6+3].st.set(tc[2], tc[1]);
		data[count*6+3].rgba = color;
#endif
		data[count*6+4].xyz = tq.getPos(pos.x +(glyphinfo.bearing + glyphinfo.width + 3 * Font::ATLAS_PAD), pos.y + fontheight, scale);
		data[count*6+4].st.set(tc[2], tc[3]);
		data[count*6+4].rgba = color;


		data[count*6+5] = data[count*6+0];
#if 0
		data[count*6+5].xyz = tq.getPos(pos.x + glyphinfo.bearing - 1 * Font::ATLAS_PAD, pos.y + fontheight, scale);
		data[count*6+5].st.set(tc[0], tc[3]);
		data[count*6+5].rgba = color;
#endif
		count++;

		pos.x += glyphinfo.advance;
	}

	m_fontShader->setSystemMap(SamplerType::Diffuse, (D3D9texture*)tex);

//	ulonglong_t t2 = OsUtil::microseconds();
	drawChars(count);

//	ulonglong_t t3 = OsUtil::microseconds();
	return pos - offset;
}

void D3D9draw::drawChars(int count)
{
#if 0
	m_fontShader->setSU();
	d3d9StateManager->setVertexDeclaration(m_vertDecl->getObject());

	UINT cPasses = m_fontShader->begin(Technique::Main);
	for (UINT i = 0; i < cPasses; i++) {
		m_fontShader->beginPass(i);
		d3d9Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count*2, m_fontVerts, sizeof(BlendVertex));
		m_fontShader->endPass();
	}
	m_fontShader->end();
#else
	HRESULT hr;

	DynVb dynvb = d3d9VertexBufferManager->appendVb(count*6*sizeof(BlendVertex));
	memcpy(dynvb.writePtr, m_fontVerts, count*6*sizeof(BlendVertex));
	d3d9VertexBufferManager->endAppendVb();

	V(d3d9Device->SetStreamSource(0, dynvb.vb, dynvb.offset, sizeof(BlendVertex)));
	d3d9StateManager->setVertexDeclaration(m_vertDecl->getObject());

	UINT cPasses = m_fontShader->begin(Technique::Main);
	for (UINT i = 0; i < cPasses; i++) {
		m_fontShader->beginPass(i);
		d3d9Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, count*2);
		m_fontShader->endPass();
	}
	m_fontShader->end();
#endif
}

void D3D9draw::drawPrimitiveUP(Material *mat, Technique tech, void *vertices, int numverts)
{

}

void D3D9draw::drawPostUP(Material *mat, PostMesh *mesh)
{
	D3D9shader *shader = findShader(mat);

	if (!shader) {
		return;
	}

	drawPostUP(shader, mesh);
}

void D3D9draw::drawPostUP(D3D9shader *shader, PostMesh *mesh)
{
	d3d9StateManager->setVertexDeclaration(m_postVertDecl->getObject());

	UINT cPasses = shader->begin(Technique::Main);
	for (UINT i = 0; i < cPasses; i++) {
		shader->beginPass(i);
		d3d9Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, mesh->m_numVertices, mesh->m_numIndices/3, mesh->m_indices, D3DFMT_INDEX16, mesh->m_vertices, sizeof(Vector3));
		shader->endPass();
	}
	shader->end();
}

D3D9shader *D3D9draw::findShader(Material *mat, Technique tech)
{
	if (!mat) {
		static D3D9shader *debugshader = d3d9ShaderManager->findShaderDX("_debug");

		if (tech != Technique::Main) {
			return 0;
		}
		return debugshader;
	}

	setMaterialUniforms(mat);

	ShaderMacro macro = g_shaderMacro;
	const ShaderMacro &matmacro = mat->getShaderMacro();
	macro.mergeFrom(&matmacro);

	D3D9shader *shader = (D3D9shader*)d3d9ShaderManager->findShader(mat->getShaderNameId(), macro);

	if (!shader->haveTechnique(tech)) {
		return 0;
	}
	shader->setCoupled(mat);
	if (mat->isPixelToTexelEnabled()) {
		int width = mat->getPixelToTexelWidth();
		int height = mat->getPixelToTexelHeight();
		shader->setPixelToTexel(width, height);
	}

	return shader;
}

AX_END_NAMESPACE

