/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "d3d9private.h"

AX_BEGIN_NAMESPACE


D3D9Query::D3D9Query()
{
	m_queryPos = 0;
	m_readPos = 0;
	m_quering = false;

	HRESULT hr;
	for (int i = 0; i < NUMBER; i++) {
		V(dx9_device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_queries[i]));
	}
}

D3D9Query::~D3D9Query()
{
	D3D9_SCOPELOCK;

	for (int i = 0; i < NUMBER; i++) {
		SAFE_RELEASE(m_queries[i]);
	}
}

bool D3D9Query::canQuery() const
{
	if (m_queryPos - m_readPos > NUMBER - 1)
		return false;
	else
		return true;
}

bool D3D9Query::beginQuery()
{
	AX_ASSERT(!m_quering);

	if (m_queryPos - m_readPos > NUMBER - 1)
		return false;

	IDirect3DQuery9 *object = m_queries[m_queryPos%NUMBER];
	object->Issue(D3DISSUE_BEGIN);
	m_quering = true;
	return true;
}

void D3D9Query::endQuery()
{
	AX_ASSERT(m_quering);
	IDirect3DQuery9 *object = m_queries[m_queryPos%NUMBER];
	object->Issue(D3DISSUE_END);
	m_quering = false;

	m_queryPos++;
}

int D3D9Query::getResult()
{
	if (m_readPos >= m_queryPos)
		return -1;

	IDirect3DQuery9 *object = m_queries[m_readPos%NUMBER];
	DWORD numberOfPixelsDrawn;
	HRESULT hr = object->GetData(&numberOfPixelsDrawn, sizeof(numberOfPixelsDrawn), 0);

	if (hr == S_OK) {
		m_readPos++;
		return numberOfPixelsDrawn;
	}

	return -1;
}

void D3D9Query::reset()
{
	m_queryPos = 0;
	m_readPos = 0;
	m_quering = false;
}


D3D9querymanager::D3D9querymanager()
{

}

D3D9querymanager::~D3D9querymanager()
{

}

Query *D3D9querymanager::allocQuery()
{
	D3D9_SCOPELOCK;

	if (m_freeQueries.empty()) {
		D3D9Query *query = new D3D9Query();
		m_queries.push_back(query);
		int handle = (int)m_queries.size() - 1;
		query->m_id = handle;
		return query;
	}

	int handle = m_freeQueries.front();
	m_freeQueries.pop_front();
	m_queries[handle]->reset();
	return m_queries[handle];
}

void D3D9querymanager::freeQuery( Query*& query )
{
	D3D9_SCOPELOCK;

	D3D9Query *d3d9query = (D3D9Query*)query;
	m_freeQueries.push_back(d3d9query->m_id);
}

void D3D9querymanager::issueQuery( Query *query, int frameId, const BoundingBox &bbox )
{
	D3D9Query *d3d9query = static_cast<D3D9Query*>(query);
	if (!d3d9query->canQuery())
		return;

	ActiveQuery *active = m_issuedQueryAlloc.alloc();
	query->m_queryFrame = frameId;
	active->frameTarget = g_renderSystem->getFrameTarget();
	active->query = d3d9query;
	active->frameId = frameId;
	active->bbox = bbox;
	active->issued = false;

	m_frameIssuedQuery[query->getType()].push_back(active);
}

void D3D9querymanager::syncFrame()
{
	for (int i = 0; i < Query::QueryType_Number; i++) {
		List<ActiveQuery*>::iterator it = m_activeQuery[i].begin();

		while (it != m_activeQuery[i].end()) {
			ActiveQuery *issued = *it;
			D3D9Query *query = static_cast<D3D9Query*>(issued->query);

			int result = query->getResult();

			if (result < 0) {
				++it;
				continue;
			}

			issued->query->m_resultFrame = issued->frameId;
			issued->query->m_result = result;

			it = m_activeQuery[i].erase(it);

			m_issuedQueryAlloc.free(issued);
		}

		// append frame issued
		m_activeQuery[i].insert(m_activeQuery[i].end(), m_frameIssuedQuery[i].begin(), m_frameIssuedQuery[i].end());
		m_frameIssuedQuery[i].clear();
	}
}

const List<D3D9querymanager::ActiveQuery*>& D3D9querymanager::getActiveQuery( Query::QueryType type ) const
{
	return m_activeQuery[type];
}

AX_END_NAMESPACE

