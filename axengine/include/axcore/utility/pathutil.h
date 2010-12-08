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

	static std::string cleanPath(const std::string &path);

	static const char *skipDir(const char *pathfile);
	static void toUnixPath(std::string &path);
	static void convertSlash(std::string &path, char slash);

	// static function
	static void splitPath(const std::string &path, std::string &dir, std::string &fname, std::string &ext);
	static std::string removeExt(const std::string &path);
	static std::string removeDir(const std::string &path);
	static std::string removeDirOne(const std::string &in);
	static std::string removeFilename(const std::string &in);

	// get directory from full file path
	static std::string getDir(const std::string &path);
	static std::string getExt(const std::string &path);
	static std::string getName(const std::string &path);
	static bool createDir(const std::string &OSPath);
	static bool haveDir(const std::string &path);
	static std::string normalizePath(const std::string &path);

	// return an empty string if can't get a correct relative path
	static std::string getRelativePath(const std::string &filename, const char *rootname = nullptr);

	static bool getFileModifiedTime(const std::string &filename, longlong_t *t);
	static StringSeq findFiles(const std::string &dir, const std::string &filter, bool dironly = false);
	static StringSeq listFileByExts(const std::string &base, const std::string &path, const std::string &exts, uint_t flags);
	static FileInfoSeq getFileInfos(const std::string &base, const std::string &path, const std::string &exts, int flags);
};

AX_END_NAMESPACE

#endif // end AX_PATHUTIL_H
