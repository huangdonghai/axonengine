/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_STRING_H 
#define AX_STRING_H

AX_BEGIN_NAMESPACE

template< class T, class AllocT = Allocator<T> >
class Sequence : public std::vector< T, AllocT >
{};

template< class T, class AllocT = Allocator<T> >
class List : public std::list< T, AllocT >
{};

template<class _Kty,
	class _Ty,
	class _Hasher = std::tr1::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>,
	class _Alloc = std::allocator<std::pair<const _Kty, _Ty> > >
class Dict : public std::tr1::unordered_map<_Kty,_Ty,_Hasher,_Keyeq,_Alloc> {};

template<class _Kty,
	class _Hasher = std::tr1::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>,
	class _Alloc = std::allocator<_Kty> >
class DictSet : public std::tr1::unordered_set<_Kty,_Hasher,_Keyeq,_Alloc> {};


typedef Sequence<byte_t> ByteSeq;
typedef Sequence<int>	IntSeq;

typedef std::string String;
typedef Sequence<String>			StringSeq;
typedef List<String>				StringList;
typedef std::pair<String, String>	StringPair;
typedef Sequence<StringPair>		StringPairSeq;
typedef std::wstring WString;


AX_API WString u2w(const String &utf8str);
AX_API WString u2w(const char *utf8str);
AX_API String l2u(const char *localstr);
AX_API String l2u(const String &lstr);
AX_API String u2l(const char *localstr);
AX_API String u2l(const String &lstr);
AX_API String w2u(const wchar_t *localstr);
AX_API String w2u(const WString &localstr);
AX_API WString l2w(const char *localstr);
AX_API WString l2w(const String &lstr);
AX_API String w2l(const wchar_t *localstr);
AX_API String w2l(const WString &lstr);

#define _(ascii_str) (ascii_str)

struct AX_API StringUtil {
	static int vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);
	static int CDECL snprintf(char *buffer, size_t count, const char *format, ...);
	static int CDECL sprintf(String &str, const char *format, ...);
	static int stricmp(const char *string1, const char *string2);
	static int strnicmp(const char *string1, const char *string2, size_t count);
	static char *strncpyz(char *strDest, const char *strSource, size_t count);
	static char *strlwr(char *string);
	static StringList tokenize(const char *text, char split = ' ');
	static StringSeq tokenizeSeq(const char *text, char split = ' ');
	static bool filterString(const char *filter, const char *fname, bool casesensitive);
	static String CDECL format(const char *fmt, ...);
};

inline int StringUtil::vsnprintf(char *buffer, size_t count, const char *format, va_list argptr) {
	return ::vsnprintf_s(buffer, count, _TRUNCATE, format, argptr);
}

inline int CDECL StringUtil::snprintf(char *buffer, size_t count, const char *format, ...) {
	va_list argptr;
	int len;

	va_start(argptr,format);
	len = ::vsnprintf_s(buffer, count, _TRUNCATE, format, argptr);
	va_end(argptr);

	return len;
}

inline int CDECL StringUtil::sprintf(String &str, const char *format, ...) {
	char tmp[1024];
	va_list argptr;
	int len;

	va_start(argptr,format);
	len = ::vsnprintf_s(tmp, ArraySize(tmp), _TRUNCATE, format, argptr);
	va_end(argptr);

	str = tmp;

	return len;
}

inline String CDECL StringUtil::format(const char *fmt, ...)
{
	char tmp[1024];
	va_list argptr;
	int len;

	va_start(argptr, fmt);
	len = ::vsnprintf_s(tmp, ArraySize(tmp), _TRUNCATE, fmt, argptr);
	va_end(argptr);

	return String(tmp, len);
}

inline int StringUtil::stricmp(const char *string1, const char *string2) {
	return ::_stricmp(string1, string2);
}

inline int StringUtil::strnicmp(const char *string1, const char *string2, uint_t count) {
	return ::_strnicmp(string1, string2, count);
}

inline char *StringUtil::strncpyz(char *strDest, const char *strSource, uint_t count) {
	if (!strDest){
		return NULL;
	}
	if (!strSource){
		if (count){
			strDest[0] = 0;
		}
		return strDest;
	}
	::strncpy(strDest, strSource, count-1);
	strDest[count-1] = 0;
	return strDest;
}

inline char *StringUtil::strlwr(char *string) {
	return _strlwr(string);
}


AX_END_NAMESPACE


#endif // AX_STRING_H
