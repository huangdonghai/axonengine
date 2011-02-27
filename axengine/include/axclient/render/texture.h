/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_RENDER_TEXTURE_H
#define AX_RENDER_TEXTURE_H

AX_BEGIN_NAMESPACE

AX_DECLARE_REFPTR(TextureResource);

//--------------------------------------------------------------------------

class Texture
{
public:
	enum InitFlag {
		AutoGenMipmap = 1,
		RenderTarget = 2,
		Dynamic = 4
	};

	typedef Flags_<InitFlag> InitFlags;

	Texture(const std::string &name);
	Texture(const std::string &name, InitFlags flags);
	Texture(const std::string &debugname, TexFormat format, const Size &size, InitFlags flags);
	Texture(const std::string &debugname, TexType textype, TexFormat format, const Size &size, float depth, InitFlags flags);
	~Texture();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

	// get some info
	Size size() const;
	TexFormat format() const;

	void setSamplerState(const SamplerDesc &desc);
	const SamplerDesc &getSamplerState() const;
	void setClampMode(SamplerDesc::ClampMode clampMode);
	void setFilterMode(SamplerDesc::FilterMode filterMode);
	void setBorderColor(SamplerDesc::BorderColor borderColor);
	void setComparison(bool comparison);

	Texture *clone() const;

	void saveToFile(const std::string &filename);

	// management
	static bool isExist(const std::string &name);
	static FixedString normalizeKey(const std::string &name);
#if 0
	static void Texture::texlist_f(const CmdArgs &args);
#endif

	phandle_t getPHandle() const;

protected:
	Texture();
	void _init(const std::string &debugname, TexType textype, TexFormat format, const Size &size, float depth, InitFlags flags);

private:
	Texture(const TextureResourcePtr &resource, const SamplerDesc &desc);
	TextureResourcePtr m_resource;
	SamplerDesc m_samplerDesc;

	typedef Dict<FixedString,bool> ExistDict;

	static ExistDict m_existDict;
};

AX_END_NAMESPACE

#endif // AX_RENDER_TEXTURE_H

