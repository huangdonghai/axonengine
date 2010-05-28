/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_QUERY_H
#define AX_RENDER_QUERY_H

AX_BEGIN_NAMESPACE

struct AsioQuery {
	AtomicInt m_atomic;
	int m_queryFrame;
	int m_resultFrame;
	int m_result;
};

class AX_API Query {
public:
	enum QueryType {
		QueryType_Vis,
		QueryType_Shadow,
		QueryType_Number
	};

	friend class BlockAlloc<Query>;

	Query();
	virtual ~Query();

	void setType(QueryType type) { m_type = type; }
	QueryType getType() const { return m_type; }
	void issueQuery(int frameId, const BoundingBox &bbox);

protected:
	QueryType m_type;

public:
	int m_queryFrame;
	int m_resultFrame;
	int m_result;
};

#if 0
class QueryManager {
public:
	virtual ~QueryManager() {}

	virtual Query *allocQuery() = 0;
	virtual void freeQuery(Query*& query) = 0;
	virtual void issueQuery(Query *query, int frameId, const BoundingBox &bbox) = 0;
};
#endif

AX_END_NAMESPACE
#endif
