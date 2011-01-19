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
	enum {
		EMPTY_HANDLE = 0,
		MAX_HANDLES = 65536
	};

	FixedStringManager();
	~FixedStringManager();

	const std::string &getString(int handle) const;
	int findString(const std::string &str);
	int findString(const char *lpcz);

	static FixedStringManager &instance();

private:
	Dict<const char*,int,HashCstr, EqualCstr> m_dict;
	std::vector<const std::string*> m_strings;
};

class AX_API FixedString
{
public:
	FixedString()
	{
		m_handle = FixedStringManager::EMPTY_HANDLE;
#ifdef _DEBUG
		m_string = 0;
#endif
	}

	FixedString(int id)
	{
		m_handle = id;
#ifdef _DEBUG
		m_string = &FixedStringManager::instance().getString(m_handle);
#endif
	}

	FixedString(const FixedString &rhs)
	{
		m_handle = rhs.m_handle;
#ifdef _DEBUG
		m_string = rhs.m_string;
#endif
	}

	FixedString(const std::string &str)
	{
		m_handle = FixedStringManager::instance().findString(str.c_str());
#ifdef _DEBUG
		m_string = &FixedStringManager::instance().getString(m_handle);
#endif
	}

	FixedString(const char *lpcz)
	{
		m_handle = FixedStringManager::instance().findString(lpcz);
#ifdef _DEBUG
		m_string = &FixedStringManager::instance().getString(m_handle);
#endif
	}

	~FixedString()
	{}

	int id() const { return m_handle; }

	const std::string &toString() const
	{
		return FixedStringManager::instance().getString(m_handle);
	}
	const char *c_str() const
	{
		return FixedStringManager::instance().getString(m_handle).c_str();
	}

	FixedString &operator=(const FixedString &rhs)
	{
		m_handle = rhs.m_handle;
#ifdef _DEBUG
		m_string = rhs.m_string;
#endif
		return *this;
	}

	FixedString &operator=(const std::string &rhs)
	{
		m_handle = FixedStringManager::instance().findString(rhs);
#ifdef _DEBUG
		m_string = &FixedStringManager::instance().getString(m_handle);
#endif
		return *this;
	}

	FixedString &operator=(const char *lpcz)
	{
		m_handle = FixedStringManager::instance().findString(lpcz);
#ifdef _DEBUG
		m_string = &FixedStringManager::instance().getString(m_handle);
#endif
		return *this;
	}

	bool operator==(const FixedString &rhs) const
	{
		return m_handle == rhs.m_handle;
	}

	size_t hash() const
	{ return m_handle; }

	bool empty() const
	{ return m_handle == FixedStringManager::EMPTY_HANDLE; }
//		explicit operator size_t() const { return m_handle; }

private:
	int m_handle;
#ifdef _DEBUG
	const std::string *m_string;
#endif
};

AX_END_NAMESPACE

AX_DECLARE_HASH_FUNCTION(FixedString);

#endif
