#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class TextureResource : public KeyedObject
{
public:
	enum Status {
		Constructed,
		Loading,
		Loaded,
		Missed
	};

	typedef Texture::InitFlags InitFlags;

	TextureResource(const FixedString &key, InitFlags flags);
	TextureResource(const FixedString &key, TexFormat format, int width, int height, InitFlags flags);
	virtual ~TextureResource();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

	phandle_t getPHandle() { return &m_handle; }

	static TextureResourcePtr findResource(const String &name, int flags);
	static TextureResourcePtr createResource(const String &debugname, TexFormat format, int width, int height, int flags);

private:
	Handle m_handle;
	AsioRead m_asioRead;

	typedef Dict<FixedString, TextureResource*> ResourceDict;
	static ResourceDict ms_texDict;
	static List<TextureResource*> ms_asioList;
};

AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
