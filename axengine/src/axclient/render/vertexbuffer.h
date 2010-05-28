#ifndef AX_RENDER_VERTEXBUFFER_H
#define AX_RENDER_VERTEXBUFFER_H

AX_BEGIN_NAMESPACE

class VertexObject
{
public:

public:
	Handle m_h;
	VertexType m_vt;
	int m_offset;		// byte offset
	int m_dataSize;
	int m_count;
	bool m_chained;
};

class InstanceObject
{
public:
public:
	Handle m_h;
	int m_offset;
	int m_count;
};

class IndexObject
{
public:
public:
	Handle m_h;
	ElementType m_elementType;
	int m_dataSize;
	int m_count;
	int m_activeCount;
	int m_offset;		// index offset, not byte offset
	bool m_chained;
};

struct DynamicBuf {
	Handle m_h;
	int offset;
	void *writePtr;
};

class VertexBufferTraits {};
class IndexBufferTraits {};

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
	byte_t m_pageCache[PageSize];
};

class BufferManager
{
public:
	typedef ChainedBuffer<1024*1024, VertexBufferTraits> VertexBufferChain;
	typedef ChainedBuffer<256*1024, VertexBufferTraits> InstanceBufferChain;
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
