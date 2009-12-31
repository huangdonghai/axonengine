/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE

	static inline GLenum trBufferHint(Primitive::Hint hint) {
		switch (hint) {
		case Primitive::HintStatic:
			return GL_STATIC_DRAW;
		case Primitive::HintDynamic:
			return GL_STREAM_DRAW;
		case Primitive::HintFrame:
			return GL_STREAM_DRAW;
		default: AX_NO_DEFAULT;
			return GL_DYNAMIC_DRAW;
		}
	}

	//--------------------------------------------------------------------------
	// class GLvertexbuffer
	//--------------------------------------------------------------------------

	GLvertexbuffer::GLvertexbuffer() {
		m_object = 0;
		m_dataSize = 0;
	}

	GLvertexbuffer::~GLvertexbuffer() {
		resetData();
	}

	void GLvertexbuffer::setData(const void *p, int size, Primitive::Hint primhint) {
		m_hint = trBufferHint(primhint);
		if (m_hint == GL_STATIC_DRAW || size != m_dataSize) {
			resetData();
		}

		if (m_object == 0) {
			glGenBuffers(1, &m_object);
			AX_ASSERT(m_object);

			g_statistic->incValue(stat_numVertexBuffers);
			g_statistic->addValue(stat_vertexBufferMemory, size);

			glBindBuffer(GL_ARRAY_BUFFER, m_object);
			glBufferData(GL_ARRAY_BUFFER, size, p, m_hint);
		} else {
			glBindBuffer(GL_ARRAY_BUFFER, m_object);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, p);
		}

		m_dataSize = size;
	}

	void GLvertexbuffer::resetData() {
		if (m_object == 0)
			return;

		glDeleteBuffers(1, &m_object);
		g_statistic->decValue(stat_numVertexBuffers);
		g_statistic->subValue(stat_vertexBufferMemory, m_dataSize);

		m_object = 0;
		m_dataSize = 0;
	}

	void GLvertexbuffer::bind() {
		AX_ASSERT(m_object);

		glBindBuffer(GL_ARRAY_BUFFER, m_object);
	}


	//--------------------------------------------------------------------------
	// class GLindexbuffer
	//--------------------------------------------------------------------------

	GLindexbuffer::GLindexbuffer() {
		m_object = 0;
		m_dataSize = 0;
		m_count = 0;
		m_activeCount = 0;
	}
	GLindexbuffer::~GLindexbuffer() {
		resetData();
	}

	void GLindexbuffer::setData(const ushort_t *p, int count, Primitive::Hint primhint, int activeCount) {
		m_hint = trBufferHint(primhint);

		m_count = count;
		int size = count * 2;

		setActiveCount(activeCount);
		if (activeCount == 0) {
			activeCount = count;
		}

		int activeSize = activeCount * 2;

		if (primhint == Primitive::HintFrame) {
			m_count = activeCount;
			size = activeSize;
		}

		if (m_hint == GL_STATIC_DRAW || size != m_dataSize)
			resetData();

		if (m_object == 0) {
			glGenBuffers(1, &m_object);
			AX_ASSERT(m_object);

			g_statistic->incValue(stat_numIndexBuffers);
			g_statistic->addValue(stat_indexBufferMemory, size);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_object);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, p, m_hint);
		} else {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_object);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, activeSize, p);
		}
		m_dataSize = size;
	}

	void GLindexbuffer::resetData() {
		if (m_object == 0)
			return;

		glDeleteBuffers(1, &m_object);

		g_statistic->decValue(stat_numIndexBuffers);
		g_statistic->subValue(stat_indexBufferMemory, m_dataSize);

		m_object = 0;
		m_dataSize = 0;
	}

	void GLindexbuffer::bind() {
		AX_ASSERT(m_object);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_object);
	}

	void GLindexbuffer::drawElements(GLenum mode) {
		bind();
		drawElementsWithoutBind(mode);
	}

	void GLindexbuffer::drawElementsWithoutBind(GLenum mode) {
		int numcount = m_count;

		if (m_activeCount) {
			numcount = m_activeCount;
		}

		glDrawElements(mode, numcount, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	}

	inline int GLindexbuffer::getNumElements(GLenum mode, int numindexes) {
		switch (mode) {
		case GL_LINES:
			return numindexes / 2;
		case GL_TRIANGLES:
			return numindexes / 3;
		}

		Errorf("GLindexbuffer::getNumElements: not support element mode %d", mode);
		return 0;
	}

	bool GLindexbuffer::haveData() const {
		return m_dataSize > 0;
	}

	void *GLindexbuffer::mapBuffer() {
		bind();
		return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
	}

	void GLindexbuffer::unmapBuffer() {
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}


AX_END_NAMESPACE

