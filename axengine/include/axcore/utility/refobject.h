/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CORE_REFOBJECT_H
#define AX_CORE_REFOBJECT_H

AX_BEGIN_NAMESPACE

	//--------------------------------------------------------------------------
	// class RefObject
	//--------------------------------------------------------------------------

	class AX_API RefObject {
	public:
		void addref();
		void release();

		inline int getRefCount() const { return m_refcount; }
		inline FixedString getKey() const { return m_key; }
		inline void setKey(const FixedString &newkey) { m_key = newkey; }

		virtual void deleteThis() { delete this; }

	protected:
		RefObject();
		virtual ~RefObject();

		int m_refcount;
		FixedString m_key;
	};

	inline RefObject::RefObject() : m_refcount(1)
	{}

	inline RefObject::~RefObject()
	{}

	inline void RefObject::addref()
	{ OsUtil::atomicIncrement(m_refcount); }

	inline void RefObject::release()
	{
		if (OsUtil::atomicDecrement(m_refcount) == 0) {
			deleteThis();
		}
	}

	//--------------------------------------------------------------------------
	// class ResultPtr
	//
	// we need this to indicate the object's refcount already incremented, so if
	// assign to RefPtr, no need add reference count again.
	//
	// this pointer doesn't call addref, and when it be destructed, doesn't call
	// release
	//--------------------------------------------------------------------------
	template <class Ty>
	class ResultPtr {
	public:
		ResultPtr(Ty *obj = 0) : m_object(obj) {}
		ResultPtr(const ResultPtr &ptr) : m_object(ptr.m_object) {}
		
		operator Ty*() const { return m_object; }
		Ty &operator*() const { return *m_object; }
		Ty *operator->() const { return m_object; }
		Ty *get() const { return m_object; }
	public:
		Ty *m_object;
	};

	//--------------------------------------------------------------------------
	// class RefPtr
	//--------------------------------------------------------------------------

	template <class T> class RefPtr
	{
	public:
		// construction and destruction
		RefPtr(T *pObject = nullptr);
		RefPtr(const RefPtr &ptr);
		RefPtr(const ResultPtr<T>& ptr) { m_object = ptr.m_object; }
		~RefPtr();

		// implicit conversions
		operator T*() const;
		T &operator*() const;
		T *operator->() const;
		T *get() const { return m_object; }

		// assignment
		RefPtr &operator=(const RefPtr &ptr);
		RefPtr &operator=(const ResultPtr<T>& ptr) { if (m_object) m_object->release(); m_object = ptr.m_object; return *this; }
		RefPtr &operator=(T *pObject);

		// comparisons
		bool operator==(T *pObject) const;
		bool operator!=(T *pObject) const;
		bool operator==(const RefPtr &ptr) const;
		bool operator!=(const RefPtr &ptr) const;

		// logic
		bool operator!() const { return m_object == 0; }
		operator bool() const { return m_object != 0; }

		// attach an object, don't increment ref-count
		RefPtr &attach(T *obj) { if (m_object) m_object->release(); m_object = obj; return *this; }

		// another style of detach and attach
		RefPtr &operator<<(T *obj) { return attach(obj); }

		// clear
		void clear() { operator=(nullptr); }

	protected:
		// the managed pointer
		T *m_object;
	};

#define AX_DECLARE_REFPTR(classname) \
	class classname; \
	typedef RefPtr<classname> classname##Ptr; \
	typedef ResultPtr<classname> classname##Rp

	// Use for casting a smart pointer of one type to a pointer or smart pointer
	// of another type.
#define AX_REFPTR_CAST(type, smartptr) ((type*) (void*) (smartptr.get()))

	//---------------------------------------------------------------------------
	template <class T>
	inline RefPtr<T>::RefPtr(T *pObject)
	{
		m_object = pObject;
		if (m_object)
			m_object->addref();
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline RefPtr<T>::RefPtr(const RefPtr &ptr)
	{
		m_object = ptr.m_object;
		if (m_object)
			m_object->addref();
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline RefPtr<T>::~RefPtr()
	{
		if (m_object)
			m_object->release();
	}
#if 1
	//---------------------------------------------------------------------------
	template <class T>
	inline RefPtr<T>::operator T*() const
	{
		return m_object;
	}
#endif
	//---------------------------------------------------------------------------
	template <class T>
	inline T &RefPtr<T>::operator*() const
	{
		return *m_object;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline T *RefPtr<T>::operator->() const
	{
		return m_object;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline RefPtr<T>& RefPtr<T>::operator=(const RefPtr &ptr)
	{
		if (m_object != ptr.m_object) {
			if (m_object)
				m_object->release();
			m_object = ptr.m_object;
			if (m_object)
				m_object->addref();
		}
		return *this;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline RefPtr<T>& RefPtr<T>::operator=(T *pObject)
	{
		if (m_object != pObject) {
			if (m_object)
				m_object->release();
			m_object = pObject;
			if (m_object)
				m_object->addref();
		}
		return *this;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool RefPtr<T>::operator==(T *pObject) const
	{
		return (m_object == pObject);
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool RefPtr<T>::operator!=(T *pObject) const
	{
		return (m_object != pObject);
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool RefPtr<T>::operator==(const RefPtr &ptr) const
	{
		return (m_object == ptr.m_object);
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool RefPtr<T>::operator!=(const RefPtr &ptr) const
	{
		return (m_object != ptr.m_object);
	}
	//---------------------------------------------------------------------------

	template <class To, class From>
	inline RefPtr<To> refptr_cast(const RefPtr<From> ptr) {
		return RefPtr<To>((To*) (void*) (ptr.get()));
	}
}

#endif
