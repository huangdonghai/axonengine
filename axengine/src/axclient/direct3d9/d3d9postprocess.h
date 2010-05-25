/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9POSTPROCESS_H
#define AX_D3D9POSTPROCESS_H

AX_BEGIN_NAMESPACE

struct PostMesh {
	PostMesh();
	~PostMesh();

	void init(int num_vertices, int num_indices);

	static bool setupScreenQuad(PostMesh*& mesh, const Rect &rect);
	static bool setupHexahedron(PostMesh*& mesh, Vector3 vertes[8]);
	static bool setupBoundingBox(PostMesh*& mesh, const BoundingBox &bbox);

	int m_numVertices;
	Vector3 *m_vertices;
	int m_numIndices;
	ushort_t *m_indices;
};

class D3D9Postprocess {
public:
	enum Type {
		DrawQuad, Blur, Glow, ToneMapping
	};

	enum {
		MAX_VERTICES = 16,
		MAX_INDICES = 6 * 2 * 3
	};
	D3D9Postprocess();
	~D3D9Postprocess();

	void process(Type type, IDirect3DTexture9 *texture);

	void drawQuad(IDirect3DTexture9 *texture);
	void maskVolume(Vector3 volume[8]);
	void maskVolumeTwoSides(Vector3 volume[8]);
#if 0
	void maskShadow(Vector3 volume[8], const Matrix4 &matrix, D3D9texture *tex, bool front = false);
	void maskShadow(Vector3 volume[8], const Matrix4 &matrix, D3D9texture *tex, const Vector4 &minrange, const Vector4 &maxrange, const Matrix4 &scaleoffset, bool front = false);
	void shadowBlur(D3D9texture *texture, bool is_du);
#endif

	void issueQueryList(const List<D3D9querymanager::ActiveQuery*>& querylist);
	void issueBboxQuery(const BoundingBox &bbox);

	void drawLight(Vector3 volume[8], QueuedLight *light);
	void drawLightShadowed(Vector3 volume[8], QueuedLight *light, const RenderCamera &shadowCamera);
	void drawGlobalLight(Vector3 volume[8], QueuedLight *light);

	void measureHistogram(IDirect3DTexture9 *tex, int index);
	void downscale4x4(IDirect3DTexture9 *tex, const Rect &rect);

	void genericPP(const String &shadername, IDirect3DTexture9 *src);
	void genericPP(const String &shadername, RenderTarget *target, IDirect3DTexture9 *src);
	void genericPP(const String &shadername, IDirect3DTexture9 *src1, IDirect3DTexture9 *src2);
	void genericPP(const String &shadername, RenderTarget *target, IDirect3DTexture9 *src1, IDirect3DTexture9 *src2);

protected:
	DX9_Shader *getShader(const String &name);

private:
	PostMesh *m_screenQuad;
	PostMesh *m_hexahedron;

	Material *m_mtrDrawQuad;
	Material *m_mtrMaskVolume;

	Material *m_mtrPointLight;
	Material *m_mtrGlobalLight;

	DX9_Shader *m_shaderDrawQuad;
	DX9_Shader *m_shaderHistogram;
	DX9_Shader *m_shaderDownscale4x4;
	DX9_Shader *m_shaderQuery;

	Dict<String, DX9_Shader*>	m_genericShaders;
};

AX_END_NAMESPACE

#endif
