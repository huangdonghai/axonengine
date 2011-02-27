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

Texture::Texture(const std::string &name)
{
	FixedString key = normalizeKey(name);
	m_resource = TextureResource::findResource(key, 0);
}

Texture::Texture(const std::string &name, InitFlags flags/*=0*/)
{
	FixedString key = normalizeKey(name);
	m_resource = TextureResource::findResource(key, flags);
}

Texture::Texture(const std::string &debugname, TexFormat format, const Size &size, InitFlags flags /*= 0*/)
{
	m_resource = new TextureResource(debugname, format, size, flags);
}

Texture::Texture(const TextureResourcePtr &resource, const SamplerDesc &desc) : m_resource(resource), m_samplerDesc(desc)
{}

Texture::Texture(const std::string &debugname, TexType textype, TexFormat format, const Size &size, float depth, InitFlags flags)
{
	_init(debugname, textype, format, size, depth, flags);
}

Texture::Texture()
{}




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

void Texture::setBorderColor( SamplerDesc::BorderColor borderColor )
{
	m_samplerDesc.borderColor = borderColor;
}

void Texture::setComparison(bool comparison)
{
	m_samplerDesc.comparison = comparison;
}


void Texture::saveToFile(const std::string &filename)
{
	// TODO
}

phandle_t Texture::getPHandle() const
{
	return m_resource->getPHandle();
}


Texture * Texture::clone() const
{
	return new Texture(m_resource, m_samplerDesc);
}

Size Texture::size() const
{
	return m_resource->size();
}

TexFormat Texture::format() const
{
	return m_resource->format();
}

void Texture::_init(const std::string &debugname, TexType textype, TexFormat format, const Size &size, float depth, InitFlags flags)
{
	m_resource = new TextureResource(debugname, textype, format, size.width, size.height, depth, flags);
}

AX_END_NAMESPACE

