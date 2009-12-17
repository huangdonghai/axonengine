/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GFX_OBJECT_H
#define AX_GFX_OBJECT_H

namespace Axon { namespace Gfx {

	class GfxEntity;

	class GfxObject : public Object, public ObjAnimatable
	{
	public:
		// script
		AX_DECLARE_CLASS(GfxObject, Object, "GfxObject")
		AX_END_CLASS()

		enum GfxType {
			kVirtualBase,
			kParticleEmitter,
			kRibbonEmitter,
			kSound,
			kModel
		};

		GfxObject();
		virtual ~GfxObject();

		virtual Type getGfxType() const { return kVirtualBase; }
		virtual void update() {}
		virtual void render() {}

	private:
		GfxEntity* m_entity;

		VectorTrack m_pos;
		VectorTrack m_rotations;
		FloatTrack m_scale;
	};

}} // namespace Axon::Gfx

#endif // AX_GFX_OBJECT_H
