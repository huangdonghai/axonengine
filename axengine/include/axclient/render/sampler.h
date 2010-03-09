#ifndef AX_RENDER_SAMPLER_H
#define AX_RENDER_SAMPLER_H

AX_BEGIN_NAMESPACE

class SamplerData;

class Sampler
{
public:
	enum ClampMode {
		CM_Repeat,
		CM_Clamp,
		CM_ClampToEdge,	// only used in engine internal
		CM_ClampToBorder // only used in engine internal
	};

	enum FilterMode {
		FM_Nearest,
		FM_Linear,
		FM_Bilinear,
		FM_Trilinear
	};

	Sampler();
	Sampler(const String &name, ClampMode clampMode, FilterMode filterMode);
	~Sampler();

	bool isNull() const { return !m_d; }

private:
	SamplerData *m_d;
};

AX_END_NAMESPACE

#endif // AX_RENDER_SAMPLER_H
