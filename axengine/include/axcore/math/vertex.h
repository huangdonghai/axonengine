/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_VERTEX_H
#define AX_CORE_VERTEX_H

AX_BEGIN_NAMESPACE

struct Joint {
	Quaternion rotate;
	Vector3 pos;
};

struct VertexType {
	enum Type {
		kMesh, kDebug, kBlend, kChunk, kNumber
	};
	AX_DECLARE_ENUM(VertexType);

	int stride() const;
};

// 64 bytes
struct SkinVertex {
	Vector3 position;
	Rgba color;
	Vector2 streamTc;
	Vector4 normal;
	Vector3 tangent;
	Vector3 oldPosition;
};

// 64 bytes
struct MeshVertex {
	Vector3 position;
	Rgba color;
	Vector4 streamTc;	// channel 0 and 1
	Vector4 normal;		// normal.w is mirroredTbn flag, is 1 or -1
	Vector4 tangent;	// tangent.w is pad, or used somewhere
};

// 16 bytes
struct DebugVertex {
	Vector3 position;
	Rgba color;
};

struct BlendVertex {
	Vector3 position;
	Vector2 streamTc;
	Rgba color;
};

struct ChunkVertex {
	Vector3 position;
};

inline int VertexType::stride() const
{
	static int strides[] = { sizeof(MeshVertex), sizeof(DebugVertex), sizeof(BlendVertex), sizeof(ChunkVertex) };
	return strides[t];
}

AX_END_NAMESPACE

#endif // end guardian

