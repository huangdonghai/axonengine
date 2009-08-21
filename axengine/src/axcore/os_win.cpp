/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

#include <direct.h>
#include <io.h>
#include <windows.h>
#include <mmsystem.h>

namespace Axon {


	/*-----------------------------------------------------------------------------
		Nonstandard C API wrapper
	-----------------------------------------------------------------------------*/
	bool OsUtil::mkdir(const char* dirname ) {
		WString wdirname = u2w(dirname );
		int ret = ::_wmkdir(wdirname.c_str() );

		if (ret == -1 ) {
			errno_t err;
			_get_errno(&err );

			if (err == EEXIST ) {
				//Debugf("mkdir: Directory was not created because '%s' is the name of an existing file, directory, or device.\n", dirname );
				return false;
			} else if (err == ENOENT ) {
				//Debugf("mkdir(%s): Path was not found.\n", dirname );
				mkdir(PathUtil::removeFilename(dirname ).c_str() );
			}

			return mkdir(dirname );
		}

		return true;
	}

	String OsUtil::getworkpath() {
		wchar_t buf[260];
		wchar_t * v = ::_wgetcwd(buf, ArraySize(buf));
		AX_ASSURE(v);
		return w2u(buf);
	}


	int OsUtil::getScreenWidth()
	{
		return ::GetSystemMetrics(SM_CXSCREEN);
	}

	int OsUtil::getScreenHeight()
	{
		return ::GetSystemMetrics(SM_CYSCREEN);
	}


} // namespace Axon
