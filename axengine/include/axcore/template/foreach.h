/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_FOREACH_H
#define AX_FOREACH_H

#if defined(__GNUG__) && !defined(__ICC)

/* make use of typeof-extension */
template <typename T>
class AxForeachContainer {
public:
	inline AxForeachContainer(const T &t) : c(t), brk(0), i(c.begin()), e(c.end()) { }
	const T c;
	int brk;
	typename T::const_iterator i, e;
};

#define AX_FOREACH(variable, container)                                \
	for (AxForeachContainer<__typeof__(container)> _container_(container); \
	!_container_.brk && _container_.i != _container_.e;              \
	__extension__  ({ ++_container_.brk; ++_container_.i; }))                       \
	for (variable = *_container_.i;; __extension__ ({--_container_.brk; break;}))

#else

struct ForeachContainerBase
{};

template <typename T>
class AxForeachContainer : public ForeachContainerBase {
public:
	inline AxForeachContainer(const T &t):brk(0), i(t.begin()), e(t.end())
	{
	};

	mutable int brk;
	mutable typename T::const_iterator i, e;
	inline bool condition() const
	{
		return (!brk++ && i != e);
	}
};

template <typename T>
inline T *axForeachPointer(const T &)
{
	return 0;
}

template <typename T>
inline AxForeachContainer<T> axForeachContainerNew(const T &t)
{
	return AxForeachContainer<T>(t);
}

template <typename T>
inline const AxForeachContainer<T> *axForeachContainer(const ForeachContainerBase *base, const T *)
{
	return static_cast<const AxForeachContainer<T> *>(base);
}

#define AX_FOREACH(variable, container) \
	for (const ForeachContainerBase &_container_ = axForeachContainerNew(container); axForeachContainer(&_container_, axForeachPointer(container))->condition(); ++axForeachContainer(&_container_, axForeachPointer(container))->i) \
	for (variable = *axForeachContainer(&_container_, axForeachPointer(container))->i; axForeachContainer(&_container_, axForeachPointer(container))->brk; --axForeachContainer(&_container_, axForeachPointer(container))->brk)

#endif

#endif // end guardian

