#ifndef AX_SHAREDDATA_H
#define AX_SHAREDDATA_H

AX_BEGIN_NAMESPACE

class AX_API SharedData
{
public:
	inline SharedData() {}
	inline SharedData(const SharedData &) {}

	int incref() { return m_ref.incref(); }
	int decref() { return m_ref.decref(); }

private:
	mutable AtomicInt m_ref;

private:
	// using the assignment operator would lead to corruption in the ref-counting
	SharedData &operator=(const SharedData &);
};

template <class T> class SharedDataPointer
{
public:
	typedef T Type;
	typedef T *pointer;

	inline void detach() { if (d && d->m_ref != 1) detach_helper(); }
	inline T &operator*() { detach(); return *d; }
	inline const T &operator*() const { return *d; }
	inline T *operator->() { detach(); return d; }
	inline const T *operator->() const { return d; }
	inline operator T *() { detach(); return d; }
	inline operator const T *() const { return d; }
	inline T *data() { detach(); return d; }
	inline const T *data() const { return d; }
	inline const T *constData() const { return d; }

	inline bool operator==(const SharedDataPointer<T> &other) const { return d == other.d; }
	inline bool operator!=(const SharedDataPointer<T> &other) const { return d != other.d; }

	inline SharedDataPointer() { d = 0; }
	inline ~SharedDataPointer() { if (d && !d->m_ref.decref()) delete d; }

	explicit SharedDataPointer(T *data);
	inline SharedDataPointer(const SharedDataPointer<T> &o) : d(o.d) { if (d) d->m_ref.incref(); }
	inline SharedDataPointer<T> & operator=(const SharedDataPointer<T> &o) {
		if (o.d != d) {
			if (o.d)
				o.d->m_ref.incref();
			if (d && !d->m_ref.decref())
				delete d;
			d = o.d;
		}
		return *this;
	}
	inline SharedDataPointer &operator=(T *o) {
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

	inline void swap(SharedDataPointer &other)
	{ std::swap(d, other.d); }

protected:
	T *clone();

private:
	void detach_helper();

	T *d;
};

template <class T> class ExplicitlySharedDataPointer
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
		if(d && !d->m_ref.decref())
			delete d;

		d = 0;
	}

	inline operator bool () const { return d != 0; }

	inline bool operator==(const ExplicitlySharedDataPointer<T> &other) const { return d == other.d; }
	inline bool operator!=(const ExplicitlySharedDataPointer<T> &other) const { return d != other.d; }
	inline bool operator==(const T *ptr) const { return d == ptr; }
	inline bool operator!=(const T *ptr) const { return d != ptr; }

	inline ExplicitlySharedDataPointer() { d = 0; }
	inline ~ExplicitlySharedDataPointer() { if (d && !d->m_ref.decref()) delete d; }

	explicit ExplicitlySharedDataPointer(T *data);
	inline ExplicitlySharedDataPointer(const ExplicitlySharedDataPointer<T> &o) : d(o.d) { if (d) d->m_ref.incref(); }

	template<class X>
	inline ExplicitlySharedDataPointer(const ExplicitlySharedDataPointer<X> &o) : d(static_cast<T *>(o.data()))
	{
		if(d)
			d->m_ref.incref();
	}

	inline ExplicitlySharedDataPointer<T> & operator=(const ExplicitlySharedDataPointer<T> &o) {
		if (o.d != d) {
			if (o.d)
				o.d->m_ref.incref();
			if (d && !d->m_ref.decref())
				delete d;
			d = o.d;
		}
		return *this;
	}
	inline ExplicitlySharedDataPointer &operator=(T *o) {
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

	inline void swap(ExplicitlySharedDataPointer &other)
	{ std::swap(d, other.d); }

protected:
	T *clone();

private:
	void detach_helper();

	T *d;
};

template <class T>
inline SharedDataPointer<T>::SharedDataPointer(T *adata) : d(adata)
{ if (d) d->m_ref.incref(); }

template <class T>
inline T *SharedDataPointer<T>::clone()
{
	return new T(*d);
}

template <class T>
inline void SharedDataPointer<T>::detach_helper()
{
	T *x = clone();
	x->m_ref.incref();
	if (!d->m_ref.decref())
		delete d;
	d = x;
}

template <class T>
inline T *ExplicitlySharedDataPointer<T>::clone()
{
	return new T(*d);
}

template <class T>
inline void ExplicitlySharedDataPointer<T>::detach_helper()
{
	T *x = clone();
	x->m_ref.incref();
	if (!d->m_ref.decref())
		delete d;
	d = x;
}

template <class T>
inline ExplicitlySharedDataPointer<T>::ExplicitlySharedDataPointer(T *adata) : d(adata)
{ if (d) d->m_ref.incref(); }

AX_END_NAMESPACE

#endif // AX_SHAREDDATA_H
