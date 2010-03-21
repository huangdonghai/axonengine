#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class HardwareTexture;
class TextureBackend;
class TextureData;
class Texture2;

class HardwareTexture : public RefObject
{
public:
private:
	handle_t m_handle;
};
AX_DECLARE_REFPTR(HardwareTexture);

class HardwareTextureManager
{
public:
	HardwareTextureManager();
	~HardwareTextureManager();

	HardwareTexturePtr findTexture(const String &name);
	HardwareTexturePtr createTexture();

private:
	Dict<FixedString, HardwareTexture*> TexDict;
};

class TextureBackend : public RenderResource
{
public:
	TextureBackend();
	~TextureBackend();

	void sync(TextureData *src);

private:
	SamplerState m_samplerState;
	HardwareTexturePtr m_hardwareRes;
};

class TextureData : public RenderData
{
public:
//	TextureData();
	TextureData(const String &name);
	TextureData(const String &debugname, TexFormat format, int width, int height);
//	virtual ~TextureData();

	void uploadSubTexture(const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
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
