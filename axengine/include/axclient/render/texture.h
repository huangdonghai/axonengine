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
// class Texture
//--------------------------------------------------------------------------

class Texture
{
public:
	friend class TextureManager;

	enum InitFlag {
		AutoGenMipmap = 1,
		RenderTarget = 2,
		Dynamic = 4
	};

	typedef Flags_<InitFlag> InitFlags;

	enum TexType {
		TexType_2D,
		TexType_3D,
		TexType_CUBE,
	};

	Texture(const std::string &name);
	Texture(const std::string &name, InitFlags flags);
	Texture(const std::string &debugname, TexFormat format, const Size &size, InitFlags flags);
	~Texture();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

	// get some info
	Size size() const;
	TexFormat getFormat();

	void setSamplerState(const SamplerDesc &desc);
	const SamplerDesc &getSamplerState() const;
	void setClampMode(SamplerDesc::ClampMode clampMode);
	void setFilterMode(SamplerDesc::FilterMode filterMode);

	Texture *clone() const;

	void saveToFile(const std::string &filename);

	// management
	static bool isExist(const std::string &name);
	static FixedString normalizeKey(const std::string &name);
#if 0
	static void Texture::texlist_f(const CmdArgs &args);
#endif

	phandle_t getPHandle() const;

private:
	Texture(const TextureResourcePtr &resource, const SamplerDesc &desc);
	TextureResourcePtr m_resource;
	SamplerDesc m_samplerDesc;

	typedef Dict<FixedString,bool> ExistDict;

	static ExistDict m_existDict;
};

#if 0
//--------------------------------------------------------------------------
// class TextureManager
//--------------------------------------------------------------------------

class TextureManager {
public:
	TextureManager();
	virtual ~TextureManager();

	// called in main thread
	Texture *loadTexture(const String &texname, Texture::InitFlags flags=0);
	Texture *createTexture(const String &debugname, TexFormat format, int width, int height, Texture::InitFlags flags = 0);
	bool isExist(const FixedString &key);

	// called in draw thread
	virtual Texture *createObject() = 0;

private:
	friend class Texture;
	// internal use
	void uploadSubTexture(Texture *tex, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	void generateMipmap(Texture *tex);
	void freeTexture(Texture *tex);
	void texlist_f(const CmdArgs &args);
	// end internal use

protected:
	struct LoadCmd {
		Texture *texture;
		FixedString texName;	// if filename is empty, than use format, width, height to init
		Texture::InitFlags initFlags;
		TexFormat format;
		int width;
		int height;
	};
	typedef std::list<LoadCmd> LoadCmdList;

	struct UploadCmd {
		Texture *texture;
		Rect rect;
		const void *pixel;
		TexFormat format;
		Rgba color; // if pixel is null, then use this color
	};
	typedef std::list<UploadCmd> UploadCmdList;

	struct FreeCmd {
		Texture *texture;
		int frameId;
	};
	typedef std::list<FreeCmd> FreeCmdList;

	typedef Dict<FixedString,bool> ExistDict;
	typedef Dict<FixedString,Texture*> TextureDict;

	// command buffer
	LoadCmdList m_loadCmdList;
	UploadCmdList m_uploadCmdList;
	FreeCmdList m_freeCmdList;
	typedef IntrusiveList<Texture, &Texture::m_needGenMipmapLink> MipmapList;
	MipmapList m_needGenMipmapHead;
	typedef IntrusiveList<Texture, &Texture::m_needFreeLink> NeedfreeList;
	NeedfreeList m_needFreeHead;

	// texture manager's hash table
	ExistDict m_existDict;
	TextureDict m_textureDict;
};
#endif
AX_END_NAMESPACE

#endif // AX_RENDER_TEXTURE_H

