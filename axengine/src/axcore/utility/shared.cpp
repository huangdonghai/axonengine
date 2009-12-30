/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

struct FilterInfo {
	enum FilterType {
		FT_ALL,
		FT_EXT,
		FT_PIECE,
		FT_SPECIAL,
		FT_SEARCH,
		FT_MAX,
	};

	String str;
	int type;

public:
	bool isIn(const String& name) const;
};

Filter::~Filter() {
	for (size_t i=0; i<mfilters.size(); i++) {
		delete mfilters[i];
	}
}

Filter::Filter(const String& filterStr) {
	StringList strs = StringUtil::tokenize(filterStr.c_str(), L';');

	if (strs.size() == 0) {
		strs.push_back("*");
	}

	StringList::iterator it = strs.begin();
	for (; it != strs.end(); ++it) {
		FilterInfo* pfi = new FilterInfo;
		FilterInfo& fi = *pfi;
		const String& orgStr = *it;
		fi.str = orgStr;
		if (orgStr == "*") {
			fi.type = FilterInfo::FT_ALL;
			AX_ASSERT(strs.size() == 1);
			mfilters.push_back(pfi);
			break;

		} else if (strchr(orgStr.c_str(), '*') != NULL) {
			if (strchr(orgStr.c_str() + 1, '*') != NULL) {
				fi.type = FilterInfo::FT_SEARCH;
			} else if (orgStr[1] == L'.') {
				fi.type = FilterInfo::FT_EXT;
				fi.str = orgStr.c_str() + 1;
			} else {
				fi.type = FilterInfo::FT_PIECE;
				fi.str = orgStr.c_str() + 1;
			}
		} else if (strchr(orgStr.c_str(), '?') != NULL) {
			fi.type = FilterInfo::FT_SEARCH;
		} else if (strchr(orgStr.c_str(), '.') != NULL) {
			if (orgStr[0] == L'.') {
				fi.type = FilterInfo::FT_EXT;
			} else {
				fi.type = FilterInfo::FT_PIECE;
			}
		} else {
			fi.type = FilterInfo::FT_PIECE;
		}
		mfilters.push_back(pfi);
	}
}


static bool IsMatch(const char* pName, const char* pFilter) {
	for (; *pFilter; pFilter++) {
		if (*pFilter == L'?') {
			if (*pName == 0)
				return false;
			pName++;
		} else if (*pFilter == L'*') {
			pFilter++;
			if (*pFilter == 0)
				return true;
			for (; *pName ; pName++) {
				if (IsMatch(pName, pFilter))
					return true;
			}
			return false;
		} else {
			if (*pName != *pFilter)
				return false;
			pName++;
		}
	}
	if (pName != 0)
		return false;
	return true;
}

bool FilterInfo::isIn(const String& name) const {
	switch (this->type) {
	case FilterInfo::FT_ALL:
		return true;
	case FilterInfo::FT_EXT:
		{
			size_t len = name.length();
			const char* pNameExt = name.c_str() + len - this->str.length();
			return ::strcmp(pNameExt, this->str.c_str()) == 0;
		}
	case FilterInfo::FT_PIECE:
		return strstr(name.c_str(), this->str.c_str()) != NULL;

	case FilterInfo::FT_SPECIAL:
		return this->str == name;

	case FilterInfo::FT_SEARCH:
		return IsMatch(name.c_str(), this->str.c_str());

	default:
		AX_NO_DEFAULT;
		return false;
	}
}

bool Filter::In(const String& name) const {
	std::vector<FilterInfo*>::const_iterator it = mfilters.begin();
	for (; it != mfilters.end(); ++it) {
		if ((*it)->isIn(name)) {
			return true;
		}
	}
	return false;

}
#if 0
Strings Tokenize(const String& in, wchar_t split) {
	Strings strcol;

	if (in.empty())
		return strcol;

	String str;

	const wchar_t* token = in.c_str();

	for (; ;) {
		/* skip whitespace */
		while (*token && UInt(*token) <= L' ' || *token == split) {
			token++;
		}

		if (!*token) break;

		str.clear();

		// handle quoted strings
		if (*token == L'\"') {
			token++;
			while (*token && *token != L'\"') {
				str += *token++;
			}
			strcol.push_back(str);
			str.clear();
			if (!*token) {
				break;
			} else {
				token++;
				continue;
			}
		}

		do {
			str+=*token; token++;
		} while (UInt(*token) > L' ' && *token != split);
		strcol.push_back(str);
	}

	return strcol;
}

StringList Tokenize(const String& in, char split) {
	StringList strcol;

	if (in.empty())
		return strcol;

	String str;

	const char* token = in.c_str();

	for (; ;) {
		/* skip whitespace */
		while (*token && uint_t(*token) <= ' ' || *token == split) {
			token++;
		}

		if (!*token) break;

		str.clear();

		// handle quoted strings
		if (*token == '\"') {
			token++;
			while (*token && *token != '\"') {
				str += *token++;
			}
			strcol.push_back(str);
			str.clear();
			if (!*token) {
				break;
			} else {
				token++;
				continue;
			}
		}

		do {
			str+=*token; token++;
		} while (uint_t(*token) > ' ' && *token != split);
		strcol.push_back(str);
	}

	return strcol;
}

StringSeq TokenizeSeq(const String& in, char split) {
	StringSeq strcol;

	if (in.empty())
		return strcol;

	String str;

	const char* token = in.c_str();

	for (; ;) {
		/* skip whitespace */
		while (*token && uint_t(*token) <= ' ' || *token == split) {
			token++;
		}

		if (!*token) break;

		str.clear();

		// handle quoted strings
		if (*token == '\"') {
			token++;
			while (*token && *token != '\"') {
				str += *token++;
			}
			strcol.push_back(str);
			str.clear();
			if (!*token) {
				break;
			} else {
				token++;
				continue;
			}
		}

		do {
			str+=*token; token++;
		} while (uint_t(*token) > ' ' && *token != split);
		strcol.push_back(str);
	}

	return strcol;
}
#endif




AX_END_NAMESPACE



