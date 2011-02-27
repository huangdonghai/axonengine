#ifndef AX_RENDER_LIGHT_P_H
#define AX_RENDER_LIGHT_P_H

AX_BEGIN_NAMESPACE

class RenderLight::ShadowGenerator
{
public:
	class SplitInfo {
	public:
		SplitInfo(RenderLight::ShadowGenerator *shadowInfo, int shadowSize)
		{
			m_updateFrame = -1;
		}

		~SplitInfo()
		{}

	public:
		RenderCamera m_camera;
		Vector3 m_volume[8];
		int m_updateFrame;
		float m_csmExtend;
		BoundingBox m_csmBbox;	// calc scale and offset
	};

	ShadowGenerator(RenderLight *light, int numSplits, int shadowSize);
	~ShadowGenerator();

	bool init();
	void update(RenderScene *qscene);
	void updatePoint(RenderScene *qscene);
	void updateSpot(RenderScene *qscene);
	int updateSplitDist(float f[RenderLight::MAX_CSM_SPLITS+1], float nd, float fd);
	bool checkIfNeedUpdateSplit(int index);
	void updateGlobalSplit(RenderScene *scene, int index, float *f);
	void updateZfar(RenderScene *scene, float *f);
	void updateGlobal(RenderScene *scene);
	bool initGlobal();
	void checkCsmTarget();
	bool initPoint();
	bool initSpot();
	void useShadowMap();
	void unuseShadowMap();

public:
	class ShadowMap;

	RenderLight *m_light;
	int m_numSplits;
	SplitInfo *m_splits[RenderLight::MAX_SPLITS];
	int m_updateFrame;
	int m_shadowMapSize;
	ShadowMap *m_shadowMap;

	// some cached info for check if need update
	Vector3 m_origin;
	Matrix3 m_axis;
	float m_radius;

	Vector4 m_splitScaleOffsets[RenderLight::MAX_CSM_SPLITS];
	float m_csmRange;
	float m_csmLambda;
	int m_csmZfar;
	int m_numCsmSplits;
};

class RenderLight::ShadowGenerator::ShadowMap
{
public:
	ShadowMap(TexType texType, int size)
	{
		m_texType = texType;
		m_size = size;
		m_renderTarget = 0;
	}

	~ShadowMap()
	{
		freeReal();
	}

	void allocReal()
	{
		if (m_renderTarget)
			return;

		m_renderTarget = findShadowMap(m_texType, m_size);
	}

	void freeReal()
	{
		freeShadowMap(m_texType, m_size, m_renderTarget);
		m_renderTarget = 0;
	}


	static RenderTarget *findShadowMap(TexType texType, int size)
	{
		std::list<RenderTarget *> &rtlist = ms_shadowMapPool[texType][size];
		RenderTarget *rt = 0;
		TexFormat format;
		if (rtlist.empty()) {
			if (texType == TexType::_2D) {
				format = g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_ShadowMap];
				rt = new RenderTarget(format, Size(size, size));
				rt->setFilterMode(SamplerDesc::FilterMode_Linear);
				rt->setClampMode(SamplerDesc::ClampMode_Border);
				rt->setBorderColor(SamplerDesc::BorderColor_One);
			} else if (texType == TexType::CUBE) {
				format = g_renderDriverInfo.suggestFormats[RenderDriverInfo::SuggestedFormat_CubeShadowMap];
				rt = new RenderTarget(texType, format, size, size, 1);
				rt->setFilterMode(SamplerDesc::FilterMode_Nearest);
				rt->setClampMode(SamplerDesc::ClampMode_Clamp);
				rt->setBorderColor(SamplerDesc::BorderColor_One);
			} else {
				AX_WRONGPLACE;
			}
		} else {
			rt = rtlist.front();
			rtlist.pop_front();
		}

		if (format.isDepth())
			rt->setComparison(true);

		return rt;
	}

	static void freeShadowMap(TexType texType, int size, RenderTarget *rt)
	{
		ms_shadowMapPool[texType][size].push_front(rt);
	}

public:
	TexType m_texType;
	int m_size;
	RenderTarget *m_renderTarget;

	static Dict<int, std::list<RenderTarget *>> ms_shadowMapPool[TexType::MaxType];
};


AX_END_NAMESPACE

#endif // AX_RENDER_LIGHT_P_H
