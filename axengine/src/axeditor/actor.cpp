/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

namespace Axon { namespace Editor {

	void Actor::beginTransform()
	{
		m_oldMatrix = m_oldmatrixNoScale = getMatrix();
		m_oldscale = m_oldmatrixNoScale.axis.removeScale();
	}

	void Actor::doTransform(const AffineMat& mat, bool local)
	{
		AffineMat newmatrix;
		if (local) {
			newmatrix = m_oldmatrixNoScale * mat;
		} else {
			newmatrix = mat * m_oldmatrixNoScale;
		}

		newmatrix.axis *= m_oldscale;

		setMatrix(newmatrix );

		if (_isnan(newmatrix.origin.x)) {
			Errorf("NaN");
		}
	}

	Action* Actor::endTransform()
	{
		TransformHis* his = new TransformHis(m_context, "Transform", m_id, m_oldMatrix, getMatrix());
		return his;
	}

	void Actor::setOrigin(int index, float f)
	{
		AX_ASSERT(index >= 0 && index < 3);
#if 0
		Vector3 org = m_gameNode->getOrigin_p();
		org[index] = f;
		m_gameNode->setOrigin_p(org);
#else
		AffineMat matrix = getMatrix();
		matrix.origin[index] = f;
		setMatrix(matrix);
#endif
	}

	void Actor::setOrigin( const Vector3& pos )
	{
		AffineMat mat = getMatrix();
		mat.origin = pos;
		setMatrix(mat);
	}

	void Actor::setRotate(int index, float f)
	{
		AX_ASSERT(index >= 0 && index < 3);
#if 0
		Matrix3 axis = m_gameNode->getAxis_p();
		Matrix3 ortho = axis;
		ortho.removeShear();
		Rotate rotate;

		rotate.fromAxis(ortho);
		rotate[index] = f;

		ortho = rotate.toAxis();

		m_gameNode->setAxis_p(ortho);
#else
		AffineMat matrix = getMatrix();
		Matrix3 ortho = matrix.axis;
		ortho.removeShear();
		Rotate rotate;

		rotate.fromAxis(ortho);
		rotate[index] = f;

		ortho = rotate.toAxis();
		matrix.axis = ortho;

		setMatrix(matrix);
#endif
	}

	void Actor::setDeleted(bool deleted)
	{
		if (!m_isDeleted && deleted)
			doDeleteFlagChanged(true);
		else if (m_isDeleted && !deleted)
			doDeleteFlagChanged(false);

		m_isDeleted = deleted;
		m_actorDirty = true;
	}

	void Actor::setId(int newid)
	{
		m_context->removeActor(this);
		m_id = newid;
		m_context->addActor(this);
	}

	void Actor::setAxis( const Matrix3& axis )
	{
		AffineMat mat = getMatrix();
		mat.axis = axis;
		setMatrix(mat);
	}

	Actor::Actor(Context* ctx)
	{
		m_context = ctx;
		m_isHovering = false;
		m_isSelected = false;
		m_isDeleted = false;
		m_isInGame = false;

		m_id = m_context->generateActorId();
		m_context->addActor(this);
		m_actorDirty = true;
	}

	Actor::~Actor()
	{
		m_context->removeActor(this);
	}

	//--------------------------------------------------------------------------
	// class ActorList
	//--------------------------------------------------------------------------

	Action* ActorList::endTransform() const {
		if (empty()) return nullptr;

		Context* ctx = front()->getContext();
		GroupHis* group = new GroupHis(ctx, "Transform");

		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			Action* his = (*it)->endTransform();
			if (his) group->append(his);
		}

		return group;
	}

	void ActorList::setNodeProperty(const String& propname, const Variant& value) const
	{
		if (empty()) return;

		Context* ctx = front()->getContext();
		GroupHis* group = nullptr;
		if (!containsOne()) {
			group = new GroupHis(ctx, "Edit Property");
		}

		History* his = nullptr;
		ActorList::const_iterator it = begin();
		for (; it != end(); ++it) {
			Actor* actor = *it;

			Variant oldvalue = actor->getProperty(propname.c_str());
			his = new PropertyEditHis(ctx, actor,propname,oldvalue,value);
			if (his && group) group->append(his);

			actor->setProperty(propname,value);
		}

		if (!containsOne()) {
			ctx->addHistory(group);
		} else {
			ctx->addHistory(his);
		}
	}

}} // namespace Axon::Editor

