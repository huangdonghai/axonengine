/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE


Query::Query()
{
	m_queryFrame = 0;
	m_result = 0;
	g_apiWrap->createQuery(m_handle);
}

Query::~Query()
{
	g_apiWrap->deleteQuery(m_handle);
}

void Query::issueVisQuery(int frameId, const BoundingBox &bbox)
{
	m_queryFrame = frameId;
	m_bbox = bbox;
	m_result = -1;
	g_renderContext->addVisQuery(this);
}

void Query::issueCsmQuery( int frameId, const BoundingBox &bbox )
{
	m_queryFrame = frameId;
	m_bbox = bbox;
	m_result = -1;
	g_renderContext->addCsmQuery(this);
}


#if 0
QueryManager::QueryManager()
{

}

QueryManager::~QueryManager()
{

}
Query *QueryManager::allocQuery()
{
	return m_queryAlloc.alloc();
}

void QueryManager::freeQuery( Query*& query )
{
	m_queryAlloc.free(query);
	query = 0;
}

void QueryManager::issueQuery( Query *query, int frameId, const BoundingBox &bbox )
{
	ActiveQuery *active = m_issuedQueryAlloc.alloc();
	query->m_queryFrame = frameId;
	active->query = query;
	active->frameId = frameId;
	active->bbox = bbox;
	active->issued = false;

	m_activeQuery[query->m_type].push_back(active);
}

void QueryManager::runFrame()
{
	for (int i = 0; i < Query::QueryType_Number; i++) {
		std::list<ActiveQuery*>::iterator it = m_activeQuery[i].begin();

		while (it != m_activeQuery[i].end()) {
			ActiveQuery *issued = *it;

			// object is freed
			if (issued->query->m_objectId < 0) {
				it = m_activeQuery[i].erase(it);
				continue;
			}

			int result = gRenderDriver->getQueryResult(issued->query->m_objectId);

			if (result < 0) {
				++it;
				continue;
			}

			issued->query->m_resultFrame = issued->frameId;
			issued->query->m_result = result;

			it = m_activeQuery[i].erase(it);

			m_issuedQueryAlloc.free(issued);
		}
	}
}

const std::list<QueryManager::ActiveQuery*>& QueryManager::getActiveQuery( Query::QueryType type ) const
{
	return m_activeQuery[type];
}
#endif

AX_END_NAMESPACE
