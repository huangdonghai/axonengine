/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_EDITOR_MAP_ENTITY_H
#define AX_EDITOR_MAP_ENTITY_H

namespace Axon { namespace Editor { namespace MapEdit {

	//--------------------------------------------------------------------------
	// class Entity, Editor Entity
	//--------------------------------------------------------------------------

	class AX_API Entity : public MapActor {
	public:
		Entity();
		Entity(const String& type);
		virtual ~Entity();

		// implement Actor
		virtual void doRender();
		virtual Type getType() const { return kEntity; }
		virtual MapActor* clone() const;

		GameActor* getGameEntity() const { return m_gameEntity; }

	protected:
		GameActor* m_gameEntity;
		RenderMesh* m_iconPrim;
	};

}}} // namespace Axon::Editor::MapEdit

#endif // AX_EDITOR_ENTITY_H
