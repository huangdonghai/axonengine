/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GL_RENDER_H
#define AX_GL_RENDER_H

AX_BEGIN_NAMESPACE

	// VertexDef
	enum VertexDef {
		VDF_invalid = -1,
		VDF_position,				// Vector3
		VDF_normal,					// Vector3
		VDF_color,					// Color4
		VDF_baseTc,					// Vector2
		VDF_lightmapTc,				// Vector2
		VDF_alphamapTc,				// Vector2, for terrain

		// for per-pixel lighting
		VDF_tangent,				// Vector3
		VDF_binormal,				// Vector3
	};
	typedef Sequence<VertexDef> VertexDefSeq;

	enum { AX_MAX_TEXTURE_UNITS = 16 };

	class GLgeometry;

	class GLrender {
	public:
		static void setMaterialParameter(Material *mat);

		static void draw(Material *mat, Technique tech, GLgeometry *prim);
		static void draw(GLshader *shader, Technique tech, GLgeometry *prim);
		static void bindVertexBuffer(const VertexDefSeq &defs, GLsizei stride, GLenum bufId, GLuint offset);
		static void bindVertexBuffer(const VertexDef *defs, GLsizei stride, GLenum bufId, GLuint offset);
		static void bindVertexBuffer(VertexType vt, GLenum bufId, GLuint offset);
		static void checkForCgError();
		static void checkForCgError(const char *shaderfile);
		static void checkErrors();
		static void setViewport(const Rect &view_rect);
	};

	class GLfontrender {
	public:
		enum {
			NumCharsPerBatch = 256
		};
		GLfontrender();
		~GLfontrender();

		void initialize();
		void finalize();
		Vector2 drawString(Font *font, Rgba color, const TextQuad &tq, const Vector2 &xy, const wchar_t *str, size_t len, const Vector2 &scale, bool italic = false);

	protected:
		void draw(int count);

	private:
		GLvertexbuffer m_vertexBuffer;
#if 0
		GLindexbuffer m_indexBuffer;
#endif
		GLshader *m_shader;
		float m_posOffset;
	};

AX_END_NAMESPACE

#endif // end guardian

