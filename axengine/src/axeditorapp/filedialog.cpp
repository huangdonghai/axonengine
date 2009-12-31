/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "application.h"
#include "filedialog.h"
#include "filepreview.h"

//------------------------------------------------------------------------------
// Class FileItem
//------------------------------------------------------------------------------

FileItem::FileItem(QTreeWidget *parent, const FileInfo &info, bool showext)
	: QTreeWidgetItem(parent, info.isDir ? TypeDir : TypeFile)
	, m_fileInfo(info)
{
	QString tmp;

	if (!m_fileInfo.isDir && !showext) {
		setText(0, u2q(PathUtil::removeExt(m_fileInfo.filename)));
	} else {
		setText(0, u2q(m_fileInfo.filename));
	}
	const tm &t = m_fileInfo.localtime;
	setText(2, tmp.sprintf("%d-%d-%d %d:%d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min));
	
	if (m_fileInfo.isDir) {
		setIcon(0, QIcon(":/images/dirclosed-16.png"));
	} else {
		setIcon(0, QIcon(":/fileicons/xfile_16.png"));
		setText(1, tmp.sprintf("%d", m_fileInfo.filesize));
	}

	setData(0, Qt::UserRole, (qulonglong)&m_fileInfo);
}

FileItem::FileItem(QTreeWidgetItem *parent, const FileInfo &info, bool showext)
	: QTreeWidgetItem(parent, info.isDir ? TypeDir : TypeFile)
	, m_fileInfo(info)
{
	QString tmp;

	if (!m_fileInfo.isDir && !showext) {
		setText(0, u2q(PathUtil::removeExt(m_fileInfo.filename)));
	} else {
		setText(0, u2q(m_fileInfo.filename));
	}

	const tm &t = m_fileInfo.localtime;
	setText(2, tmp.sprintf("%d-%d-%d %d:%d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min));

	if (m_fileInfo.isDir) {
		setIcon(0, QIcon(":/images/dirclosed-16.png"));
	} else {
		setIcon(0, QIcon(":/fileicons/xfile_16.png"));
		setText(1, tmp.sprintf("%d", m_fileInfo.filesize));
	}

	setData(0, Qt::UserRole, (qulonglong)&m_fileInfo);
}

FileItem::~FileItem() {}

bool FileItem::operator<(const QTreeWidgetItem &other) const {
	if (type() == other.type()) {
		bool ok;
		FileInfo *info = (FileInfo*)other.data(0, Qt::UserRole).toULongLong(&ok);

		if (!ok)
			return QTreeWidgetItem::operator <(other);

		QTreeWidget *tree = treeWidget();
		int column = tree->sortColumn();

		if (column == 0)
			return m_fileInfo.filename < info->filename;

		if (column == 1)
			return m_fileInfo.filesize < info->filesize;

		if (column == 2)
			return m_fileInfo.filetime < info->filetime;
	}

	return type() < other.type();
}

//------------------------------------------------------------------------------
// Class FileDialog
//------------------------------------------------------------------------------

FileDialog::FileDialog(QWidget *parent)
    : QDialog(parent)
	, m_viewMode(List)
	, m_fileMode(AnyFile)
	, m_acceptMode(AcceptOpen)
	, m_dirIcon(":/images/dirclosed-16.png")
	, m_fileIcon(":/fileicons/xfile_16.png")
	, m_previewWidget(NULL)
{
	ui.setupUi(this);

	ui.toolButton1->setDefaultAction(ui.actionBack);
	ui.toolButton2->setDefaultAction(ui.actionParentDir);
	ui.toolButton3->setDefaultAction(ui.actionNewDirectory);
	ui.toolButton4->setDefaultAction(ui.actionListView);
	ui.toolButton5->setDefaultAction(ui.actionDetailView);

	ui.treeWidget->setRootIsDecorated(false);
	ui.treeWidget->setIconSize(QSize(16, 16));
	ui.treeWidget->setSortingEnabled(true);
	ui.treeWidget->sortItems(0, Qt::AscendingOrder);
	ui.treeWidget->setColumnWidth(0, 100);

	ui.rootDirTree->setRootIsDecorated(false);
	ui.rootDirTree->setIconSize(QSize(16, 16));
	ui.actionBack->setDisabled(true);

	QActionGroup *ag = new QActionGroup(this);
	ag->addAction(ui.actionListView);
	ag->addAction(ui.actionDetailView);

	QObject::connect(ui.treeWidget->header(), SIGNAL(sectionClicked(int)), this, SLOT(SortByColumn(int)));

	setRootDir("/");

	updateWidget();
}

FileDialog::~FileDialog()
{
	//SafeDelete(mPreviewWidget);
}

void FileDialog::browseDir(const QString &dir, bool addHistory)
{
	ui.treeWidget->clear();

	if (addHistory) {
		m_browsHistory.push_back(m_curDir);
		if (m_browsHistory.size() > 0)
			ui.actionBack->setEnabled(true);
	}

	if (dir == m_rootDir) {
		ui.actionParentDir->setDisabled(true);
	} else {
		ui.actionParentDir->setEnabled(true);
	}

	m_curDir = dir;
	QString &filter = ui.filterComboBox->currentText();
	m_fileInfos = g_fileSystem->getFileInfos(q2u(dir), q2u(filter), File::List_sort);

	// add folder first
	size_t i;
	QString tmp;
	for (i = 0; i < m_fileInfos.size(); i++) {
		FileItem *item = new FileItem(ui.treeWidget, m_fileInfos[i]);
	}
	ui.treeWidget->resizeColumnToContents(0);
	ui.treeWidget->resizeColumnToContents(1);
	ui.treeWidget->resizeColumnToContents(2);

	// set dir combobox
	int level = 0;
	ui.dirComboBox->clear();
	ui.dirComboBox->addItem(m_dirIcon, m_rootDir);
	level++;

	for (int i = m_rootDir.length(); i < m_curDir.size(); i++) {
		if (m_curDir[i] != '/')
			continue;
		tmp = m_curDir.left(i + 1);
		ui.dirComboBox->addItem(m_dirIcon, tmp);
		level++;
	}

	ui.dirComboBox->setCurrentIndex(level - 1);
}

void FileDialog::setRootDir(const QString &dir)
{
	m_rootDir = dir;

	if (!PathUtil::isDirectoryLetter(m_rootDir[m_rootDir.length()-1].toAscii()))
		m_rootDir += '/';

	m_rootInfos = g_fileSystem->getFileInfos(q2u(m_rootDir), "", File::List_sort);

	// add folder first
	size_t i;
	QString tmp;

	ui.rootDirTree->clear();
	for (i = 0; i < m_rootInfos.size(); i++) {
		if (!m_rootInfos[i]. isDir)
			continue;

		FileItem *item = new FileItem(ui.rootDirTree, m_rootInfos[i]);
	}

	browseDir(m_rootDir, false);
}

void FileDialog::setViewMode(ViewMode mode)
{
	// NOT IMPLEMENTED
}

void FileDialog::setFileMode(FileMode mode)
{
	m_fileMode = mode;

	updateWidget();
}

void FileDialog::setAcceptMode(AcceptMode mode)
{
	m_acceptMode = mode;

	updateWidget();
}

void FileDialog::setDefaultSuffix(const QString &suffix)
{
	m_defaultSuffix = suffix;
}

void FileDialog::setFilter(const QString &filter)
{
	StringList strs = StringUtil::tokenize(q2u(filter).c_str(), L'|');

	m_filters.clear();
	for (StringList::iterator it=strs.begin(); it!=strs.end(); ++it) {
		m_filters.push_back(u2q(*it));
	}
	if (!m_filters.contains("*.*")) {
		m_filters.push_back("*.*");
	}

	updateWidget();
}

void FileDialog::setFilters(const QStringList &filters)
{
	m_filters = filters;

	updateWidget();
}

QString FileDialog::getFileName() const
{
	return m_curFileName;
}


void FileDialog::SortByColumn(int column)
{

}

void FileDialog::updateWidget()
{
	// filename widget
	if (m_fileMode == ExistingFile)
		ui.filenameEdit->setDisabled(true);
	else
		ui.filenameEdit->setEnabled(true);

	// ok button
	if (m_acceptMode == AcceptOpen) {
		ui.okButton->setText(tr("&Open"));
	} else {
		ui.okButton->setText(tr("&Save"));
	}

	// filter combobox widget
	QStringList::const_iterator it = m_filters.begin();
	ui.filterComboBox->clear();
	for (; it != m_filters.end(); ++it) {
		ui.filterComboBox->addItem(*it);
	}
}

void FileDialog::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column )
{
	FileItem *f_item = dynamic_cast<FileItem*>(item);

	if (!f_item)
		return;

	if (f_item->isDirectory()) {
		QString fullpath = m_curDir + item->text(0) + "/";

		browseDir(fullpath, true);
	} else {
		on_okButton_clicked();
	}
}

void FileDialog::on_rootDirTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	QString fullpath = m_rootDir + item->text(0) + "/";

	browseDir(fullpath, true);
}


void FileDialog::on_actionBack_triggered()
{
	if (m_browsHistory.size() <= 0) {
		return;
	}

	QString fullpath = m_browsHistory.last();
	m_browsHistory.pop_back();

	if (m_browsHistory.size() == 0)
		ui.actionBack->setDisabled(true);

	browseDir(fullpath, false);
}

void FileDialog::on_actionParentDir_triggered()
{
	QString par_path = m_curDir;

	// eat ending '/'
	size_t s = par_path.size();
	if (par_path[s - 1] == '/') {
		par_path.resize(s - 1);
		s--;
	}

	for (s--; s >= 0; s--) {
		if (par_path[s] == '/') {
			par_path.resize(s + 1);
			break;
		}
	}

	browseDir(par_path, true);
}

void FileDialog::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
	FileItem *f_item = dynamic_cast<FileItem*>(item);

	if (!f_item)
		return;

	if (f_item->isDirectory()) {
		if (m_fileMode < DirectoryAndFile)
			return;
	} else {
		if (m_fileMode == DirectoryOnly)
			return;
	}

	QString name = u2q(f_item->m_fileInfo.filename);
	QString curFullName = m_curDir + name;
	if (m_curFileName == curFullName)
		return;

	m_curFileName = curFullName;
	ui.filenameEdit->setText(name);

	if (ui.previewCheckBox->checkState() != Qt::Unchecked) {

		if (m_previewWidget != NULL){
			ui.previewFrame->layout()->removeWidget(m_previewWidget);
			delete m_previewWidget;
			m_previewWidget = NULL;
		}

		FilePreview preview;
		QWidget *widget = preview.CreateWidget(q2u(curFullName), ui.previewFrame);
		if (widget != NULL) {
			m_previewWidget = widget;
			ui.previewFrame->layout()->addWidget(widget);
		}
	}
}



void FileDialog::on_dirComboBox_activated(int)
{
	QString &txt = ui.dirComboBox->currentText();

	browseDir(txt, true);
}

void FileDialog::on_filterComboBox_activated(int)
{
	browseDir(m_curDir, false);
}

void FileDialog::on_okButton_clicked()
{
	if (ui.filenameEdit->text().isEmpty())
		return;

	emit accept();
}


QString FileDialog::getDirectory(QWidget *parent, const QString &caption, const QString &dir)
{
	return QString();
}

QString FileDialog::getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter)
{
	FileDialog dlg(parent);

	dlg.setWindowTitle(caption);
	dlg.setFilter(filter);
	dlg.setFileMode(ExistingFile);
	dlg.setAcceptMode(AcceptOpen);
	dlg.setRootDir(dir);
	dlg.setWindowModality(Qt::WindowModal);

	int result = dlg.exec();

	if (result == QDialog::Accepted) {
		return dlg.getFileName();
	} else {
		return QString();
	}
}

QString FileDialog::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter)
{
	FileDialog dlg(parent);

	dlg.setWindowTitle(caption);
	dlg.setFilter(filter);
	dlg.setFileMode(AnyFile);
	dlg.setAcceptMode(AcceptSave);
	dlg.setRootDir(dir);

	if (dlg.exec() == QDialog::Accepted) {
		return dlg.getFileName();
	} else {
		return QString();
	}
}



void FileDialog::on_filenameEdit_textChanged(const QString & text)
{
	m_curFileName = m_curDir + text;

	if (text.indexOf('.') == -1 && m_filters.count())
		m_curFileName += *m_filters.begin();
}