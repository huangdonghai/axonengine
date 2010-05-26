#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class TextureWrap : public KeyedObject
{
public:
	enum Status {
		Constructed,
		Loading,
		Loaded,
		Missed
	};

	TextureWrap(const FixedString &key);
	TextureWrap(const FixedString &key, TexFormat format, int width, int height);
	virtual ~TextureWrap();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

private:
	Handle m_handle;
	AsioRead m_asioRead;

	static Dict<FixedString, TextureWrap*> ms_texDict;
	static List<TextureWrap*> ms_asioList;
};

AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
