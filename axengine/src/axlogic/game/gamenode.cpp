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
	// class GameNode
	//--------------------------------------------------------------------------

	GameNode::GameNode() {
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

	GameNode::~GameNode() {
	}

	BoundingBox GameNode::getLocalBoundingBox() {
		if (!m_renderEntity) {
			return BoundingBox::UnitBox;
		}

		return m_renderEntity->getLocalBoundingBox();
	}

	BoundingBox GameNode::getBoundingBox() {
		if (!m_renderEntity) {
			return getLocalBoundingBox().getTransformed(m_matrix_p);
		}

		return m_renderEntity->getBoundingBox();
	}

	Vector3 GameNode::getOrigin_p() const {
		return m_matrix_p.origin;
	}

	void GameNode::setOrigin_p(const Vector3& pos) {
		m_matrix_p.origin = (pos);
		m_matrixDirty = true;
		onMatrixChanged();
	}

	const Matrix3& GameNode::getAxis_p() const {
		return m_matrix_p.axis;
	}

	void GameNode::setAxis_p(const Matrix3& axis) {
		m_matrix_p.axis = (axis);
		m_matrixDirty = true;
		onMatrixChanged();
	}

	void GameNode::setMatrix_p(const AffineMat& matrix) {
		m_matrix_p = matrix;
		m_matrixDirty = true;
		onMatrixChanged();
	}

	const AffineMat& GameNode::getMatrix_p() const {
		return m_matrix_p;
	}

	void GameNode::setInstanceColor( const Vector3& color )
	{
		m_instanceColor_p = color;
		onMatrixChanged();
	}


	void GameNode::doSelectTest() const {
		if (!m_renderEntity) {
			return;
		}

		if (!m_renderEntity->isVisable())
			return;

		g_renderSystem->testActor(m_renderEntity);
	}

	void GameNode::doDebugRender() const
	{}

	void GameNode::writeXml(File* f, int indent/*=0 */) {
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

	void GameNode::readXml(const TiXmlElement* node) {
		const TiXmlAttribute* attr = node->FirstAttribute();

		for (; attr; attr = attr->Next()) {
			this->setProperty(attr->Name(), attr->Value());
		}
	}

	void GameNode::doSpawn()
	{
		if (m_spawned) {
			Errorf("already spawned");
		}

		m_spawned = true;

		reload();
	}

	void GameNode::doRemove()
	{
		if (!m_spawned) {
			Errorf("not even spawned");
		}

		clear();
		m_spawned = false;
	}

	void GameNode::onPropertyChanged()
	{
		if (!m_spawned)
			return;

		reload();
	}

	void GameNode::onMatrixChanged()
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

