/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef FLAGBOX_H
#define FLAGBOX_H


class FlagBoxModelItem
{
public:
	FlagBoxModelItem(const QString &name, unsigned int value, bool checked = false)
		: m_name(name), m_value(value), m_checked(checked) {}

	inline unsigned int value() const { return m_value; }

	inline QString name() const { return m_name; }
	inline void setName(const QString &name) { m_name = name; }

	inline bool isChecked() const { return m_checked; }
	inline void setChecked(bool checked) { m_checked = checked; }

private:
	QString m_name;
	unsigned int m_value;
	bool m_checked;
};

class FlagBoxModel: public QAbstractItemModel
{
	Q_OBJECT
public:
	FlagBoxModel(QObject *parent = 0);
	virtual ~FlagBoxModel();

	inline FlagBoxModelItem itemAt(int index) const
	{ return m_items.at(index); }

	inline FlagBoxModelItem &item(int index)
	{ return m_items[index]; }

	inline QList<FlagBoxModelItem> items() const { return m_items; }
	void setItems(const QList<FlagBoxModelItem> &items);

	virtual int rowCount(const QModelIndex &parent) const;
	virtual int columnCount(const QModelIndex &parent) const;
	virtual bool hasChildren(const QModelIndex &parent) const
	{ return rowCount(parent) > 0; }

	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	static int bitcount(int mask);

private:
	QList<FlagBoxModelItem> m_items;
};

class FlagBox: public QComboBox
{
	Q_OBJECT
public:
	FlagBox(QWidget *parent = 0);
	virtual ~FlagBox();

	inline FlagBoxModelItem itemAt(int index) const
	{ return m_model->itemAt(index); }

	inline FlagBoxModelItem &item(int index)
	{ return m_model->item(index); }

	inline QList<FlagBoxModelItem> items() const
	{ return m_model->items(); }

	inline void setItems(const QList<FlagBoxModelItem> &items)
	{ m_model->setItems(items); }

	void hidePopup();

private slots:
	void slotActivated(int index);

private:
	FlagBoxModel *m_model;
};


#endif // end guardian
