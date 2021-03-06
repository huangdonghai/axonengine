/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GFXEDIT_VIEW_H
#define AX_GFXEDIT_VIEW_H

namespace Axon { namespace Editor { namespace GfxEdit {

	class GfxView : public View {
	public:
		GfxView(GfxContext* ctx);
		virtual ~GfxView();

	protected:
		virtual void doRender();

	private:
	};

}}} // namespace Axon::Editor::GfxEdit

#endif
