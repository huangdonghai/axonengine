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
	void incref();
	void decref();

	inline int getRefCount() const { return m_ref.getref(); }
	inline FixedString getKey() const { return m_key; }
	inline void setKey(const FixedString &newkey) { m_key = newkey; }

	virtual void deleteThis() { delete this; }

protected:
	RefObject();
	virtual ~RefObject();

	AtomicInt m_ref;
	FixedString m_key;
};

inline RefObject::RefObject() : m_ref(0)
{}

inline RefObject::~RefObject()
{}

inline void RefObject::incref()
{ m_ref.incref(); }

inline void RefObject::decref()
{
	if (m_ref.decref() == 0) {
		deleteThis();
	}
}

//--------------------------------------------------------------------------
// class RefPtr
//--------------------------------------------------------------------------

template <class T> class RefPtr
{
public:
	// construction and destruction
	RefPtr(T *pObject = nullptr);
	RefPtr(const RefPtr &ptr);
	~RefPtr();

	// implicit conversions
	operator T*() const;
	T &operator*() const;
	T *operator->() const;
	T *get() const { return m_object; }

	// assignment
	RefPtr &operator=(const RefPtr &ptr);
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
	RefPtr &attach(T *obj) { if (m_object) m_object->decref(); m_object = obj; return *this; }

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

// Use for casting a smart pointer of one type to a pointer or smart pointer
// of another type.
#define AX_REFPTR_CAST(type, smartptr) ((type*) (void*) (smartptr.get()))

//---------------------------------------------------------------------------
template <class T>
inline RefPtr<T>::RefPtr(T *pObject)
{
	m_object = pObject;
	if (m_object)
		m_object->incref();
}
//---------------------------------------------------------------------------
template <class T>
inline RefPtr<T>::RefPtr(const RefPtr &ptr)
{
	m_object = ptr.m_object;
	if (m_object)
		m_object->incref();
}
//---------------------------------------------------------------------------
template <class T>
inline RefPtr<T>::~RefPtr()
{
	if (m_object)
		m_object->decref();
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
			m_object->decref();
		m_object = ptr.m_object;
		if (m_object)
			m_object->incref();
	}
	return *this;
}
//---------------------------------------------------------------------------
template <class T>
inline RefPtr<T>& RefPtr<T>::operator=(T *pObject)
{
	if (m_object != pObject) {
		if (m_object)
			m_object->decref();
		m_object = pObject;
		if (m_object)
			m_object->incref();
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


template <class T>
class CopyOnWritePointer
{
public:
	typedef T Type;
	typedef T *pointer;

	inline void detach() { if (d && d->getref() != 1) detach_helper(); }
	inline T &operator*() { detach(); return *d; }
	inline const T &operator*() const { return *d; }
	inline T *operator->() { detach(); return d; }
	inline const T *operator->() const { return d; }
	inline operator T *() { detach(); return d; }
	inline operator const T *() const { return d; }
	inline T *data() { detach(); return d; }
	inline const T *data() const { return d; }
	inline const T *constData() const { return d; }

	inline bool operator==(const CopyOnWritePointer<T> &other) const { return d == other.d; }
	inline bool operator!=(const CopyOnWritePointer<T> &other) const { return d != other.d; }

	inline CopyOnWritePointer() { d = 0; }
	inline ~CopyOnWritePointer() { if (d) d->decref(); }

	explicit CopyOnWritePointer(T *data);
	inline CopyOnWritePointer(const CopyOnWritePointer<T> &o) : d(o.d) { if (d) d->m_ref.incref(); }
	inline CopyOnWritePointer<T> & operator=(const CopyOnWritePointer<T> &o) {
		if (o.d != d) {
			SafeIncRef(o);
			SafeDecRef(d);
			d = o.d;
		}
		return *this;
	}
	inline CopyOnWritePointer &operator=(T *o) {
		if (o != d) {
			SafeIncRef(o);
			SafeDecRef(d);
			d = o;
		}
		return *this;
	}

	inline bool operator!() const { return !d; }

	inline void swap(CopyOnWritePointer &other)
	{ std::swap(d, other.d); }

protected:
	T *clone();

private:
	void detach_helper();

	T *d;
};

template <class T> class ExplicitlyCopyOnWritePointer
{
public:
	typedef T Type;
	typedef T *pointer;

	inline T &operator*() const { return *d; }
	inline T *operator->() { return d; }
	inline T *operator->() const { return d; }
	inline T *data() const { return d; }
	inline const T *constData() const { return d; }

	inline void detach() { if (d && d->m_ref != 1) detach_helper(); }

	inline void reset()
	{
		if(d) d->decref();
		d = 0;
	}

	inline operator bool () const { return d != 0; }

	inline bool operator==(const ExplicitlyCopyOnWritePointer<T> &other) const { return d == other.d; }
	inline bool operator!=(const ExplicitlyCopyOnWritePointer<T> &other) const { return d != other.d; }
	inline bool operator==(const T *ptr) const { return d == ptr; }
	inline bool operator!=(const T *ptr) const { return d != ptr; }

	inline ExplicitlyCopyOnWritePointer() { d = 0; }
	inline ~ExplicitlyCopyOnWritePointer() { if (d && !d->m_ref.decref()) delete d; }

	explicit ExplicitlyCopyOnWritePointer(T *data);
	inline ExplicitlyCopyOnWritePointer(const ExplicitlyCopyOnWritePointer<T> &o) : d(o.d) { if (d) d->m_ref.incref(); }

	template<class X>
	inline ExplicitlyCopyOnWritePointer(const ExplicitlyCopyOnWritePointer<X> &o) : d(static_cast<T *>(o.data()))
	{
		if(d)
			d->m_ref.incref();
	}

	inline ExplicitlyCopyOnWritePointer<T> & operator=(const ExplicitlyCopyOnWritePointer<T> &o) {
		if (o.d != d) {
			if (o.d)
				o.d->m_ref.incref();
			if (d && !d->m_ref.decref())
				delete d;
			d = o.d;
		}
		return *this;
	}
	inline ExplicitlyCopyOnWritePointer &operator=(T *o) {
		if (o != d) {
			if (o)
				o->m_ref.incref();
			if (d && !d->m_ref.decref())
				delete d;
			d = o;
		}
		return *this;
	}

	inline bool operator!() const { return !d; }

	inline void swap(ExplicitlyCopyOnWritePointer &other)
	{ std::swap(d, other.d); }

protected:
	T *clone();

private:
	void detach_helper();

	T *d;
};

template <class T>
inline CopyOnWritePointer<T>::CopyOnWritePointer(T *adata) : d(adata)
{ if (d) d->m_ref.incref(); }

template <class T>
inline T *CopyOnWritePointer<T>::clone()
{
	return new T(*d);
}

template <class T>
inline void CopyOnWritePointer<T>::detach_helper()
{
	T *x = clone();
	x->incref();
	SafeDecRef(d);
	d = x;
}

template <class T>
inline T *ExplicitlyCopyOnWritePointer<T>::clone()
{
	return new T(*d);
}

template <class T>
inline void ExplicitlyCopyOnWritePointer<T>::detach_helper()
{
	T *x = clone();
	x->m_ref.incref();
	if (!d->m_ref.decref())
		delete d;
	d = x;
}

template <class T>
inline ExplicitlyCopyOnWritePointer<T>::ExplicitlyCopyOnWritePointer(T *adata) : d(adata)
{ if (d) d->m_ref.incref(); }


AX_END_NAMESPACE

#endif
