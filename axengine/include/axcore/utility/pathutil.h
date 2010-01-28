/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_PATHUTIL_H
#define AX_PATHUTIL_H

AX_BEGIN_NAMESPACE

// path: dir name ext
// path: dir fullname

struct AX_API PathUtil {
	static bool isDirectoryLetter(char ch) { return ch == '\\' || ch == '/'; }

	static String cleanPath(const String &path);

	static const char *skipDir(const char *pathfile);
	static void toUnixPath(String &path);
	static void convertSlash(String &path, char slash);

	// static function
	static void splitPath(const String &path, String &dir, String &fname, String &ext);
	static String removeExt(const String &path);
	static String removeDir(const String &path);
	static String removeDirOne(const String &in);
	static String removeFilename(const String &in);

	// get directory from full file path
	static String getDir(const String &path);
	static String getExt(const String &path);
	static String getName(const String &path);
	static bool createDir(const String &OSPath);
	static bool haveDir(const String &path);
	static String normalizePath(const String &path);

	// return an empty string if can't get a correct relative path
	static String getRelativePath(const String &filename, const char *rootname = nullptr);

	static bool getFileModifiedTime(const String &filename, longlong_t *t);
	static StringSeq findFiles(const String &dir, const String &filter, bool dironly = false);
	static StringSeq listFileByExts(const String &base, const String &path, const String &exts, uint_t flags);
	static FileInfoSeq getFileInfos(const String &base, const String &path, const String &exts, int flags);
};

AX_END_NAMESPACE

#endif // end AX_PATHUTIL_H
