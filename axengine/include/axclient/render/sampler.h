#ifndef AX_RENDER_SAMPLER_H
#define AX_RENDER_SAMPLER_H

AX_BEGIN_NAMESPACE

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

	Sampler(const String &name, ClampMode clampMode, FilterMode filterMode);


private:
	ClampMode m_clampMode;
	FilterMode m_filterMode;
	Color4 m_boardColor;
	TexturePtr m_texture;
};

AX_END_NAMESPACE

#endif // AX_RENDER_SAMPLER_H
