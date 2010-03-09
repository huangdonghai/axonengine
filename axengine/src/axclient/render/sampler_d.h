#ifndef AX_SAMPLERDATA_H
#define AX_SAMPLERDATA_H

AX_BEGIN_NAMESPACE

class SamplerData
{
public:
	SamplerData();
	~SamplerData();

private:
	AtomicInt m_ref;
	Sampler::ClampMode m_clampMode;
	Sampler::FilterMode m_filterMode;
	Color4 m_boardColor;
	TexturePtr m_texture;
};

AX_END_NAMESPACE

#endif // AX_SAMPLERDATA_H
