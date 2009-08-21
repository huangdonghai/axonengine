/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_FOG_H
#define AX_RENDER_FOG_H

namespace Axon{ namespace Render {

	struct QueuedFog;

	class Fog : public Entity {
	public:
#if 0
		enum Type {
			kNone, kLiner, kExp, kExp2, kHeight, kVolume
		};
#endif
		Fog() : Entity(Entity::kFog) {}
		~Fog() {}

#if 0
		Fog::Type getFogType() const { return m_fogType; }
		void setFogType(Fog::Type val) { m_fogType = val; }
#endif
		Vector4 getFogColor() const { return m_fogColor; }
		void setFogColor(Vector4 val) { m_fogColor = val; }
		float getFogDensity() const { return m_fogDensity; }
		void setFogDensity(float val) { m_fogDensity = val; }

		void fillQueuedFog(QueuedFog* queued);
		QueuedFog* getQueuedFog() const { return m_queuedFog;}

		// implement fog
		virtual BoundingBox getLocalBoundingBox();
		virtual BoundingBox getBoundingBox();
		virtual Entity::Kind getType() const { return Entity::kFog; }
		virtual Primitives getAllPrimitives() { return Primitives(); }


	private:
#if 0
		Fog::Type m_fogType;
#endif
		Vector4 m_fogColor;
		float m_fogDensity;
		QueuedFog* m_queuedFog;
	};


	struct QueuedFog {
#if 0
		Fog::Type m_fogType;
#endif
		Vector4 m_fogParams;
	};

	inline void Fog::fillQueuedFog(QueuedFog* queued) {
		m_queuedFog = queued;
#if 0
		m_queuedFog->m_fogType = m_fogType;
#endif
		m_queuedFog->m_fogParams = m_fogColor;
		m_queuedFog->m_fogParams.w = m_fogDensity;
	}

}} // namespace Axon::Render

#endif // AX_RENDER_FOG_H

