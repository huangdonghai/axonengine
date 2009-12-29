/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GL_PRIMITIVE_H
#define AX_GL_PRIMITIVE_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// class GLprimitive
	//--------------------------------------------------------------------------

	class GLprimitive {
	public:
		GLprimitive();
		virtual ~GLprimitive();

		virtual void initialize(Primitive* source_primitive) = 0;
		virtual void finalize() = 0;
		virtual void update() = 0;
		virtual void draw(Technique tech) = 0;
//		virtual void drawElements() = 0;

		void setOverloadMaterial(Material* mat) {
			m_overloadMaterial = mat;
		}
		void unsetOverloadMaterial() {
			m_overloadMaterial = NULL;
		}

		Material* getMaterial() { return m_material; }
		inline bool isMatrixSet() const { return m_isMatrixSet; }
		inline const Matrix4& getMatrix() const { return m_matrix; }

	protected:
		Primitive* m_source;
		Primitive::Hint m_hint;
		Material* m_material;

	public:
		Material* m_overloadMaterial;
		const InstancePrim::ParamSeq* m_instanceParams;
		GLindexbuffer* m_overloadedIndexbuffer;
		int m_activeIndexes;

		bool m_isMatrixSet;
		Matrix4 m_matrix;

		Texture* m_lightmap;
	};

	//--------------------------------------------------------------------------
	// class GLgeometry
	//--------------------------------------------------------------------------

	class GLgeometry : public GLprimitive {
	public:
		GLgeometry();
		virtual ~GLgeometry();

		virtual void initialize(Primitive* source_primitive);
		virtual void finalize();
		virtual void update();
		virtual void draw(Technique tech);


		void bindVertexBuffer();
		void drawElements();
		void drawElementsInstanced();

	protected:
		void initPoint();
		void initPointSprite();
		void initLine();
		void initLineStrip();
		void initMesh();

		void updatePoint();
		void updatePointSprite();
		void updateLine();
		void updateLineStrip();
		void updateMesh();

	protected:
		bool m_isFrameLieftime;

		// vertex info
		VertexDefSeq m_vertexDefs;
		GLvertexbuffer m_vertexBuffer;
		int m_vertexStride;
		int m_vertexOffset;
		int m_vertexCount;

		// index info
		GLindexbuffer m_indexBuffer;
#if 0
		int m_indexOffset;
		int m_indexCount;
		int                 m_currentIndexNum;
#endif
		GLenum m_elementType;		// GL_POINTS, GL_LINES...

		Rgba m_color;
		float m_geometrySize;		// point size or line width

		size_t m_curStage;			// for rendering
		uint_t m_usableTMU;
	};

	//--------------------------------------------------------------------------
	// class GLtext
	//--------------------------------------------------------------------------


	class GLtext : public GLprimitive {
	public:
		GLtext();
		virtual ~GLtext();

		virtual void initialize(Primitive* source_primitive);
		virtual void finalize();
		virtual void update();
		virtual void draw(Technique tech);

	protected:
#if 0
		Vector2 drawString(Rgba color, const TextQuad& tq, const Vector2& xy, const wchar_t* str, size_t len, const Vector2& scale, bool italic = false);
#endif
	private:
		Rect m_rect;				// draw on this rect
		Vector3 m_position;			// for simple text
		bool m_isSimpleText;
		float m_aspect;
		int m_format;			// format flags
		Font* m_font;				// font used
		String m_text;				// string to draw
		TextPrim::HorizonAlign m_horizonAlign;
		TextPrim::VerticalAlign m_verticalAlign;
		Rgba m_color;

#if 0
		static Material* m_fontMtr;
#endif
	};

	//--------------------------------------------------------------------------
	// class GLterrain
	//--------------------------------------------------------------------------

	class GLterrain : public GLgeometry {
	public:
		GLterrain();
		virtual ~GLterrain();

		// implement GLprimitive
		virtual void initialize(Primitive* source_primitive);
		virtual void finalize();
		virtual void update();
		virtual void draw(Technique tech);

	protected:

	private:
		Vector4 m_terrainRect;

		Texture* m_colorTexture;
		Texture* m_normalTexture;
		Vector4 m_zoneRect;

		Vector4 m_chunkRect;
		int m_numLayers;
		ChunkPrim::Layer m_layers[ChunkPrim::MAX_LAYERS];
		bool m_layerVisible;
	};

	//--------------------------------------------------------------------------
	// class GLgroup
	//--------------------------------------------------------------------------

	class GLgroup : public GLprimitive {
	public:
		GLgroup();
		virtual ~GLgroup();

		virtual void initialize(Primitive* source_primitive);
		virtual void finalize();
		virtual void update();
		virtual void draw(Technique tech);
//		virtual void drawElements();

	private:
		Sequence<int>		m_primitives;
	};

	//--------------------------------------------------------------------------
	// class GLref
	//--------------------------------------------------------------------------

	class GLref : public GLprimitive {
	public:
		GLref();
		virtual ~GLref();

		virtual void initialize(Primitive* source_primitive);
		virtual void finalize();
		virtual void update();
		virtual void draw(Technique tech);
//		virtual void drawElements();

	private:
		int m_cachedId;
		GLindexbuffer m_indexBuffer;
	};

	//--------------------------------------------------------------------------
	// class GLinstance
	//--------------------------------------------------------------------------

	class GLinstance : public GLprimitive {
	public:
		GLinstance();
		virtual ~GLinstance();

		virtual void initialize(Primitive* src);
		virtual void finalize();
		virtual void update();
		virtual void draw(Technique tech);
//		virtual void drawElements();

	private:
		int m_instanced;
		InstancePrim::ParamSeq m_params;
	};

	//--------------------------------------------------------------------------
	// class GLprimitivemanager
	//--------------------------------------------------------------------------

	struct BufferPage {
		GLenum bufferId;
		GLuint bufferSize;
		GLuint curOffset;
		BufferPage* next;			// link to next page
	};

	class GLprimitivemanager : public ThreadSafe {
	public:
		GLprimitivemanager();
		~GLprimitivemanager();

		void initialize();
		void finalize();
		void beginFrame();
		int cachePrimitive(Primitive* prim);
		void uncachePrimitive(Primitive* prim);
		void endFrame();
		GLprimitive* getPrimitive(int handle);

	protected:
		void findStaticFreeSlot(int& handle);
		bool isStatic(int id) { return(size_t(id) & FRAME_FLAG) == 0; }
		bool isFrameHandle(int id) { return !isStatic(id); }
		void linkId(int id, GLprimitive* glprim);
		GLprimitive* createPrim(Primitive* prim);

	private:
		enum {
			MAX_PRIMITIVES = 4096,
			BUFFER_PAGE_SIZE = 1024 * 1024,
			FRAME_FLAG = 0x80000000,
			INDEX_MASK = ~FRAME_FLAG,
		};

		int m_framenum;
		
		int m_numStaticPrims;
		GLprimitive* m_staticPrims[MAX_PRIMITIVES];

		int m_numFramePrims;
		GLprimitive* m_framePrims[MAX_PRIMITIVES];

		int numFreePrims;
		void* m_freePrimLink;

		int m_numWaitForDelete;
		int m_waitForDelete[MAX_PRIMITIVES];

		BufferPage* m_frameVertexBuffers;
		BufferPage* m_frameIndexBuffers;
	};

AX_END_NAMESPACE

#endif // AX_GL_PRIMITIVE_H
