/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_ACTOR_H
#define AX_EDITOR_ACTOR_H

namespace Axon { namespace Editor {

	class AX_API Actor {
	public:
		Actor(Context* ctx);
		virtual ~Actor();

		enum Flag {
			Selected = 1, Hovering = 2, Hided = 4, Deleted = 8, Locked = 0x10
		};

		virtual Actor* clone() const = 0;
		virtual void doDeleteFlagChanged(bool del) = 0;
		virtual void doRender() = 0;

		virtual void setMatrix(const AffineMat& matrix) = 0;
		virtual const AffineMat& getMatrix() const = 0;

		virtual BoundingBox getBoundingBox() = 0;

		virtual Variant getProperty(const String& propname) = 0;
		virtual void setProperty(const String& propname, const Variant& value) = 0;
		virtual void doPropertyChanged() = 0;

		virtual Rgb getColor() const = 0;
		virtual void setColor(Rgb val) = 0;

		void beginTransform();
		void doTransform(const AffineMat& mat, bool local);
		void setOrigin(int index, float f);
		void setRotate(int index, float f);
		Action* endTransform();

		void setHovering(bool ishovering) { m_isHovering = ishovering; }
		void setSelected(bool is_selected) { m_isSelected = is_selected; }
		void setDeleted(bool deleted);
		bool isSelected() const { return m_isSelected; }
		bool isDeleted() const { return m_isDeleted; }
		int getId() const { return m_id; }
		void setId(int newid);
		const Vector3& getOrigin() const { return getMatrix().origin; }
		void setOrigin(const Vector3& pos);
		const Matrix3& getAxis() const { return getMatrix().axis; }
		void setAxis(const Matrix3& axis);
		Context* getContext() const { return m_context; }
		void setContext(Context* val) { m_context = val; }

	protected:
		Context* m_context;
		int m_id;
		bool m_isHovering : 1;
		bool m_isSelected : 1;
		bool m_isDeleted : 1;
		bool m_isInGame : 1;
		AffineMat m_oldMatrix;
		AffineMat m_oldmatrixNoScale;
		float m_oldscale;
		bool m_actorDirty;
	};

	//--------------------------------------------------------------------------
	// class ActorList
	//--------------------------------------------------------------------------

	class AX_API ActorList : public List<Actor*> {
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
		std::for_each(begin(), end(), std::mem_fun(&Actor::beginTransform));
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
		return back()->getMatrix().origin;
	}

	inline const Matrix3& ActorList::getBackAxis() const {
		AX_ASSERT(!empty());
		return back()->getMatrix().axis;
	}

	inline Vector3 ActorList::getFrontOrigin() const {
		AX_ASSERT(!empty());
		return front()->getMatrix().origin;
	}

	inline const Matrix3& ActorList::getFrontAxis() const {
		AX_ASSERT(!empty());
		return front()->getMatrix().axis;
	}

	inline void ActorList::doPropertyChanged() const {
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			(*it)->doPropertyChanged();
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

}} // namespace Axon::Editor

#endif

