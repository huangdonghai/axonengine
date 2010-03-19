#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class HardwareTexture;
class TextureBackend;
class TextureFrontend;
class NewTexture;

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

	HardwareTextureRp findTexture(const String &name);
	HardwareTextureRp createTexture();

private:
	Dict<FixedString, HardwareTexture*> TexDict;
};

class TextureBackend : public RenderBackendResource
{
public:
	TextureBackend();
	~TextureBackend();

	void sync(TextureFrontend *src);

private:
	SamplerState m_samplerState;
	HardwareTexturePtr m_hardwareRes;
};

class TextureFrontend : public RenderFrontendResource
{
public:
private:
	SamplerState m_samplerState;

	TextureBackend *m_backend;
};

AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
