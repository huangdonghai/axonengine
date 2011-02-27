#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class TextureResource : public KeyedObject, public IEventHandler
{
	friend class Texture;

public:
	enum Status {
		Constructed,
		Loading,
		Loaded,
		Missed
	};

	typedef Texture::InitFlags InitFlags;

	TextureResource(const FixedString &key, InitFlags flags);
	TextureResource(const FixedString &key, TexFormat format, const Size &size, InitFlags flags);
	TextureResource(const FixedString &key, TexType textype, TexFormat format, int width, int height, float depth, InitFlags flags);
	virtual ~TextureResource();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();
	Size size() const;
	TexFormat format() const;

	phandle_t getPHandle() { return m_handle; }

	// implement IEventHandler
	virtual bool event(Event *e);

	// implement RefObject
	virtual void onDestroy();
	virtual bool canBeDeletedNow();

	static TextureResourcePtr findResource(const FixedString &name, int flags);

protected:
	void _init(const FixedString &key, TexType textype, TexFormat format, int width, int height, float depth, InitFlags flags);

private:
	phandle_t m_handle;
	bool m_isFileTexture;
	bool m_fileLoaded;
	TexType m_texType;
	TexFormat m_format;
	int m_width, m_height, m_depth;

	InitFlags m_initFlags;

	IoRequest *m_ioRequest;

	typedef Dict<FixedString, TextureResource*> ResourceDict;
	static ResourceDict ms_resources;
	static TextureResourcePtr ms_default;
};

AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
