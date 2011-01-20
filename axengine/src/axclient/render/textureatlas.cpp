/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE


TextureAtlas::TextureAtlas()
	: m_frameNum(0)
	, m_initialized(false)
	, m_numTextures(0)
	, m_textureSize(0)
	, m_chunkWidth(0)
	, m_chunkHeight(0)
	, m_texFormat(TexFormat::BGRA8)
	, m_atlasPerTexture(0)
	, m_totalAtlas(0)
	, m_atlasId(NULL)
	, m_atlasFrame(NULL)
{
}

TextureAtlas::~TextureAtlas()
{}

void TextureAtlas::initialize(const std::string &name, int chunk_width, int chunk_height, int side_length, TexFormat format)
{
	if (m_initialized) {
		Errorf("TextureAtlas::initialize: has initialized");
		return;
	}

	if (format.isDXTC()) {
		Errorf("TextureAtlas::initialize: texture pool doesn't support DXT compressed texture");
		return;
	}

	m_name = name;

	m_chunkWidth = chunk_width;
	m_chunkHeight = chunk_height;

	const RenderDriverInfo *info = &g_renderDriverInfo;

	if (side_length > info->maxTextureSize) {
		m_textureSize = info->maxTextureSize;
		float f = (float)side_length / m_textureSize;
		m_numTextures = ceil(f * f);
	} else {
		m_textureSize = side_length;
		m_numTextures = 1;
	}

	if (m_numTextures > MAX_TEXTURES) {
		Errorf("TextureAtlas::initialize: mNumTexture > MAX_TEXTURES");
		return;
	}

	int i;
	for (i=1; i<m_textureSize; i<<=1)
		;

	m_textureSize = i;
	m_rowsPerTexture = m_textureSize / m_chunkHeight;
	m_colsPerTexture = m_textureSize / m_chunkWidth;
	m_atlasPerTexture = m_rowsPerTexture * m_colsPerTexture;
	m_totalAtlas = m_atlasPerTexture * m_numTextures;
	m_texFormat = format;

	m_atlasId = new int[m_totalAtlas];
	memset(m_atlasId, INVALID_Chunk, sizeof(int) * m_totalAtlas);
	m_atlasFrame = new int[m_totalAtlas];
	memset(m_atlasFrame, 0, sizeof(uint_t) * m_totalAtlas);

	int image_size = format.calculateDataSize(m_textureSize, m_textureSize);

	std::string tex_name;

	for (i=0; i<m_numTextures; i++) {
		// maybe some machine fast enough so spend time < 1ms, so we add i to
		// texture name, make sure name is unique
		StringUtil::sprintf(tex_name, "_texture_pool_%s", m_name.c_str());
#if 0
		m_textures[i] << dynamic_cast<Texture*>(g_assetManager->createEmptyAsset(Asset::kTexture));
		AX_ASSERT(m_textures[i]);
		m_textures[i]->initialize(format, m_textureSize, m_textureSize);
		g_assetManager->addAsset(Asset::kTexture, tex_name, m_textures[i]);
#else
		m_textures[i] = new Texture(tex_name, format, Size(m_textureSize, m_textureSize), Texture::Dynamic);
#endif
	}

	return;
}

void TextureAtlas::finalize()
{
	SafeDeleteArray(m_atlasId);
	SafeDeleteArray(m_atlasFrame);
}

uint_t TextureAtlas::newFrame()
{
	return ++m_frameNum;
}

bool TextureAtlas::isChunkResident(int id)
{
	return m_atlasHash.find(id) != m_atlasHash.end();
}

// if texture pool is full, return false
bool TextureAtlas::updateChunk(int id, byte_t *data, TexFormat format)
{
	int index;
	int i;

	if (m_atlasHash.find(id) != m_atlasHash.end())
		index = m_atlasHash[id];
	else {
		int oldest = m_frameNum;
		for (i=0; i<m_totalAtlas; i++) {
			if (m_atlasFrame[i] < oldest) {
				oldest = m_atlasFrame[i];
				index = i;
			}
		}
		// texture pool is full, fire error
		if (oldest == m_frameNum) {
			Errorf("TextureAtlas::updateChunk: no enough room to store new chunk");
			return false;
		}

		m_atlasHash[id] = index;

		// check if overwrite used chunk, delete old chunkid hash
		int userId = m_atlasId[index];
		if (userId != INVALID_Chunk) {
			m_atlasHash.erase(userId);
		}
	}

	// upload texture
	int tex;
	Rect rect;
	getChunkInfoByIndex(index, tex, rect);
	m_textures[tex]->uploadSubTexture(rect, data, format);

	// register this id
	m_atlasId[index] = id;
	m_atlasFrame[index] = m_frameNum;
	return true;
}

void TextureAtlas::getChunkInfo(int id, Texture*& tex, Vector4 &tc)
{
	if (m_atlasHash.find(id) == m_atlasHash.end())
		Errorf("TextureAtlas::getChunkInfo: Chunk %d not resident", id);

	int tex_num;
	Rect rect;
	getChunkInfoByIndex(m_atlasHash[id], tex_num, rect);

	tex = m_textures[tex_num];
	float tc_scale = 1.f / m_textureSize;
	tc[0] =(rect.x) * tc_scale;
	tc[1] =(rect.y) * tc_scale;
	tc[2] =(rect.x + rect.width) * tc_scale;
	tc[3] =(rect.y + rect.height) * tc_scale;
}

void TextureAtlas::getChunkInfoByIndex(int index, int &tex, Rect &rect)
{
	AX_ASSERT(index < m_totalAtlas);

	tex = index / m_atlasPerTexture;
	index = index % m_atlasPerTexture;
	int row = index / m_colsPerTexture;
	int col = index % m_colsPerTexture;

	rect = Rect(col*m_chunkWidth, row*m_chunkHeight, m_chunkWidth, m_chunkHeight);
}

void TextureAtlas::setTextureFilterMode(SamplerDesc::FilterMode mode)
{
	for (int i = 0; i < m_numTextures; i++) {
		SamplerDesc desc = m_textures[i]->getSamplerState();
		desc.filterMode = mode;
		m_textures[i]->setSamplerState(desc);
	}
}

AX_END_NAMESPACE

