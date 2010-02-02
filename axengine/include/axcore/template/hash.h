/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_HASH_H
#define AX_CORE_HASH_H

#define AX_DECLARE_HASH_FUNCTION(Type) \
namespace std { namespace tr1 { \
	template<> \
	struct hash<AX_NAMESPACE::Type> { \
		size_t operator()(const AX_NAMESPACE::Type &obj) const { return obj.hash(); } \
	}; \
}} \


AX_BEGIN_NAMESPACE

/// 
inline size_t hash_string(const char *s)
{
	unsigned long h = 0;
	for (; *s; ++s)
		h = 5*h + *s;

	return size_t(h);
}

inline size_t hash_string(const wchar_t *s)
{
	unsigned long h = 0;
	for (; *s; ++s)
		h = 5*h + *s;

	return size_t(h);
}

/// 
inline size_t hash_istring(const char *s)
{
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s)
		h = 5*h + tolower(*s);

	return size_t(h);
}

/// 
inline size_t hash_filename(const char *s)
{
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s) {
		char letter = *s;
		if (letter=='\\') {
			letter='/';
		}
		h = 5*h + tolower(letter);
	}

	return size_t(h);
}

/// 
inline size_t hash_istring(const wchar_t *s)
{
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s)
		h = 5*h + tolower(*s);

	return size_t(h);
}

/// 
inline size_t hash_filename(const wchar_t *s)
{
	if (!s)
		return 0;

	uint_t h = 0;
	for (; *s; ++s) {
		char letter = *s;
		if (letter=='\\') {
			letter='/';
		}
		h = 5 * h + tolower(letter);
	}

	return size_t(h);
}

struct hash_cstr
{
	inline size_t operator()(const char *s) const { return hash_string(s); }
};

struct hash_istr
{
	inline size_t operator()(const String &s) const { return hash_istring(s.c_str()); }
	inline size_t operator()(const char *s) const { return hash_istring(s); }
};

struct hash_pathname
{
	inline size_t operator()(const String &s) const { return hash_filename(s.c_str()); }
	inline size_t operator()(const char *s) const { return hash_filename(s); }
};

inline char __tofilenamechar(char c)
{
	c = tolower(c);
	if (c == '\\') c = '/';
	return c;
}

struct equal_pathname
{
	inline bool operator()(const String &s1, const String &s2) const {
		return operator()(s1.c_str(), s2.c_str());
	}
	inline bool operator()(const char *s1, const char *s2) const {
		for (uint_t i = 0; *s1 && *s2; i++, s1++, s2++) {
			if (__tofilenamechar(*s1) != __tofilenamechar(*s2))
				return false;
		}
		if (*s1 || *s2)	// if one isn't end
			return false;

		return true;
	}
};

struct equal_istr
{
	inline bool operator()(const String &s1, const String &s2) const {
		return StringUtil::stricmp(s1.c_str(), s2.c_str()) == 0;
	}
	inline bool operator()(const char *s1, const char *s2) const {
		return StringUtil::stricmp(s1, s2) == 0;
	}
};

struct equal_cstr
{
	inline bool operator()(const char *s1, const char *s2) const {
		return strcmp(s1, s2) == 0;
	}
};

template<typename T>
void hash_combine(size_t & seed, T const & v) {
	seed ^= std::tr1::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

AX_END_NAMESPACE

#endif // AX_CORE_HASH_H
