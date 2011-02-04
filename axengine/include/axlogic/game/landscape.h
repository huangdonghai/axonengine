/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_GAME_HULL_H
#define AX_GAME_HULL_H

AX_BEGIN_NAMESPACE

class GameWorld;

//--------------------------------------------------------------------------

class AX_API Fixed : public GameObject
{
	AX_DECLARE_CLASS(Fixed, GameObject)
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
	Landscape *m_landscape;
};

//--------------------------------------------------------------------------

class AX_API StaticFixed : public Fixed
{
	AX_DECLARE_CLASS(StaticFixed, Fixed)
		AX_PROP(model)
	AX_END_CLASS()

	friend class Landscape;

public:
	StaticFixed();
	virtual ~StaticFixed();

	std::string get_model() const { return m_modelName; }
	void set_model(const std::string &val);

protected:
	// implement GameObject
	virtual void onReload();
	virtual void onReset();

protected:
	std::string m_modelName;
	HavokModel *m_model;
	PhysicsRigid *m_rigid;
};

//--------------------------------------------------------------------------

class AX_API TerrainFixed : public Fixed, public IObserver
{
	AX_DECLARE_CLASS(TerrainFixed, Fixed)
	AX_END_CLASS()

public:
	TerrainFixed() {}
	TerrainFixed(RenderTerrain *terr);
	virtual ~TerrainFixed();

	// implement Fixed
	virtual void onReload();
	virtual void onReset();

	// implement IObserver
	virtual void beNotified(IObservable *subject, int arg);

private:
	RenderTerrain *m_renderTerrain;
	PhysicsTerrain *m_physicsTerrain;
};

//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
class AX_API TreeFixed : public Fixed
{
	AX_DECLARE_CLASS(TreeFixed, Fixed)
		AX_PROP(tree)
	AX_END_CLASS()

public:
	TreeFixed();
	virtual ~TreeFixed();

	// implement GameObject
	virtual void onReload();
	virtual void onReset();

	std::string get_tree() const { return m_treeName; }
	void set_tree(const std::string &val);

protected:
	std::string m_treeName;
	RenderTree *m_renderTree;
};
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

//--------------------------------------------------------------------------

class AX_API Landscape : public GameActor
{
	AX_DECLARE_CLASS(Landscape, GameActor)
	AX_END_CLASS()

public:
	enum { MaxFixed = 8192 };

	Landscape() {}
	Landscape(GameWorld *world);
	virtual ~Landscape();

	void addFixed(Fixed *fixed);
	void removeFixed(Fixed *fixed);

	Fixed *getFixed(int num) const;

	GameWorld *getGameWorld() const { return m_world; }

	void buildKdTree();

private:
	Fixed *m_fixeds[MaxFixed];
};

inline Fixed *Landscape::getFixed(int num) const
{
	AX_ASSERT(num >= 0 && num < MaxFixed);
	return m_fixeds[num];
}

AX_END_NAMESPACE

#endif // end guardian

