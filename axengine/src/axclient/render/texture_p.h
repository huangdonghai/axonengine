#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class TextureResource : public KeyedObject, public IEventHandler
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

	// implement IEventHandler
	virtual bool event(Event *e);

	static TextureResourcePtr findResource(const FixedString &name, int flags);
	static TextureResourcePtr createResource(const FixedString &debugname, TexFormat format, int width, int height, int flags);

protected:
	void loadFileMemory(int size, void *data);

private:
	Handle m_handle;
	AsioRequest *m_asioRequest;
	Image *m_asioImage;
	AtomicInt m_isUploaded;

	typedef Dict<FixedString, TextureResource*> ResourceDict;
	static ResourceDict ms_resources;
	static std::list<TextureResource*> ms_asioList;
};

AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
