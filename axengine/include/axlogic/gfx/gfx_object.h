/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you
have read the license and understand and accept it fully.
*/



#ifndef AX_GFX_OBJECT_H
#define AX_GFX_OBJECT_H

AX_BEGIN_NAMESPACE

class GfxEntity;

class GfxObject : public Object
{
	AX_DECLARE_CLASS(GfxObject, Object)
		AX_SIMPLEPROP(tm)
	AX_END_CLASS()
public:
	enum GfxType {
		kVirtualBase,
		kParticleEmitter,
		kRibbonEmitter,
		kSound,
		kModel
	};

	GfxObject();
	virtual ~GfxObject();

	virtual GfxType getGfxType() const { return kVirtualBase; }

	const AffineMat &getTm() const { return m_tm; }
	void setTm(const AffineMat &mat) { m_tm = mat; }

	// interface, routed from GfxEntity
	virtual BoundingBox getLocalBoundingBox();
	virtual BoundingBox getBoundingBox();
	virtual Primitives getHitTestPrims();
	virtual void frameUpdate(QueuedScene *qscene);
	virtual void issueToQueue(QueuedScene *qscene);

protected:
	GfxEntity *m_entity;

	// animatable properties
	AffineMat m_tm;
};

AX_END_NAMESPACE

#endif // AX_GFX_OBJECT_H
