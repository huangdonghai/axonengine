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


typedef std::vector<byte_t> ByteSeq;
typedef std::vector<int> IntSeq;

typedef std::vector<std::string> StringSeq;
typedef std::list<std::string> StringList;
typedef std::pair<std::string, std::string> StringPair;
typedef std::vector<StringPair> StringPairSeq;


AX_API std::wstring u2w(const std::string &utf8str);
AX_API std::wstring u2w(const char *utf8str);
AX_API std::string l2u(const char *localstr);
AX_API std::string l2u(const std::string &lstr);
AX_API std::string u2l(const char *localstr);
AX_API std::string u2l(const std::string &lstr);
AX_API std::string w2u(const wchar_t *localstr);
AX_API std::string w2u(const std::wstring &localstr);
AX_API std::wstring l2w(const char *localstr);
AX_API std::wstring l2w(const std::string &lstr);
AX_API std::string w2l(const wchar_t *localstr);
AX_API std::string w2l(const std::wstring &lstr);

#define _(ascii_str) (ascii_str)

struct AX_API StringUtil {
	static int vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);
	static int CDECL snprintf(char *buffer, size_t count, const char *format, ...);
	static int CDECL sprintf(std::string &str, const char *format, ...);
	static int stricmp(const char *string1, const char *string2);
	static int strnicmp(const char *string1, const char *string2, size_t count);
	static char *strncpyz(char *strDest, const char *strSource, size_t count);
	static char *strlwr(char *string);
	static StringList tokenize(const char *text, char split = ' ');
	static StringSeq tokenizeSeq(const char *text, char split = ' ');
	static bool filterString(const char *filter, const char *fname, bool casesensitive);
	static std::string CDECL format(const char *fmt, ...);
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

inline int CDECL StringUtil::sprintf(std::string &str, const char *format, ...) {
	char tmp[1024];
	va_list argptr;
	int len;

	va_start(argptr,format);
	len = ::vsnprintf_s(tmp, ArraySize(tmp), _TRUNCATE, format, argptr);
	va_end(argptr);

	str = tmp;

	return len;
}

inline std::string CDECL StringUtil::format(const char *fmt, ...)
{
	char tmp[1024];
	va_list argptr;
	int len;

	va_start(argptr, fmt);
	len = ::vsnprintf_s(tmp, ArraySize(tmp), _TRUNCATE, fmt, argptr);
	va_end(argptr);

	return std::string(tmp, len);
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
