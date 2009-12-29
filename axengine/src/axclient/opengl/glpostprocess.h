/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GL_POSTPROCESS_H
#define AX_GL_POSTPROCESS_H

AX_BEGIN_NAMESPACE

	class GLpostprocess {
	public:
		enum Type {
			DrawQuad, Blur, Glow, ToneMapping
		};
		GLpostprocess();
		~GLpostprocess();

		void process(Type type, GLtexture* texture);

		void drawQuad(GLtexture* texture);
		void maskVolume(Vector3 volume[8]);
		void maskShadow(Vector3 volume[8], const Matrix4& matrix, GLtexture* tex, bool front = false);
		void maskShadow(Vector3 volume[8], const Matrix4& matrix, GLtexture* tex, const Vector4& minrange, const Vector4& maxrange, const Matrix4& scaleoffset, bool front = false);
		void shadowBlur(GLtexture* texture, bool is_du);

		void measureHistogram(GLtexture* tex, int index);
		void downscale4x4(GLtexture* tex, const Rect& rect);

		void genericPP(const String& shadername, GLtexture* src);
		void genericPP(const String& shadername, RenderTarget* target, GLtexture* src);
		void genericPP(const String& shadername, GLtexture* src1, GLtexture* src2);
		void genericPP(const String& shadername, RenderTarget* target, GLtexture* src1, GLtexture* src2);

	protected:
		void updateScreenQuad(const Rect& rect);
		GLshader* getShader(const String& name);

	private:
		MeshPrim* m_screenQuad;
		GLgeometry* m_screenQuadGeo;
		MeshPrim* m_boxVolume;		// eight vertexes, six face hexahedron

		MaterialPtr m_matDrawQuad;
		MaterialPtr m_matMaskVolume;
		MaterialPtr m_matShadowMask;
		MaterialPtr m_matShadowBlur;
		GLshader* m_shaderDrawQuad;
		GLshader* m_shaderHistogram;
		GLshader* m_shaderDownscale4x4;

		Dict<String, GLshader*>	m_genericShaders;
	};

AX_END_NAMESPACE

#endif // end guardian

