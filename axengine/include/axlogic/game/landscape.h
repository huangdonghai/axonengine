/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_HULL_H
#define AX_GAME_HULL_H

namespace Axon { namespace Game {

	class World;

	//--------------------------------------------------------------------------
	// class Fixed
	//--------------------------------------------------------------------------

	class AX_API Fixed : public Node {
	public:
		AX_DECLARE_CLASS(Fixed, Node, "Game.Fixed")
		AX_END_CLASS()

		friend class Landscape;

		Fixed();
		virtual ~Fixed();

		virtual bool isFixed() const { return true; }

	protected:

	protected:
		enum {
			InvalidNum = -1
		};
		int m_num;
		Landscape* m_landscape;
	};

	//--------------------------------------------------------------------------
	// class StaticFixed
	//--------------------------------------------------------------------------

	class AX_API StaticFixed : public Fixed {
	public:
		AX_DECLARE_CLASS(StaticFixed, Fixed, "Game.StaticFixed")
			AX_PROP(model)
		AX_END_CLASS()

		StaticFixed();
		virtual ~StaticFixed();

		String get_model() const { return m_modelName; }
		void set_model(const String& val);

	protected:
		// implement Node
		virtual void reload();
		virtual void clear();

	protected:
		String m_modelName;
		PhysicsModel* m_model;
		physicsRigid* m_rigid;
	};

	//--------------------------------------------------------------------------
	// class TerrainFixed
	//--------------------------------------------------------------------------

	class AX_API TerrainFixed : public Fixed, public IObserver {
	public:
		AX_DECLARE_CLASS(TerrainFixed, Fixed, "Game.TerrainFixed")
		AX_END_CLASS()

		TerrainFixed() {}
		TerrainFixed(Render::Terrain* terr);
		virtual ~TerrainFixed();

		// implement Fixed
		virtual void reload();
		virtual void clear();

		// implement IObserver
		virtual void doNotify(IObservable* subject, int arg);

	private:
		Render::Terrain* m_renderTerrain;
		Physics::Terrain* m_physicsTerrain;
	};

	//--------------------------------------------------------------------------
	// class TreeFixed
	//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	class AX_API TreeFixed : public Fixed {
	public:
		AX_DECLARE_CLASS(TreeFixed, Fixed, "Game.TreeFixed")
			AX_PROP(tree)
		AX_END_CLASS()

		TreeFixed();
		virtual ~TreeFixed();

		// implement Node
		virtual void reload();
		virtual void clear();

		String get_tree() const { return m_treeName; }
		void set_tree(const String& val);

	protected:
		String m_treeName;
		RenderTree* m_renderTree;
	};
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

	//--------------------------------------------------------------------------
	// class Landscape
	//--------------------------------------------------------------------------

	class AX_API Landscape : public Entity {
	public:
		AX_DECLARE_CLASS(Landscape, Entity, "Game.Landscape")
		AX_END_CLASS()

		enum { MaxFixed = 1024 };

		Landscape() {}
		Landscape(World* world);
		virtual ~Landscape();

		void addFixed(Fixed* fixed);
		void removeFixed(Fixed* fixed);

		Fixed*              getFixed(int num) const;

		World* getGameWorld() const { return m_world; }

	private:
		Fixed* m_fixeds[MaxFixed];
		FreeList<Fixed*>	m_freeList;
	};

	inline Fixed* Landscape::getFixed(int num) const{
		AX_ASSERT(num >= 0 && num < MaxFixed);
		return m_fixeds[num];
	}

}}

#endif // end guardian

