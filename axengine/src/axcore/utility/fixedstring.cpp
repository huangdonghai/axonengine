/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"


namespace Axon {

	FixedStringManager::FixedStringManager()
	{
		// add empty string in index 0
		findString(String());
	}

	FixedStringManager::~FixedStringManager()
	{}

	const String& FixedStringManager::getString( int handle ) const
	{
		SCOPE_LOCK;

		if (handle >= s2i(m_strings.size()) || handle < 0) {
			Errorf("invalid handle");
		}

		const String* refstring = m_strings[handle];
		if (!refstring) {
			Errorf("not found");
		}

		return *refstring;
	}

	int FixedStringManager::findString( const String& str )
	{
		SCOPE_LOCK;

		Dict<const char*, int, hash_cstr, equal_cstr>::const_iterator it = m_dict.find(str.c_str());

		if (it == m_dict.end()) {
			const String* newsz = new String(str);
			m_strings.push_back(newsz);
			int handle = m_strings.size() - 1;
			m_dict[newsz->c_str()] = handle;
			return handle;
		} else {
			return it->second;
		}

	}

	FixedStringManager& FixedStringManager::get()
	{
		static FixedStringManager instance;
		return instance;
	}

} // namespace Axon
