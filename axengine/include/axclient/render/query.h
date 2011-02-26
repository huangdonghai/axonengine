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

class AX_API Query
{
	friend class RenderContext;

public:
	bool isWaitingResult() const { return m_result == -1; }
	void issueVisQuery(int frameId, const BoundingBox &bbox);
	void issueCsmQuery(int frameId, const BoundingBox &bbox);

private:
	// only create and delete by RenderContext
	Query();
	virtual ~Query();

public:
	Handle m_handle;
	int m_queryFrame;
	BoundingBox m_bbox;
	volatile int m_result;
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
