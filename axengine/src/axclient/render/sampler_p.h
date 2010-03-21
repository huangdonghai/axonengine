#ifndef AX_SAMPLERDATA_H
#define AX_SAMPLERDATA_H

AX_BEGIN_NAMESPACE

class SamplerData : public RenderResource
{
public:
	SamplerData();
	~SamplerData();

	// texture parameters
	void setClampMode(Sampler::ClampMode clampmwode);
	void setFilterMode(Sampler::FilterMode filtermode);
	void setBorderColor(Sampler::BorderColor borderColor);
	void setHardwareShadowMap(bool enable);

private:
	friend class SharedDataPointer<SamplerData>;

	Sampler::ClampMode m_clampMode;
	Sampler::FilterMode m_filterMode;
	Sampler::BorderColor m_boardColor;
	int m_textureId;
};

AX_END_NAMESPACE

#endif // AX_SAMPLERDATA_H
