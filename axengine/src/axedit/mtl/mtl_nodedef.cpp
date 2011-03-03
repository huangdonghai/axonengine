#include "mtl_local.h"

AX_BEGIN_NAMESPACE

class NodeDefManager
{
public:
	NodeDefManager()
	{
		const char * filename = "shaders/nodedef.xml";

		char *buffer = 0;
		size_t size = 0;

		size = g_fileSystem->readFile(filename, (void**)&buffer);

		if (!size || !buffer) {
			Debugf("%s: cann't open file %s\n", __func__, filename);
			return;
		}

		TiXmlDocument doc;

		doc.Parse(buffer, 0, TIXML_ENCODING_UTF8);
		g_fileSystem->freeFile(buffer);

		if (doc.Error()) {
			Errorf("%s: error parse %s in line %d - %s"
				, __func__
				, filename
				, doc.ErrorRow()
				, doc.ErrorDesc());
			doc.Clear();
			return;
		}

		const TiXmlElement *root = doc.FirstChildElement("nodeDefs");
		const TiXmlAttribute *attr = 0;

		if (!root) Errorf("error format");

		const TiXmlElement *elem = 0;
		for (elem = root->FirstChildElement("node"); elem; elem = elem->NextSiblingElement("node")) {
			MtlNodeDef def;
			if (def.parseXml(elem)) {
				m_defs.push_back(def);
			}
		}
	}

	~NodeDefManager() {}

	std::vector<MtlNodeDef> m_defs;
};

MtlNodeDef::MtlNodeDef()
{
}

MtlNodeDef::~MtlNodeDef()
{
}


const std::vector<MtlNodeDef>& MtlNodeDef::getNodeDefs()
{
	NodeDefManager defManager;
	return defManager.m_defs;
}

bool MtlNodeDef::parseXml(const TiXmlElement *elem)
{
	const char *name = elem->Attribute("name");
	const char *nodekind = elem->Attribute("kind");

	if (!name || !nodekind) {
		Debugf("error format");
		return false;
	}

	m_name = name;
	m_nodeKind = stringToKind(nodekind);

	const TiXmlElement *socket = 0;
	for (socket = elem->FirstChildElement(); socket; socket = socket->NextSiblingElement()) {
		if (socket->ValueTStr() == "input") {
			MtlSocketDef socketDef;
			if (socketDef.parseXml(socket)) {
				m_inSockets.push_back(socketDef);
			}
		} else if (socket->ValueTStr() == "ouput") {
			MtlSocketDef socketDef;
			if (socketDef.parseXml(socket)) {
				m_outSockets.push_back(socketDef);
			}
		} else {
			Debugf("unkonwn element");
		}
	}

	return true;
}

MtlNodeDef::NodeKind MtlNodeDef::stringToKind(const char *str)
{
	// kConst, kUniform, kVarying, kParameter, kSampler, kOperator, kFunction, kRoot

#define CHECK(name, val) \
	if (Strequ(str, name)) return val;

	CHECK("const", kConst);
	CHECK("uniform", kUniform);
	CHECK("varying", kVarying);
	CHECK("parameter", kParameter);
	CHECK("sampler", kSampler);
	CHECK("operator", kOperator);
	CHECK("function", kFunction);
	CHECK("root", kRoot);
#undef CHECK

	Errorf("unknown kind");
	return MtlNodeDef::kConst;
}

MtlSocketDef::MtlSocketDef()
{
}

MtlSocketDef::~MtlSocketDef()
{
}

bool MtlSocketDef::parseXml(const TiXmlElement *elem)
{
	const char *name = elem->Attribute("name");
	const char *datatype = elem->Attribute("datatype");

	if (!name || !datatype) return false;

	m_name = name;
	m_dataType = stringToDatatype(datatype);

	return true;
}

MtlSocketDef::DataType MtlSocketDef::stringToDatatype(const char *str)
{
//	kFollow = -2, // follow preparameter
//	kAny = -1,
//	kFloat, kFloat2, kFloat3, kFloat4, kMatrix3, kMatrix34, kMatrix4

#define CHECK(name, val) \
	if (Strequ(str, name)) return val;

	CHECK("input0", kInput0);
	CHECK("any", kAny);
	CHECK("float", kFloat);
	CHECK("float2", kFloat2);
	CHECK("float3", kFloat3);
	CHECK("float4", kFloat4);
	CHECK("matrix3", kMatrix3);
	CHECK("matrix34", kMatrix34);
	CHECK("matrix4", kMatrix4);

#undef CHECK

	Errorf("error format");
	return kAny;
}

AX_END_NAMESPACE