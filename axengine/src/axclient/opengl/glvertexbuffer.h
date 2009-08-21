/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GLVERTEXBUFFER_H
#define AX_GLVERTEXBUFFER_H

namespace Axon { namespace Render {

	//--------------------------------------------------------------------------
	// class GLvertexbuffer
	//--------------------------------------------------------------------------

	class GLvertexbuffer {
	public:
		GLvertexbuffer();
		~GLvertexbuffer();

		void setData(const void* p, int size, Primitive::Hint);
		void resetData();
		void bind();

		GLuint getObject() const { return m_object; }

	private:
		GLuint m_object;
		GLenum m_hint;
		int m_dataSize;
	};

	//--------------------------------------------------------------------------
	// class GLindexbuffer
	//--------------------------------------------------------------------------

	class GLindexbuffer {
	public:
		GLindexbuffer();
		~GLindexbuffer();

		void setData(const ushort_t* p, int count, Primitive::Hint, int activeCount = 0);
		void resetData();
		void bind();
		void* mapBuffer();
		void unmapBuffer();
		bool haveData() const;

		GLuint getObject() const { return m_object; }

		int getActiveCount() const;
		void setActiveCount(int val);

		void drawElements(GLenum mode);
		void drawElementsWithoutBind(GLenum mode);

	protected:
		static int getNumElements(GLenum mode, int numindexes);

	private:
		GLuint m_object;
		GLenum m_hint;
		int m_dataSize;
		int m_count;
		int m_activeCount;
	};

	inline int GLindexbuffer::getActiveCount() const {
		return m_activeCount;
	}

	inline void GLindexbuffer::setActiveCount(int val)
	{
		AX_ASSERT(m_activeCount <= m_count);
		m_activeCount = val;
	}


}} // namespace Axon::Render

#endif // AX_GLVERTEXBUFFER_H

