#ifndef AX_CORE_MODEL_H
#define AX_CORE_MODEL_H

AX_BEGIN_NAMESPACE

enum {
	MAX_NAME = 64,
	MAX_FILENAME = 128,
	UNWRAP_UV_DIVIDED = 32
};

class VertexDeclaration
{
public:
	enum DataType
	{
		DT_NONE = 0,
		DT_BYTE4,
		DT_FLOAT,
		DT_FLOAT2,
		DT_FLOAT3,
		DT_FLOAT4
	};

	enum DataUsage
	{
		DU_NONE = 0,
		DU_POSITION = 1,
		DU_COLOR = 2,
		DU_NORMAL = 4,
		DU_TANGENT = 8,
		DU_BINORMAL = 16,
		DU_TEXCOORD = 32,
		DU_BLENDWEIGHTS = 64,  // usually 4 weights, but 3 can be stored with 1 implied. Can be stored as 4*uint8, so quantized where 1.0f => 0xff (255),
		DU_BLENDINDICES = 128, // usually 4 hkUint8s in a row. So can reference 256 blend transforms (bones)
		DU_USERDATA = 256
	};

	struct Element {
		int offset;
		DataType dataType;
		DataUsage usage;
		int usageIndex;
	};

	int m_stride;
	std::vector<Element> m_elements;
};

struct QuadStripData {
	float width;
	float height;
	ushort_t *indices;
};

struct MeshData {
	char name[MAX_NAME];
	char material[MAX_FILENAME];
	int numVertices;
	int numIndices;
	Vector3 *postions;
	Vector2 *texcoords;
	Vector3 *normals;
	Rgba * colors;
	ushort_t *indices;
	ushort_t *quadstrips;
	ushort_t *orphanTriangles;
};

struct SkeletonData {};

struct SkinData {};

struct AnimationData {};

AX_END_NAMESPACE

#endif // AX_CORE_MODEL_H
