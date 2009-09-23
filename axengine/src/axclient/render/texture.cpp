/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Render {

	static TextureManager* s_textureManager;

	Texture::~Texture()
	{}

	void Texture::uploadSubTexture( const Rect& rect, const void* pixels, TexFormat format /*= TexFormat::AUTO*/ )
	{
		s_textureManager->uploadSubTexture(this, rect, pixels, format);
	}

	void Texture::generateMipmap()
	{
		s_textureManager->generateMipmap(this);
	}

	void Texture::deleteThis()
	{
		s_textureManager->freeTexture(this);
	}

	TexturePtr Texture::load( const FixedString& name, InitFlags flags/*=0*/ )
	{
		return s_textureManager->loadTexture(name, flags);
	}

	TexturePtr Texture::create( const String& debugname, TexFormat format, int width, int height, InitFlags flags /*= 0*/ )
	{
		return s_textureManager->createTexture(debugname, format, width, height, flags);
	}

	bool Texture::isExist( const FixedString& name )
	{
		return false;
	}

	void Texture::initManager()
	{}

	void Texture::finalizeManager()
	{}

	TextureManager::TextureManager()
	{
		m_frameId = 0;
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

	TexturePtr TextureManager::loadTexture( const FixedString& texname, Texture::InitFlags flags/*=0*/ )
	{
		// find if already loaded
		TextureDict::const_iterator it = m_textureDict.find(texname);
		if (it != m_textureDict.end()) {
			return it->second;
		}

		// create a new texture object
		TexturePtr tex = createObject();

		// pending to render thread
		LoadCmd cmd;
		cmd.texture = tex;
		cmd.texName = texname;
		cmd.initFlags = flags;
		cmd.format = 0;
		cmd.width = 0;
		cmd.height = 0;

		m_loadCmdList.push_back(cmd);

		// add to hash table
		m_textureDict[texname] = tex;

		return tex;
	}

	TexturePtr TextureManager::createTexture( const String& debugname, TexFormat format, int width, int height, Texture::InitFlags flags /*= 0*/ )
	{
		// create object
		TexturePtr tex = createObject();

		LoadCmd cmd;
		cmd.texture = tex;
		cmd.initFlags = flags;
		cmd.format = format;
		cmd.width = width;
		cmd.height = height;

		m_loadCmdList.push_back(cmd);

		std::stringstream ss;
		ss << debugname << "_" << m_frameId << "_" << g_system->generateId();
		m_textureDict[ss.str()] = tex;

		return tex;
	}

	bool TextureManager::isExist( const FixedString& texname )
	{
		ExistDict::const_iterator it = m_existDict.find(texname);
		if (it != m_existDict.end())
			return it->second;

		String filename = texname.toString() + ".dds";

		bool result = g_fileSystem->isFileExist(filename);
		m_existDict[texname] = result;

		return result;
	}

	void TextureManager::uploadSubTexture( Texture* tex, const Rect& rect, const void* pixels, TexFormat format /*= TexFormat::AUTO*/ )
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
		void* clonedPixels = g_renderQueue->allocType<byte_t>(size);
		memcpy(clonedPixels, pixels, size);
		cmd.pixel = clonedPixels;

		m_uploadCmdList.push_back(cmd);
	}

	void TextureManager::generateMipmap( Texture* tex )
	{
		if (tex->m_needGenMipmap.isEmpty())
			m_needGenMipmapHead.addToEnd(tex->m_needGenMipmap);
	}

	void TextureManager::freeTexture( Texture* tex )
	{
		if (tex->m_needFree.isEmpty())
			m_needFreeHead.addToEnd(tex->m_needFree);
	}

}} // namespace Axon::Render

