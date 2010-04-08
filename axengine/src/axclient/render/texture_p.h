#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class HardwareTexture;
class TextureBackend;
class TextureData;
class Texture2;

AX_DECLARE_REFPTR(HardwareTexture);

class HardwareTexture : public KeyedObject
{
public:
	enum Status {
		Constructed,
		Loading,
		Loaded,
		Missed
	};

	HardwareTexture(const FixedString &key);
	HardwareTexture(const FixedString &key, TexFormat format, int width, int height);
	virtual ~HardwareTexture();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

	static FixedString normalizeKey(const String &name);
	static HardwareTexturePtr findTexture(const FixedString &name);
	static HardwareTexturePtr createTexture(const String &debugname, TexFormat format, int width, int height);

private:
	handle_t m_handle;
	AsioRead m_asioRead;

	static Dict<FixedString, HardwareTexture*> ms_texDict;
	static List<HardwareTexture*> ms_asioList;
};

class TextureBackend : public RenderBackend
{
public:
	TextureBackend(TextureData *data);
	virtual ~TextureBackend();

	virtual void sync();

	void init(const String &name);
	void initUnique(const String &debugname, TexFormat format, int width, int height);
	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

private:
	TextureData *m_data;
	SamplerState m_samplerState;
	HardwareTexturePtr m_hardwareRes;
};

class TextureData : public RenderData
{
public:
	friend class TextureBackend;

	TextureData();
	TextureData(const String &name);
	TextureData(const String &debugname, TexFormat format, int width, int height);
	virtual ~TextureData();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format);
	void generateMipmap();

	// texture parameters
	void setClampMode(SamplerState::ClampMode clampmwode);
	void setFilterMode(SamplerState::FilterMode filtermode);
	void setBorderColor(SamplerState::BorderColor bordercolor);

private:
	SamplerState m_samplerState;
	TextureBackend *m_backend;
};

AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
