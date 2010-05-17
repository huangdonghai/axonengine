/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"
#include "../minizip/unzip.h"
#include "gzip.h"



AX_BEGIN_NAMESPACE

namespace {
	/// default game path
	const char *const DEFAULT_DATA_PATH = "data";

	int fileLength(FILE *fp)
	{
		int pos;
		int end;

		pos = ftell(fp);
		fseek(fp, 0, SEEK_END);
		end = ftell(fp);
		fseek(fp, pos, SEEK_SET);

		return end;
	}

	String fullPath(const String &base, const String &path, const String &file)
	{
		String fullpath;

		fullpath = base;

		// if ending isn't '/' or '\\', add one
		if (!PathUtil::isDirectoryLetter(fullpath[fullpath.length()])) {
			fullpath += "/";
		}
		fullpath += path + AX_PATH_SEP + file;

		for (int i=0; fullpath[i]; i++) {
			if (PathUtil::isDirectoryLetter(fullpath[i]))
				fullpath.at(i) = AX_PATH_SEP;
		}

		return fullpath;
	}

	String fullPath(const String &base, const String &file)
	{
		String fullpath = fullPath(base, file, "");

		//	fullpath[fullpath.length()-1] = 0;
		fullpath.resize(fullpath.length()-1);

		return fullpath;
	}

	class AsioThread : public Thread, public ThreadSafe
	{
	public:
		void flush()
		{
			SCOPE_LOCK;

			while (!m_readEntries.empty()) {
				OsUtil::sleep(0);
			}
		}

		virtual void doRun()
		{
			while (1) {
				AsioRead *request = getFirstRequest();

				if (!request) {
					OsUtil::sleep(0);
					continue;
				}

				// do read
				request->filesize = g_fileSystem->readFile(request->filename, &request->filedata);
				request->syncCounter.decref();

				removeFirstRequest();
			}
		}

		void queRequest(AsioRead *request)
		{
			SCOPE_LOCK;
			m_readEntries.push_back(request);
		}

	protected:
		AsioRead *getFirstRequest()
		{
			SCOPE_LOCK;
			if (m_readEntries.empty())
				return 0;
			return m_readEntries.front();
		}

		void removeFirstRequest()
		{
			SCOPE_LOCK;
			m_readEntries.pop_front();
		}

	private:
		List<AsioRead*> m_readEntries;
	};

	AsioThread *s_asioThread = 0;

} // anonymous namespace

//------------------------------------------------------------------------------
// class PakedFolder
//------------------------------------------------------------------------------

PakedFolder::PakedFolder()
	: m_parent(0)
{}

PakedFolder::~PakedFolder()
{}

void PakedFolder::addPath(const String &fullpath)
{
	getPath(fullpath);
}

void PakedFolder::addFile(const String &fullpath, PakedFile *info)
{
	String path = PathUtil::getDir(fullpath);

	PakedFolder *pf = getPath(path);

	AX_ASSERT(pf);

	pf->m_packedFileDict[fullpath] = info;
}

void PakedFolder::getFileInfos(const String &path, const String &exts, int flags, OUT FileInfoSeq &fis)
{
	String new_path = path;

	if (!PathUtil::isDirectoryLetter(*new_path.rbegin())) {
		new_path += '/';
	}

	PakedFolder *pf = getPath(new_path, false);

	if (!pf)
		return;

	if (!(flags & File::List_Nodirectory)) {
		PackedFolderList::const_iterator it = pf->m_childrenList.begin();
		for (; it != pf->m_childrenList.end(); ++it) {
			FileInfo info;
			memset(&info, 0, sizeof(info));

			info.fullpath = (*it)->m_fullName;
			info.fullpath.resize(info.fullpath.size() - 1);
			info.isDir = true;
			PathUtil::splitPath(info.fullpath, info.filepath, info.filename, info.fileext);
			info.filename = PathUtil::removeDir(info.fullpath);

			fis.push_back(info);
		}
	}

	if (flags & File::List_Nofile)
		return;

	PackedFileDict::iterator it;
	Filter filter(exts);


	for (it = pf->m_packedFileDict.begin(); it != pf->m_packedFileDict.end(); ++it) {
		FileInfo info;
		PakedFile *pf = it->second;
		unz_file_info file_info;
		char entryName[260];

		info.fullpath = it->first;
		info.isDir = false;

		unzSetOffset(pf->packfile->unzfile, pf->offset);
		if (unzGetCurrentFileInfo(pf->packfile->unzfile, &file_info, entryName, sizeof(entryName), NULL, 0, NULL, 0) != UNZ_OK)
			continue;

		info.filesize = file_info.uncompressed_size;
		info.filetype = File::kStdio;
		info.localtime.tm_year = file_info.tmu_date.tm_year - 1900;
		info.localtime.tm_mon = file_info.tmu_date.tm_mon;
		info.localtime.tm_mday = file_info.tmu_date.tm_mday;
		info.localtime.tm_hour = file_info.tmu_date.tm_hour;
		info.localtime.tm_min = file_info.tmu_date.tm_min;
		info.localtime.tm_sec = file_info.tmu_date.tm_sec;

		info.filetime = mktime(&info.localtime);

		PathUtil::splitPath(info.fullpath, info.filepath, info.filename, info.fileext);
		info.filename = PathUtil::removeDir(info.fullpath);

		if (!(flags & File::List_DnotFilterFile)) {
			if (!filter.In(info.filename))
				continue;
		}
		fis.push_back(info);
	}
}

PakedFolder *PakedFolder::getPath(const String &path, bool bAddPath)
{
	if (path == m_fullName)
		return this;

	if (StringUtil::strnicmp(m_fullName.c_str(), path.c_str(), m_fullName.size()) != 0)
		Errorf(_("error path"));

	String::size_type pos = path.find(AX_PATH_SEP_STR, m_fullName.size());

	// get subpath name
	String subpath = path;
	if (pos != String::npos) {
		subpath.resize(pos + 1);	// we need '/', so + 1
	}

	// find subpath
	PakedFolder *ret = NULL;
	PackedFolderList::iterator it;
	for (it = m_childrenList.begin(); it != m_childrenList.end(); ++it) {
		if ((*it)->m_fullName == subpath) {
			ret = *it;
			break;
		}
	}

	// if not found, add new one
	if (it == m_childrenList.end()) {
		if (!bAddPath)
			return NULL;

		ret = new PakedFolder();
		ret->m_fullName = subpath;
		ret->m_parent = this;
		m_childrenList.push_back(ret);
	}

	return ret->getPath(path);
}


//------------------------------------------------------------------------------
// class File
//------------------------------------------------------------------------------

File::File() : m_handle(0), m_unzFile(NULL), m_size(0)
{
	g_fileSystem->attachFileObject(this);
}

File::~File()
{
	close();

	/// delete from FileSystem
	g_fileSystem->detachFileObject(this);
}

String File::getName() const
{
	SCOPE_LOCK;
	return m_name;
}

File::Type File::getType() const
{
	SCOPE_LOCK;
	return m_type;
}

File::AccessMode File::getMode() const
{
	SCOPE_LOCK;
	return m_mode;
}

size_t File::read(void *buffer, size_t len)
{
	SCOPE_LOCK;

	if (m_type == kStdio) {
		len = fread(buffer, 1, len, m_handle);

		AX_ASSERT(len != size_t(-1));
	} else {
		len = unzReadCurrentFile(m_unzFile, buffer, (uint_t)len);
		AX_ASSERT(len != size_t(-1));
	}

	return len;
}

char *File::readLine(char *buffer, int n)
{
	SCOPE_LOCK;

	if (m_type == kStdio) {
		return fgets(buffer, n, m_handle);
	} else {
		// TODO
		Errorf(_("File::readLine: not implemented yet"));
		return NULL;
	}
}

size_t File::write(const void *buffer, size_t len)
{
	SCOPE_LOCK;

	size_t size = len;
	const byte_t *buf = (byte_t*)buffer;

	bool failedonce = false;
	while (size) {
		size_t written = 0;
		written = fwrite(buf, 1, size, m_handle);

		if (!written) {
			if (!failedonce) {
				failedonce = true;
			} else {
				return 0;
			}
		} else if (written == size_t(-1)) {
			return 0;
		}

		size -= written;
		buf += written;
	}

	if (m_forceFlush) {
		fflush(m_handle);
	}

	return len;
}

int File::writeLine(const char *str)
{
	SCOPE_LOCK;

	return fputs(str, m_handle);
}

bool File::isReading() const
{
	return (m_mode == ReadMode);
}

size_t File::size()
{
	SCOPE_LOCK;

	return m_size;
}

int File::seek(int offset, File::SeekMode origin)
{
	SCOPE_LOCK;

	int whence;

	if (m_type == kPacked) {
		if (offset == 0 && origin == SeekSet) {
			unzSetOffset(m_unzFile, m_offset);
			return unzOpenCurrentFile(m_unzFile);
		} else {
			return -1;
		}
	}

	switch (origin) {
	case SeekCurrent:
		whence = SEEK_CUR;
		break;
	case SeekEnd:
		whence = SEEK_END;
		break;
	case SeekSet:
		whence = SEEK_SET;
		break;
	default:
		whence = SEEK_SET;
	}

	return fseek(m_handle, offset, whence);
}

int File::fTell()
{
	SCOPE_LOCK;

	if (m_type == kPacked) {
		return unztell(m_unzFile);
	} else {
		return ftell(m_handle);
	}
}

bool File::isEnd()
{
	return (fTell() == m_size);
}

int File::flush()
{
	SCOPE_LOCK;
	return fflush(m_handle);
}

int File::forceFlush()
{
	SCOPE_LOCK;
	return setvbuf(m_handle, NULL, _IONBF, 0);
}

int File::close()
{
	SCOPE_LOCK;

	int ret = 0;
	if (m_type == File::kPacked) {
		if (m_unzFile) {
			ret = unzCloseCurrentFile(m_unzFile);
			ret|= unzClose(m_unzFile);
		}
	} else {
		if (m_handle)
			ret = fclose(m_handle);
	}

	m_handle = 0;
	m_unzFile = 0;

	return ret;
}

size_t CDECL File::printf(const char *format, ...)
{
	char temp[4096];
	va_list argptr;
	int len;

	va_start(argptr, format);
	len = vsnprintf(temp, ArraySize(temp), format, argptr);
	va_end(argptr);

	return write(temp, len);
}

//------------------------------------------------------------------------------
// class FileSystem
//------------------------------------------------------------------------------

FileSystem::FileSystem()
	: m_numFilesInPack(0)
{
	s_asioThread = new AsioThread;
	s_asioThread->startThread();
}

FileSystem::~FileSystem()
{
	finalize();
}

void FileSystem::initialize()
{
	SCOPE_LOCK;

	finalize();

	Printf(_("Initializing FileSystem...\n"));
	m_workPath = PathUtil::normalizePath(OsUtil::getworkpath());

//		addGameDirectory(m_workPath);

	m_gamePath = PathUtil::removeFilename(m_workPath);

	m_dataPath = m_gamePath + AX_PATH_SEP_STR + DEFAULT_DATA_PATH;

	addGameDirectory(m_dataPath);

	checkGamePath();

#if 0
	m_dataPath = m_gamePath + AX_PATH_SEP_STR + DEFAULT_DATA_PATH;

	addGameDirectory(m_dataPath);
#endif
	addSearchPath();

	addModPath();

	Printf(_("Initialized FileSystem\n"));
}

void FileSystem::finalize()
{}


void FileSystem::addGameDirectory(const String &dir, bool extractSrc)
{
	SCOPE_LOCK;

	/* check if already added */
	SearchDirs::const_iterator pos = m_searchDirs.begin();
	for (; pos != m_searchDirs.end(); ++pos) {
		if (pos->dir == dir) {
			Printf("duplicated game directory '%s', ingored\n", dir.c_str());
			return;
		}
	}

	m_searchDirs.push_front(SearchDir(dir, extractSrc));

	/* add any packfile files */
	StringSeq strvec = PathUtil::findFiles(dir, "*.pak");

	/* sort alphabetically */
	std::sort(strvec.begin(), strvec.end());

	StringSeq::iterator it = strvec.begin();
	for (; it != strvec.end(); ++it) {
		String fullpath = fullPath(dir, *it);

		FilePackage *packfile = loadPakFile(fullpath, *it);

		m_pakFiles.push_back(packfile);
	}

	Printf(_("..Added game directory '%s'\n"), dir.c_str());
}


bool FileSystem::ospathToDataPath(const String &path, String &gpath)
{
	SCOPE_LOCK;

	size_t len = m_dataPath.length();
	size_t i;

	gpath.clear();

	// check if leading game data path
	for (i=0; i<len; i++) {
		if (PathUtil::isDirectoryLetter(m_dataPath[i])) {
			if (! PathUtil::isDirectoryLetter(path[i])) {
				break;
			}
		} else {
			if (tolower(m_dataPath[i]) != tolower(path[i]))
				break;
		}
	}

	// yes, have game data path leading
	if (i==len) {
		if (PathUtil::isDirectoryLetter(path[len])) {
			gpath = path.c_str() + len + 1;
		}
		return true;
	}

	// check if have DEFAULT_DATA_PATH set
	size_t pos = path.find(DEFAULT_DATA_PATH);

	if (pos == std::string::npos)
		return false;

	if (!PathUtil::isDirectoryLetter(path[pos-1]))
		return false;

	if (!PathUtil::isDirectoryLetter(path[ pos + strlen(DEFAULT_DATA_PATH) ]))
		return false;

	gpath = path.c_str() + pos + strlen(DEFAULT_DATA_PATH) + 1;

	return true;
}

String FileSystem::dataPathToOsPath(const String &gpath)
{
	String fullpath = m_dataPath;
	if (gpath.length()) {
		if (!PathUtil::isDirectoryLetter(gpath[0])) {
			fullpath += "/";
		}
		fullpath += gpath;
	}
	return fullpath;
}

String FileSystem::getDataPath()
{
	return m_dataPath;
}

StringSeq FileSystem::fileListByExts(const String &path, const String &exts, int flags)
{
	SCOPE_LOCK;

	String datapath = m_dataPath;
	datapath += "/";
	String fullpath = dataPathToOsPath(path);

	return PathUtil::listFileByExts(datapath, fullpath, exts, flags);
}

FileInfoSeq FileSystem::getFileInfos(const String &path, const String &exts, int flags)
{
	SCOPE_LOCK;

	FileInfoSeq fis;

	m_pakedFolders.getFileInfos(path, exts, flags, fis);

	AX_FOREACH(const SearchDir &sp, m_searchDirs) {
		String datapath = sp.dir + AX_PATH_SEP_STR;
		String fullpath = sp.dir;

		if (path.length()) {
			if (!PathUtil::isDirectoryLetter(path[0])) {
				fullpath += AX_PATH_SEP_STR;
			}
			fullpath += path ;
		}

		FileInfoSeq temp = PathUtil::getFileInfos(datapath, fullpath, exts, flags);

		fis.insert(fis.end(), temp.begin(), temp.end());
	}
#if 1
	std::sort(fis.begin(), fis.end());
	fis.erase(std::unique(fis.begin(), fis.end()), fis.end());
#endif
	return fis;
}


File *FileSystem::openFileByMode(const String &filename, File::AccessMode mode)
{
	SCOPE_LOCK;

	bool sync;
	File *file = 0;

	sync = false;

	switch (mode) {
	case File::ReadMode:
		file = openFileRead(filename);
		break;
	case File::WriteMode:
		file = openFileWrite(filename);
		break;
	case File::AppendMode:
		file = openFileAppend(filename);
		break;
	default:
		Errorf(_("error FileMode is %d"), mode);
	}

	return file;
}

File *FileSystem::openFileRead(const String &filename)
{
	SCOPE_LOCK;

	File *file = nullptr;

	if (filename.empty())
		return file;

	bool doExtract = false;

	if (g_cvarSystem && fs_extract.getBool())
		doExtract = true;

	SearchDirs::iterator sp = m_searchDirs.begin();

	for (; sp != m_searchDirs.end(); ++sp) {
		/* real file */
		String path = fullPath(sp->dir, filename);

		FILE *fp = NULL;
		gzFile gzf = NULL;

		fp = fopen(u2l(path).c_str(), "rb");
		if (!fp) {
			continue;
		}

		/* got normal file */
		file = new File;
		file->m_type = File::kStdio;
		file->m_handle = fp;
		file->m_name = filename;
		file->m_mode = File::ReadMode;

		file->m_size =  fileLength(fp);
		attachFileObject(file);

		if (sp->extractSrc && doExtract) {
			String extractDir = m_gamePath + '/' + fs_extractDir.getString() + '/' + filename;

			PathUtil::createDir(extractDir);

			FILE *fw = fopen(u2l(extractDir).c_str(), "wb");
			if (fw) {
				byte_t *buf = new byte_t[file->m_size];
				file->read( buf, file->m_size);
				file->seek(0, File::SeekSet);
				fwrite(buf, file->m_size, 1, fw);
				fclose(fw);
				delete[] buf;
			}
		}

		return file;
	}

	/// not found, try packed file
	String fn = filename;

	EntryDict::iterator it = m_pakedFileDict.find(fn);
	if (it != m_pakedFileDict.end()) {
		PakedFile *packedfile = it->second;

		file = new File;
		file->m_type = File::kPacked;
		file->m_unzFile = unzReOpen(u2l(packedfile->packfile->fullpath).c_str(), packedfile->packfile->unzfile);
		file->m_name = filename;
		file->m_mode = File::ReadMode;

		unz_file_info file_info;
		unzSetOffset(file->m_unzFile, packedfile->offset);
		if (unzGetCurrentFileInfo(file->m_unzFile, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
			return file;

		file->m_size =  file_info.uncompressed_size;

		unzOpenCurrentFile(file->m_unzFile);
		attachFileObject(file);

		return file;
	}
	return file;
}

File *FileSystem::openFileWrite(const String &filename)
{
	SCOPE_LOCK;

	String path = filename;
	if (filename[1] != L':') {
		path = fullPath(m_modPath, filename);
	}

	PathUtil::createDir(path);

	FILE *fp = NULL;
	gzFile gzf = NULL;

	{
		fp = fopen(u2l(path).c_str(), "wb");
		if (!fp) {
			return nullptr;
		}
	}

	File *cf(new File);

	cf->m_type = File::kStdio;
	cf->m_handle = fp;
	cf->m_forceFlush = false;
	cf->m_name = filename;
	cf->m_mode = File::WriteMode;

	attachFileObject(cf);

	return cf;
}

File *FileSystem::openFileAppend(const String &filename)
{
	SCOPE_LOCK;

	String path = fullPath(m_modPath, filename);

	PathUtil::createDir(path);

	FILE *fp = fopen(u2l(path).c_str(), "ab+");

	if (!fp) {
		return nullptr;
	}

	File *cf(new File);

	cf->m_type = File::kStdio;
	cf->m_handle = fp;
	cf->m_forceFlush = false;
	cf->m_name = filename;
	cf->m_mode = File::WriteMode;

	attachFileObject(cf);

	return cf;
}

size_t FileSystem::readFile(const String &filename, void **buffer)
{
	SCOPE_LOCK;

	if (buffer)
		*buffer = 0;	// clear it first

	if (filename.empty()) {
		return 0;
	}

	File *fp = openFileRead(filename);
	std::auto_ptr<File> autofp(fp);

	if (!fp) {
		return 0;
	}

	size_t len = fp->size();

	if (!len)
		return 0;

	if (!buffer) {
		fp->close();
		return len;
	}

	*buffer = (void*)new sbyte_t[len+1];

	fp->read(*buffer, len);

	((char*)(*buffer))[len] = 0;

	fp->close();

	return len;
}

void FileSystem::freeFile(void *buffer)
{
	SCOPE_LOCK;

	SafeDeleteArray(buffer);
}

void FileSystem::writeFile(const String &filename, const void *buffer, size_t size)
{
	SCOPE_LOCK;

	if (filename.empty()) {
		return;
	}

	File *fp = openFileWrite(filename);
	if (!fp) {
		return;
	}

	fp->write(buffer, size);

	fp->close();
}


int FileSystem::compareFileModifyTime(const String &srcfile, const String &destfile)
{
	SCOPE_LOCK;

	longlong_t srcTime, dstTime;

	if (!getFileModifyTime(srcfile, &srcTime)) {
		return -2;
	}

	if (!getFileModifyTime(destfile, &dstTime)) {
		return 2;
	}

	return srcTime < dstTime ? -1 : srcTime > dstTime ? 1 : 0;
}


void FileSystem::attachFileObject(File *cf)
{
	SCOPE_LOCK;

	m_fileList.push_front(cf);
}

void FileSystem::detachFileObject(File *cf)
{
	SCOPE_LOCK;

	FileList::iterator it = m_fileList.begin();
	for (; it!=m_fileList.end(); ++it) {
		if (*it == cf) {
			m_fileList.erase(it);
			break;
		}
	}
}

FilePackage *FileSystem::loadPakFile(const String &fullpath, const String &filename)
{
	unzFile unzfile;
	unz_global_info global_info;
	unz_file_info file_info;
	char entryName[260];
	FilePackage *packfile;

	uint_t i;
	size_t len;

	unzfile = unzOpen(u2l(fullpath).c_str());
	if (unzGetGlobalInfo(unzfile, &global_info) != UNZ_OK) {
		return NULL;
	}

	m_numFilesInPack += global_info.number_entry;

	packfile = new FilePackage;

	packfile->fullpath = fullpath;
	packfile->filename = filename;

	len = packfile->filename.length();
	if (len > 4 && !StringUtil::stricmp(packfile->filename.c_str() + len - 4, ".pak")) {
		packfile->filename.resize(len - 4);
	}

	packfile->unzfile = unzfile;
	packfile->numEntries = global_info.number_entry;
	unzGoToFirstFile(unzfile);

	for (i=0; i<global_info.number_entry; i++, unzGoToNextFile(unzfile)) {
		if (unzGetCurrentFileInfo(unzfile, &file_info, entryName, sizeof(entryName), NULL, 0, NULL, 0) != UNZ_OK)
			break;

		String packedname = entryName;

		if (packedname[packedname.size()-1] == AX_PATH_SEP) {
			m_pakedFolders.addPath(packedname);
		} else {
			PakedFile *packed = new PakedFile;
			packed->packfile = packfile;
			packed->offset = unzGetOffset(unzfile);

			m_pakedFileDict[packedname] = packed;
			m_pakedFolders.addFile(packedname, packed);
		}
	}

	return packfile;
}

bool FileSystem::getFileModifyTime(const String &filename, longlong_t *time) const
{
	SCOPE_LOCK;

	if (filename.empty()) {
		return false;
	}

	SearchDirs::const_iterator sp;
	for (sp = m_searchDirs.begin(); sp != m_searchDirs.end(); ++sp) {
		String path = fullPath(sp->dir, filename);
		if (PathUtil::getFileModifiedTime(path.c_str(), time))
			return true;
	}

	/// not found, try packed file
	String fn = filename;

	EntryDict::const_iterator it = m_pakedFileDict.find(fn);
	if (it != m_pakedFileDict.end()) {
		PakedFile *packedfile = it->second;

		unz_file_info file_info;
		unzSetOffset(packedfile->packfile->unzfile, packedfile->offset);
		if (unzGetCurrentFileInfo(packedfile->packfile->unzfile, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
			return false;

		if (time)
			*time = file_info.dosDate;

		return true;
	}
	return false;
}

bool FileSystem::isFileExist(const String &filename) const
{
	return getFileModifyTime(filename, nullptr);
}

size_t FileSystem::readTextFile(const String &filename, void** buffer)
{
	return 0;
}

String FileSystem::getDefaultDataPath()
{
	String result = OsUtil::getworkpath();

	size_t i;
	for (i=result.length(); i>0; i--) {
		if (PathUtil::isDirectoryLetter(result[i-1])) {
			result.resize(i);
			break;
		}
	}

	result += DEFAULT_DATA_PATH;

	return result;
}

void FileSystem::addModPath()
{
	// init to data path first
	m_modPath = m_dataPath;

	String modfilename = m_gamePath + "/modpath.txt";
	FILE *f = fopen(u2l(modfilename).c_str(), "rb");
	if (!f) {
		return;
	}

	char linebuf[256];

	fgets(linebuf, s2i(ArraySize(linebuf)), f);
	fclose(f);

	if (!linebuf[0]) {
		return;
	}

	m_modPath = m_gamePath + "/" + linebuf;

	addGameDirectory(m_modPath);
}

void FileSystem::addSearchPath()
{
	String modfilename = m_gamePath + "/searchpath.txt";
	FILE *f = fopen(u2l(modfilename).c_str(), "rb");
	if (!f) {
		return;
	}

	char linebuf[256];
	while (fgets(linebuf, s2i(ArraySize(linebuf)), f)) {
		if (!linebuf[0]) {
			return;
		}

		if (linebuf[0] == '/')
			continue;

		char dir[256];
		int isExtractSrc = 0;

		int v = sscanf(linebuf, "%s %d", dir, &isExtractSrc);
		if (v < 2)
			isExtractSrc = 0;

		String path = m_gamePath + "/" + dir;

		// remove \r\d
		size_t s = path.size();
		while (s > 0) {
			if (path[s-1] >= 32) {
				break;
			}
			s--;
		}
		path.resize(s);

		addGameDirectory(path, isExtractSrc != 0);
	}

	fclose(f);
}

String FileSystem::modPathToOsPath(const String &modpath)
{
	String fullpath = m_modPath;
	if (modpath.length()) {
		if (!PathUtil::isDirectoryLetter(modpath[0])) {
			fullpath += "/";
		}
		fullpath += modpath;
	}
	return fullpath;
}

void FileSystem::checkGamePath()
{
	String modfilename = m_gamePath + "/gamepath.txt";
	FILE *f = fopen(u2l(modfilename).c_str(), "rb");
	if (!f) {
		return;
	}
	char linebuf[256];
	if (fgets(linebuf, s2i(ArraySize(linebuf)), f)) {
		if (!linebuf[0]) {
			return;
		}

		String path = m_gamePath + "/" + linebuf;

		// remove \r\d
		size_t s = path.size();
		while (s > 0) {
			if (path[s-1] >= 32) {
				break;
			}
			s--;
		}
		path.resize(s);

		m_gamePath = PathUtil::cleanPath(path);
	}

	fclose(f);
}

void FileSystem::queAsioRead( AsioRead *entry )
{

}

void FileSystem::flushAsio()
{

}
AX_END_NAMESPACE

