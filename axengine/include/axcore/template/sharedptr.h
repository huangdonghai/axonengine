/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_SHAREDPTR_H
#define AX_CORE_SHAREDPTR_H


AX_BEGIN_NAMESPACE

struct DeleteArray {
	void operator()(void *ptr)
	{
		delete[] ptr;
	}
};

using std::tr1::shared_ptr;

template<class T>
class shared_array : public std::tr1::shared_ptr<T> {
public:
	typedef std::tr1::shared_ptr<T> BaseType;

	shared_array() : BaseType()
	{}

	explicit shared_array(T *obj) : BaseType(obj, DeleteArray())
	{}
};

template <class T>
class ScopedPtr : public Noncopyable
{
	friend T *GetPointer_(ScopedPtr<T> const &);

public:
	explicit ScopedPtr(T *_p) : p(_p) {}
	~ScopedPtr() { if (p) delete p; }

	T * operator->() const { return p; }
	T & operator*() const { return *p; }
	operator bool() const { return p != 0; }

	void swap(ScopedPtr &rhs) { std::swap(p, rhs->p); }
private:
	T *p;
};

template<class T> inline void swap(ScopedPtr<T> & a, ScopedPtr<T> & b) // never throws
{
	a.swap(b);
}

template<class T> inline T * GetPointer_(ScopedPtr<T> const & p)
{
	return p.p;
}

template <class T>
class HolderPtr : public Noncopyable
{
	friend T *GetPointer_(HolderPtr<T> const &);

public:
	HolderPtr() : p(0) {}
	HolderPtr(T *_p) : p(_p) {}
	~HolderPtr() { reset(); }

	T * operator->() const { return p; }
	T & operator*() const { return *p; }
	T & operator=(T *rhs) { reset(); p=rhs; return *this; }
	operator bool() const { return p != 0; }

	void swap(HolderPtr &rhs) { std::swap(p, rhs->p); }

private:
	void reset() { if (p) delete p; p = 0; }
	T *p;
};

template<class T> inline void swap(HolderPtr<T> &a, HolderPtr<T> &b) // never throws
{
	a.swap(b);
}

template<class T> inline T * GetPointer_(HolderPtr<T> const &p)
{
	return p.p;
}

#if 0
// smart pointer
class SharedCount {
public:
	inline SharedCount() : m_refcount(0) {}
	inline ~SharedCount() {}

	inline int addRef() {
		return OsUtil::atomicIncrement(m_refcount);
	}
	inline int release() {
		return OsUtil::atomicDecrement(m_refcount);
	}

private:
	int m_refcount;
};

template< typename T >
class SharedRef;

// use delete to destruct the object
template< typename T >
class SharedPtr {
	typedef SharedPtr<T>	ThisType;
public:
	typedef T ValueType;
	typedef ValueType *Pointer;
	typedef const Pointer ConstPointer;
	typedef SharedRef<T>	RefType;

	SharedPtr() : m_obj(NULL), m_counter(NULL) {}
	explicit SharedPtr(T *obj) : m_obj(obj), m_counter(NULL) {
		if (obj) {
			m_counter = new SharedCount();
			m_counter->addRef();
		}
	}
	explicit SharedPtr(T *obj, SharedCount *counter) : m_obj(obj), m_counter(counter) {
		if (obj && !m_counter) {
			m_counter = new SharedCount();
		}
		if (m_counter)
			m_counter->addRef();
	}
	SharedPtr(const ThisType &other) : m_obj(other.m_obj), m_counter(other.m_counter) {
		if (m_counter)
			m_counter->addRef();
	}
	SharedPtr(const RefType &other) : m_obj(other.GetPointer()), m_counter(other.GetCounter()) {
		if (m_counter)
			m_counter->addRef();
	}
	template< typename Q >
	SharedPtr(const SharedRef<Q>& other) : m_obj(NULL), m_counter(NULL) {
		if (other.GetPointer() == NULL)
			return;

		m_obj = dynamic_cast<Pointer>(other.GetPointer());
		if (!m_obj)
			Errorf(_("SharedPtr convertion failured"));

		m_counter = other.GetCounter();
		if (m_counter)
			m_counter->addRef();
	}
	template< typename Q >
	SharedPtr(const SharedPtr<Q>& other) : m_obj(NULL), m_counter(NULL) {
		if (other.GetPointer() == NULL)
			return;

		m_obj = dynamic_cast<Pointer>(other.GetPointer());
		if (!m_obj)
			Errorf(_("SharedPtr convertion failured"));

		m_counter = other.GetCounter();
		if (m_counter)
			m_counter->addRef();
	}

	// destructor
	~SharedPtr() {
		clear();
	}

	ThisType &operator=(const ThisType &other) {
		clear();
		m_obj = other.m_obj;
		m_counter = other.m_counter;
		if (m_counter)
			m_counter->addRef();
		return *this;
	}

	T *operator->() const {
		return m_obj;
	}

	operator T*() const {
		return m_obj;
	}

	T &operator*() const {
		return *m_obj;
	}

	bool isValid() const {
		return (m_obj != 0);
	}

	void clear() {
		if (!m_counter)
			return;

		if (m_counter->release() == 0) {
			SafeDelete(m_obj);
			SafeDelete(m_counter);
		} else {
			m_obj = NULL;
			m_counter = NULL;
		}
	}
	template< typename Q >
	ThisType &operator=(const SharedRef<Q>& other) {
		clear();
		if (other.GetPointer() == NULL)
			return *this;

		m_obj = dynamic_cast<Pointer>(other.GetPointer());
		if (!m_obj)
			Errorf(_("SharedPtr convertion failured"));

		m_counter = other.GetCounter();
		if (m_counter)
			m_counter->addRef();

		return *this;
	}

	template< typename Q >
	ThisType &operator=(const SharedPtr<Q>& other) {
		clear();
		if (other.GetPointer() == NULL)
			return *this;

		m_obj = dynamic_cast<Pointer>(other.GetPointer());
		if (!m_obj)
			Errorf(_("SharedPtr convertion failured"));

		m_counter = other.GetCounter();
		if (m_counter)
			m_counter->addRef();

		return *this;
	}

	bool operator==(const ThisType &other) {
		return m_obj == other.m_obj && m_counter == other.m_counter;
	}

	bool operator!=(const ThisType other) {
		return m_obj != other.m_obj || m_counter != other.m_counter;
	}

	Pointer getPointer() const { return m_obj; }
	SharedCount *getCounter() const { return m_counter; }

private:
	ValueType *m_obj;
	SharedCount *m_counter;
};

// use delete[] to destruct the object
template< typename T >
class SharedArray : public SharedPtr<T> {
public:
	typedef SharedArray<T>	ThisType;
	typedef T ValueType;
	typedef ValueType *Pointer;
	typedef const Pointer ConstPointer;

	SharedArray() : m_obj(NULL), m_counter(NULL) {}
	explicit SharedArray(T *obj) : m_obj(obj), m_counter(NULL) {
		if (obj) {
			m_counter = new SharedCount();
			m_counter->addRef();
		}
	}
	SharedArray(const ThisType &other) : m_obj(other.m_obj), m_counter(other.m_counter) {
		if (m_counter)
			m_counter->addRef();
	}
	~SharedArray() {
		clear();
	}

	ThisType &operator=(const ThisType &other) {
		clear();
		m_obj = other.m_obj;
		m_counter = other.m_counter;
		if (m_counter)
			m_counter->addRef();

		return *this;
	}

	T *operator->() const {
		return m_obj;
	}

	operator T*() const {
		return m_obj;
	}

	T &operator*() const {
		return *m_obj;
	}

	bool isValid() const {
		return (m_obj != 0);
	}

	void clear() {
		if (!m_counter)
			return;

		if (m_counter->release() == 0) {
			SafeDeleteArray(m_obj);
			SafeDelete(m_counter);
		} else {
			m_obj = NULL;
			m_counter = NULL;
		}
	}

private:
	ValueType *m_obj;
	SharedCount *m_counter;
};

// use free() to destruct the object
template< typename T >
class SharedBuf {
public:
	typedef SharedBuf<T>	ThisType;
	typedef T ValueType;
	typedef ValueType *Pointer;
	typedef const Pointer ConstPointer;

	SharedBuf() : mObj(NULL), mCounter(NULL) {}
	explicit SharedBuf(T *obj) : mObj(obj), mCounter(NULL) {
		if (obj) {
			mCounter = new SharedCount();
			mCounter->AddRef();
		}
	}
	SharedBuf(const ThisType &other) : mObj(other.mObj), mCounter(other.mCounter) {
		if (mCounter)
			mCounter->AddRef();
	}
	~SharedBuf() {
		Clear();
	}

	ThisType &operator=(const ThisType &other) {
		Clear();
		mObj = other.mObj;
		mCounter = other.mCounter;
		if (mCounter)
			mCounter->AddRef();

		return *this;
	}

	T *GetPointer() const { return mObj; }
	T *operator->() const { return mObj; }
	operator T*() const { return mObj; }
	T &operator*() const { return *mObj; }
	bool IsValid() const { return mObj != NULL; }

	void Clear() {
		if (!mCounter)
			return;

		if (mCounter->Release() == 0) {
			SafeFree(mObj);
			SafeDelete(mCounter);
		} else {
			mObj = NULL;
			mCounter = NULL;
		}
	}

private:
	ValueType *mObj;
	SharedCount *mCounter;
};

/*!
\class SharedRef
\brief A reference pointer

A reference pointer, like shared pointer, but will never manage
counter and object life circle, just store object pointer and counter
pointer in it.

\sa SharedPtr
*/
template<typename T>
class SharedRef {
public:
	typedef SharedRef<T>	ThisType;
	typedef T ValueType;
	typedef ValueType *Pointer;
	typedef const Pointer ConstPointer;
	typedef SharedPtr<T>	SharedPointer;

	SharedRef()
		: mObj(NULL)
		, mCounter(NULL) {}

	SharedRef(const ThisType &other)
		: mObj(other.mObj)
		, mCounter(other.mCounter) {}

	SharedRef(const SharedPointer &other)
		: mObj(other.GetPointer())
		, mCounter(other.GetCounter()) {}

	explicit SharedRef(T *obj)
		: mObj(obj)
		, mCounter(NULL) {
			if (obj) {
				mCounter = new SharedCount();
			}
	}

	~SharedRef() {
	}

	ThisType &operator=(const ThisType &other) {
		mObj = other.mObj;
		mCounter = other.mCounter;
		return *this;
	}

	ThisType &operator=(const SharedPointer &other) {
		mObj = other.mObj;
		mCounter = other.mCounter;
		return *this;
	}

	Pointer operator->() const {
		return mObj;
	}

	operator Pointer() const {
		return mObj;
	}

	T &operator*() const {
		return *mObj;
	}

	bool IsValid() const {
		return (mObj != 0);
	}

	Pointer GetPointer() const { return mObj; }
	SharedCount *GetCounter() const { return mCounter; }

private:
	ValueType *mObj;
	SharedCount *mCounter;
};

// render resource, register counted
template< class host >
class SharedObject : public host {
public:
	SharedObject() {}

	~SharedObject() {}

	int AddRef(void) {
		return mCounter.AddRef();
	}

	int Release(void){
		int c = mCounter.Release();
		if (0 == c){
			delete this;
			return 0;
		}
		return c;
	}

private:
	SharedCount mCounter;
};
#endif

AX_END_NAMESPACE


#endif // end guardian

