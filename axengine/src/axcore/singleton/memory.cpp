/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

#define USE_LIBC_MALLOC 0

namespace Axon {

	MemoryInfo gMemoryInfo;

	MemoryHeap* xGetHeap() {
		static bool initialized = false;
		static MemoryHeap heap;
		if (!initialized) {
			heap.initialize();

			initialized = true;
		}
		return &heap;
	}

	struct MemoryHeap::Page {								// allocation page
		void* data;					// data pointer to allocated memory
		uint_t dataSize;				// number of bytes of memory 'data' points to
		Page* next;					// next free page in same page manager
		Page* prev;					// used only when allocated
		uint_t largestFree;			// this data used by the medium-size heap manager
		void* firstFree;				// pointer to first free entry
	};

	//------------------------------------------------------------------------------
	// class MemoryHeap
	//------------------------------------------------------------------------------

	#define SMALL_HEADER_SIZE ((uint_t) (sizeof(byte_t) + sizeof(byte_t)))
	#define MEDIUM_HEADER_SIZE ((uint_t) (sizeof(MediumHeapEntry) + sizeof(byte_t)))
	#define LARGE_HEADER_SIZE ((uint_t) (sizeof(uint_t*) + sizeof(byte_t)))

	#define ALIGN_SIZE(bytes)		(((bytes) + ALIGN - 1) & ~(ALIGN - 1))
	#define SMALL_ALIGN(bytes)	(ALIGN_SIZE((bytes) + SMALL_HEADER_SIZE) - SMALL_HEADER_SIZE)
	#define MEDIUM_SMALLEST_SIZE (ALIGN_SIZE(256) + ALIGN_SIZE(MEDIUM_HEADER_SIZE))

	enum {
		ALIGN = 8,									// memory alignment in bytes
		PAGE_SIZE = 65536 - sizeof(MemoryHeap::Page),
		INVALID_ALLOC = 0xdd,
		SMALL_ALLOC = 0xaa,						// small allocation
		MEDIUM_ALLOC = 0xbb,						// medium allocation
		LARGE_ALLOC = 0xcc,						// large allocation

		SMALL_BLOCK_MAX = 256,						// bigger than this that is medium memory block
		MEDIUM_BLOCK_MAX = 32768,					// bigger than this that is large memory block
	};


	struct MediumHeapEntry {
		MemoryHeap::Page* page;					// pointer to page
		uint_t size;					// size of block
		MediumHeapEntry* prev;					// previous block
		MediumHeapEntry* next;					// next block
		MediumHeapEntry* prevFree;				// previous free block
		MediumHeapEntry* nextFree;				// next free block
		uint_t freeBlock;				// non-zero if free block
	};

	MemoryHeap::MemoryHeap()
		: m_initialized(false)
	{
	}
	MemoryHeap::~MemoryHeap() {
	}

	void MemoryHeap::initialize() {
		SCOPE_LOCK;

		if (m_initialized)
			return;

		m_osallocs = 0;
		m_pageRequests = 0;
		m_pagesAllocated = 0;								// reset page allocation counter

		m_largeFirstUsedPage = NULL;								// init large heap manager
		m_swapPage = NULL;

		memset(m_smallFirstFree, 0, sizeof(m_smallFirstFree));	// init small heap manager
		m_smallFirstUsedPage = NULL;
		m_smallCurPage = allocatePage(PAGE_SIZE);
		AX_ASSERT(m_smallCurPage);
		m_smallCurPageOffset = SMALL_ALIGN(0);

		m_mediumFirstFreePage = NULL;							// init medium heap manager
		m_mediumLastFreePage = NULL;
		m_mediumFirstUsedPage = NULL;

		m_initialized = true;
	}

	void MemoryHeap::finalize() {
		SCOPE_LOCK;

		return;

		MemoryHeap::Page* p;

		if (m_smallCurPage) {
			freePage(m_smallCurPage);				// free small-heap current allocation page
		}
		p = m_smallFirstUsedPage;					// free small-heap allocated pages 
		while (p) {
			MemoryHeap::Page* next = p->next;
			freePage(p);
			p= next;
		}

		p = m_largeFirstUsedPage;					// free large-heap allocated pages
		while (p) {
			MemoryHeap::Page* next = p->next;
			freePage(p);
			p = next;
		}

		p = m_mediumFirstFreePage;					// free medium-heap allocated pages
		while (p) {
			MemoryHeap::Page* next = p->next;
			freePage(p);
			p = next;
		}

		p = m_mediumFirstUsedPage;					// free medium-heap allocated completely used pages
		while (p) {
			MemoryHeap::Page* next = p->next;
			freePage(p);
			p = next;
		}

		releaseSwappedPages();			

		AX_ASSERT(m_pagesAllocated == 0);
	}

	void* MemoryHeap::alloc(uint_t size) {
		SCOPE_LOCK;

		if (!m_initialized)
			initialize();

		if (!size) {
			return NULL;
		}

	#if USE_LIBC_MALLOC
		gMemoryInfo.allocLarge(0);
		return ::malloc(size);
	#else
		if (size < SMALL_BLOCK_MAX) {
			return smallAllocate(size);
		}
		if (size < MEDIUM_BLOCK_MAX) {
			return mediumAllocate(size);
		}
		return largeAllocate(size);
	#endif
	}

	void MemoryHeap::free(void* p) {
		SCOPE_LOCK;

		if (!p) {
			return;
		}

	#if USE_LIBC_MALLOC
		gMemoryInfo.freeLarge(0);
		::free(p);
	#else
		switch (((byte_t *)(p))[-1]) {
		case SMALL_ALLOC:
			smallFree(p);
			break;
		case MEDIUM_ALLOC:
			mediumFree(p);
			break;
		case LARGE_ALLOC:
			largeFree(p);
			break;
		default:
			Errorf(_("MemoryHeap::Free: invalid memory block"));
			break;
		}
	#endif
	}

	void* MemoryHeap::alloc16(uint_t size) {
		SCOPE_LOCK;

		byte_t *ptr, *alignedPtr;

		ptr = (byte_t *) alloc(size + 16 + 4);
		alignedPtr = (byte_t *) (((size_t) ptr) + 15 & ~15);
		if (alignedPtr - ptr < 4) {
			alignedPtr += 16;
		}
		*((size_t *)(alignedPtr - 4)) = (size_t) ptr;
		return (void *) alignedPtr;
	}

	void MemoryHeap::free16(void* p) {
		SCOPE_LOCK;

		Free((void *) *((size_t *) (((byte_t *) p) - 4)));
	}

	MemoryHeap::Page* MemoryHeap::allocatePage(uint_t bytes) {
		SCOPE_LOCK;

		MemoryHeap::Page* p;

		m_pageRequests++;

		if (m_swapPage && m_swapPage->dataSize == bytes) {			// if we've got a swap page somewhere
			p = m_swapPage;
			m_swapPage = NULL;
		} else {
			uint_t size;

			size = bytes + sizeof(MemoryHeap::Page);

			p = (MemoryHeap::Page *) ::malloc(size + ALIGN - 1);

			AX_ASSERT(p);

			p->data = (void *) ALIGN_SIZE((size_t)((byte_t *)(p)) + sizeof(MemoryHeap::Page));
			p->dataSize = size - sizeof(MemoryHeap::Page);
			p->firstFree = NULL;
			p->largestFree = 0;
			m_osallocs++;
		}

		p->prev = NULL;
		p->next = NULL;

		m_pagesAllocated++;

		return p;
	}

	void MemoryHeap::freePage(MemoryHeap::Page* p) {
		SCOPE_LOCK;

		AX_ASSERT(p);

		if (p->dataSize == PAGE_SIZE && !m_swapPage) {			// add to swap list?
			m_swapPage = p;
		} else {
			freePageReal(p);
		}

		m_pagesAllocated--;
	}

	void* MemoryHeap::smallAllocate(uint_t bytes) {
		// we need the at least sizeof(uint_t) bytes for the free list
		if (bytes < sizeof(uint_t)) {
			bytes = sizeof(uint_t);
		}

		// increase the number of bytes if necessary to make sure the next small allocation is aligned
		bytes = SMALL_ALIGN(bytes);

		gMemoryInfo.allocSmall((bytes / ALIGN+1)*ALIGN);

		byte_t *smallBlock = (byte_t *)(m_smallFirstFree[bytes / ALIGN]);
		if (smallBlock) {
			uintptr_t *link = (uintptr_t *)(smallBlock + SMALL_HEADER_SIZE);
			smallBlock[1] = SMALL_ALLOC;					// allocation identifier
			m_smallFirstFree[bytes / ALIGN] = (void *)(*link);
			return (void *)(link);
		}

		uint_t bytesLeft = (long)(PAGE_SIZE) - m_smallCurPageOffset;
		// if we need to allocate a new page
		if (bytes >= bytesLeft) {
			m_smallCurPage->next = m_smallFirstUsedPage;
			m_smallFirstUsedPage = m_smallCurPage;
			m_smallCurPage = allocatePage(PAGE_SIZE);
			if (!m_smallCurPage) {
				return NULL;
			}
			// make sure the first allocation is aligned
			m_smallCurPageOffset = SMALL_ALIGN(0);
		}

		smallBlock = ((byte_t *)m_smallCurPage->data) + m_smallCurPageOffset;
		smallBlock[0]		= (byte_t)(bytes / ALIGN);		// write # of bytes/ALIGN
		smallBlock[1]		= SMALL_ALLOC;					// allocation identifier
		m_smallCurPageOffset  += bytes + SMALL_HEADER_SIZE;	// increase the offset on the current page
		return (smallBlock + SMALL_HEADER_SIZE);			// skip the first two bytes
	}

	void MemoryHeap::smallFree(void *ptr) {
		((byte_t *)(ptr))[-1] = INVALID_ALLOC;

		byte_t *d = ((byte_t *)ptr) - SMALL_HEADER_SIZE;
		uint_t *dt = (uint_t *)ptr;
		// index into the table with free small memory blocks
		uint_t ix = *d;

		// check if the index is correct
		if (ix > (SMALL_BLOCK_MAX / ALIGN)) {
			Errorf(_("smallFree: invalid memory block"));
		}

		gMemoryInfo.freeSmall((ix+1) * ALIGN);

		*dt = (uintptr_t)m_smallFirstFree[ix];	// write next index
		m_smallFirstFree[ix] = (void *)d;		// link
	}

	void* MemoryHeap::mediumAllocateFromPage(MemoryHeap::Page* p, uint_t sizeNeeded) {
		MediumHeapEntry* best, *nw = NULL;
		byte_t* ret;

		best = (MediumHeapEntry *)(p->firstFree);			// first block is largest

		AX_ASSERT(best);
		AX_ASSERT(best->size == p->largestFree);
		AX_ASSERT(best->size >= sizeNeeded);

		// if we can allocate another block from this page after allocating sizeNeeded bytes
		if (best->size >= (uint_t)(sizeNeeded + MEDIUM_SMALLEST_SIZE)) {
			nw = (MediumHeapEntry *)((byte_t *)best + best->size - sizeNeeded);
			nw->page = p;
			nw->prev = best;
			nw->next = best->next;
			nw->prevFree = NULL;
			nw->nextFree = NULL;
			nw->size = sizeNeeded;
			nw->freeBlock = 0;			// used block
			if (best->next) {
				best->next->prev = nw;
			}
			best->next = nw;
			best->size -= sizeNeeded;

			p->largestFree = best->size;
		} else {
			if (best->prevFree) {
				best->prevFree->nextFree = best->nextFree;
			} else {
				p->firstFree = (void *)best->nextFree;
			}
			if (best->nextFree) {
				best->nextFree->prevFree = best->prevFree;
			}

			best->prevFree  = NULL;
			best->nextFree  = NULL;
			best->freeBlock = 0;			// used block
			nw = best;

			// change by mahdi
			if (p->firstFree) {
				p->largestFree = ((MediumHeapEntry *)(p->firstFree))->size;
			} else {
				p->largestFree = 0;
			}
		}

		gMemoryInfo.allocMedium(nw->size);

		ret = (byte_t *)(nw) + ALIGN_SIZE(MEDIUM_HEADER_SIZE);
		ret[-1] = MEDIUM_ALLOC;		// allocation identifier

		return (void *)(ret);

	}

	void* MemoryHeap::mediumAllocate(uint_t bytes) {
		MemoryHeap::Page* p;
		void* data;

		uint_t sizeNeeded = ALIGN_SIZE(bytes) + ALIGN_SIZE(MEDIUM_HEADER_SIZE);

		// find first page with enough space
		for (p = m_mediumFirstFreePage; p; p = p->next) {
			if (p->largestFree >= sizeNeeded) {
				break;
			}
		}

		if (!p) {								// need to allocate new page?
			p = allocatePage(PAGE_SIZE);
			if (!p) {
				return NULL;					// malloc failure!
			}
			p->prev = NULL;
			p->next = m_mediumFirstFreePage;
			if (p->next) {
				p->next->prev = p;
			}
			else {
				m_mediumLastFreePage = p;
			}

			m_mediumFirstFreePage = p;

			p->largestFree = PAGE_SIZE;
			p->firstFree = (void *)p->data;

			MediumHeapEntry *e;
			e = (MediumHeapEntry *)(p->firstFree);
			e->page = p;
			// make sure ((Uint8 *)e + e->size) is aligned
			e->size = PAGE_SIZE & ~(ALIGN - 1);
			e->prev = NULL;
			e->next = NULL;
			e->prevFree = NULL;
			e->nextFree = NULL;
			e->freeBlock = 1;
		}

		data = mediumAllocateFromPage(p, sizeNeeded);		// allocate data from page

		// if the page can no longer serve memory, move it away from free list
		// (so that it won't slow down the later alloc queries)
		// this modification speeds up the pageWalk from O(N) to O(sqrt(N))
		// a call to free may swap this page back to the free list

		if (p->largestFree < MEDIUM_SMALLEST_SIZE) {
			if (p == m_mediumLastFreePage) {
				m_mediumLastFreePage = p->prev;
			}

			if (p == m_mediumFirstFreePage) {
				m_mediumFirstFreePage = p->next;
			}

			if (p->prev) {
				p->prev->next = p->next;
			}
			if (p->next) {
				p->next->prev = p->prev;
			}

			// link to "completely used" list
			p->prev = NULL;
			p->next = m_mediumFirstUsedPage;
			if (p->next) {
				p->next->prev = p;
			}
			m_mediumFirstUsedPage = p;
			return data;
		} 

		// re-order linked list (so that next malloc query starts from current
		// matching block) -- this speeds up both the page walks and block walks

		if (p != m_mediumFirstFreePage) {
			AX_ASSERT(m_mediumLastFreePage);
			AX_ASSERT(m_mediumFirstFreePage);
			AX_ASSERT(p->prev);

			m_mediumLastFreePage->next = m_mediumFirstFreePage;
			m_mediumFirstFreePage->prev = m_mediumLastFreePage;
			m_mediumLastFreePage = p->prev;
			p->prev->next = NULL;
			p->prev = NULL;
			m_mediumFirstFreePage = p;
		}

		return data;

	}

	void MemoryHeap::mediumFree(void *ptr) {
		((byte_t *)(ptr))[-1] = INVALID_ALLOC;

		MediumHeapEntry* e = (MediumHeapEntry *)((byte_t *)ptr - ALIGN_SIZE(MEDIUM_HEADER_SIZE));
		MemoryHeap::Page* p = e->page;
		bool isInFreeList;

		gMemoryInfo.freeMedium(e->size);

		isInFreeList = p->largestFree >= MEDIUM_SMALLEST_SIZE;

		AX_ASSERT(e->size);
		AX_ASSERT(e->freeBlock == 0);

		MediumHeapEntry *prev = e->prev;

		// if the previous block is free we can merge
		if (prev && prev->freeBlock) {
			prev->size += e->size;
			prev->next = e->next;
			if (e->next) {
				e->next->prev = prev;
			}
			e = prev;
		} else {
			e->prevFree = NULL;				// link to beginning of free list
			e->nextFree = (MediumHeapEntry *)p->firstFree;
			if (e->nextFree) {
				AX_ASSERT(!(e->nextFree->prevFree));
				e->nextFree->prevFree = e;
			}

			p->firstFree = e;
			p->largestFree = e->size;
			e->freeBlock = 1;				// mark block as free
		}

		MediumHeapEntry *next = e->next;

		// if the next block is free we can merge
		if (next && next->freeBlock) {
			e->size += next->size;
			e->next = next->next;

			if (next->next) {
				next->next->prev = e;
			}

			if (next->prevFree) {
				next->prevFree->nextFree = next->nextFree;
			} else {
				AX_ASSERT(next == p->firstFree);
				p->firstFree = next->nextFree;
			}

			if (next->nextFree) {
				next->nextFree->prevFree = next->prevFree;
			}
		}

		if (p->firstFree) {
			p->largestFree = ((MediumHeapEntry *)(p->firstFree))->size;
		} else {
			p->largestFree = 0;
		}

		// did e become the largest block of the page ?

		if (e->size > p->largestFree) {
			AX_ASSERT(e != p->firstFree);
			p->largestFree = e->size;

			if (e->prevFree) {
				e->prevFree->nextFree = e->nextFree;
			}
			if (e->nextFree) {
				e->nextFree->prevFree = e->prevFree;
			}

			e->nextFree = (MediumHeapEntry *)p->firstFree;
			e->prevFree = NULL;
			if (e->nextFree) {
				e->nextFree->prevFree = e;
			}
			p->firstFree = e;
		}

		// if page wasn't in free list (because it was near-full), move it back there
		if (!isInFreeList) {

			// remove from "completely used" list
			if (p->prev) {
				p->prev->next = p->next;
			}
			if (p->next) {
				p->next->prev = p->prev;
			}
			if (p == m_mediumFirstUsedPage) {
				m_mediumFirstUsedPage = p->next;
			}

			p->next = NULL;
			p->prev = m_mediumLastFreePage;

			if (m_mediumLastFreePage) {
				m_mediumLastFreePage->next = p;
			}
			m_mediumLastFreePage = p;
			if (!m_mediumFirstFreePage) {
				m_mediumFirstFreePage = p;
			}
		} 
	}

	void* MemoryHeap::largeAllocate(uint_t bytes) {
		MemoryHeap::Page *p = allocatePage(bytes + ALIGN_SIZE(LARGE_HEADER_SIZE));

		gMemoryInfo.allocLarge(p->dataSize);

		AX_ASSERT(p);

		if (!p) {
			return NULL;
		}

		byte_t* d = (byte_t*)(p->data) + ALIGN_SIZE(LARGE_HEADER_SIZE);
		uint_t* dw = (uint_t*)(d - ALIGN_SIZE(LARGE_HEADER_SIZE));
		dw[0]	= (uintptr_t)p;				// write pointer back to page table
		d[-1]	= LARGE_ALLOC;			// allocation identifier

		// link to 'large used page list'
		p->prev = NULL;
		p->next = m_largeFirstUsedPage;
		if (p->next) {
			p->next->prev = p;
		}
		m_largeFirstUsedPage = p;

		return (void *)(d);

	}
	void MemoryHeap::largeFree(void *ptr) {
		MemoryHeap::Page* pg;

		((byte_t *)(ptr))[-1] = INVALID_ALLOC;

		// get page pointer
		pg = (MemoryHeap::Page *)(*((uintptr_t *)(((byte_t *)ptr) - ALIGN_SIZE(LARGE_HEADER_SIZE))));

		gMemoryInfo.freeLarge(pg->dataSize);

		// unlink from doubly linked list
		if (pg->prev) {
			pg->prev->next = pg->next;
		}
		if (pg->next) {
			pg->next->prev = pg->prev;
		}
		if (pg == m_largeFirstUsedPage) {
			m_largeFirstUsedPage = pg->next;
		}
		pg->next = pg->prev = NULL;

		freePage(pg);

	}

	void MemoryHeap::releaseSwappedPages(void) {
		if (m_swapPage) {
			freePageReal(m_swapPage);
		}
		m_swapPage = NULL;

	}
	void MemoryHeap::freePageReal(MemoryHeap::Page* p) {
		AX_ASSERT(p);
		::free(p);
	}



	//------------------------------------------------------------------------------
	// class MemoryStack
	//------------------------------------------------------------------------------

	enum {
		STACK_PAGE_SIZE = 1024*1024
	};

	struct MemoryStack::Mark {
		uint_t pos;
		MemoryHeap::Page* page;
		uint_t offset;

		Mark* next;
	};

	MemoryStack::MemoryStack()
		: m_initialized(false)
		, m_curPage(NULL)
		, m_markStack(NULL)
		, m_curPos(0)
		, m_curPagePos(0)
	{}

	MemoryStack::~MemoryStack() {
		finalize();
	}

	void MemoryStack::initialize() {
		clear();
		m_initialized = true;
	}

	void MemoryStack::finalize() {
		clear();
	}

	size_t MemoryStack::setMark() {
		MemoryStack::Mark* mark = (MemoryStack::Mark*) Malloc(sizeof(MemoryStack::Mark));

		// record info
		mark->pos = m_curPos;
		mark->page = m_curPage;
		mark->offset = m_curPagePos;

		// setup link
		mark->next = m_markStack;
		m_markStack = mark;

		return m_markStack->pos;
	}

	void* MemoryStack::alloc(uint_t size) {
		size = ALIGN_SIZE(size);

		if (!m_curPage) {
			m_curPage = xGetHeap()->allocatePage(std::max<uint_t>(STACK_PAGE_SIZE, size));
			memset(m_curPage->data, 0, m_curPage->dataSize);
		}

		if (m_curPage->dataSize - m_curPagePos < size) {
			MemoryHeap::Page* page = xGetHeap()->allocatePage(std::max<uint_t>(STACK_PAGE_SIZE, size));
			AX_ASSERT(page->dataSize >= size);
			memset(page->data, 0, page->dataSize);
			// add to link
			page->next = m_curPage;
			m_curPage = page;
			m_curPagePos = 0;
		}

		void* data = (byte_t*)m_curPage->data + m_curPagePos;
		m_curPagePos += size;
		m_curPos += size;

		return data;
	}

	void* MemoryStack::alloc16(uint_t size) {
		byte_t *ptr, *alignedPtr;

		ptr = (byte_t *) alloc(size + 15);
		alignedPtr = (byte_t *) (((size_t) ptr) + 15 & ~15);
		return (void *) alignedPtr;
	}

	void MemoryStack::popMark(size_t pos) {
		if (!m_markStack)
			Errorf(_("MemoryStack::PopMark: no mark stack to pop"));

		if (m_markStack->pos != pos)
			Errorf(_("MemoryStack::PopMark: pop pos %d != real pos %d"), pos, m_markStack->pos);

		// pop to this pos
		m_curPage = m_markStack->page;
		m_curPos = m_markStack->pos;
		m_curPagePos = m_markStack->offset;

		// free mark
		MemoryStack::Mark* mark = m_markStack;
		m_markStack = mark->next;
		Free(mark);

		// clear unused pages
		clearUnusedPages();
	}

	void MemoryStack::clearUnusedPages() {
		MemoryHeap::Page* p = m_curPage->next;
		m_curPage->next = NULL;
		while (p) {
			MemoryHeap::Page* next = p->next;
			xGetHeap()->freePage(p);
			p = next;
		}
	}

	void MemoryStack::clear() {
		MemoryHeap::Page* p = m_curPage;
		while (p) {
			MemoryHeap::Page* next = p->next;
			xGetHeap()->freePage(p);
			p = next;
		}
		m_curPage = NULL;
		m_curPos = 0;
		m_curPagePos = 0;
	}

	//------------------------------------------------------------------------------
	// global function
	//------------------------------------------------------------------------------

	void* Malloc(size_t size) {
#if 0
		if (xGetHeap() == NULL) {
			xGetHeap() = new MemoryHeap;
			xGetHeap()->initialize();
		}
#endif 
		if (!size) {
			return NULL;
		}
		void *mem = xGetHeap()->alloc(s2i(size));
		return mem;

	}
	void Free(void* ptr) {
		if (!ptr) {
			return;
		}
		xGetHeap()->free(ptr);
	}

	void* Malloc16(size_t size) {
		if (!size) {
			return NULL;
		}
		void *mem = xGetHeap()->alloc16(s2i(size));
		// make sure the memory is 16 byte aligned
		AX_ASSERT((((size_t)mem) & 15) == 0);
		return mem;
	}

	void Free16(void* ptr) {
		if (!ptr) {
			return;
		}
		// make sure the memory is 16 byte aligned
		AX_ASSERT((((size_t)ptr) & 15) == 0);
		xGetHeap()->free16(ptr);
	}

} // namespace Axon

