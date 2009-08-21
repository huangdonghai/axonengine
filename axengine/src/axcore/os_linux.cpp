
#include "pch.h"
#include "private.h"
#include <sys/stat.h>			// for mkdir
#include <sys/types.h> 

namespace AX {


/*-----------------------------------------------------------------------------
	Nonstandard C API wrapper
-----------------------------------------------------------------------------*/

bool
Mkdir( const char* dirname ) {
	int ret = ::mkdir( dirname, 0777 );

	if( ret == -1 ) {
#if 0
		errno_t err;
		_get_errno( &err );
#endif
		if( errno == EEXIST ) {
			Debugf( _("Mkdir: Directory was not created because '%s' is the name of an existing file, directory, or device.\n"), dirname );
			return false;
		} else if( errno == ENOENT ) {
			Debugf( _("Mkdir(%s): Path was not found.\n"), dirname );
			Mkdir( StripFilename( dirname ).c_str() );
		}

		return Mkdir( dirname );
	}

	return true;
}

char*
Getcwd( char *buffer, int maxlen ) {
	return ::getcwd( buffer, maxlen );
}

int
Vsnprintf( char *buffer, size_t count, const char* format, va_list argptr ) {
	return ::vsnprintf( buffer, count, format, argptr );
}

int CDECL
Snprintf( char *buffer, size_t count, const char *format, ... ) {
	va_list		argptr;
	int		len;

	va_start( argptr,format );
	len = ::vsnprintf( buffer, count, format, argptr );
	va_end( argptr );

	return len;
}

int CDECL
Sprintf( String& str, const char* format, ... ) {
	char	tmp[1024];
	va_list	argptr;
	int	len;

	va_start( argptr,format );
	len = ::vsnprintf( tmp, ArraySize( tmp ), format, argptr );
	va_end( argptr );

	str = tmp;

	return len;
}

int
Stricmp( const char *string1, const char *string2 ) {
	return ::strcasecmp( string1, string2 );
}

int
Strnicmp( const char *string1, const char *string2, uint_t count ) {
	return ::strncasecmp( string1, string2, count );
}

char*
Strncpyz( char *strDest, const char *strSource, uint_t count ) {
	if( !strDest ){
		return NULL;
	}
	if( !strSource ){
		if( count ){
			strDest[0] = 0;
		}
		return strDest;
	}
	::strncpy( strDest, strSource, count-1 );
	strDest[count-1] = 0;
	return strDest;
}

char*
Strlwr( char *string ) {
	char* s = string;
	while( *s ) {
		*s = ::tolower( *s );
	}
	return string;
}

char*
Strcpy( char* dst, const char* src ) {
	return ::strcpy( dst, src );
}

char*
Strcat( char* dst, const char* src ) {
	return ::strcat( dst, src );
}

int
Strcmp( const char* s1, const char* s2 ) {
	return ::strcmp( s1, s2 );
}

size_t
Strlen( const char* s ) {
	return ::strlen( s );
}

const char*
Strchr( const char* s, int c ) {
	return ::strchr( s, c );
}

size_t
Strcspn( const char* s, const char* sub ) {
	return ::strcspn( s, sub );
}

char*
Strdup( const char* s ) {
	return ::strdup( s );
}

char*
Strncat( char* dst, const char* src, size_t count ) {
	return ::strncat( dst, src, count );
}

int
Strncmp( const char* s1, const char* s2, size_t count ) {
	return ::strncmp( s1, s2, count );
}

const char*
Strpbrk( const char* s, const char* sub ) {
	return ::strpbrk( s, sub );
}

const char*
Strrchr( const char* s, int c ) {
	return ::strrchr( s, c );
}

size_t
Strspn( const char* s, const char* sub ) {
	return ::strspn( s, sub );
}

const char*
Strstr( const char* s1, const char* s2 ) {
	return ::strstr( s1, s2 );
}

char*
Strtok( char* s, const char* delimit ) {
	return ::strtok( s, delimit );
}

char*
Strupr( char* str ) {
	char* s = str;
	while( *s ) {
		*s = ::toupper( *s );
	}
	return str;
}

int
Atoi( const char* s ) {
	return atoi( s );
}

longlong_t
Atoi64( const char* string ) {
	return atoll( string );
}

double
Atof( const char* string ) {
	return atof( string );
}

/*-----------------------------------------------------------------------------
	Wide char version C API wrapper
-----------------------------------------------------------------------------*/
#if 0
bool
Mkdir( const String& dirname ) {
	SCOPE_CONVERT;
	
	return Mkdir( STR2U( dirname ) );
}

wchar_t*
Getcwd( wchar_t *buffer, int maxlen ) {
	SCOPE_CONVERT;
	
	char	ustr[maxlen];
	
	Getcwd( ustr, maxlen );
	wchar_t* wstr = U2W( ustr );
	::memcpy( buffer, wstr, maxlen * sizeof( wchar_t ) );
	
	return buffer;
}

int
Vsnprintf( wchar_t *buffer, size_t count, const wchar_t* format, va_list argptr ) {
	return ::vswprintf( buffer, count, format, argptr );
}

int CDECL
Snprintf( wchar_t *buffer, size_t count, const wchar_t *format, ... ) {
	va_list		argptr;
	int		len;

	va_start( argptr,format );
	len = ::vswprintf( buffer, count, format, argptr );
	va_end( argptr );

	return len;
}


int CDECL
Sprintf( String& str, const wchar_t* format, ... ) {
	wchar_t	tmp[1024];
	va_list	argptr;
	int	len;

	va_start( argptr,format );
	len = ::vswprintf( tmp, ArraySize( tmp ), format, argptr );
	va_end( argptr );

	str = tmp;

	return len;
}

int CDECL
Sprintf( Ustring& str, const char* format, ... ) {
	char	tmp[1024];
	va_list	argptr;
	int	len;

	va_start( argptr,format );
	len = ::vsnprintf( tmp, ArraySize( tmp ), format, argptr );
	va_end( argptr );

	str = tmp;

	return len;
}


int
Stricmp( const wchar_t *string1, const wchar_t *string2 ) {
	return ::wcscasecmp( string1, string2 );
}

int
Strnicmp( const wchar_t *string1, const wchar_t *string2, UInt count ) {
	return ::wcsncasecmp( string1, string2, count );
}

wchar_t*
Strncpyz( wchar_t *strDest, const wchar_t *strSource, UInt count ) {
	if( !strDest ){
		return NULL;
	}
	if( !strSource ){
		if( count ){
			strDest[0] = 0;
		}
		return strDest;
	}
	::wcsncpy( strDest, strSource, count-1 );
	strDest[count-1] = 0;
	return strDest;
}

wchar_t*
Strlwr( wchar_t *string ) {
	wchar_t* ws = string;
	while( *ws ) {
		*ws = ::towlower( *ws );
	}
	return string;
}

wchar_t*
Strcpy( wchar_t* dst, const wchar_t* src ) {
	return ::wcscpy( dst, src );
}

wchar_t*
Strcat( wchar_t* dst, const wchar_t* src ) {
	return ::wcscat( dst, src );
}

int
Strcmp( const wchar_t* s1, const wchar_t* s2 ) {
	return ::wcscmp( s1, s2 );
}

size_t
Strlen( const wchar_t* s ) {
	return ::wcslen( s );
}

const wchar_t*
Strchr( const wchar_t* s, wchar_t c ) {
	return ::wcschr( s, c );
}

size_t
Strcspn( const wchar_t* s, const wchar_t* sub ) {
	return ::wcscspn( s, sub );
}

wchar_t*
Strdup( const wchar_t* s ) {
	return wcsdup( s );
}

wchar_t*
Strncat( wchar_t* dst, const wchar_t* src, size_t count ) {
	return ::wcsncat( dst, src, count );
}

int
Strncmp( const wchar_t* s1, const wchar_t* s2, size_t count ) {
	return ::wcsncmp( s1, s2, count );
}

const wchar_t*
Strpbrk( const wchar_t* s, const wchar_t* sub ) {
	return ::wcspbrk( s, sub );
}

const wchar_t*
Strrchr( const wchar_t* s, wchar_t c ) {
	return ::wcsrchr( s, c );
}

size_t
Strspn( const wchar_t* s, const wchar_t* sub ) {
	return ::wcsspn( s, sub );
}

const wchar_t*
Strstr( const wchar_t* s1, const wchar_t* s2 ) {
	return ::wcsstr( s1, s2 );
}

wchar_t*
Strtok( wchar_t* s, const wchar_t* delimit ) {
	Errorf( L"Not Implemented" );
	return NULL;
}

wchar_t*
Strupr( wchar_t* string ) {
	wchar_t* ws = string;
	while( *ws ) {
		*ws = ::towupper( *ws );
	}
	return string;
}

int
Atoi( const wchar_t* s ) {
	wchar_t* endptr;
	return ::wcstol( s, &endptr, 10 );
}

Long
Atoi64( const wchar_t* string ) {
	wchar_t* endptr;
	return ::wcstoll( string, &endptr, 10 );
}

double
Atof( const wchar_t* string ) {
	wchar_t* endptr;
	return ::wcstof( string, &endptr );
}
#endif
} // namespace AX
