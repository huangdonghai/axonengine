/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_EDITOR_MAP_ACTOR_H
#define AX_EDITOR_MAP_ACTOR_H

AX_BEGIN_NAMESPACE

class MapContext;

//--------------------------------------------------------------------------
// class Agent
//--------------------------------------------------------------------------

class AX_API MapAgent : public Agent {
public:
	enum Type {
		kNone, kStatic, kSpeedTree, kBrush, kEntity
	};

	enum Flag {
		Selected = 1, Hovering = 2, Hided = 4, Deleted = 8, Locked = 0x10
	};

	MapAgent();
	virtual ~MapAgent();

	// implement Editor::Agent
	virtual MapAgent *clone() const;
	virtual void doDeleteFlagChanged(bool del);
	virtual void doRender();

	virtual void setMatrix(const AffineMat &matrix);
	virtual const AffineMat &getMatrix() const { return m_gameObj->getMatrix_p(); }

	virtual BoundingBox getBoundingBox() { return m_gameObj->getBoundingBox(); }

	// properties
	virtual Variant getProperty(const String &propname);
	virtual void setProperty(const String &propname, const Variant &value);
	virtual void doPropertyChanged();

	virtual Rgb getColor() const;
	virtual void setColor(Rgb val);

	// MapAgent interface
	virtual Type getType() const { return kNone; }

	MapContext *getMapContext() const { return (MapContext*)(m_context); }

	void bindToGame();
	void unbindToGame();
	void doHitTest();

	void writeXml(File *f, int indent=0) const;
	void readXml(const TiXmlElement *node);

	// base function
	GameObject *getGameObject() const { return m_gameObj; }

	static const char *typeToString(Type t);
	static Type stringToType(const char *str);

protected:
	GameObject *m_gameObj;

private:
	LinePrim *m_bboxLine;
};

//--------------------------------------------------------------------------
// class MapStatic
//--------------------------------------------------------------------------

class AX_API MapStatic : public MapAgent {
public:
	MapStatic();
	MapStatic(const String &nametemplate);
	virtual ~MapStatic();

	// implement editor actor
	virtual void doRender();
	virtual MapAgent *clone() const;
	virtual Type getType() const { return kStatic; }

private:
	// render
	StaticFixed *m_gameFixed;
};

//--------------------------------------------------------------------------
// class MapSpeedTree
//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
class AX_API MapSpeedTree : public MapAgent {
public:
	MapSpeedTree();
	MapSpeedTree(const String &nametemplate);
	virtual ~MapSpeedTree();

	// implement editor actor
	virtual void doRender();
	virtual MapAgent *clone() const;
	virtual Type getType() const { return kSpeedTree; }

private:
	// render
	TreeFixed *m_gameFixed;
};
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

AX_END_NAMESPACE

#endif // end guardian

