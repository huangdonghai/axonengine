/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_FILESYSTEM_H
#define AX_FILESYSTEM_H

#define AX_PATH_SEP '/'
#define AX_PATH_SEP_STR "/"

AX_BEGIN_NAMESPACE


class FileInfo;
struct PakedFile;
struct FilePackage;

struct SearchDir {
	SearchDir(const String &dir, bool extractSrc = false) : dir(dir), extractSrc(extractSrc) {}

	String dir;
	bool extractSrc;
};
typedef List<SearchDir> SearchDirs;

class PakedFolder;
class File;
class FileSystem;


// global variable
extern const char *const DEFAULT_CACHES_PATH;

//------------------------------------------------------------------------------
// class File
//------------------------------------------------------------------------------

class AX_API File : public ThreadSafe
{
public:
	friend class FileSystem;
	
	enum AccessMode {
		ReadMode,
		WriteMode,
		AppendMode
	};

	enum Type {
		kStdio,
		kPacked
	};

	enum SeekMode {
		SeekCurrent,
		SeekEnd,
		SeekSet
	};

	enum ListFlag {
		List_Nodirectory = 1,	// don't return directory
		List_Nofile = 2,	// don't return file
		List_Sorted = 8,	// sort result
		List_NeedCVS = 0x10,	// need CVS directory, don't ignore it. default don't return CVS directory
		List_DnotFilterFile = 0x20,	// don't filter file by filters, default filter file
		List_FilterDirectory = 0x40,	// filter directory by filters, default don't filter directory
	};

	File();
	virtual ~File();

	FILE *getHandle() const { return m_handle; }
	String getName() const;
	Type getType() const;
	AccessMode getMode() const;
	size_t read(void *buffer, size_t len);
	char *readLine(char *buffer, int n);
	size_t write(const void *buffer, size_t len);
	int writeLine(const char *str);
	bool isReading() const;
	size_t size();
	int seek(int offset, SeekMode origin);
	int fTell();
	bool isEnd();
	int flush();
	int forceFlush();
	int close();
	size_t CDECL printf(const char *format, ...);

	// read and write
	inline int readInt() { int i; read(&i, 4); return i; }
	inline float readFloat() { float f; read(&f, 4); return f; }
	inline String readString() {
		String result;
		char ch;

		while (read(&ch, 1) && ch)
			result.push_back(ch);

		return result;
	}

	inline size_t writeInt(int i) { return write(&i, 4); }
	inline size_t writeFloat(float f) { return write(&f, 4); }
	inline size_t writeString(const String &s) { return write(s.c_str(), s.length() + 1); }

private:
	Type m_type;
	AccessMode m_mode;
	FILE *m_handle;
	void *m_unzFile;
	bool m_forceFlush;
	int m_pos;
	int m_size;
	int m_offset;
	String m_name;
};


typedef List<File*>			FileList;

class FileInfo
{
public:
	bool operator<(const FileInfo &rhs) const { return fullpath < rhs.fullpath; }
	bool operator==(const FileInfo &rhs) const { return fullpath == rhs.fullpath; }

	String fullpath;
	uint_t filetime;
	size_t filesize;
	bool isDir;
	File::Type filetype;

	// parsed file info
	String filepath;			// not include name and ext
	String filename;			// include ext
	String fileext;
	struct tm localtime;
};

typedef Sequence<FileInfo>	FileInfoSeq;

struct PakedFile {
	FilePackage *packfile;
	int offset;
};

struct FilePackage {
	String fullpath;
	String filename;
	void *unzfile;
	int checksum;
	int checksumkey;
	int numEntries;
	int referenced;
};

//------------------------------------------------------------------------------
// class PakedFolder
//------------------------------------------------------------------------------

class AX_API PakedFolder
{
public:
	PakedFolder();
	~PakedFolder();

	void addPath(const String &fullpath);
	void addFile(const String &fullpath, PakedFile *info);
	void getFileInfos(const String &path, const String &exts, int flags, OUT FileInfoSeq &fis);

protected:
	PakedFolder *getPath(const String &path, bool bAddPath = true);

private:
	typedef Dict<String,PakedFile*> PackedFileDict;
	typedef List<PakedFolder*> PackedFolderList;
	String m_fullName;
	String m_name;
	PackedFileDict m_packedFileDict;
	PakedFolder *m_parent;
	PackedFolderList m_childrenList;
};

struct AsioRead {
	String filename;
	size_t filesize;
	void *filedata;
	AtomicInt syncCounter; // if count == 0, data is ready
};

//------------------------------------------------------------------------------
// class FileSystem
//------------------------------------------------------------------------------

class AX_API FileSystem : public ThreadSafe
{
public:
	friend class File;

	FileSystem();
	virtual ~FileSystem();

	void initialize();
	void finalize();

	void addGameDirectory(const String &dir, bool extractSrc = false);

	bool ospathToDataPath(const String &path, String &gpath);
	String dataPathToOsPath(const String &gpath);
	String modPathToOsPath(const String &modpath);
	String getDataPath();
	StringSeq fileListByExts(const String &path, const String &exts, int flags);
	FileInfoSeq getFileInfos(const String &path, const String &exts, int flags);

	/// below open file method return a File smart pointer, if failure, it contain
	/// NULL pointer. when the pointer destructed, file object auto released
	File *openFileByMode(const String &filename, File::AccessMode mode);
	File *openFileRead(const String &filename);
	File *openFileWrite(const String &filename);
	File *openFileAppend(const String &filename);

	size_t readFile(const String &filename, void** buffer);
	size_t readTextFile(const String &filename, void** buffer);
	void freeFile(void *buffer);
	void writeFile(const String &filename, const void *buffer, size_t size);

	bool getFileModifyTime(const String &filename, longlong_t *time) const;
	bool isFileExist(const String &filename) const;

	void queAsioRead(AsioRead *entry);
	void flushAsio();

	/*
	 * Compare file modify time
	 *
	 * Return Values
	 *  -2 First file is not exist. 
	 *  -1 First file time is less than second file time. 
	 *  0 First file time is equal to second file time. 
	 *  1 First file time is greater than second file time. 
	 *  2 Second file is not exist. 
	 */
	int compareFileModifyTime(const String &m_srcfile, const String &m_destfile);

	static String getDefaultDataPath();

protected:
	void attachFileObject(File *cf);
	void detachFileObject(File *cf);
	FilePackage *loadPakFile(const String &fullpath, const String &filename);
	void addPakedFileInfo(const FileInfo *fi);

	void checkGamePath();
	void addSearchPath();
	void addModPath();

private:
	SearchDirs m_searchDirs;
	FileList m_fileList;

	String m_workPath;		// exe run path
	String m_dataPath;		// main data path
	String m_gamePath;		// game root path
	String m_modPath;		// modification path, if no set, same as m_dataPath
	int m_numFilesInPack;
	int m_hashSeed;

	typedef Dict<String, PakedFile*, HashPath, EqualPath> EntryDict;
	typedef Sequence<const FilePackage*> FilePackages;	
	FilePackages m_pakFiles;
	EntryDict m_pakedFileDict;
	PakedFolder m_pakedFolders;
};

AX_END_NAMESPACE

#endif // AX_FILESYSTEM_H
