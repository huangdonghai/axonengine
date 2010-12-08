/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9QUERY_H
#define AX_D3D9QUERY_H

AX_BEGIN_NAMESPACE

class D3D9Query : public Query {
public:
	enum {
		NUMBER = 2
	};

	friend class D3D9querymanager;

	D3D9Query();
	virtual ~D3D9Query();

	bool canQuery() const;
	bool beginQuery();
	void endQuery();
	int getResult(); // if return -1, not queried
	void reset(); // reset state

private:
	int m_id;
	IDirect3DQuery9 *m_queries[NUMBER];
	int m_queryPos;
	int m_readPos;
	bool m_quering;
};

class D3D9querymanager : public QueryManager {
public:
	struct ActiveQuery {
		RenderTarget *frameTarget;
		D3D9Query *query;
		int frameId;
		BoundingBox bbox;
		bool issued;
	};

	enum {
		MAX_ISSUED_QUERIES = 1024
	};

	D3D9querymanager();
	virtual ~D3D9querymanager();

	virtual Query *allocQuery();
	virtual void freeQuery(Query*& query);
	virtual void issueQuery(Query *query, int frameId, const BoundingBox &bbox);

	// must called sync main thread
	void syncFrame();

	// called in render thread
	const std::list<ActiveQuery*>& getActiveQuery(Query::QueryType type) const;

private:
	std::vector<D3D9Query*> m_queries;
	std::list<int> m_freeQueries;

	BlockAlloc<ActiveQuery> m_issuedQueryAlloc;
	std::list<ActiveQuery*> m_activeQuery[Query::QueryType_Number];
	std::list<ActiveQuery*> m_frameIssuedQuery[Query::QueryType_Number];
};

AX_END_NAMESPACE

#endif
