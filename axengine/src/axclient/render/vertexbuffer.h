#ifndef AX_RENDER_VERTEXBUFFER_H
#define AX_RENDER_VERTEXBUFFER_H

AX_BEGIN_NAMESPACE

enum ElementType {
	ElementType_PointList,
	ElementType_LineList,
	ElementType_TriList,
	ElementType_TriStrip
};

extern RenderApi *g_renderApi;

class VertexBufferTraits
{
public:
	static handle_t create(int size) { g_renderApi->createVertexBuffer(size, Primitive2::HintDynamic); }
	static void *lock(handle_t h) { g_renderApi->lockVertexBuffer(h); }
	static void unlock(handle_t h) { g_renderApi->unlockVertexBuffer(h); }
	static void free(handle_t h) { g_renderApi->deleteVertexBuffer(h); }
};

class IndexBufferTraits
{
public:
	static handle_t create(int size) { g_renderApi->createIndexBuffer(size, Primitive2::HintDynamic); }
	static void *lock(handle_t h) { g_renderApi->lockIndexBuffer(h); }
	static void unlock(handle_t h) { g_renderApi->unlockIndexBuffer(h); }
	static void free(handle_t h) { g_renderApi->deleteIndexBuffer(h); }
};

class InstanceBufferTraits
{
public:
	static handle_t create(int size) { g_renderApi->createInstanceBuffer(size, Primitive2::HintDynamic); }
	static void *lock(handle_t h) { g_renderApi->lockInstanceBuffer(h); }
	static void unlock(handle_t h) { g_renderApi->unlockInstanceBuffer(h); }
	static void free(handle_t h) { g_renderApi->deleteInstanceBuffer(h); }
};


struct DynamicBuf {
	handle_t buffer;
	int offset;
	void *writePtr;
};

template <int PageSize, class Traits>
class ChainedBuffer
{
public:
	ChainedBuffer()
	{
		m_curIndex = -1;
	}

	virtual ~ChainedBuffer()
	{}

	int getSize() const { return m_pages.size() * PageSize; }
	int getUsed() const { if (m_curIndex < 0) return 0; else return m_curIndex * PageSize + m_curOffset; }

	void beginFrame()
	{
		m_isFrameAllocating = true;
		m_curIndex = -1;
		m_curOffset = 0;
		m_curPointer = 0;
		nextPage();
	}

	DynamicBuf frameAlloc(int datasize)
	{
		AX_ASSERT(m_isFrameAllocating);
		// check if need change page
		DynamicBuf result;

		if (datasize > PageSize) {
			Errorf("datasize too large");
		}

		if (m_curOffset + datasize > PageSize) {
			nextPage();
		}

		result.buffer = m_pages[m_curIndex];
		result.offset = m_curOffset;
		result.writePtr = (byte_t*)m_curPointer+m_curOffset;

		m_curOffset += datasize;

		return result;
	}

	void endFrame()
	{
		AX_ASSERT(m_isFrameAllocating);
		if (m_curIndex>=0) {
			m_pages[m_curIndex]->Unlock();
		}

		m_isFrameAllocating = false;
	}

	DynamicBuf appendAlloc(int datasize)
	{
		AX_ASSERT(!m_isFrameAllocating);
		// check if need change page
		DynamicBuf result;

		if (datasize > PageSize) {
			Errorf("datasize too large");
		}

		if (m_curOffset + datasize > PageSize) {
			nextPage(true);
		}

		result.buffer = m_pages[m_curIndex];
		result.offset = m_curOffset;
		result.buffer->Lock(m_curOffset, datasize, &result.writePtr, lockAppend);

		m_curOffset += datasize;

		return result;
	}

	void endAppend()
	{
		m_pages[m_curIndex]->Unlock();
	}

	void clear() {}

protected:
	void nextPage(bool isAppend = false)
	{
		if (m_curIndex >= 0) {
			m_pages[m_curIndex]->Unlock();
			m_curPointer = 0;
		}

		m_curIndex++;

		if (s2i(m_pages.size()) <= m_curIndex) {
			m_pages.push_back(CreatePage(PageSize));
		}

		HRESULT hr;
		m_curOffset = 0;
		V(m_pages[m_curIndex]->Lock(0, 0, &m_curPointer, lockNewFrame));
		if (isAppend)
			V(m_pages[m_curIndex]->Unlock())

		AX_ASSERT(m_curPointer);
	}

public:
	Sequence<handle_t> m_pages;
	int m_curIndex;
	int m_curOffset;
	void *m_curPointer;
	bool m_isFrameAllocating;
};

class VertexBuffer {};
class IndexBuffer {};
class InstanceBuffer {};

class BufferManager
{
public:
	typedef ChainedBuffer<1024*1024, VertexBufferTraits> VertexBufferChain;
	typedef ChainedBuffer<256*1024, InstanceBufferTraits> InstanceBufferChain;
	typedef ChainedBuffer<512*1024, IndexBufferTraits> IndexBufferChain;

	void beginAlloc();
	DynamicBuf allocVb(int datasize);
	DynamicBuf allocInstance(int datasize);
	DynamicBuf allocIb(int datasize);
	void endAlloc();

private:
	VertexBufferChain m_vertexBufferChain;
	InstanceBufferChain m_instanceBufferChain;
	IndexBufferChain m_indexBufferChain;
};

AX_END_NAMESPACE

#endif // AX_RENDER_VERTEXBUFFER_H
