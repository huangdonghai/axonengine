#ifndef AX_TEXTUREDATA_H
#define AX_TEXTUREDATA_H

AX_BEGIN_NAMESPACE

class HardwareTexture : public RefObject
{
public:
private:
	handle_t m_handle;
};
AX_DECLARE_REFPTR(HardwareTexture);

class TextureBackend : public RenderBackendResource
{
public:
	TextureBackend();
	~TextureBackend();

	void sync(Texture *src);

private:
	SamplerState m_samplerState;
	HardwareTexturePtr m_hardwareRes;
};


AX_END_NAMESPACE

#endif // AX_TEXTUREDATA_H
