#ifndef AX_RENDER_VERTEXBUFFER_H
#define AX_RENDER_VERTEXBUFFER_H

AX_BEGIN_NAMESPACE

enum ElementType {
	ElementType_PointList,
	ElementType_LineList,
	ElementType_TriList,
	ElementType_TriStrip
};

class VertexBufferWrap
{
public:
	VertexBufferWrap();

private:
	Primitive::Hint m_hint;
	Handle m_h;
	int m_dataSize;
};

class IndexBufferWrap
{
public:
	IndexBufferWrap();

private:
	Handle m_h;
	int m_dataSize;
	int m_count;
	int m_activeCount;
	int m_offset;		// index offset, not byte offset
};


extern ApiWrapper *g_apiWrapper;

class VertexBufferTraits
{
public:
	static Handle create(int size) { return Handle(0); /*g_apiWrapper->createVertexBuffer(size, Primitive2::HintDynamic);*/ }
//	static void *lock(Handle h) { g_apiWrapper->lockVertexBuffer(h); }
//	static void unlock(Handle h) { g_apiWrapper->unlockVertexBuffer(h); }
//	static void free(Handle h) { g_apiWrapper->deleteVertexBuffer(h); }
};

class IndexBufferTraits
{
public:
	static Handle create(int size) { return Handle(0); /*g_renderApi->createIndexBuffer(size, Primitive2::HintDynamic);*/ }
//	static void *lock(Handle h) { g_apiWrapper->lockIndexBuffer(h); }
//	static void unlock(Handle h) { g_apiWrapper->unlockIndexBuffer(h); }
//	static void free(Handle h) { g_apiWrapper->deleteIndexBuffer(h); }
};

class InstanceBufferTraits
{
public:
	static Handle create(int size) { return Handle(0);/*g_apiWrapper->createInstanceBuffer(size, Primitive2::HintDynamic);*/ }
//	static void *lock(Handle h) { g_apiWrapper->lockInstanceBuffer(h); }
//	static void unlock(Handle h) { g_apiWrapper->unlockInstanceBuffer(h); }
//	static void free(Handle h) { g_apiWrapper->deleteInstanceBuffer(h); }
};


struct DynamicBuf {
	Handle buffer;
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
	Sequence<Handle> m_pages;
	int m_curIndex;
	int m_curOffset;
	void *m_curPointer;
	bool m_isFrameAllocating;
};

class InstanceBuffer
{};

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
