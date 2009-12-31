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
			kVertex, kDebug, kBlend, kChunk
		} t;
		AX_DECLARE_ENUM(VertexType);
	};

	class VertexDecl {
	public:
		enum ElementDecl {
			Postion,
			StreamTc,
			StreamTc2,
			Color,
			Normal,
			Tangent,
			Binormal,
			BoneIndices,
			BoneWeights
		};
		VertexDecl(const Sequence<ElementDecl>& decls);

		int getStride() const;
		int getElementOffset() const;

	private:
		Sequence<ElementDecl> m_elemDecls;
	};

	struct MeshVertex {
		Vector3 xyz;
		Vector2 st;
		Rgba rgba;
		Vector2 st2;		// for ambient st or lightmap st
		Vector3 normal;
		Vector3 tangent;
		Vector3 binormal;
	};

	// define some vertex format
	struct DebugVertex {
		Vector3 xyz;
		Rgba rgba;
	};

	struct BlendVertex {
		Vector3 xyz;
		Vector2 st;
		Rgba rgba;
	};

	struct ChunkVertex {
		Vector3 xyz;
	};

AX_END_NAMESPACE

#endif // end guardian

