/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

Texture::ExistDict Texture::m_existDict;


//--------------------------------------------------------------------------
// class Texture
//--------------------------------------------------------------------------

Texture::Texture(const std::string &name, InitFlags flags/*=0*/)
{
	FixedString key = normalizeKey(name);
	m_resource = TextureResource::findResource(key, flags);
}

Texture::Texture(const std::string &debugname, TexFormat format, int width, int height, InitFlags flags /*= 0*/)
{
	m_resource = TextureResource::createResource(debugname, format, width, height, flags);
}

Texture::~Texture()
{}

void Texture::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format /*= TexFormat::AUTO*/)
{
	m_resource->uploadSubTexture(rect, pixels, format);
}

void Texture::generateMipmap()
{
	m_resource->generateMipmap();
}

#if 0
void Texture::deleteThis()
{
}

Texture *new Texture(const String &name, InitFlags flags/*=0*/)
{
	return s_textureManager->loadTexture(name, flags);
}

Texture *Texture::create( const String &debugname, TexFormat format, int width, int height, InitFlags flags /*= 0*/ )
{
	return s_textureManager->createTexture(debugname, format, width, height, flags);
}
#endif

bool Texture::isExist(const std::string &name)
{
	FixedString key = normalizeKey(name);

	ExistDict::const_iterator it = m_existDict.find(key);
	if (it != m_existDict.end())
		return it->second;

	std::string filename = key.toString() + ".dds";

	bool result = g_fileSystem->isFileExist(filename);
	m_existDict[key] = result;

	return result;
}

#if 0
void Texture::initManager()
{ /* do nothing */ }

void Texture::finalizeManager()
{ /* do nothing */ }
#endif

FixedString Texture::normalizeKey(const std::string &name)
{
	std::string normalizedName = PathUtil::normalizePath(name);
	FixedString key;

	if (!PathUtil::haveDir(name))
		normalizedName = "textures/" + normalizedName;

	key = PathUtil::removeExt(normalizedName);

	return key;
}

void Texture::setSamplerState(const SamplerDesc &desc)
{
	m_samplerDesc = desc;
}

const SamplerDesc &Texture::getSamplerState() const
{
	return m_samplerDesc;
}


void Texture::setClampMode(SamplerDesc::ClampMode clampMode)
{
	m_samplerDesc.clampMode = clampMode;
}


void Texture::setFilterMode(SamplerDesc::FilterMode filterMode)
{
	m_samplerDesc.filterMode = filterMode;
}


void Texture::saveToFile(const std::string &filename)
{

}


phandle_t Texture::getPHandle() const
{
	return m_resource->getPHandle();
}

#if 0
void Texture::texlist_f(const CmdArgs &args)
{
	Printf("std::list texture(s):\n");

	int count = 0;
	TextureDict::const_iterator it = m_textureDict.begin();
	for (; it != m_textureDict.end(); ++it) {
		Texture *tex = it->second;
		if (!tex) {
			continue;
		}

		int width, height, depth;
		tex->getSize(width, height, depth);
		Printf("%4d %4d %4d %8s %s\n",tex->getref(), width, height, tex->getFormat().toString(), tex->m_key.c_str());
		count++;
	}

	Printf("total %d texture(s)\n", count);
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

Texture *TextureManager::loadTexture(const String &texname, Texture::InitFlags flags/*=0*/)
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
	Texture *tex = createObject();

	if (g_renderDriver->isInRenderingThread()) {
		tex->initialize(key, flags);
	} else {
		// pending to render thread
		LoadCmd cmd;
		cmd.texture = tex.get();
		cmd.texName = key;
		cmd.initFlags = flags;
		cmd.format = 0;
		cmd.width = 0;
		cmd.height = 0;

		m_loadCmdList.push_back(cmd);
	}

	// add to hash table
	m_textureDict[key] = tex.get();
	tex->m_key = key;

	return tex;
}

Texture *TextureManager::createTexture(const std::string &debugname, TexFormat format, int width, int height, Texture::InitFlags flags /*= 0*/)
{
	std::stringstream ss;
	ss << "_" << debugname << "$" << g_system->generateId();

	FixedString key = Texture::normalizeKey(ss.str());

	// create object
	Texture *tex = createObject();

	if (g_renderDriver->isInRenderingThread()) {
		// init immedially
		tex->initialize(format, width, height, flags);
	} else {
		// pending to render thread
		LoadCmd cmd;
		cmd.texture = tex.get();
		cmd.initFlags = flags;
		cmd.format = format;
		cmd.width = width;
		cmd.height = height;

		m_loadCmdList.push_back(cmd);
	}

	m_textureDict[key] = tex.get();
	tex->m_key = key;

	return tex;
}

bool TextureManager::isExist(const FixedString &key)
{
	ExistDict::const_iterator it = m_existDict.find(key);
	if (it != m_existDict.end())
		return it->second;

	std::string filename = key.toString() + ".dds";

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
	void *clonedPixels = g_renderFrame->allocType<byte_t>(size);
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
	Printf("std::list texture(s):\n");

	int count = 0;
	TextureDict::const_iterator it = m_textureDict.begin();
	for (; it != m_textureDict.end(); ++it) {
		Texture *tex = it->second;
		if (!tex) {
			continue;
		}

		int width, height, depth;
		tex->getSize(width, height, depth);
		Printf("%4d %4d %4d %8s %s\n",tex->getref(), width, height, tex->getFormat().toString(), tex->m_key.c_str());
		count++;
	}

	Printf("total %d texture(s)\n", count);
}

//---------------------------------------------------------------------------

Dict<FixedString, TextureResource*> TextureResource::ms_resources;
std::list<TextureResource*> TextureResource::ms_asioList;

TextureResource::TextureResource(const FixedString &key)
{
	setKey(key);

	ms_resources[key] = this;

	m_asioRead.filename = key;
	m_asioRead.syncCounter.incref();
	g_fileSystem->queAsioRead(&m_asioRead);

	ms_asioList.push_back(this);
}


TextureResource::TextureResource(const FixedString &key, TexFormat format, int width, int height)
{
	setKey(key);

	ms_resources[key] = this;

	g_apiWrap->createTexture2D(&m_handle, format, width, height, Texture::IF_NoMipmap);
}

TextureResource::~TextureResource()
{
	ms_resources.erase(getKey());

	if (m_handle);
		g_apiWrap->deleteTexture2D(&m_handle);
}

void TextureResource::uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format)
{
	if (!m_handle) return;

	g_apiWrap->uploadSubTexture(&m_handle, rect, pixels, format);
}

void TextureResource::generateMipmap()
{
	if (!m_handle) return;

	g_apiWrap->generateMipmap(&m_handle);
}

FixedString TextureResource::normalizeKey( const std::string &name )
{
	std::string key;

	if (!PathUtil::haveDir(name))
		key = "textures/" + name;
	else
		key = name;

	key = PathUtil::removeExt(key);

	return key;
}

TextureWrapPtr TextureResource::findTexture(const FixedString &key)
{
	Dict<FixedString, TextureResource*>::const_iterator it = ms_resources.find(key);

	if (it != ms_resources.end())
		return it->second;

	return 0;
}

TextureWrapPtr TextureResource::createTexture(const std::string &debugname, TexFormat format, int width, int height)
{
	std::stringstream ss;
	ss << "_" << debugname << "$" << g_system->generateId();

	FixedString key = ss.str();

	TextureResource *result = new TextureResource(key, format, width, height);

	return result;
}
#endif

AX_END_NAMESPACE

