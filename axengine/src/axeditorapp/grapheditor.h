#ifndef GRAPHEDITOR_H
#define GRAPHEDITOR_H

#include <QGraphicsView>

#include "private.h"
#include <axcore/template/graph.h>


#if 0
class GraphEdge
{
	friend class GraphNode;
public:

private:
	GraphNode *m_from;
	int m_fromSocket;
	GraphNode *m_to;
	int m_toSocket;

	IntrusiveLink<GraphEdge> m_srcLink;
	IntrusiveLink<GraphEdge> m_dstLink;
};

class GraphNode
{
	friend class GraphEdge;
public:

private:
	IntrusiveLink<GraphNode> m_link;

	// geometry, for editor
	Vector2 m_pos;

	IntrusiveList<GraphEdge, &GraphEdge::m_srcLink> m_linkOutList;
	IntrusiveList<GraphEdge, &GraphEdge::m_dstLink> m_linkInList;
};

class Graph
{
public:
private:
	IntrusiveList<GraphNode> m_nodeList;
};
#endif


class GraphEdgeItem : public QGraphicsItem
{
public:
private:
	GraphEdge *m_edge;
};

class GraphNodeItem : public QGraphicsItem
{
public:
private:
	GraphNode *m_node;
};

class GraphEditor : public QGraphicsView
{
	Q_OBJECT

public:
	GraphEditor(QWidget *parent);
	~GraphEditor();

private:
	float m_scale;
	QPointF m_offset;
};

#endif // GRAPHEDITOR_H
