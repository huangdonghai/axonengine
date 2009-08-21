/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef PROPERTYEDITOR_MODEL_H
#define PROPERTYEDITOR_MODEL_H

#include <QAbstractItemModel>
#include "propertyeditor_items.h"

class PropertyEditorModel: public QAbstractItemModel
{
	Q_OBJECT
public:
	PropertyEditorModel(QObject *parent = 0);
	~PropertyEditorModel();

	inline IProperty *initialInput() const
	{ return m_initialInput; }

	inline QModelIndex indexOf(IProperty *property, int column = 0) const
	{
		const int row = rowOf(property);
		return row == -1 ?  QModelIndex() : createIndex(row, column, property);
	}

	inline IProperty *privateData(const QModelIndex &index) const
	{ return static_cast<IProperty*>(index.internalPointer()); }

	Qt::ItemFlags flags(const QModelIndex &index) const;

//	bool resourceImageDropped(const QModelIndex &index, const ResourceMimeData &m);

signals:
	void propertyChanged(IProperty *property);
	void resetProperty(const QString &name);

public slots:
	void setInitialInput(IProperty *initialInput);
	void refresh(IProperty *property);

public:
	//
	// QAbstractItemModel interface
	//
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;

	virtual QModelIndex parent(const QModelIndex &index) const;

	virtual int rowCount(const QModelIndex &parent) const;
	virtual int columnCount(const QModelIndex &parent) const;
	virtual bool hasChildren(const QModelIndex &parent) const
	{ return rowCount(parent) > 0; }

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
	void         refreshData(IProperty* prop);
	virtual bool isEditable(const QModelIndex &index) const;
	virtual QModelIndex buddy(const QModelIndex &index) const;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
	QString columnText(int column) const;

private:
	inline IProperty *childAt(IProperty *parent, int pos) const
	{
		if (parent && parent->kind() == IProperty::kGroup)
			return static_cast<IPropertyGroup*>(parent)->propertyAt(pos);

		return 0;
	}

	inline IProperty *parentOf(IProperty *property) const
	{ return property ? property->parent() : 0; }

private:
	int rowOf(IProperty *property) const;

	IProperty *m_initialInput;
};

#endif // PROPERTYEDITOR_MODEL_H
