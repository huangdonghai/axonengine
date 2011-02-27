/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_LIGHT_H
#define AX_RENDER_LIGHT_H

AX_BEGIN_NAMESPACE

class AX_API RenderLight : public RenderEntity
{
	friend struct RenderScene;
	friend class RenderWorld;
	friend class RenderContext;
	friend class ShadowGenerator;

public:
	enum Type {
		kInvalid, kGlobal, kPoint, kSpot
	};

	enum Limits {
		MAX_CSM_SPLITS = 4,
		MAX_CUBE_SPLITS = 6,
		MAX_SPLITS = 6,
		NUM_VOLUME_VERTEXES = 8
	};

	typedef Vector3 VolumeVertexes[RenderLight::NUM_VOLUME_VERTEXES];

	RenderLight();
	RenderLight(Type t, const Vector3 &pos, Rgb color);
	RenderLight(Type t, const Vector3 &pos, Rgb color, float radius);
	virtual ~RenderLight();

	Type lightType() const { return m_type; }
	void setLightType(Type t) { m_type = t; }
	bool isGlobal() const { return m_type == kGlobal; }
	bool isPoint() const { return m_type == kPoint; }
	bool isSpot() const { return m_type == kSpot; }

	// get and set attributes
	bool isCastShadowMap() const { return m_castShadowMap; }
	void setCastShadowMap(bool val) { m_castShadowMap = val; }

	void setShadowMapSize(int val) { m_preferShadowMapSize = val; }

	float radius() const { return m_radius; }
	void setRadius(float val) { m_radius = val; }

	float spotAngle() const { return m_spotAngle; }
	void setSpotAngle(float val) { m_spotAngle = val; }

	Color3 lightColor() const { return m_color; }
	void setLightColor(const Color3 &color, float intensity = 1.0f, float specularX = 1.0f);
	Color3 skyColor() const { return m_skyColor; }
	void setSkyColor(const Color3 &color, float skyIntensity = 1.0f);
	void setEnvColor(const Color3 &color, float envIntensity = 1.0f);
	Vector3 lightDirection() const { return m_affineMat.origin.getNormalized(); }

	// shadow
	bool checkShadow(RenderScene *qscene);

	void freeShadowMap();
	bool genShadowMap(RenderScene *qscene);

	int getShadowMemoryUsed() const;

	// implement Actor
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual void issueToScene(RenderScene *qscene);

protected:
	void initShadowGenerator();
	void clearShadowGenerator();

	// for light buffer drawing
	void prepareLightBuffer(RenderScene *scene);
	void prepareLightBuffer_Global(RenderScene *scene);
	void prepareLightBuffer_Point(RenderScene *scene);
	void prepareLightBuffer_Spot(RenderScene *scene);

private:
	class ShadowGenerator;

	Type m_type;
	bool m_castShadowMap;
	float m_radius;
	float m_spotAngle;
	Color3 m_color;
	float m_hdrStops;
	Color3 m_skyColor;
	Color3 m_envColor;

	int m_preferShadowMapSize;

	// shadow info
	ShadowGenerator *m_shadowGen;
	IntrusiveLink<RenderLight> m_shadowLink;
	int m_shadowMemoryUsed;

	// runtime
	VolumeVertexes m_lightVolume;
	// if not intersect neap clip plane, we can use volume's front face to draw light buffer
	bool m_isIntersectsNearPlane;
	Matrix4 m_projMatrix;
	bool m_isShadowed;
};

AX_END_NAMESPACE

#endif // AX_RENDER_LIGHT_H
