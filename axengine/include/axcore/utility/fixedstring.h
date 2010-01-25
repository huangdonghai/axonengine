/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_FIXED_STRING_H
#define AX_FIXED_STRING_H

AX_BEGIN_NAMESPACE

// manager fixed string here. never free
class AX_API FixedStringManager : public ThreadSafe
{
public:
	enum { EMPTY_HANDLE = 0 };
	FixedStringManager();
	~FixedStringManager();

	const String &getString(int handle) const;
	int findString(const String &str);
	int findString(const char *lpcz);

	static FixedStringManager &get();

private:
	Dict<const char*,int,hash_cstr, equal_cstr> m_dict;
	Sequence<const String*> m_strings;
};

class AX_API FixedString
{
public:
	FixedString()
	{ m_handle = FixedStringManager::EMPTY_HANDLE; }

	FixedString(const FixedString &rhs)
	{
		m_handle = rhs.m_handle;
	}

	FixedString(const String &str)
	{
		m_handle = FixedStringManager::get().findString(str.c_str());
	}

	FixedString(const char *lpcz)
	{
		m_handle = FixedStringManager::get().findString(lpcz);
	}

	~FixedString()
	{}

	const String &toString() const {
		return FixedStringManager::get().getString(m_handle);
	}

	operator const String&() const {
		return FixedStringManager::get().getString(m_handle);
	}

	const char *c_str() const {
		return FixedStringManager::get().getString(m_handle).c_str();
	}

	FixedString &operator=(const FixedString &rhs) {
		m_handle = rhs.m_handle;
		return *this;
	}

	FixedString &operator=(const String &rhs) {
		m_handle = FixedStringManager::get().findString(rhs);
		return *this;
	}

	FixedString &operator=(const char *lpcz) {
		m_handle = FixedStringManager::get().findString(lpcz);
		return *this;
	}

	bool operator==(const FixedString &rhs) const {
		return m_handle == rhs.m_handle;
	}

	size_t hash() const { return m_handle; }

	bool empty() const { return m_handle == FixedStringManager::EMPTY_HANDLE; }
//		explicit operator size_t() const { return m_handle; }

private:
	int m_handle;
};

}

namespace std { namespace tr1 {

	template<>
	struct hash<::Axon::FixedString> {
		size_t operator()(const ::Axon::FixedString &sm) const { return sm.hash(); }
	};

}}

#endif
