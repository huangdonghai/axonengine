/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_EDITOR_MAP_ACTOR_H
#define AX_EDITOR_MAP_ACTOR_H

namespace Axon { namespace Editor { namespace MapEdit {

	class MapContext;

	//--------------------------------------------------------------------------
	// class Actor
	//--------------------------------------------------------------------------

	class AX_API MapActor : public Editor::Actor {
	public:
		enum Type {
			kNone, kStatic, kSpeedTree, kBrush, kEntity
		};

		enum Flag {
			Selected = 1, Hovering = 2, Hided = 4, Deleted = 8, Locked = 0x10
		};

		MapActor();
		virtual ~MapActor();

		// implement Editor::Actor
		virtual MapActor* clone() const;
		virtual void doDeleteFlagChanged(bool del);
		virtual void doRender();

		virtual void setMatrix(const AffineMat& matrix);
		virtual const AffineMat& getMatrix() const { return m_gameNode->getMatrix_p(); }

		virtual BoundingBox getBoundingBox() { return m_gameNode->getBoundingBox(); }

		// properties
		virtual Variant getProperty(const String& propname);
		virtual void setProperty(const String& propname, const Variant& value);
		virtual void doPropertyChanged();

		virtual Rgb getColor() const;
		virtual void setColor(Rgb val);

		// MapActor interface
		virtual Type getType() const { return kNone; }

		MapContext* getMapContext() const { return (MapContext*)(m_context); }

		void bindToGame();
		void unbindToGame();
		void doSelect();

		void writeXml(File* f, int indent=0) const;
		void readXml(const TiXmlElement* node);

		// base function
		GameNode* getGameNode() const { return m_gameNode; }

		static const char* typeToString(Type t);
		static Type stringToType(const char* str);

	protected:
		GameNode* m_gameNode;

	private:
		Render::Line* m_bboxLine;
	};

	//--------------------------------------------------------------------------
	// class Static
	//--------------------------------------------------------------------------

	class AX_API Static : public MapActor {
	public:
		Static();
		Static(const String& nametemplate);
		virtual ~Static();

		// implement editor actor
		virtual void doRender();
		virtual MapActor* clone() const;
		virtual Type getType() const { return kStatic; }

	private:
		// render
		Game::StaticFixed* m_gameFixed;
	};

	//--------------------------------------------------------------------------
	// class SpeedTree
	//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	class AX_API SpeedTree : public MapActor {
	public:
		SpeedTree();
		SpeedTree(const String& nametemplate);
		virtual ~SpeedTree();

		// implement editor actor
		virtual void doRender();
		virtual MapActor* clone() const;
		virtual Type getType() const { return kSpeedTree; }

	private:
		// render
		Game::TreeFixed* m_gameFixed;
	};
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

}}} // namespace Axon::Editor::MapEdit

namespace Axon {
	typedef Editor::MapEdit::MapActor MapActor;
}

#endif // end guardian

