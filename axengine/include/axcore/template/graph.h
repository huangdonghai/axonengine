#ifndef AX_CORE_GRAPH_H
#define AX_CORE_GRAPH_H

AX_BEGIN_NAMESPACE

// forward declaration
class GraphEdge;
class GraphNode;
class Graph;

class GraphEdge
{
public:

protected:
	friend class GraphNode;
	GraphNode *m_srcNode;
	int m_srcSocket;
	GraphNode *m_dstNode;
	int m_dstSocket;

	IntrusiveLink<GraphEdge> m_srcLink;
	IntrusiveLink<GraphEdge> m_dstLink;
};


class GraphNode
{
public:
	friend class Graph;

protected:
	IntrusiveLink<GraphNode> m_link;

	// geometry, for editor
	Vector2 m_pos;

	IntrusiveList<GraphEdge, &GraphEdge::m_srcLink> m_linkOutList;
	IntrusiveList<GraphEdge, &GraphEdge::m_dstLink> m_linkInList;
};


class Graph
{
public:

protected:
	IntrusiveList<GraphNode> m_nodeList;
};

template <class GraphT, class NodeT, class EdgeT>
class GraphEdge_ : public GraphEdge
{
public:
	typedef typename NodeT NodeType;

	NodeType *getSrcNode() const { return m_srcNode; }
	NodeType *getDstNode() const { return m_dstNode;}

private:
};

template <class GraphT, class NodeT, class EdgeT>
class GraphNode_ : public GraphNode
{
public:
	typedef EdgeT EdgeType;
};

template <class GraphT, class NodeT, class EdgeT>
class Graph_ : public Graph
{
public:
	typedef NodeT NodeType;
	typedef EdgeT EdgeType;
};

AX_END_NAMESPACE

#endif // AX_CORE_GRAPH_H
