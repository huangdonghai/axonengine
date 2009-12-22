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

	class GameWorld;

	//--------------------------------------------------------------------------
	// class Fixed
	//--------------------------------------------------------------------------

	class AX_API Fixed : public GameNode
	{
		AX_DECLARE_CLASS(Fixed, GameNode, "Game.Fixed")
		AX_END_CLASS()

	public:
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

	class AX_API StaticFixed : public Fixed
	{
		AX_DECLARE_CLASS(StaticFixed, Fixed, "Game.StaticFixed")
			AX_PROP(model)
		AX_END_CLASS()
	public:
		StaticFixed();
		virtual ~StaticFixed();

		String get_model() const { return m_modelName; }
		void set_model(const String& val);

	protected:
		// implement GameNode
		virtual void reload();
		virtual void clear();

	protected:
		String m_modelName;
		HavokModel* m_model;
		PhysicsRigid* m_rigid;
	};

	//--------------------------------------------------------------------------
	// class TerrainFixed
	//--------------------------------------------------------------------------

	class AX_API TerrainFixed : public Fixed, public IObserver
	{
		AX_DECLARE_CLASS(TerrainFixed, Fixed, "Game.TerrainFixed")
		AX_END_CLASS()

	public:
		TerrainFixed() {}
		TerrainFixed(RenderTerrain* terr);
		virtual ~TerrainFixed();

		// implement Fixed
		virtual void reload();
		virtual void clear();

		// implement IObserver
		virtual void doNotify(IObservable* subject, int arg);

	private:
		RenderTerrain* m_renderTerrain;
		PhysicsTerrain* m_physicsTerrain;
	};

	//--------------------------------------------------------------------------
	// class TreeFixed
	//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	class AX_API TreeFixed : public Fixed
	{
		AX_DECLARE_CLASS(TreeFixed, Fixed, "Game.TreeFixed")
			AX_PROP(tree)
		AX_END_CLASS()

	public:
		TreeFixed();
		virtual ~TreeFixed();

		// implement GameNode
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

	class AX_API Landscape : public GameEntity
	{
		AX_DECLARE_CLASS(Landscape, GameEntity, "Game.Landscape")
		AX_END_CLASS()

	public:
		enum { MaxFixed = 1024 };

		Landscape() {}
		Landscape(GameWorld* world);
		virtual ~Landscape();

		void addFixed(Fixed* fixed);
		void removeFixed(Fixed* fixed);

		Fixed* getFixed(int num) const;

		GameWorld* getGameWorld() const { return m_world; }

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

