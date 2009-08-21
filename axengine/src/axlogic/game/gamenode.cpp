/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

namespace Axon { namespace Game {

	//--------------------------------------------------------------------------
	// class Node
	//--------------------------------------------------------------------------

	Node::Node() {
		m_outdoorOnly = false;
		m_castShadow = true;
		m_recvShadow = true;
		m_lodRatio = 1.0f;
		m_matrix_p.setIdentity();
		m_instanceColor_p.set(1,1,1);
		m_matrixDirty = true;

		m_spawned = false;
		m_updateFlags.setAll();

		// asset
		m_physicsEntity = nullptr;
		m_renderEntity = nullptr;
		m_soundEntity = nullptr;
	}

	Node::~Node() {
	}

	BoundingBox Node::getLocalBoundingBox() {
		if (!m_renderEntity) {
			return BoundingBox::UnitBox;
		}

		return m_renderEntity->getLocalBoundingBox();
	}

	BoundingBox Node::getBoundingBox() {
		if (!m_renderEntity) {
			return getLocalBoundingBox().getTransformed(m_matrix_p);
		}

		return m_renderEntity->getBoundingBox();
	}

	Vector3 Node::getOrigin_p() const {
		return m_matrix_p.origin;
	}

	void Node::setOrigin_p(const Vector3& pos) {
		m_matrix_p.origin = (pos);
		m_matrixDirty = true;
		onMatrixChanged();
	}

	const Matrix3& Node::getAxis_p() const {
		return m_matrix_p.axis;
	}

	void Node::setAxis_p(const Matrix3& axis) {
		m_matrix_p.axis = (axis);
		m_matrixDirty = true;
		onMatrixChanged();
	}

	void Node::setMatrix_p(const AffineMat& matrix) {
		m_matrix_p = matrix;
		m_matrixDirty = true;
		onMatrixChanged();
	}

	const AffineMat& Node::getMatrix_p() const {
		return m_matrix_p;
	}

	void Node::setInstanceColor( const Vector3& color )
	{
		m_instanceColor_p = color;
		onMatrixChanged();
	}


	void Node::doSelectTest() const {
		if (!m_renderEntity) {
			return;
		}

		if (!m_renderEntity->isVisable())
			return;

		g_renderSystem->testActor(m_renderEntity);
	}

	void Node::doDebugRender() const
	{}

	void Node::writeXml(File* f, int indent/*=0 */) {
		String indstr(indent*2, ' ');
#define INDENT if (indent) f->printf("%s", indstr.c_str());

		INDENT; f->printf("<node className=\"%s\"\n", this->getTypeInfo()->getTypeName());
#if 0
		// write properties
		TypeInfo* typeinfo = getTypeInfo();

		while (typeinfo) {
			const MemberSeq& members = typeinfo->getMembers();

			AX_FOREACH(Member* m, members) {
				if (!m->isProperty()) {
					continue;
				}

				if (m->isConst()) {
					continue;
				}

				INDENT; f->printf("  %s=\"%s\"\n", m->getName(), getProperty(m->getName()).toString().c_str());
			}

			typeinfo = typeinfo->getBaseTypeInfo();
		}
#else
		writeProperties(f, indent);
#endif
		INDENT; f->printf("/>\n");

#undef INDENT
	}

	void Node::readXml(const TiXmlElement* node) {
		const TiXmlAttribute* attr = node->FirstAttribute();

		for (; attr; attr = attr->Next()) {
			this->setProperty(attr->Name(), attr->Value());
		}
	}

	void Node::doSpawn()
	{
		if (m_spawned) {
			Errorf("already spawned");
		}

		m_spawned = true;

		reload();
	}

	void Node::doRemove()
	{
		if (!m_spawned) {
			Errorf("not even spawned");
		}

		clear();
		m_spawned = false;
	}

	void Node::onPropertyChanged()
	{
		if (!m_spawned)
			return;

		reload();
	}

	void Node::onMatrixChanged()
	{
		if (!m_spawned)
			return;

		if (m_renderEntity) {
			m_renderEntity->setMatrix(m_matrix_p);
			m_renderEntity->setInstanceColor(m_instanceColor_p);
			m_renderEntity->updateToWorld();
		}

		if (m_physicsEntity)
			m_physicsEntity->setMatrix(m_matrix_p);

		if (m_soundEntity)
			m_soundEntity->setMatrix(m_matrix_p);
	}

}} // namespace Axon::Game

