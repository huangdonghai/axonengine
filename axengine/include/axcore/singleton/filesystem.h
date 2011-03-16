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
	SearchDir(const std::string &dir, bool extractSrc = false) : dir(dir), extractSrc(extractSrc) {}

	std::string dir;
	bool extractSrc;
};

class PakedFolder;
class File;
class FileSystem;


//------------------------------------------------------------------------------

class AX_API File : public ThreadSafe
{
	friend class FileSystem;

public:
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
	std::string getName() const;
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
	inline std::string readString() {
		std::string result;
		char ch;

		while (read(&ch, 1) && ch)
			result.push_back(ch);

		return result;
	}

	inline size_t writeInt(int i) { return write(&i, 4); }
	inline size_t writeFloat(float f) { return write(&f, 4); }
	inline size_t writeString(const std::string &s) { return write(s.c_str(), s.length() + 1); }

private:
	Type m_type;
	AccessMode m_mode;
	FILE *m_handle;
	void *m_unzFile;
	bool m_forceFlush;
	int m_pos;
	int m_size;
	int m_offset;
	std::string m_name;
};


class FileInfo
{
public:
	bool operator<(const FileInfo &rhs) const { return fullpath < rhs.fullpath; }
	bool operator==(const FileInfo &rhs) const { return fullpath == rhs.fullpath; }

	std::string fullpath;
	uint_t filetime;
	size_t filesize;
	bool isDir;
	File::Type filetype;

	// parsed file info
	std::string filepath;			// not include name and ext
	std::string filename;			// include ext
	std::string fileext;
	struct tm localtime;
};

typedef std::vector<FileInfo>	FileInfoSeq;

struct PakedFile {
	FilePackage *packfile;
	int offset;
};

struct FilePackage {
	std::string fullpath;
	std::string filename;
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

	void addPath(const std::string &fullpath);
	void addFile(const std::string &fullpath, PakedFile *info);
	void getFileInfos(const std::string &path, const std::string &exts, int flags, OUT FileInfoSeq &fis);

protected:
	PakedFolder *getPath(const std::string &path, bool bAddPath = true);

private:
	typedef Dict<std::string,PakedFile*> PackedFileDict;
	typedef std::list<PakedFolder*> PackedFolderList;
	std::string m_fullName;
	std::string m_name;
	PackedFileDict m_packedFileDict;
	PakedFolder *m_parent;
	PackedFolderList m_childrenList;
};

class AX_API IoRequest {
public:
	friend class AsioThread;
	friend class FileSystem;

	IoRequest(const std::string &filename, IEventHandler *handler);
	~IoRequest();

	IEventHandler *eventHandler() const { return m_eventHandler; }
	const std::string &fileName() const { return m_filename; }
	void freeData();
	int fileSize() const { return m_filesize; }
	void *fileData() const { return m_filedata; }

private:
	IEventHandler *m_eventHandler;
	std::string m_filename;
	int m_filesize;
	void *m_filedata;
};

class AX_API AsioCompletedEvent : public Event
{
public:
	AsioCompletedEvent(IoRequest *asioRead);

	IoRequest *asioRequest() { return m_asioRequest; }

private:
	IoRequest *m_asioRequest;
};

class AsioThread : public Thread, public ThreadSafe
{
public:
	void flush();
	virtual RunningStatus doRun();
	void queRequest(IoRequest *request);

protected:
	IoRequest *getFirstRequest();

private:
	std::list<IoRequest*> m_readEntries;
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

	void addGameDirectory(const std::string &dir, bool extractSrc = false);

	bool ospathToDataPath(const std::string &path, std::string &gpath);
	std::string dataPathToOsPath(const std::string &gpath);
	std::string modPathToOsPath(const std::string &modpath);
	std::string getDataPath();
	StringSeq fileListByExts(const std::string &path, const std::string &exts, int flags);
	FileInfoSeq getFileInfos(const std::string &path, const std::string &exts, int flags);

	/// below open file method return a File smart pointer, if failure, it contain
	/// NULL pointer. when the pointer destructed, file object auto released
	File *openFileByMode(const std::string &filename, File::AccessMode mode);
	File *openFileRead(const std::string &filename);
	File *openFileWrite(const std::string &filename);
	File *openFileAppend(const std::string &filename);

	size_t readFile(const std::string &filename, void** buffer);
	size_t readTextFile(const std::string &filename, void** buffer);
	void freeFile(void *buffer);
	void writeFile(const std::string &filename, const void *buffer, size_t size);

	bool getFileModifyTime(const std::string &filename, longlong_t *time) const;
	bool isFileExist(const std::string &filename) const;

	void queAsioRead(IoRequest *entry);
	void syncRead(IoRequest *entry);
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
	int compareFileModifyTime(const std::string &m_srcfile, const std::string &m_destfile);

	static std::string getDefaultDataPath();

protected:
	void attachFileObject(File *cf);
	void detachFileObject(File *cf);
	FilePackage *loadPakFile(const std::string &fullpath, const std::string &filename);
	void addPakedFileInfo(const FileInfo *fi);

	void checkGamePath();
	void addSearchPath();
	void addModPath();

private:
	std::list<SearchDir> m_searchDirs;
	std::list<File*> m_fileList;

	std::string m_workPath;		// exe run path
	std::string m_dataPath;		// main data path
	std::string m_gamePath;		// game root path
	std::string m_modPath;		// modification path, if no set, same as m_dataPath
	int m_numFilesInPack;
	int m_hashSeed;

	typedef Dict<std::string, PakedFile*, HashPath, EqualPath> EntryDict;
	typedef std::vector<const FilePackage*> FilePackages;	
	FilePackages m_pakFiles;
	EntryDict m_pakedFileDict;
	PakedFolder m_pakedFolders;
};

AX_END_NAMESPACE

#endif // AX_FILESYSTEM_H
