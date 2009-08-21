

#include "../private.h"
#include <sys/stat.h>			// file stat
#include <dirent.h>				// browse directory
#include <dlfcn.h>				// dynamic library load
#include <sys/time.h>			// gettimeofday


namespace AX {

void CDECL
Errorf( const char* fmt, ... ) {
    va_list		argptr;
    int			len;
    char		buffer[1024];

    String		err_msg = _("Occurred critical error. There are error message:\n\n");

    va_start( argptr, fmt );
    len = Vsnprintf( buffer, ArraySize( buffer ), fmt, argptr );
    va_end( argptr );

    err_msg += buffer;

	printf( "%s", err_msg.c_str() );

    if( gSystem ) {
		gSystem->Print( S_COLOR_RED );
		gSystem->Print( err_msg.c_str() );
    }

	throw "engine error";
	MessageBoxW( NULL, err_msg.c_str(), "Error", MB_ICONERROR | MB_OK | MB_TASKMODAL );

    exit( 1 );
}

void CDECL
Debugf( const char* fmt, ... ) {
    va_list		argptr;
    int			len;
    char		buffer[1024];

    va_start( argptr, fmt );
    len = Vsnprintf( buffer, ArraySize( buffer ), fmt, argptr );
    va_end( argptr );

    printf( "%s", buffer );

    if( gSystem ) {
		fprintf( gLogFile, "%s", buffer );
		gSystem->Print( S_COLOR_YELLOW );
		gSystem->Print( buffer );
    }
}

void CDECL
Printf( const char* fmt, ... ) {
    va_list		argptr;
    int			len;
    char		buffer[1024];

    va_start( argptr, fmt );
    len = Vsnprintf( buffer, ArraySize( buffer ), fmt, argptr );
    va_end( argptr );

    printf( "%s", buffer );

    if( gSystem ) {
		fprintf( gLogFile, "%s", buffer );
		gSystem->Print( buffer );
    }
}

bool
GetFileModifiedTime( const String& filename, longlong_t* t ) {
	const char* fn = filename.c_str();
	struct stat buf;

	if( stat( fn, &buf ) != 0 )
		return false;

	if( t )
		*t = buf.st_mtime;

	return true;
}

StringSeq
FindFiles( const String& dir, const String& filter, bool dironly ) {
	uint_t flags = 0;
	if( dironly ) {
		flags |= FileList_nofile;
	} else {
		flags |= FileList_nodirectory;
	}

	return FileListByExts( dir + '/', dir, filter, flags );
}

StringSeq
FileListByExts( const String& base, const String& path, const String& exts, uint_t flags ) {

	StringSeq		strvec;
	FileInfos	fileinfos;

	fileinfos = GetFileInfos( base, path, exts, flags );
	for( size_t i=0; i<fileinfos.size(); i++ ) {
		strvec.push_back( fileinfos[i].fullpath );
	}
	return strvec;
}

static bool
CmpFileInfoNameLess( const FileInfo& left, const FileInfo& right ) {
	return left.filename < right.filename;
}

FileInfos
GetFileInfos( const String& base, const String& path, const String& filters, int flags ) {
	FileInfos		fileinfos;
	int				num_files = 0;
	size_t			baselen = base.length();
	Filter			filter( filters );

	String 			dirname = path;
	if( IsDirectoryLetter( dirname[dirname.length() - 1] ) ) {
		dirname.resize( dirname.length() - 1 );
	}

	DIR*			pdir = NULL;
	struct stat		statbuf;
	struct dirent*	pdirent;

	if( ::lstat( dirname.c_str(), &statbuf ) != 0 )
		return fileinfos;				// can't read stat, return

	if( !S_ISDIR( statbuf.st_mode ) ) {
		return fileinfos;				// not a directory
	}
	
	dirname += AX_PATH_SEP;

	pdir = ::opendir( dirname.c_str() );
	if( pdir == NULL )
		return fileinfos;

	FileInfo	info;
	while( ( pdirent = ::readdir( pdir ) ) != NULL ) {
		if( ::lstat( pdirent->d_name, &statbuf ) == 0 )
			continue;					// can't read stat, goto next file

		const char* entname = pdirent->d_name;
		
		if( S_ISDIR( statbuf.st_mode ) ) {
			if( Strequ( entname, "." ) || Strequ( entname, ".." ) )
				continue;

			if( !(flags & FileList_needCVS) ) {
				if( Strequ( entname, "CVS" ) )
					continue;
			}

			if( flags & FileList_filterDirectory ) {
				if( !filter.In( entname ) )
					continue;
			}

			info.isDir = true;
		} else {
			if( flags & FileList_nofile )
				continue;

			if( !( flags & FileList_nofilterfile ) ) {
				if( !filter.In( entname ) )
					continue;
			}

			info.isDir = false;
		}

		// all checked, add to list
		info.fullpath = (path + "/" + entname).c_str() + baselen;
		info.filetype = FileType_real;
		info.filesize = statbuf.st_size;
		info.filetime = statbuf.st_mtime;
//		info.localtime = *_localtime64( &finddata.time_write );

		FileSystem::SplitPath( info.fullpath, info.filepath, info.filename, info.fileext );
		info.filename = FileSystem::StripPath( info.fullpath );

		fileinfos.push_back( info );

		num_files++;
	}

	// all readed, close dir
	if( ::closedir( pdir ) != 0 )
		Errorf( _("GetFileInfos: closedir error") );
	
	
	if( flags & FileList_sort ) {
		std::sort( fileinfos.begin(), fileinfos.end(), CmpFileInfoNameLess );
	}

	return fileinfos;
#if 0
	handle = _wfindfirst( const_cast<wchar_t *>(pattern.c_str()), &finddata );
	if( handle == -1 ) {
		return fileinfos;
	}

	do {
		FileInfo info;
		if( finddata.attrib & _A_SUBDIR ) {
			if( !Strcmp( finddata.name, L"." ) || !Strcmp( finddata.name, L".." ) ) {
				continue;
			}

			if( !(flags & FileList_needCVS) ) {
				if( !Strcmp( finddata.name, L"CVS" ) ) {
					continue;
				}
			}

			if( flags & FileList_nodirectory ) {
				continue;
			}

			if( flags & FileList_filterDirectory ) {
				if( !filter.In( finddata.name ) )
					continue;
			}

			info.isDir = true;
		} else {
			if( flags & FileList_nofile ) {
				continue;
			}

			if( !( flags & FileList_nofilterfile ) ) {
				if( !filter.In( finddata.name ) )
					continue;
			}

			info.isDir = false;
		}

		info.fullpath = (path + L"/" + finddata.name).c_str() + baselen;
		info.filetype = FileType_real;
		info.filesize = finddata.size;
		info.filetime = finddata.time_write;
		info.localtime = *_localtime64( &finddata.time_write );

		FileSystem::SplitPath( info.fullpath, info.filepath, info.filename, info.fileext );
		info.filename = FileSystem::StripPath( info.fullpath );

		fileinfos.push_back( info );

		num_files++;

	} while( _wfindnext( handle, &finddata ) != -1 );

	_findclose( handle );

	if( flags & FileList_sort ) {
		std::sort( fileinfos.begin(), fileinfos.end(), CmpFileInfoNameLess );
	}

	return fileinfos;
#endif
}

uint_t
Milliseconds( void ) {
	return Microseconds()  / 1000;
}

ulonglong_t
Microseconds( void ) {
	static longlong_t 	starttime;
	struct timeval 	tp;

	::gettimeofday( &tp, NULL );
	
	longlong_t curtime = tp.tv_sec * 1000 * 1000 + tp.tv_usec;
	
	if ( !starttime ) {
		starttime = curtime;
	}

	return curtime - starttime;
}

// TODO...
String
GetClipboardString() {
	String		out;
	return out;
}

handle_t
LoadSysDll( const String& name ) {
	String libname = "lib";
	libname += name + ".so";

	handle_t hDll = (handle_t)::dlopen( libname.c_str(), RTLD_LAZY );
	if( NULL == hDll ) {
		Errorf( _("LoadSysDll: can't load dll [%s]\n"), libname.c_str() );
	}
	return hDll;
}

handle_t
LoadDll( const String& name ) {
#ifdef _DEBUG
	String	libname = name + String("D");
	return LoadSysDll( libname );
#else
	return LoadSysDll( name );
#endif
}

void*
GetProcEntry( handle_t handle, const String& name ) {
	void* ret = ::dlsym( handle, name.c_str() );

	/* check that no error occured */
	const char* error_msg = dlerror();
	if (error_msg) {
		Errorf( _("GetProcEntry: Error locating %s - %s\n"), name.c_str(), error_msg );
	}
	
	return ret;
}

bool
FreeDll( handle_t handle ) {
	return ::dlclose( handle ) == 0 ? true : false;
}

static bool		__inited;
static bool		__haveit;
static byte_t	__macAddress[6];

bool
GetMacAddress( byte_t address[6] ) {
	if( __inited ) {
		memcpy( address, __macAddress, 6 );
		return __haveit;
	}

	if( 0 ) {
	} else {
		// or we return a rand address
		Debugf( _("GetMacAddress: can't find network adapter\n") );

		// trigger srand
		Milliseconds();

		// rand
		int rand1 = rand();
		int rand2 = rand();
		int rand3 = rand();

		__macAddress[0] = rand1 & 0xFF;
		__macAddress[1] = (rand1 >> 8) & 0xFF;
		__macAddress[2] = rand2 & 0xFF;
		__macAddress[3] = (rand2 >> 8) & 0xFF;
		__macAddress[4] = rand3 & 0xFF;
		__macAddress[5] = (rand3 >> 8) & 0xFF;
		__haveit = false;
		__inited = true;
	}

	memcpy( address, __macAddress, 6 );
	return __haveit;
}

String
GetHostName() {
	return "";
}


} // namespace AX
