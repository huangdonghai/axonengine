/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GFXEDIT_CONTEXT_H
#define AX_GFXEDIT_CONTEXT_H

AX_BEGIN_NAMESPACE

	class GfxContext : public Context {
	public:
		GfxContext();
		virtual ~GfxContext();

		// implement Context
		virtual void doRender(const RenderCamera& camera, bool world = false);
		virtual void doSelect(const RenderCamera& camera, int part);

	private:
		GfxView* m_view;
	};

AX_END_NAMESPACE

#endif
