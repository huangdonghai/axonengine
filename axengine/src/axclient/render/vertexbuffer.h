#ifndef AX_RENDER_VERTEXBUFFER_H
#define AX_RENDER_VERTEXBUFFER_H

AX_BEGIN_NAMESPACE

class VertexObject
{
public:
	VertexObject();
	~VertexObject();

	void init(const void *p, int count, Primitive::Hint, VertexType vt);
	void clear();

public:
	phandle_t m_h;
	Primitive::Hint m_hint;
	VertexType m_vt;
	int m_offset;		// byte offset
	int m_dataSize;
	int m_count;
	bool m_chained;
	Handle m_localHandle;
};

class InstanceObject
{
public:
	InstanceObject();
	~InstanceObject();

	void init(int numInstances, const InstancePrim::Param* params);

public:
	phandle_t m_h;
	int m_offset;
	int m_count;
};

class IndexObject
{
public:
	IndexObject();
	~IndexObject();

	void init(const ushort_t *p, int count, Primitive::Hint, ElementType et, int activeCount = 0);
	void clear();

public:
	phandle_t m_h;
	Primitive::Hint m_hint;
	ElementType m_elementType;
	int m_dataSize;
	int m_count;
	int m_activeCount;
	int m_offset;		// index offset, not byte offset
	bool m_chained;
	Handle m_localHandle;
};

struct DynamicBuf {
	phandle_t phandle;
	int offset;
	void *writePtr;
};

struct VertexBufferTraits
{
	static void allocPage(phandle_t &h, int size)
	{
		g_apiWrap->createVertexBuffer(h, size, Primitive::HintDynamic);
	}
	static void uploadPage(phandle_t h, int size, const void *p)
	{
		g_apiWrap->uploadVertexBuffer(h, size, p, 0);
	}
	static void releasePage(phandle_t h)
	{
		g_apiWrap->deleteVertexBuffer(h);
	}
};

struct IndexBufferTraits
{
	static void allocPage(phandle_t &h, int size)
	{
		g_apiWrap->createIndexBuffer(h, size, Primitive::HintDynamic);
	}
	static void uploadPage(phandle_t h, int size, const void *p)
	{
		g_apiWrap->uploadIndexBuffer(h, size, p, 0);
	}
	static void releasePage(phandle_t h)
	{
		g_apiWrap->deleteIndexBuffer(h);
	}
};

template <int PageSize, class Traits>
class ChainedBuffer
{
public:
	enum { MAX_PAGES = 16 };

	ChainedBuffer()
	{
		m_curIndex = -1;
		m_numPages = 0;
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

		result.phandle = m_pages[m_curIndex];
		result.offset = m_curOffset;
		result.writePtr = m_pageCache + m_curOffset;

		m_curOffset += datasize;

		return result;
	}

	void endFrame()
	{
		AX_ASSERT(m_isFrameAllocating);
		if (m_curIndex>=0) {
			Traits::uploadPage(m_pages[m_curIndex], m_curOffset, m_pageCache);
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
			Traits::uploadPage(m_pages[m_curIndex], m_curOffset, m_pageCache);
		}

		m_curIndex++;
		m_curOffset = 0;

		if (m_numPages <= m_curIndex) {
//			m_pages.push_back(CreatePage(PageSize));
			Traits::allocPage(m_pages[m_numPages], PageSize);
			m_numPages++;
		}
	}

public:
	phandle_t m_pages[MAX_PAGES];
	int m_numPages;
	int m_curIndex;
	int m_curOffset;
	bool m_isFrameAllocating;
	byte_t m_pageCache[PageSize];
};

class BufferManager
{
public:
	typedef ChainedBuffer<1024*1024, VertexBufferTraits> VertexBufferChain;
	typedef ChainedBuffer<256*1024, VertexBufferTraits> InstanceBufferChain;
	typedef ChainedBuffer<512*1024, IndexBufferTraits> IndexBufferChain;

	BufferManager();
	~BufferManager();

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
