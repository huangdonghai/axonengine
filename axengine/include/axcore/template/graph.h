#ifndef AX_CORE_GRAPH_H
#define AX_CORE_GRAPH_H

AX_BEGIN_NAMESPACE

// forward declaration
template <class NodeT>
class GraphEdge;

template <class T>
class GraphNode;

template <class NodeT>
class Graph;

template <class NodeT>
class GraphEdge
{
public:

private:
	friend typename NodeT;
	NodeT *m_from;
	int m_fromSocket;
	NodeT *m_to;
	int m_toSocket;

	IntrusiveLink<GraphEdge> m_srcLink;
	IntrusiveLink<GraphEdge> m_dstLink;
};


template <class T>
class GraphNode
{
public:
	typedef GraphEdge<GraphNode> EdgeType;

	friend class Graph<GraphNode<T>>;

private:
	IntrusiveLink<GraphNode> m_link;

	// geometry, for editor
	Vector2 m_pos;

	IntrusiveList<EdgeType, &EdgeType::m_srcLink> m_linkOutList;
	IntrusiveList<EdgeType, &EdgeType::m_dstLink> m_linkInList;
};


template <class NodeT>
class Graph
{
public:
	typedef NodeT NodeType;
	typedef typename NodeType::EdgeType EdgeType;

private:
	IntrusiveList<NodeT> m_nodeList;
};

AX_END_NAMESPACE

#endif // AX_CORE_GRAPH_H
