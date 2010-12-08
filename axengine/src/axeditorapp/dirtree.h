/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef DIRTREE_H
#define DIRTREE_H

#include <QTreeWidget>
#include "private.h"

class ParticleEditor;

class DirTree : public QTreeWidget
{
	Q_OBJECT

public:
	DirTree(QWidget *parent);
	~DirTree();

	QString getFilter() const { return m_filter; }
	void setFilter(QString val) { m_filter = val; }
	QString getRoot() const { return m_root; }
	void setRoot(QString val) { m_root = val; }
	bool isShowExt() const { return m_showExt; }
	void setShowExt(bool val) { m_showExt = val; }
	bool isHideFile() const { return m_hideFile; }
	void setHideFile(bool val) { m_hideFile = val; }

	void refresh();
	void browseSub_r(const std::string &path, QTreeWidgetItem *parentItem);
	void removeEmpty();
	void removeFile();

protected:

private:
	QString m_filter;
	QString m_root;
	bool m_showExt;
	bool m_hideFile;
};

#endif // DIRTREE_H
