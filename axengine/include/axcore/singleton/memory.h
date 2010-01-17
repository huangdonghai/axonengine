/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_MEMORY_H
#define AX_MEMORY_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class MemoryHeap
//--------------------------------------------------------------------------

struct MemoryInfo {
	int totalAllocated;
	int smallAllocated;
	int mediumAllocated;
	int largeAllocated;

	int totalBlocks;
	int smallBlocks;
	int mediumBlocks;
	int largeBlocks;

	int frameTotalAllocated;
	int frameSmallAllocated;
	int frameMediumAllocated;
	int frameLargeAllocated;

	int frameTotalBlocks;
	int frameSmallBlocks;
	int frameMediumBlocks;
	int frameLargeBlocks;

	void allocSmall(int bytes);
	void freeSmall(int bytes);
	void allocMedium(int bytes);
	void freeMedium(int bytes);
	void allocLarge(int bytes);
	void freeLarge(int bytes);
	void resetFrame();
};

inline void MemoryInfo::allocSmall(int bytes) {
	totalAllocated += bytes;
	totalBlocks++;
	frameTotalAllocated += bytes;
	frameTotalBlocks++;

	smallAllocated += bytes;
	smallBlocks++;
	frameSmallAllocated += bytes;
	frameSmallBlocks++;
}

inline void MemoryInfo::freeSmall(int bytes) {
	totalAllocated -= bytes;
	totalBlocks--;
	frameTotalAllocated -= bytes;
	frameTotalBlocks--;

	smallAllocated -= bytes;
	smallBlocks--;
	frameSmallAllocated -= bytes;
	frameSmallBlocks--;
}

inline void MemoryInfo::allocMedium(int bytes) {
	totalAllocated += bytes;
	totalBlocks++;
	frameTotalAllocated += bytes;
	frameTotalBlocks++;

	mediumAllocated += bytes;
	mediumBlocks++;
	frameMediumAllocated += bytes;
	frameMediumBlocks++;
}

inline void MemoryInfo::freeMedium(int bytes) {
	totalAllocated -= bytes;
	totalBlocks--;
	frameTotalAllocated -= bytes;
	frameTotalBlocks--;

	mediumAllocated -= bytes;
	mediumBlocks--;
	frameMediumAllocated -= bytes;
	frameMediumBlocks--;
}

inline void MemoryInfo::allocLarge(int bytes) {
	totalAllocated += bytes;
	totalBlocks++;
	frameTotalAllocated += bytes;
	frameTotalBlocks++;

	largeAllocated += bytes;
	largeBlocks++;
	frameLargeAllocated += bytes;
	frameLargeBlocks++;
}

inline void MemoryInfo::freeLarge(int bytes) {
	totalAllocated -= bytes;
	totalBlocks--;
	frameTotalAllocated -= bytes;
	frameTotalBlocks--;

	largeAllocated -= bytes;
	largeBlocks--;
	frameLargeAllocated -= bytes;
	frameLargeBlocks--;
}

inline void MemoryInfo::resetFrame() {
	frameTotalAllocated = 0;
	frameSmallAllocated = 0;
	frameMediumAllocated = 0;
	frameLargeAllocated = 0;

	frameTotalBlocks = 0;
	frameSmallBlocks = 0;
	frameMediumBlocks = 0;
	frameLargeBlocks = 0;
}

AX_API extern MemoryInfo gMemoryInfo;

class MemoryHeap : public ThreadSafe {
public:
	struct Page;

	MemoryHeap();
	~MemoryHeap();

	void initialize();
	void finalize();

	void *alloc(uint_t size);
	void free(void *p);
	void *alloc16(uint_t size);			// allocate 16 byte aligned memory
	void free16(void *p);				// free 16 byte aligned memory

	// move to public for MemoryStack use
	Page *allocatePage(uint_t bytes);
	void freePage(Page *p);

protected:

	void *smallAllocate(uint_t bytes);
	void smallFree(void *ptr);

	void *mediumAllocateFromPage(Page *p, uint_t sizeNeeded);
	void *mediumAllocate(uint_t bytes);
	void mediumFree(void *ptr);

	void *largeAllocate(uint_t bytes);
	void largeFree(void *ptr);

	void releaseSwappedPages(void);
	void freePageReal(Page *p);

private:
	bool m_initialized;
	void *m_smallFirstFree[256];
	Page *m_smallCurPage;			
	uint_t m_smallCurPageOffset;			
	Page *m_smallFirstUsedPage;		

	Page *m_mediumFirstFreePage;	
	Page *m_mediumLastFreePage;		
	Page *m_mediumFirstUsedPage;	

	Page *m_largeFirstUsedPage;		

	Page *m_swapPage;

	uint_t m_pagesAllocated;				

	uint_t m_pageRequests;				
	uint_t m_osallocs;					
};

//------------------------------------------------------------------------------
// for C style api
//------------------------------------------------------------------------------


AX_API void *Malloc(size_t size);
AX_API void Free(void *ptr);
AX_API void *Malloc16(size_t size);
AX_API void Free16(void *ptr);

template<typename T>
T *TypeAlloc(size_t count = 1) {
	return (T*)Malloc(count * sizeof(T));
}

template<typename T>
void TypeFree(T*& ptr) {
	if (!ptr) {
		return;
	}
	Free(ptr);
	ptr = NULL;
}

template<typename T>
T *TypeNew() {
	T *p = TypeAlloc<T>(1);

	new(p) T;

	return p;
}

template<typename T, typename Arg0>
T *TypeNew(Arg0 arg1) {
	T *p = TypeAlloc<T>(1);

	new(p) T(arg1);

	return p;
}

template<typename T>
void TypeDelete(T*& ptr) {
	if (ptr) {
		ptr->~T();
		TypeFree(ptr);
	}
}

template<typename T>
T *TypeAlloc16(size_t count = 1) {
	return (T*)Malloc16(count * sizeof(T));
}

template<typename T>
void TypeFree16(T*& ptr) {
	if (!ptr)
		return;

	Free16(ptr);
	ptr = NULL;
}

template<typename T>
void TypeFreeContainer(T &t) {
	T::iterator it = t.begin();
	for (; it != t.end(); ++it) {
		TypeFree(*it);
	}
}

template<typename T>
void TypeZero(T *type) {
	memset(type, 0, sizeof(T));
}

template<typename T, size_t size>
void TypeZeroArray(T (&type)[size]) {
	memset(type, 0, sizeof(T)*size);
}

//------------------------------------------------------------------------------
// class MemoryStack
// 
// a simple, fast linear memory stack. SetMark and PopMark management allocation
// position tag. SetMark add a tag to current position, and PopMark pop to this
// position. between SetMark and PopMark, you can just alloc like a heap
//------------------------------------------------------------------------------

class AX_API MemoryStack {
public:
	struct Mark;

	MemoryStack();
	~MemoryStack();

	void initialize();
	void finalize();

	uint_t setMark();					// return pos for PopMark, for internal check
	void *alloc(uint_t size);
	void *alloc16(uint_t size);		// allocate 16 byte aligned memory
	void popMark(uint_t pos);

	void clear();

protected:
	void clearUnusedPages();

private:
	bool m_initialized;
	MemoryHeap::Page *m_curPage;
	Mark *m_markStack;
	uint_t m_curPos;
	uint_t m_curPagePos;
};

struct AutoMemoryMark {
	uint_t mark;
	MemoryStack *stack;

	inline AutoMemoryMark(MemoryStack *_stack) : mark(stack->setMark()), stack(_stack) {}
	inline ~AutoMemoryMark() { stack->popMark(mark); }
};

#define SCOPE_ALLOC AutoMemoryMark _auto_memory_mark(gMemoryStack);


//------------------------------------------------------------------------------
// Allocator for STL
//------------------------------------------------------------------------------

template<class T>
class Allocator : public std::allocator<T> {
	typedef std::allocator<T>		_base;
public:
	typedef typename _base::value_type 		value_type;
	typedef typename _base::pointer pointer;
	typedef typename _base::reference reference;
	typedef typename _base::const_pointer const_pointer;
	typedef typename _base::const_reference const_reference;
	typedef typename _base::size_type size_type;
	typedef typename _base::difference_type difference_type;
	
	template<class _Other>
	struct rebind {	// convert an Allocator<T> to an Allocator <_Other>
		typedef Allocator<_Other> other;
	};

	Allocator() {}

	Allocator(const Allocator<T>&) {}

	template<class _Other>
	Allocator(const Allocator<_Other>&) {}

	template<class _Other>
	Allocator<T>& operator=(const Allocator<_Other>&) {
		return (*this);
	}

	void deallocate(pointer _Ptr, size_type) {	// deallocate object at _Ptr, ignore size
		Axon::Free(_Ptr);
	}

	pointer allocate(size_type _Count) {	// allocate array of _Count elements
		return (pointer)(Axon::Malloc(_Count*sizeof(T)));
	}

	pointer allocate(size_type _Count, const void  *) {	// allocate array of _Count elements, ignore hint
		return (allocate(_Count));
	}
};

//------------------------------------------------------------------------------
// FreeList
//------------------------------------------------------------------------------

template< class T >
class FreeList {
public:
	FreeList() { m_head = nullptr; }
	~FreeList() {}

	void add(T &t);
	T *get();

private:
	void *m_head;
};

template< class T >
void FreeList<T>::add(T &t) {
	void** p = (void**)&t;
	*p = m_head;
	m_head = p;
}

template< class T >
T *FreeList<T>::get() {
	if (m_head == nullptr) {
		return 0;
	}

	T *result = (T*)m_head;
	m_head = *(intptr_t**)m_head;
	return (T*)result;
}

//------------------------------------------------------------------------------
// BlockAlloc, c-tor and d-tor are correct called
//------------------------------------------------------------------------------

template<class T, int blockSize=32>
class BlockAlloc {
public:
	BlockAlloc(void);
	~BlockAlloc(void);

	void clear(void);

	T *alloc(void);
	void free(T *element);

	int getTotalCount(void) const { return total; }
	int getAllocCount(void) const { return active; }
	int getFreeCount(void) const { return total - active; }

private:
	struct Element {
		Element *next;
		T t;
	};

	struct Block {
		Element elements[blockSize];
		Block *next;
	};

	Block *blocks;
	Element *freelist;
	int total;
	int active;
};

template<class T, int blockSize>
BlockAlloc<T,blockSize>::BlockAlloc(void) {
	blocks = NULL;
	freelist = NULL;
	total = active = 0;
}

template<class T, int blockSize>
BlockAlloc<T,blockSize>::~BlockAlloc(void) {
	clear();
}

template<class T, int blockSize>
T *BlockAlloc<T,blockSize>::alloc(void) {
	if (!freelist) {
		Block *block = TypeAlloc<Block>();
		block->next = blocks;
		blocks = block;
		for (int i = 0; i < blockSize; i++) {
			block->elements[i].next = freelist;
			freelist = &block->elements[i];
		}
		total += blockSize;
	}
	active++;
	Element *element = freelist;
	freelist = freelist->next;
	element->next = NULL;

	// call c-tor
	new(&element->t) T();
	return &element->t;
}

template<class T, int blockSize>
void BlockAlloc<T,blockSize>::free(T *t) {
	t->~T();

	Element *element = (Element *)(((unsigned char *) t) - ((int) &((Element *)0)->t));
	element->next = freelist;
	freelist = element;
	active--;
}

template<class T, int blockSize>
void BlockAlloc<T,blockSize>::clear(void) {
	while (blocks) {
		Block *block = blocks;
		blocks = blocks->next;
		TypeFree(block);
	}
	blocks = NULL;
	freelist = NULL;
	total = active = 0;
}


AX_END_NAMESPACE


//------------------------------------------------------------------------------
// new and delete operator use our memory stack
//
// memory stack don't need delete
//------------------------------------------------------------------------------

inline void *operator new(size_t count, Axon::MemoryStack *stack) {
	return stack->alloc((Axon::uint_t)count);
}

inline void *operator new[](size_t count, Axon::MemoryStack *stack) {
	return stack->alloc((Axon::uint_t)count);
}

inline void operator delete(void *p, Axon::MemoryStack *stack) {
	// do nothing
}

inline void operator delete[](void *p, Axon::MemoryStack *stack) {
	// do nothing
}

#if 0

void *operator new(size_t count);
void *operator new[](size_t count);
void operator delete(void *p);
void operator delete[](void *p);

#else

inline void *operator new(size_t count) {
	return Axon::Malloc(count);
}

inline void *operator new[](size_t count) {
	return Axon::Malloc(count);
}

inline void operator delete(void *p) {
	return Axon::Free(p);
}

inline void operator delete[](void *p) {
	return Axon::Free(p);
}

#endif

#endif // AX_MEMORY_H

