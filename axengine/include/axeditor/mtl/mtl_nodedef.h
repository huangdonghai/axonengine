#ifndef AX_MTL_NODEDEF_H
#define AX_MTL_NODEDEF_H

#include <axcore/template/graph.h>

AX_BEGIN_NAMESPACE

class ShaderValue
{
public:
	enum Type {
		kFloat, kVector2, kVector3, kVector4, kMatrix3, kMatrix, kMatrix4, kSampler
	};
};

class MtlSocketDef
{
public:
	enum { Input, Output };

	enum DataType {
		kInput0 = -2, // same as input 0
		kAny = -1,
		kFloat, kFloat2, kFloat3, kFloat4, kMatrix3, kMatrix34, kMatrix4
	};

	MtlSocketDef();
	~MtlSocketDef();

	bool parseXml(const TiXmlElement *elem);

	static DataType stringToDatatype(const char *str);

private:
	FixedString m_name;
	DataType m_dataType;
	void *m_defaultValue;
};

class MtlNodeDef
{
public:
	enum NodeKind
	{
		kConst, kUniform, kVarying, kParameter, kSampler, kOperator, kFunction, kRoot
	};

	MtlNodeDef();
	~MtlNodeDef();

	bool parseXml(const TiXmlElement *elem);

	static NodeKind stringToKind(const char *str);
	static const std::vector<MtlNodeDef>& getNodeDefs();
	
private:
	FixedString m_name;
	NodeKind m_nodeKind;
	std::vector<MtlSocketDef> m_inSockets;
	std::vector<MtlSocketDef> m_outSockets;
	std::string m_template;
};


class MtlEdge;
class MtlNode;
class MtlGraph;

class MtlEdge : public GraphEdge_<MtlGraph, MtlNode, MtlEdge>
{

};

class MtlNode : public GraphNode_<MtlGraph, MtlNode, MtlEdge>
{
public:

private:
	MtlNodeDef *m_def;
};

class MtlGraph : public Graph_<MtlGraph, MtlNode, MtlEdge>
{
public:

};


AX_END_NAMESPACE

#endif // AX_MTL_NODEDEF_H
