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

		static bool setupScreenQuad(PostMesh*& mesh, const Rect& rect);
		static bool setupHexahedron(PostMesh*& mesh, Vector3 vertes[8]);
		static bool setupBoundingBox(PostMesh*& mesh, const BoundingBox& bbox);

		int m_numVertices;
		Vector3* m_vertices;
		int m_numIndices;
		ushort_t* m_indices;
	};

	class D3D9postprocess {
	public:
		enum Type {
			DrawQuad, Blur, Glow, ToneMapping
		};

		enum {
			MAX_VERTICES = 16,
			MAX_INDICES = 6 * 2 * 3
		};
		D3D9postprocess();
		~D3D9postprocess();

		void process(Type type, D3D9texture* texture);

		void drawQuad(D3D9texture* texture);
		void maskVolume(Vector3 volume[8]);
		void maskVolumeTwoSides(Vector3 volume[8]);
#if 0
		void maskShadow(Vector3 volume[8], const Matrix4& matrix, D3D9texture* tex, bool front = false);
		void maskShadow(Vector3 volume[8], const Matrix4& matrix, D3D9texture* tex, const Vector4& minrange, const Vector4& maxrange, const Matrix4& scaleoffset, bool front = false);
		void shadowBlur(D3D9texture* texture, bool is_du);
#endif

		void issueQueryList(const List<D3D9querymanager::ActiveQuery*>& querylist);
		void issueBboxQuery(const BoundingBox& bbox);

		void drawLight(Vector3 volume[8], QueuedLight* light);
		void drawLightShadowed(Vector3 volume[8], QueuedLight* light, const RenderCamera& shadowCamera);
		void drawGlobalLight(Vector3 volume[8], QueuedLight* light);

		void measureHistogram(D3D9texture* tex, int index);
		void downscale4x4(D3D9texture* tex, const Rect& rect);

		void genericPP(const String& shadername, D3D9texture* src);
		void genericPP(const String& shadername, RenderTarget* target, D3D9texture* src);
		void genericPP(const String& shadername, D3D9texture* src1, D3D9texture* src2);
		void genericPP(const String& shadername, RenderTarget* target, D3D9texture* src1, D3D9texture* src2);

	protected:
		D3D9shader* getShader(const String& name);

	private:
		PostMesh* m_screenQuad;
		PostMesh* m_hexahedron;

		MaterialPtr m_mtrDrawQuad;
		MaterialPtr m_mtrMaskVolume;

		MaterialPtr m_mtrPointLight;
		MaterialPtr m_mtrGlobalLight;

		D3D9shader* m_shaderDrawQuad;
		D3D9shader* m_shaderHistogram;
		D3D9shader* m_shaderDownscale4x4;
		D3D9shader* m_shaderQuery;

		Dict<String, D3D9shader*>	m_genericShaders;
	};

AX_END_NAMESPACE

#endif
