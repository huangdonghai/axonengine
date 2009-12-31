/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "propertyeditor_model.h"

PropertyEditorModel::PropertyEditorModel(QObject *parent)
	: QAbstractItemModel(parent), m_initialInput(0)
{
}

PropertyEditorModel::~PropertyEditorModel()
{
}

void PropertyEditorModel::setInitialInput(IProperty *initialInput)
{
	Q_ASSERT(initialInput);

	m_initialInput = initialInput;
	reset();
}

QModelIndex PropertyEditorModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid())
		return createIndex(row, column, m_initialInput);

	return createIndex(row, column, childAt(privateData(parent), row));
}

QModelIndex PropertyEditorModel::parent(const QModelIndex &index) const
{
	if (!index.isValid() || privateData(index) == m_initialInput)
		return QModelIndex();

	Q_ASSERT(privateData(index));

	return indexOf(parentOf(privateData(index)));
}

int PropertyEditorModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return 1;

	if (const IProperty *p = privateData(parent)) {
		return (p->kind() == IProperty::kGroup) ? static_cast<const IPropertyGroup*>(p)->propertyCount() : 0;
	}

	return (m_initialInput->kind() == IProperty::kGroup) ? static_cast<const IPropertyGroup*>(m_initialInput)->propertyCount() : 0;
}

int PropertyEditorModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return 2;
}

bool PropertyEditorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (IProperty *property = privateData(index)) {
		if (role == Qt::EditRole) {
			property->setValue(value);
			refresh(property);

			IProperty *nonfake = property;
			while (nonfake != 0 && nonfake->isFake())
				nonfake = nonfake->parent();
			if (nonfake != 0 && nonfake->dirty()) {
				nonfake->setDirty(false);
				emit propertyChanged(nonfake);
			}
		}

		return true;
	}

	return false;
}

void PropertyEditorModel::refreshData(IProperty *property){
	if (property) {
		refresh(property);
		IProperty *nonfake = property;
		while (nonfake != 0 && nonfake->isFake())
			nonfake = nonfake->parent();
		if (nonfake != 0 && nonfake->dirty()) {
			nonfake->setDirty(false);
			emit propertyChanged(nonfake);
		}
	}
}

QVariant PropertyEditorModel::data(const QModelIndex &index, int role) const
{
	Q_UNUSED(role);

	if (!privateData(index))
		return QVariant();

	static const QString noname = tr("<noname>");
	const IProperty *o = privateData(index);
	switch (index.column()) {  // ### cleanup
		case 0:
			switch (role) {
			case Qt::EditRole:
			case Qt::DisplayRole:
			case Qt::ToolTipRole:
				return o->propertyName().isEmpty() ? noname : o->propertyName();
			default:
				break;
			}
			break;

		case 1: {
			switch (role) {
			case Qt::EditRole:
				return o->value();
			case Qt::ToolTipRole:
			case Qt::DisplayRole:
				return o->toString();
			case Qt::DecorationRole:
				return o->decoration();
			default:
				break;
			}
			break;
				}

		default:
			break;
	}

	return QVariant();
}

QString PropertyEditorModel::columnText(int col) const
{
	switch (col) {
		case 0: return tr("Property");
		case 1: return tr("Value");
		default: return QString();
	}
}

void PropertyEditorModel::refresh(IProperty *property)
{
	// find parent if it is a fake
	IProperty *parent = property;
	while (parent && parent->isFake())
		parent = parent->parent();

	const int parentRow = rowOf(parent);
	if (parentRow == -1)
		return;

	const QModelIndex parentIndex0 = createIndex(parentRow, 0, parent);
	const QModelIndex parentIndex1 = createIndex(parentRow, 1, parent);

	emit dataChanged(parentIndex0, parentIndex1);
	// refresh children
	if (parent->kind() == IProperty::kGroup) {
		IPropertyGroup *group =  static_cast<IPropertyGroup*>(parent);
		if (const int numRows = group->propertyCount()) {
			const  QModelIndex leftTopChild = parentIndex0.child(0, 0);
			const  QModelIndex rightBottomChild = parentIndex0.child(numRows - 1, 1);
			emit dataChanged(leftTopChild, rightBottomChild);
		}
	}
}

bool PropertyEditorModel::isEditable(const QModelIndex &index) const
{
	return index.column() == 1 && privateData(index)->hasEditor();
}

QModelIndex PropertyEditorModel::buddy(const QModelIndex &index) const
{
	if (index.column() == 0)
		return createIndex(index.row(), 1, index.internalPointer());

	return index;
}

QVariant PropertyEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal) {
		if (role != Qt::DisplayRole)
			return QVariant();

		return columnText(section);
	}

	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags PropertyEditorModel::flags(const QModelIndex &index) const
{
	Q_ASSERT(index.isValid());

	Qt::ItemFlags foo = QAbstractItemModel::flags(index);

	if (isEditable(index))
		foo |= Qt::ItemIsEditable;

	return foo;
}

int PropertyEditorModel::rowOf(IProperty *property) const
{
	Q_ASSERT(property);
	if (property == m_initialInput)
		return 0;

	IProperty *parent = property->parent();

	if (!parent || parent->kind() != IProperty::kGroup)
		return -1;

	return static_cast<const IPropertyGroup*>(parent)->indexOf(property);
}
#if 0
static bool setImage(const ResourceMimeData &image, PixmapProperty *property)
{
	QDesignerFormWindowInterface *form = property->core()->formWindowManager()->activeFormWindow();
	if (!form)
		return false;

	const QPixmap newPixmap = resourceMimeDataToPixmap(image, form);
	const QPixmap oldPixmap = qvariant_cast<QPixmap>(property->value());
	if (newPixmap.isNull() || newPixmap .serialNumber() == oldPixmap.serialNumber())
		return  false;

	property->setValue(QVariant(newPixmap));
	return true;
}

static bool setIcon(const ResourceMimeData &image, IconProperty *property)
{
	QDesignerFormWindowInterface *form = property->core()->formWindowManager()->activeFormWindow();
	if (!form)
		return false;

	const QIcon newIcon = resourceMimeDataToIcon(image, form);
	const QIcon oldIcon = qvariant_cast<QIcon>(property->value());
	if (newIcon.isNull() || newIcon .serialNumber() == oldIcon.serialNumber())
		return  false;

	property->setValue(QVariant(newIcon));
	return true;
}

bool QPropertyEditorModel::resourceImageDropped(const QModelIndex &index, const ResourceMimeData &resourceMimeData)
{
	if (!index.isValid() || resourceMimeData.type() != ResourceMimeData::Image)
		return false;    

	IProperty *property = static_cast<IProperty*>(index.internalPointer());
	if (!property || property->isFake())
		return false;

	bool changed = false;
	switch (property->value().type()) {
		case  QVariant::Icon:
			changed = setIcon(resourceMimeData, static_cast<IconProperty *>(property));
			break;
		case  QVariant::Pixmap:
			changed = setImage(resourceMimeData, static_cast<PixmapProperty *>(property));
			break;
		default:
			break;
	}
	if (changed) emit propertyChanged(property);
	return  changed;
}
#endif