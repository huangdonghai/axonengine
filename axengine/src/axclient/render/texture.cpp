/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon { namespace Render {

	TextureManager::TextureManager()
	{
		m_frameId = 0;
	}

	TextureManager::~TextureManager()
	{

	}

	TexturePtr TextureManager::loadTexture( const FixedString& texname, Texture::InitFlags flags/*=0*/ )
	{
		TexturePtr tex = createObject();

		LoadCmd cmd;
		cmd.texture = tex;
		cmd.texName = texname;
		cmd.initFlags = flags;
		cmd.format = 0;
		cmd.width = 0;
		cmd.height = 0;

		m_loadCmdList.push_back(cmd);

		return tex;
	}

	TexturePtr TextureManager::createTexture( const String& debugname, TexFormat format, int width, int height, Texture::InitFlags flags /*= 0*/ )
	{
		TexturePtr tex = createObject();

		LoadCmd cmd;
		cmd.texture = tex;
		cmd.initFlags = flags;
		cmd.format = format;
		cmd.width = width;
		cmd.height = height;

		m_loadCmdList.push_back(cmd);

		return tex;
	}

	bool TextureManager::isExist( const FixedString& texname )
	{
		ExistDict::const_iterator it = m_existDict.find(texname);
		if (it != m_existDict.end())
			return it->second;

		return false;
	}

}} // namespace Axon::Render

