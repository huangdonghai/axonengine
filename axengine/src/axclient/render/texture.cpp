/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

static TextureManager *s_textureManager;

//--------------------------------------------------------------------------
// class Texture
//--------------------------------------------------------------------------

Texture::Texture()
{
//	m_needFreeLink.setOwner(this);
//	m_needGenMipmapLink.setOwner(this);
}

Texture::~Texture()
{}

void Texture::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	if (g_renderDriver->isInRenderingThread()) {
		uploadSubTextureIm(rect, pixels, format);
		return;
	}

	s_textureManager->uploadSubTexture(this, rect, pixels, format);
}

void Texture::generateMipmap()
{
	if (g_renderDriver->isInRenderingThread()) {
		generateMipmapIm();
		return;
	}
	s_textureManager->generateMipmap(this);
}

void Texture::deleteThis()
{
	s_textureManager->freeTexture(this);
}

TextureRp Texture::load(const String &name, InitFlags flags/*=0*/)
{
	return s_textureManager->loadTexture(name, flags);
}

TextureRp Texture::create( const String &debugname, TexFormat format, int width, int height, InitFlags flags /*= 0*/ )
{
	return s_textureManager->createTexture(debugname, format, width, height, flags);
}

bool Texture::isExist(const String &name)
{
	return s_textureManager->isExist(name);
}

void Texture::initManager()
{ /* do nothing */ }

void Texture::finalizeManager()
{ /* do nothing */ }

FixedString Texture::normalizeKey( const String &name )
{
	FixedString key;

	if (!PathUtil::haveDir(name))
		key = "textures/" + name;
	else
		key = name;

	key = PathUtil::removeExt(key);

	return key;
}

void Texture::texlist_f( const CmdArgs &args )
{
	s_textureManager->texlist_f(args);
}

//--------------------------------------------------------------------------
// class TextureManager
//--------------------------------------------------------------------------

TextureManager::TextureManager()
{
	if (s_textureManager) {
		Errorf("TextureManager already instanced");
		return;
	}

	s_textureManager = this;
}

TextureManager::~TextureManager()
{
	s_textureManager = 0;
}

TextureRp TextureManager::loadTexture(const String &texname, Texture::InitFlags flags/*=0*/)
{
	FixedString key = Texture::normalizeKey(texname);

	// find if already loaded
	TextureDict::const_iterator it = m_textureDict.find(key);
	if (it != m_textureDict.end()) {
		Texture *result = it->second;
#if 0
		result->m_needFreeLink.removeFromList();
#else
		if (result->m_needFreeLink.isLinked())
			m_needFreeHead.erase(result);
#endif
		result->incref();
		return result;
	}

	if (!isExist(key))
		return 0;

	// create a new texture object
	TextureRp tex = createObject();

	if (g_renderDriver->isInRenderingThread()) {
		tex->initialize(key, flags);
	} else {
		// pending to render thread
		LoadCmd cmd;
		cmd.texture = tex;
		cmd.texName = key;
		cmd.initFlags = flags;
		cmd.format = 0;
		cmd.width = 0;
		cmd.height = 0;

		m_loadCmdList.push_back(cmd);
	}

	// add to hash table
	m_textureDict[key] = tex;
	tex->setKey(key);

	return tex;
}

TextureRp TextureManager::createTexture(const String &debugname, TexFormat format, int width, int height, Texture::InitFlags flags /*= 0*/)
{
	std::stringstream ss;
	ss << "_" << debugname << "$" << g_system->generateId();

	FixedString key = Texture::normalizeKey(ss.str());

	// create object
	TextureRp tex = createObject();

	if (g_renderDriver->isInRenderingThread()) {
		// init immedially
		tex->initialize(format, width, height, flags);
	} else {
		// pending to render thread
		LoadCmd cmd;
		cmd.texture = tex;
		cmd.initFlags = flags;
		cmd.format = format;
		cmd.width = width;
		cmd.height = height;

		m_loadCmdList.push_back(cmd);
	}

	m_textureDict[key] = tex;
	tex->setKey(key);

	return tex;
}

bool TextureManager::isExist(const FixedString &key)
{
	ExistDict::const_iterator it = m_existDict.find(key);
	if (it != m_existDict.end())
		return it->second;

	String filename = key.toString() + ".dds";

	bool result = g_fileSystem->isFileExist(filename);
	m_existDict[key] = result;

	return result;
}

void TextureManager::uploadSubTexture(Texture *tex, const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	// calculate pixel size
	if (format == TexFormat::AUTO) {
		format = tex->getFormat();
	}

	if (format == TexFormat::AUTO) {
		Errorf("unknown texture format");
		return;
	}

	size_t size = format.calculateDataSize(rect.width, rect.height);

	UploadCmd cmd;

	cmd.texture = tex;
	cmd.rect = rect;
	cmd.format = format;

	// clone pixel
	void *clonedPixels = g_renderQueue->allocType<byte_t>(size);
	memcpy(clonedPixels, pixels, size);
	cmd.pixel = clonedPixels;

	m_uploadCmdList.push_back(cmd);
}

void TextureManager::generateMipmap(Texture *tex)
{
#if 0
	tex->m_needGenMipmapLink.addToEnd(m_needGenMipmapHead);
#else
	m_needGenMipmapHead.push_back(tex);
#endif
}

void TextureManager::freeTexture(Texture *tex)
{
#if 0
	tex->m_needFreeLink.addToEnd(m_needFreeHead);
#else
	m_needFreeHead.push_back(tex);
#endif
}

void TextureManager::texlist_f(const CmdArgs &args)
{
	Printf("List texture(s):\n");

	int count = 0;
	TextureDict::const_iterator it = m_textureDict.begin();
	for (; it != m_textureDict.end(); ++it) {
		Texture *tex = it->second;
		if (!tex) {
			continue;
		}

		int width, height, depth;
		tex->getSize(width, height, depth);
		Printf("%4d %4d %4d %8s %s\n",tex->getRefCount(), width, height, tex->getFormat().getStringName(), tex->m_key.c_str());
		count++;
	}

	Printf("total %d texture(s)\n", count);
}


Texture2::Texture2()
{}

Texture2::Texture2(const String &name)
{
	m_data = new TextureData(name);
}

Texture2::Texture2(const String &debugname, TexFormat format, int width, int height)
{
	m_data = new TextureData(debugname, format, width, height);
}

Texture2::~Texture2()
{}

void Texture2::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	if (!m_data) return;
	m_data->uploadSubTexture(rect, pixels, format);
}

void Texture2::generateMipmap()
{
	if (!m_data) return;
	m_data->generateMipmap();
}

void Texture2::setClampMode(SamplerState::ClampMode clampmode)
{
	if (!m_data) return;
	m_data->setClampMode(clampmode);
}

void Texture2::setFilterMode(SamplerState::FilterMode filtermode)
{
	if (!m_data) return;
	m_data->setFilterMode(filtermode);
}

void Texture2::setBorderColor(SamplerState::BorderColor bordercolor)
{
	if (!m_data) return;
	m_data->setBorderColor(bordercolor);
}

AX_END_NAMESPACE

