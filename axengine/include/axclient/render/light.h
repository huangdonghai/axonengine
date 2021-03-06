/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_LIGHT_H
#define AX_RENDER_LIGHT_H

namespace Axon { namespace Render {

	struct QueuedScene;
	struct QueuedLight;

	class AX_API Light : public Entity {
	public:
		friend struct QueuedScene;

		enum Type {
			kInvalid, kGlobal, kPoint, kSpot
		};

		enum Limits {
			MAX_CSM_SPLITS = 4,
			MAX_CUBE_SPLITS = 6,
			MAX_SPLITS = 6,
			NUM_VOLUME_VERTEXES = 8
		};

		Light();
		Light(Type t, const Vector3& pos, Rgb color);
		Light(Type t, const Vector3& pos, Rgb color, float radius);
		virtual ~Light();

		Type getLightType() const { return m_type; }
		void setLightType(Type t) { m_type = t; }
		bool isGlobal() const { return m_type == kGlobal; }
		bool isPoint() const { return m_type == kPoint; }
		bool isSpot() const { return m_type == kSpot; }

		// get and set attributes
		bool getCastShadowMap() const { return m_castShadowMap; }
		void setCastShadowMap(bool val) { m_castShadowMap = val; }

		void setShadowMapSize(int val) { m_preferShadowMapSize = val; }

		float getRadius() const { return m_radius; }
		void setRadius(float val) { m_radius = val; }

		float getSpotAngle() const { return m_spotAngle; }
		void setSpotAngle(float val) { m_spotAngle = val; }

		void setLightColor(Rgb color, float intensity = 1.0f, float specularX = 1.0f);
		Vector4 getLightColor() const { return m_color; }
		void setSkyColor(Rgb color, float skyIntensity = 1.0f);
		void setEnvColor(Rgb color, float envIntensity = 1.0f);
		Vector3 getGlobalLightDirection() const { return m_affineMat.origin.getNormalized(); }

		void fillQueued(QueuedLight* queued);

		// shadow
		bool checkShadow(QueuedScene* qscene);
		void linkShadow();
		Light* unlinkShadow();
		void freeShadowMap();
		bool genShadowMap(QueuedScene* qscene);

		int getShadowMemoryUsed() const;

		// implement Actor
		virtual BoundingBox getLocalBoundingBox();
		virtual BoundingBox getBoundingBox();
		virtual void issueToQueue(QueuedScene* qscene);

	protected:
		void initShadowInfo();
		void clearShadowInfo();

		// for light buffer drawing
		void prepareLightBuffer(QueuedScene* scene);
		void prepareLightBuffer_Global(QueuedScene* scene);
		void prepareLightBuffer_Point(QueuedScene* scene);
		void prepareLightBuffer_Spot(QueuedScene* scene);

	private:
		class ShadowInfo;

		Type m_type;
		bool m_castShadowMap;
		float m_radius;
		float m_spotAngle;
		Vector4 m_color;
		float m_hdrStops;
		Vector3 m_skyColor;
		Vector3 m_envColor;

		int m_preferShadowMapSize;

		// shadow info
		ShadowInfo* m_shadowInfo;
		Link<Light> m_shadowLink;
		int m_shadowMemoryUsed;

		// for shadow map
		QueuedLight* m_queuedLight;
	};

	struct QueuedShadow {
		typedef Vector3 VolumeVertexes[Light::NUM_VOLUME_VERTEXES];
		int numSplitCamera;
		Camera splitCameras[Light::MAX_SPLITS];
		VolumeVertexes splitVolumes[Light::MAX_SPLITS];

		Vector4 splitScaleOffsets[Light::MAX_CSM_SPLITS];
	};

	struct QueuedLight {
		typedef Vector3 VolumeVertexes[Light::NUM_VOLUME_VERTEXES];

		QueuedEntity* queuedActor;
		Light* preQueued;

		// queued struct
		AffineMat matrix;
		Light::Type type;
		float radius;
		Vector4 pos;
		Vector4 color;
		Vector3 skyColor;
		Vector3 envColor;

		//
		// lightbuffer rendering
		//
		VolumeVertexes lightVolume;
		// if not intersect neap clip plane, we can use volume's front face to draw light buffer
		bool isIntersectsNearPlane;
		Matrix4 projMatrix;

		QueuedShadow* shadowInfo;
	};

	typedef Sequence<Light*> LightSeq;

}} // namespace Axon::Render

#endif // AX_RENDER_LIGHT_H
