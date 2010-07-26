/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_ENUM_H
#define AX_ENUM_H

#define AX_DECLARE_ENUM(type)	\
	type::Type t; \
	inline type() {}	\
	inline type(Type e) { t = e; } \
	inline type(int i) { t = (Type)i; } \
	inline operator Type() const { return t; }
//	inline operator int() const { return (int)t; } \

AX_BEGIN_NAMESPACE

template< class T >
struct Enum {
	typedef typename T::Type enum_type;

	int e;

	Enum();
	Enum(T t);
	Enum(int t);
	~Enum();

	Enum &operator=(enum_type t);
	Enum &operator=(int t);
};

template<class T>
Enum<T>::Enum() {}

template<class T>
Enum<T>::Enum(T t) {
	e = t;
}

template<class T>
Enum<T>::Enum(int t) {
	e = (T)t;
}

template<class T>
Enum<T>::~Enum() {}

template<class T>
Enum<T>& Enum<T>::operator=(enum_type t) {
	e = t;
	return *this;
}

template<class T>
Enum<T>& Enum<T>::operator=(int t) {
	e = t;
	return *this;
}

template <class E>
class Flags_ {
public:
	Flags_() {}
	Flags_(int i) : m_data(i) {}
	~Flags_() {}

	void set(int e) { m_data |= e; }
	void set(int e, bool f) { if (f) set(e); else unset(e); }
	void unset(int e) { m_data &= ~e; }
	void setAll() { m_data = -1; }
	void unsetAll() { m_data = 0; }
	bool isSet(E e) const { return (m_data&e)!= 0; }
	bool isntSet(E e) const { return !isSet(e); }
	bool isSetAny(int e) const { return (m_data & e) != 0; }
	bool isSetAll(int e) const { return (m_data & e) == e; }

	operator int() const { return m_data; }

protected:
	int m_data;
};

AX_END_NAMESPACE

#endif // end guardian

