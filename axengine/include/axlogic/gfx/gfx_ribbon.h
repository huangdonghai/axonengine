/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GFX_RIBBON_H
#define AX_GFX_RIBBON_H

namespace Axon { namespace Gfx {

	struct RibbonSegment
	{
		Vector3 pos, up, back;
		float len,len0;
	};

	class RibbonEmitter : public GfxObject
	{
		AX_DECLARE_CLASS(RibbonEmitter, GfxObject)
			AX_SIMPLEPROP(color)
			AX_SIMPLEPROP(opacity)
			AX_SIMPLEPROP(above)
			AX_SIMPLEPROP(below)
		AX_END_CLASS()

	public:
		RibbonEmitter();
		virtual ~RibbonEmitter();

		// implement GfxObject
		virtual GfxType getGfxType() const { return kRibbonEmitter; }
		virtual void update();
		virtual void render();

	private:
		// BEGIN ANIMATABLE PROPERTIES
		Rgb m_color;
		float m_opacity;
		float m_above, m_below;
		// END ANIMATABLE PROPERTIES

		float f1, f2;

		Vector3 pos;

		int manim, mtime;
		float length, seglen;
		int numsegs;

		Vector3 tpos;
		Vector4 tcolor;
		float tabove, tbelow;

		List<RibbonSegment> segs;
	};

}} // namespace Axon::Gfx

#endif // AX_GFX_RIBBON_H
