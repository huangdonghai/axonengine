#ifndef AX_CORE_MODEL_H
#define AX_CORE_MODEL_H

AX_BEGIN_NAMESPACE

enum {
	MAX_NAME = 64,
	MAX_FILENAME = 128,
	UNWRAP_UV_DIVIDED = 32
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
