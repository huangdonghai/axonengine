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
		IF_None = 0,
		IF_NoMipmap = 1,
		IF_NoDownsample = 2,
		IF_AutoGenMipmap = 4,
		IF_RenderTarget = 8
	};

	typedef Flags_<InitFlag> InitFlags;

	enum TexType {
		TT_2D,
		TT_3D,
		TT_CUBE,
	};

	Texture(const String &name, InitFlags flags=0);
	Texture(const String &debugname, TexFormat format, int width, int height, InitFlags flags = 0);
	~Texture();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	void generateMipmap();

	// get some info
	void getSize(int &width, int &height, int &depth);
	TexFormat getFormat();

	void setSamplerState(const SamplerStateDesc &desc);
	const SamplerStateDesc &getSamplerState() const;
	void setClampMode(SamplerStateDesc::ClampMode clampMode);
	void setFilterMode(SamplerStateDesc::FilterMode filterMode);

	void saveToFile(const String &filename);

	// management
	static bool isExist(const String &name);
	static FixedString normalizeKey(const String &name);
#if 0
	static void Texture::texlist_f(const CmdArgs &args);
#endif

	phandle_t getPHandle() const;

private:
	TextureResourcePtr m_resource;
	SamplerStatePtr m_samplerState;

	// for dynamic texture
	int m_width, m_height;
	TexFormat m_texformat;

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
	typedef List<LoadCmd> LoadCmdList;

	struct UploadCmd {
		Texture *texture;
		Rect rect;
		const void *pixel;
		TexFormat format;
		Rgba color; // if pixel is null, then use this color
	};
	typedef List<UploadCmd> UploadCmdList;

	struct FreeCmd {
		Texture *texture;
		int frameId;
	};
	typedef List<FreeCmd> FreeCmdList;

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

