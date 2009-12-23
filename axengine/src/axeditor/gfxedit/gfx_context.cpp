/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "gfx_local.h"

AX_BEGIN_NAMESPACE

	template< class T >
	class GfxToolFactory_ : public ToolFactory {
		virtual Tool* create(Context* context) {
			return new T(static_cast<GfxContext*>(context));
		}
	};


	GfxContext::GfxContext()
	{
		State* state = new State();
		setState(state);

		m_toolFactories[Tool::Select]		= new ToolFactory_<SelectTool>;
		m_toolFactories[Tool::Move]			= new ToolFactory_<MoveTool>;
		m_toolFactories[Tool::Rotate]		= new ToolFactory_<RotateTool>;
		m_toolFactories[Tool::Scale]		= new ToolFactory_<ScaleTool>;

		m_actionFactories[Action::Delete]	= new ActionFactory_<DeleteAction>;
		m_actionFactories[Action::Undo]		= new ActionFactory_<UndoAction>;
		m_actionFactories[Action::Redo]		= new ActionFactory_<RedoAction>;
		m_actionFactories[Action::Clone]	= new ActionFactory_<CloneAction>;

		m_view = new GfxView(this);

		m_indexedViews[0] = m_view;

		m_numViews = 1;

		m_activeView = m_view;
	}

	GfxContext::~GfxContext()
	{

	}

	void GfxContext::doRender( const RenderCamera& camera, bool world /*= false*/ )
	{

	}

	void GfxContext::doSelect( const RenderCamera& camera, int part )
	{

	}

AX_END_NAMESPACE
