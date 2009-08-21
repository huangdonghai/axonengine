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
#if 0
		void beginTransform();
		void doTransform(const AffineMat& mat, bool local);
		void setOrigin(int index, float f);
		void setRotate(int index, float f);
		Action* endTransform();
#endif
		void writeXml(File* f, int indent=0) const;
		void readXml(const TiXmlElement* node);

		// base function
		GameNode* getGameNode() const { return m_gameNode; }
#if 0
		void setHovering(bool ishovering) { m_isHovering = ishovering; }
		void setSelected(bool is_selected) { m_isSelected = is_selected; }
		void setDeleted(bool deleted); // modify 9.16
		bool isSelected() const { return m_isSelected; }
		bool isDeleted() const { return m_isDeleted; }
		int getId() const { return m_id; }
		void setId(int newid);
		Vector3 getOrigin() const { return m_gameNode->getOrigin_p(); }
		void setOrigin(const Vector3& pos) { m_gameNode->setOrigin_p(pos); m_actorDirty = true; }
		const Matrix3& getAxis() const { return m_gameNode->getAxis_p(); }
		void setAxis(const Matrix3& axis) { m_gameNode->setAxis_p(axis); }
#endif

		static const char* typeToString(Type t);
		static Type stringToType(const char* str);

	protected:
		GameNode* m_gameNode;
#if 0
		int m_id;
		bool m_isHovering : 1;
		bool m_isSelected : 1;
		bool m_isDeleted : 1;
		bool m_isInGame : 1;
		AffineMat m_oldMatrix;
		AffineMat m_oldmatrixNoScale;
		float m_oldscale;
		bool m_actorDirty;
#endif

	private:
		Render::Line* m_bboxLine;
	};

#if 0
	//--------------------------------------------------------------------------
	// class ActorList
	//--------------------------------------------------------------------------

	class AX_API ActorList : public List<MapActor*> {
	public:
		bool containsOne() const;	// if contains one object, return true
		void beginTransform() const;
		void doTransform(const AffineMat& mat, bool local) const;
		void setOrigin(int index, float f) const;
		void setRotate(int index, float f) const;
		Action* endTransform() const;
		BoundingBox getBoundingBox() const;
		Vector3 getCenter() const;
		void doSelect() const;
		Vector3 getBackOrigin() const;
		const Matrix3& getBackAxis() const;
		Vector3 getFrontOrigin() const;
		const Matrix3& getFrontAxis() const;
		void setNodeProperty(const String& propname, const Variant& value) const;
		void doPropertyChanged() const;
		void setColor(Rgb val) const;

		ActorList clone() const;
		void setSelected(bool selected) const;
		void setDeleted(bool deleted) const;
	};

	inline bool ActorList::containsOne() const {
		if (empty()) return false;
		return front() == back();
	}

	inline void ActorList::beginTransform() const {
		std::for_each(begin(), end(), std::mem_fun(&MapActor::beginTransform));
	}

	inline void ActorList::doTransform(const AffineMat& mat, bool local) const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->doTransform(mat, local);
		}
	}

	inline void ActorList::setOrigin(int index, float f) const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->setOrigin(index, f);
		}
	}

	inline void ActorList::setRotate(int index, float f) const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->setRotate(index, f);
		}
	}

	inline BoundingBox ActorList::getBoundingBox() const {
		BoundingBox result;
		result.clear();

		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			result = result.united((*it)->getBoundingBox());
		}

		return result;
	}

	inline Vector3 ActorList::getCenter() const {
		Vector3 result;
		result.clear();
		int count = 0;

		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			result += (*it)->getBoundingBox().getCenter();
			count++;
		}

		if (count) {
			result /= count;
		}
		return result;
	}

	inline void ActorList::doSelect() const {}

	inline Vector3 ActorList::getBackOrigin() const {
		AX_ASSERT(!empty());
		return back()->getOrigin();
	}

	inline const Matrix3& ActorList::getBackAxis() const {
		AX_ASSERT(!empty());
		return back()->getAxis();
	}

	inline Vector3 ActorList::getFrontOrigin() const {
		AX_ASSERT(!empty());
		return front()->getOrigin();
	}

	inline const Matrix3& ActorList::getFrontAxis() const {
		AX_ASSERT(!empty());
		return front()->getAxis();
	}

	inline void ActorList::doPropertyChanged() const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->getGameNode()->doPropertyChanged();
		}
	}

	inline ActorList ActorList::clone() const {
		ActorList result;

		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			result.push_back((*it)->clone());
		}

		return result;
	}

	inline void ActorList::setSelected(bool selected) const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->setSelected(selected);
		}
	}

	inline void ActorList::setDeleted(bool deleted) const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->setSelected(deleted);
		}
	}

	inline void ActorList::setColor(Rgb val) const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->setColor(val);
		}
	}
#endif

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

