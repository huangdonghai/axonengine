/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"


AX_BEGIN_NAMESPACE

FixedStringManager::FixedStringManager()
{
	m_strings.reserve(MAX_HANDLES);

	// add empty string in index 0
	findString(std::string());
}

FixedStringManager::~FixedStringManager()
{}

const std::string &FixedStringManager::getString(int handle) const
{
	if (handle >= s2i(m_strings.size()) || handle < 0) {
		Errorf("invalid handle");
	}

	const std::string *refstring = m_strings[handle];
	if (!refstring) {
		Errorf("not found");
	}

	return *refstring;
}

int FixedStringManager::findString(const std::string &str)
{
	SCOPED_LOCK;

	Dict<const char*, int, HashCstr, EqualCstr>::const_iterator it = m_dict.find(str.c_str());

	if (it == m_dict.end()) {
		const std::string *newsz = new std::string(str);
		m_strings.push_back(newsz);
		int handle = m_strings.size() - 1;
		m_dict[newsz->c_str()] = handle;
		AX_RELEASE_ASSERT(handle < MAX_HANDLES);
		return handle;
	} else {
		return it->second;
	}
}

int FixedStringManager::findString(const char *lpcz)
{
	if (!lpcz || !lpcz[0])
		return 0;

	SCOPED_LOCK;

	Dict<const char*, int, HashCstr, EqualCstr>::const_iterator it = m_dict.find(lpcz);

	if (it == m_dict.end()) {
		const std::string *newsz = new std::string(lpcz);
		m_strings.push_back(newsz);
		int handle = m_strings.size() - 1;
		m_dict[newsz->c_str()] = handle;
		AX_RELEASE_ASSERT(handle < MAX_HANDLES);
		return handle;
	} else {
		return it->second;
	}
}

FixedStringManager &FixedStringManager::instance()
{
	static FixedStringManager instance;
	return instance;
}

AX_END_NAMESPACE
