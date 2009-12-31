/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "dirtree.h"
#include "filedialog.h"

DirTree::DirTree(QWidget *parent)
	: QTreeWidget(parent)
{
	m_showExt = true;
	m_hideFile = false;
}

DirTree::~DirTree()
{
}

void DirTree::refresh() {
	FileInfoSeq infos = g_fileSystem->getFileInfos(q2u(m_root), q2u(m_filter), File::List_sort);

	clear();
	for (size_t i = 0; i < infos.size(); i++) {
		FileItem *item = new FileItem(this, infos[i], m_showExt);

		if (infos[i].isDir) {
			browseSub_r(infos[i].fullpath, item);
		}
	}

	removeEmpty();
	if (m_hideFile) {
		removeFile();
	}
}

void DirTree::browseSub_r(const String &path, QTreeWidgetItem *parentItem) {
	FileInfoSeq infos = g_fileSystem->getFileInfos(path, q2u(m_filter), File::List_sort);
	for (size_t i = 0; i < infos.size(); i++) {
		FileItem *child = new FileItem(parentItem, infos[i], m_showExt);

		if (infos[i].isDir) {
			browseSub_r(infos[i].fullpath, child);
		}
	}
}

static inline bool RemoveEmpty_r(FileItem *item) {
	bool notempty = false;
	for (int i = item->childCount() - 1; i >= 0; i--) {
		FileItem *child = (FileItem*)item->child(i);
		if (child->isDirectory()) {
			bool foundfile = RemoveEmpty_r(child);
			if (!foundfile) {
				item->removeChild(child);
				delete child;
			} else
				notempty = true;
		} else {
			notempty = true;
		}
	}

	return notempty;
}

void DirTree::removeEmpty() {
	for (int i = topLevelItemCount()-1; i >= 0 ; i--) {
		FileItem *item = (FileItem*)topLevelItem(i);

		if (!item->isDirectory())
			continue;

		bool notempty = RemoveEmpty_r(item);
		if (!notempty) {
			FileItem *taken = (FileItem*)takeTopLevelItem(i);
			AX_ASSERT(taken == item);
			delete taken;
		}
	}
}

static inline void RemoveFile_r(FileItem *item) {
	for (int i = item->childCount() - 1; i >= 0; i--) {
		FileItem *child = (FileItem*)item->child(i);
		if (child->isDirectory()) {
			RemoveFile_r(child);
		} else {
			child->setHidden(true);
		}
	}
}

void DirTree::removeFile()
{
	for (int i = topLevelItemCount()-1; i >= 0 ; i--) {
		FileItem *item = (FileItem*)topLevelItem(i);

		if (!item->isDirectory())
			continue;

		RemoveFile_r(item);
	}
}
