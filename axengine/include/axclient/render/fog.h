/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_FOG_H
#define AX_RENDER_FOG_H

AX_BEGIN_NAMESPACE

struct QueuedFog;

class RenderFog : public RenderEntity
{
public:
	RenderFog() : RenderEntity(RenderEntity::kFog) {}
	~RenderFog() {}

	Vector4 getFogColor() const { return m_fogColor; }
	void setFogColor(Vector4 val) { m_fogColor = val; }
	float getFogDensity() const { return m_fogDensity; }
	void setFogDensity(float val) { m_fogDensity = val; }

	void fillQueuedFog(QueuedFog* queued);
	QueuedFog* getQueuedFog() const { return m_queuedFog;}

	// implement fog
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual RenderEntity::Kind getType() const { return RenderEntity::kFog; }


private:
	Vector4 m_fogColor;
	float m_fogDensity;
	QueuedFog* m_queuedFog;
};


struct QueuedFog {
	Vector4 m_fogParams;
};

inline void RenderFog::fillQueuedFog(QueuedFog* queued) {
	m_queuedFog = queued;
	m_queuedFog->m_fogParams = m_fogColor;
	m_queuedFog->m_fogParams.w = m_fogDensity;
}

AX_END_NAMESPACE

#endif // AX_RENDER_FOG_H

