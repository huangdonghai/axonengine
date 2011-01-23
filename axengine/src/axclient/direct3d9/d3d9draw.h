/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9DRAW_H
#define AX_D3D9DRAW_H

AX_BEGIN_NAMESPACE

class D3D9geometry;

struct MeshUP;

class D3D9Draw {
public:
	enum {
		NUM_CHARS_PER_BATCH = 64
	};

	D3D9Draw();
	~D3D9Draw();

	static DX9_Shader *findShader(Material *mat, Technique tech = Technique::Main);
	static void draw(Material *mat, Technique tech, D3D9geometry *prim);
	static void draw(DX9_Shader *shader, Technique tech, D3D9geometry *prim);

	void drawPrimitiveUP(Material *mat, Technique tech, void *vertices, int numverts);
	void drawPostUP(Material *mat, MeshUP *mesh);
	void drawPostUP(DX9_Shader *shader, MeshUP *mesh);

	Vector2 drawString(Font *font, Rgba color, const TextQuad &tq, const Vector2 &xy, const wchar_t *str, size_t len, const Vector2 &scale, bool italic = false);

protected:
	void drawChars(int count);

private:
	BlendVertex m_fontVerts[NUM_CHARS_PER_BATCH*6];
	DX9_Shader *m_fontShader;
	D3D9VertDecl *m_vertDecl;
	float m_posOffset;
public:
	D3D9VertDecl *m_postVertDecl;
};

AX_END_NAMESPACE

#endif // end guardian
