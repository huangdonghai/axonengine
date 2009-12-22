/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

namespace {

	using namespace Axon;

	// given an unaligned pointer, round it up to align and
	// store the offset just before the returned pointer.
	static inline void* hkMemoryRoundUp(void* pvoid, int align=16) {
		char* p = reinterpret_cast<char*>(pvoid);
		char* aligned = reinterpret_cast<char*>(
			HK_NEXT_MULTIPLE_OF(align, reinterpret_cast<intptr_t>(p+1)));
		reinterpret_cast<int*>(aligned)[-1] = (int)(aligned - p);
		return aligned;
	}

	// given a pointer from hkMemoryRoundUp, recover the original pointer.
	static inline void* hkMemoryRoundDown(void* p) {
		int offset = reinterpret_cast<int*>(p)[-1];
		return static_cast<char*>(p) - offset;
	}

	class CustomMemory : public hkMemory {
	public:
		CustomMemory() { }

		void lock(); // platform specific thread lock
		void unlock(); // platform specific thread unlock

		inline void* _allocateChunk(int blockSize) {
			return hkMemoryRoundUp(::new char[blockSize+16]);
		}

		inline void* _deallocateChunk(void* p, int blockSize) {
			::delete [] static_cast<char*>(hkMemoryRoundDown(p));
		}

		virtual void allocateChunkBatch(void** ptrs, int numBlocks, int blockSize) {
			lock();
			for (int i = 0; i < numBlocks; ++i) {
				ptrs[i] = _allocateChunk(blockSize);
			}
			unlock();
		}

		virtual void deallocateChunkBatch(void** ptrs, int numBlocks, int blockSize) {
			lock();
			for (int i = 0; i < numBlocks; ++i) {
				ptrs[i] = _deallocateChunk(ptrs[i],blockSize);
			}
			unlock();
		}

		virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl) {
			lock();
			void* p = _allocateChunk(nbytes);
			unlock();
			return p;
		}

		virtual void deallocateChunk(void* p, int nbytes, HK_MEMORY_CLASS) {
			if (p) {
				lock();
				_deallocateChunk(p, nbytes);
				unlock();
			}
		}

		virtual void printStatistics(hkOstream* c) { }
	};

	// Stub function to print any error report functionality to stdout
	// std::puts(...) could be used here alternatively
	void errorReportFunction(const char* str, void* errorOutputObject) {
		Axon::Debugf("%s", str);
	}

	hkMemory* newMemoryManager(int argc, const char** argv, bool& debug,hkMemoryBlockServer*& server) {
		server = HK_NULL;

		// Debug memory option instead if you want:
		for (int i = 0; i < argc; ++i) {
			if (argv[i] && argv[i][0] == '-' && argv[i][1] == 'c') {
				hkDebugMemory* debugMem = hkDebugMemory::create();
				if (debugMem) {
					debug = true;
					return debugMem;
				}
			}
		}

		debug = false;
		// Minimum block allocation size is 1Mb
		server = new hkSystemMemoryBlockServer(1*1024*1024);
		return new hkFreeListMemory(server);
		//return new hkPoolMemory();
	}

	hkMemoryBlockServer* server = HK_NULL;

	class MyStremReader : public hkMemoryStreamReader {
	public:
		MyStremReader(const void* mem, int memSize) : hkMemoryStreamReader(mem, memSize, MEMORY_INPLACE) {}
		virtual ~MyStremReader() { Axon::g_fileSystem->freeFile(m_buf); }
	};


	class MyFilesystem : public hkFileSystem {
	public:
		virtual hkStreamReader* openReader(const char* name) {
			void* buf;
			size_t size = Axon::g_fileSystem->readFile(name, &buf);

			return new MyStremReader(buf, s2i(size));
		}
		virtual hkStreamWriter* openWriter(const char* name) {
			String ospath = g_fileSystem->dataPathToOsPath(name);
			return new hkStdioStreamWriter(ospath.c_str());
		}

	private:
//		virtual hkResult listDirectory(const char* basePath, DirectoryListing& listingOut);
	};

	MyFilesystem* myFileSystem;
}

AX_BEGIN_NAMESPACE


	PhysicsSystem::PhysicsSystem()
	{}

	PhysicsSystem::~PhysicsSystem()
	{}

	void PhysicsSystem::initialize() {
		// We start initializing hkBaseSystem
		bool debug = false;

		hkMemory* memoryManager = newMemoryManager(0, 0, debug,server);

		// When using the debug memory manager, we use a memory block cache size of 0 so that the memory
		// leak report contains useful information.
		int maxNumElemsOnFreeList = (debug ? 0 : 16);
		hkThreadMemory* threadMemory = new hkThreadMemory(memoryManager/*, maxNumElemsOnFreeList*/);

		hkBaseSystem::init(memoryManager, threadMemory, errorReportFunction);
		memoryManager->removeReference();


		// We now initialize the stack area to 2 mega bytes (fast temporary memory to be used by the engine).
		char* stackBuffer; {
			int stackSize = 2*1024*1024; // 2MB stack

			stackBuffer = hkAllocate<char>(stackSize, HK_MEMORY_CLASS_BASE);
			threadMemory->setStackArea(stackBuffer, stackSize);
		}

		myFileSystem = new MyFilesystem;

		hkFileSystem::replaceInstance(myFileSystem);

	}

	void PhysicsSystem::finalize() {
		hkResult res = hkBaseSystem::quit();

		/// If we have a server we can delete it
		// delete server;
	}

AX_END_NAMESPACE


