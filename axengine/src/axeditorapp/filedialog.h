/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITORAPP_FILEDIALOG_H
#define AX_EDITORAPP_FILEDIALOG_H

#include "private.h"
#include "ui_filedialog.h"

//------------------------------------------------------------------------------
// Class FileItem
//------------------------------------------------------------------------------

class FileItem : public QTreeWidgetItem {
public:
	enum {
		TypeDir = QTreeWidgetItem::UserType,
		TypeFile = TypeDir + 1
	};

	friend class FileDialog;

	FileItem(QTreeWidget *parent, const FileInfo &info, bool showext = true);
	FileItem(QTreeWidgetItem *parent, const FileInfo &info, bool showext = true);
	~FileItem();

	bool isDirectory() { return m_fileInfo.isDir; }
	const FileInfo &getFileInfo() { return m_fileInfo; }

	virtual bool operator<(const QTreeWidgetItem &other) const;

private:
	const FileInfo m_fileInfo;
};

//------------------------------------------------------------------------------
// Class FileDialog
//------------------------------------------------------------------------------

class FileDialog : public QDialog
{
    Q_OBJECT

public:
    FileDialog(QWidget *parent = 0);
    ~FileDialog();

	enum ViewMode { Detail, List };
	enum FileMode { AnyFile, ExistingFile, DirectoryAndFile, DirectoryOnly };
	enum AcceptMode { AcceptOpen, AcceptSave };
	enum DialogLabel { LookIn, FileName, FileType, Accept, Reject };

	void setRootDir(const QString &dir);
	void setViewMode(ViewMode mode);
	void setFileMode(FileMode mode);
	void setAcceptMode(AcceptMode mode);
	void setDefaultSuffix(const QString &suffix);
	void setFilter(const QString &filter);
	void setFilters(const QStringList &filters);
	QString getFileName() const;

	// public static function
	static QString getDirectory(QWidget *parent = 0, const QString &caption = QString(), const QString &dir = QString());
	static QString getOpenFileName(QWidget *parent = 0, const QString &caption = QString(), const QString &dir = QString(), const QString &filter = QString(), QString *selectedFilter = 0);
	static QString getSaveFileName(QWidget *parent = 0, const QString &caption = QString(), const QString &dir = QString(), const QString &filter = QString(), QString *selectedFilter = 0);

protected:
	void browseDir(const QString &dir, bool addHistory);
	void updateWidget();

private:
    Ui::FileDialogClass ui;
	QString m_title;
	QString m_directory;
	QStringList m_filters;
	QString m_defaultSuffix;
	QString m_rootDir;
	QString m_curDir;
	QStringList m_browsHistory;

	QString m_curFileName;

	ViewMode m_viewMode;
	FileMode m_fileMode;
	AcceptMode m_acceptMode;

	FileInfoSeq m_fileInfos;
	FileInfoSeq m_rootInfos;

	QIcon m_dirIcon;
	QIcon m_fileIcon;

	QWidget *m_previewWidget;

private slots:
	void on_filenameEdit_textChanged(const QString &);
	void on_okButton_clicked();
	void on_filterComboBox_activated(int);
	void SortByColumn(int column);

	void on_dirComboBox_activated(int);
	void on_treeWidget_itemClicked(QTreeWidgetItem*,int);
	void on_actionParentDir_triggered();
	void on_actionBack_triggered();
	void on_rootDirTree_itemDoubleClicked(QTreeWidgetItem*,int);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem*,int);
};

#endif // AX_EDITORAPP_FILEDIALOG_H
