#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class TextureWrap;
class Texture2;

AX_DECLARE_REFPTR(TextureWrap);

class TextureWrap : public KeyedObject
{
public:
	enum Status {
		Constructed,
		Loading,
		Loaded,
		Missed
	};

	TextureWrap() {}
	TextureWrap(const FixedString &key);
	TextureWrap(const FixedString &key, TexFormat format, int width, int height);
	virtual ~TextureWrap();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

	static FixedString normalizeKey(const String &name);
	static TextureWrapPtr findTexture(const FixedString &name);
	static TextureWrapPtr createTexture(const String &debugname, TexFormat format, int width, int height);

private:
	Handle m_handle;
	AsioRead m_asioRead;

	static Dict<FixedString, TextureWrap*> ms_texDict;
	static List<TextureWrap*> ms_asioList;
};

AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
