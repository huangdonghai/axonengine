/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9VERTEXBUFFER_H
#define AX_D3D9VERTEXBUFFER_H

namespace Axon { namespace Render {

	struct DynVb {
		IDirect3DVertexBuffer9* vb;
		int offset;
		void* writePtr;
	};

	struct DynIb {
		IDirect3DIndexBuffer9* ib;
		int offset;
		void* writePtr;
	};

	// forward declaration
	class D3D9instancedbuffer;
	class D3D9vertdecl;
	class D3D9vertexbuffermanager;

	//--------------------------------------------------------------------------
	// class D3D9vertexobject
	//--------------------------------------------------------------------------

	class D3D9vertexobject {
	public:
		enum VertexType {
			VertexGeneric,
			VertexBlend,
			VertexDebug,
			VertexChunk,

			VERTEXTYPE_NUMBER
		};

		D3D9vertexobject();
		~D3D9vertexobject();

		void setData(const void* p, int count, Primitive::Hint, VertexType vt);
		void resetData();
		void bind();
		void bindInstanced(D3D9instancedbuffer* instancedBuffer);
		void unbindInstanced();

		int getNumVerts() const { return m_count; }
		IDirect3DVertexBuffer9* getObject() const { return m_object; }

	protected:
		void createDeclaration();

	private:
		IDirect3DVertexBuffer9* m_object;
		D3D9vertdecl* m_declaration;
		int m_offset;		// byte offset
		int m_dataSize;
		int m_count;
		VertexType m_vt;
		bool m_inStack;
	};

	//--------------------------------------------------------------------------
	// class D3D9instancedbuffer
	//--------------------------------------------------------------------------

	class D3D9instancedbuffer {
	public:
		friend class D3D9vertexobject;
		D3D9instancedbuffer();
		~D3D9instancedbuffer();

		void setData(const GeoInstance::ParamSeq& params);

	private:
		IDirect3DVertexBuffer9* m_object;
		int m_offset;		// byte offset
		int m_count;
	};

	//--------------------------------------------------------------------------
	// class D3D9indexobject
	//--------------------------------------------------------------------------

	class D3D9indexobject {
	public:
		D3D9indexobject();
		~D3D9indexobject();

		void setData(const ushort_t* p, int count, Primitive::Hint, int activeCount = 0);
		void resetData();
		void bind();
		bool haveData() const;

		IDirect3DIndexBuffer9* getObject() const { return m_object; }

		int getActiveCount() const;
		void setActiveCount(int val);

		void drawElements(D3DPRIMITIVETYPE mode, int numverts);

	protected:
		static int calcNumElements(D3DPRIMITIVETYPE mode, int numindexes);


	private:
		IDirect3DIndexBuffer9* m_object;
		D3DPOOL m_hint;
		int m_dataSize;
		int m_count;
		int m_activeCount;
		int m_offset;		// index offset, not byte offset
		bool m_inStack;
	};

	//--------------------------------------------------------------------------
	// class D3D9vertdecl
	//--------------------------------------------------------------------------

	class D3D9vertdecl : public RefObject {
	public:
		friend class D3D9vertexbuffermanager;

		D3D9vertdecl(D3D9vertexbuffermanager* manager, D3D9vertexobject::VertexType vt, int offset);
		virtual ~D3D9vertdecl();

		IDirect3DVertexDeclaration9* getObject() { return m_d3dObject; }
		IDirect3DVertexDeclaration9* getObjectInstanced() { return m_d3dObjectInstanced; }

	protected:

	private:
		D3D9vertexbuffermanager* m_manager;
		IDirect3DVertexDeclaration9* m_d3dObject;
		IDirect3DVertexDeclaration9* m_d3dObjectInstanced;
		D3D9vertexobject::VertexType m_vt;
		int m_offset;
	};

	//--------------------------------------------------------------------------
	// class D3D9vertexbuffermanager
	//--------------------------------------------------------------------------
	template<class BufType, int PageSize, BufType (*CreatePage)(int)>
	class ChainedBuffer {
	public:
		enum {
			lockNewFrame = D3DLOCK_DISCARD/* | D3DLOCK_NOOVERWRITE*/,
			lockAppend = D3DLOCK_NOOVERWRITE
		};


		struct Result {
			BufType buffer;
			int offset;
			void* writePtr;
		};

		ChainedBuffer() {
			m_curIndex = -1;
		}

		virtual ~ChainedBuffer() {}

		int getSize() const { return m_pages.size() * PageSize; }
		int getUsed() const { if (m_curIndex < 0) return 0; else return m_curIndex * PageSize + m_curOffset; }

		void beginFrame() {
			m_isFrameAllocating = true;
			m_curIndex = -1;
			m_curOffset = 0;
			m_curPointer = 0;
			nextPage();
		}

		Result frameAlloc(int datasize) {
			AX_ASSERT(m_isFrameAllocating);
			// check if need change page
			Result result;

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

		void endFrame() {
			AX_ASSERT(m_isFrameAllocating);
			if (m_curIndex>=0) {
				m_pages[m_curIndex]->Unlock();
			}

			m_isFrameAllocating = false;
		}

		Result appendAlloc(int datasize) {
			AX_ASSERT(!m_isFrameAllocating);
			// check if need change page
			Result result;

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
		void endAppend() {
			m_pages[m_curIndex]->Unlock();
		}

		void clear() {}

	protected:
		void nextPage(bool isAppend = false) {
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
		Sequence<BufType> m_pages;
		int m_curIndex;
		int m_curOffset;
		void* m_curPointer;
		bool m_isFrameAllocating;
	};

	IDirect3DVertexBuffer9* CreateVertexBufferPage(int pagesize);

	IDirect3DIndexBuffer9* CreateIndexBufferPage(int pagesize);

	typedef ChainedBuffer<IDirect3DVertexBuffer9*, 1024*1024, CreateVertexBufferPage> VertexBufferChain;
	typedef ChainedBuffer<IDirect3DVertexBuffer9*, 256*1024, CreateVertexBufferPage> InstanceBufferChain;
	typedef ChainedBuffer<IDirect3DIndexBuffer9*, 512*1024, CreateIndexBufferPage> IndexBufferChain;

	class D3D9vertexbuffermanager {
	public:
		D3D9vertexbuffermanager();
		~D3D9vertexbuffermanager();

		// vertdecl
		D3D9vertdecl* getVertDecl(D3D9vertexobject::VertexType vt) { return vertDecls[vt]; }
		D3D9vertdecl* allocVertDecl(D3D9vertexobject::VertexType vt, int offset);
		void removeVertDecl(D3D9vertdecl* vd);

		// catch event
		void onDeviceLost();
		void onReset();

		void nextFrame();

		void beginAlloc();

		DynVb allocVb(int datasize);
		DynVb allocInstance(int datasize);
		DynIb allocIb(int datasize);

		void endAlloc();

		DynVb appendVb(int datasize);
		void endAppendVb();
		DynVb appendInstance(int datasize);
		void endAppendInstance();
		DynIb appendIb(int datasize);
		void endAppendIb();
		void reportStatices() const;

	protected:
		void init();
		void shutdown();

	private:
		VertexBufferChain m_vertexBufferChain;
		InstanceBufferChain m_instanceBufferChain;
		IndexBufferChain m_indexBufferChain;

		// vertdecl
		typedef Dict<int, D3D9vertdecl*> VertDeclPool;
		VertDeclPool m_vertDeclPool[D3D9vertexobject::VERTEXTYPE_NUMBER];
		D3D9vertdecl* vertDecls[D3D9vertexobject::VERTEXTYPE_NUMBER];
	};

}} // namespace Axon::Render


#endif // end guardian

