#include "../private.h"

AX_BEGIN_NAMESPACE

void VertexDeclaration::Element::save(Archiver &ar) const
{
	ar << offset;
	ar << (int)dataType;
	ar << (int)usage;
	ar << usageIndex;
}

void VertexDeclaration::Element::load(Archiver &ar)
{
	ar >> offset;
	ar >> (int&)dataType;
	ar >> (int&)usage;
	ar >> usageIndex;
}

void VertexDeclaration::save(Archiver &ar) const
{
	ar << m_stride;
	ar << m_elements;
}

void VertexDeclaration::load(Archiver &ar)
{
	ar >> m_stride;
	ar >> m_elements;
}

void MeshData::save(Archiver &ar) const
{
	ar << name;
	ar << material;
	ar << declaration;
	ar << numVertices;
	ar << numIndices;
	ar << floatData;
	ar << byteData;
	ar << indices;
	ar << quadstrips;
	ar << orphanTriangles;
}

void MeshData::load(Archiver &ar)
{
	ar >> name;
	ar >> material;
	ar >> declaration;
	ar >> numVertices;
	ar >> numIndices;
	ar >> floatData;
	ar >> byteData;
	ar >> indices;
	ar >> quadstrips;
	ar >> orphanTriangles;
}


void ModelData::save(Archiver &ar) const
{
	ar << (int)MagicId;
	ar << (int)Version;
	ar << meshData;
}

void ModelData::load(Archiver &ar)
{
	int id;
	int version;

	ar >> id;
	ar >> version;

	if (id != MagicId) return;
	if (version > Version) return;

	ar >> meshData;
}

AX_END_NAMESPACE
