/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GFX_ENTITY_H
#define AX_GFX_ENTITY_H

namespace Axon { namespace Gfx {

	using namespace Axon::Render;

	class GfxEntity : public RenderEntity {
	public:
		GfxEntity();
		GfxEntity(const String &name);
		virtual ~GfxEntity();

		// implement RenderEntity
		virtual BoundingBox getLocalBoundingBox();
		virtual BoundingBox getBoundingBox();
		virtual Primitives getAllPrimitives();
		virtual Primitives getSelectionPrims();
		virtual void doUpdate(QueuedScene* qscene);
		virtual void doCalculateLod(QueuedScene* qscene);
		virtual Vector4 getInstanceParam() const;
		virtual void issueToQueue(QueuedScene* qscene);

		// internal use
		void addObject(GfxObject* obj);
		void removeObject(GfxObject* obj);
		void setAnimStartTime(int time);
		void setAnimTime(int time);

	protected:

	private:
		Sequence<GfxObject*> m_objects;
	};

}} // namespace Axon::Gfx

#endif // AX_GFX_ENTITY_H
