#include "../private.h"

AX_BEGIN_NAMESPACE

Archiver::Archiver(const std::string &filename, File::AccessMode mode)
{
	m_file = g_fileSystem->openFileByMode(filename, mode);
}

Archiver::~Archiver()
{
	delete m_file;
}


void QuadStripData::save(Archiver &ar) const
{
	ar << row << column << width << height << indices;
}

void QuadStripData::load(Archiver &ar)
{
	ar >> row >> column >> width >> height >> indices;
}

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
	ar << m_floatStride;
	ar << m_byteStride;
	ar << m_ushortStride;
	ar << m_elements;
}

void VertexDeclaration::load(Archiver &ar)
{
	ar >> m_floatStride;
	ar >> m_byteStride;
	ar >> m_ushortStride;
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
	ar << ushortData;
	ar << byteData;
	ar << indices;
}

void MeshData::load(Archiver &ar)
{
	ar >> name;
	ar >> material;
	ar >> declaration;
	ar >> numVertices;
	ar >> numIndices;
	ar >> floatData;
	ar >> ushortData;
	ar >> byteData;
	ar >> indices;
}


void ModelFile::save(Archiver &ar) const
{
	ar << (int)FileId;
	ar << (int)FileVersion;
	ar << meshData;
}

void ModelFile::load(Archiver &ar)
{
	int id;
	int version;

	ar >> id;
	ar >> version;

	if (id != FileId) return;
	if (version > FileVersion) return;

	ar >> meshData;
}

AX_END_NAMESPACE
