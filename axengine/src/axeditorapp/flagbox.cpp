/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "flagbox.h"


FlagBoxModel::FlagBoxModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

FlagBoxModel::~FlagBoxModel()
{
}

void FlagBoxModel::setItems(const QList<FlagBoxModelItem> &items)
{
    m_items = items;
    emit reset();
}

int FlagBoxModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_items.count() : 0;
}

int FlagBoxModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex FlagBoxModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

QModelIndex FlagBoxModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column, 0);
}

QVariant FlagBoxModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.row() != -1);

    const FlagBoxModelItem &item = m_items.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item.name();

    case Qt::CheckStateRole:
        return item.isChecked() ? Qt::Checked : Qt::Unchecked;

    default:
        return QVariant();
    } // end switch
}

bool FlagBoxModel::setData(const QModelIndex &modelIndex, const QVariant &value, int role)
{
    Q_ASSERT(modelIndex.row() != -1);

    FlagBoxModelItem &item = m_items[modelIndex.row()];

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole: {
        item.setName(value.toString());
    } return true;

    case Qt::CheckStateRole: {
        const Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
        item.setChecked(state == Qt::Unchecked ? false : true);
        // There are special flags like QDialogButtonBox::NoButton[AtAll]
        // that affect others. Invalidate the whole model.
        emit dataChanged(index(0, 0, QModelIndex()), index(m_items.size() - 1, 0, QModelIndex()));
    } return true;

    default: break;
    } // end switch

    return false;
}

Qt::ItemFlags FlagBoxModel::flags(const QModelIndex &index) const
{
    Q_ASSERT(index.row() != -1);

    const FlagBoxModelItem &thisItem = m_items[index.row()];
    if (thisItem.value() == 0) {
        // Disabled if checked
        if (thisItem.isChecked())
            return 0;
    } else if (bitcount(thisItem.value()) > 1) {
        // Disabled if all flags contained in the mask are checked
        unsigned int currentMask = 0;
        for (int i = 0; i < m_items.size(); ++i) {
            const FlagBoxModelItem &item = m_items[i];
            if (bitcount(item.value()) == 1)
                currentMask |= item.isChecked() ? item.value() : 0;
        }
        if ((currentMask & thisItem.value()) == thisItem.value())
            return 0;
    }
    return QAbstractItemModel::flags(index);
}

// Helper function that counts the number of 1 bits in argument
int FlagBoxModel::bitcount(int mask)
{
    int count = 0;
    for (int i = 31; i >= 0; --i)
        count += ((mask >> i) & 1) ? 1 : 0;
    return count;
}


FlagBox::FlagBox(QWidget *parent)
    : QComboBox(parent)
{
    m_model = new FlagBoxModel(this);
    setModel(m_model);

    QStyleOptionComboBox opt;
    opt.initFrom(this);
    opt.editable = isEditable();
    if (style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, this))
        setItemDelegate(new QItemDelegate(this));

    connect(this, SIGNAL(activated(int)), this, SLOT(slotActivated(int)));
}

FlagBox::~FlagBox()
{
}

void FlagBox::slotActivated(int index)
{
    QVariant value = itemData(index, Qt::CheckStateRole);
    Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
    setItemData(index, (state == Qt::Unchecked ? Qt::Checked : Qt::Unchecked), Qt::CheckStateRole);
}

void FlagBox::hidePopup()
{
	QComboBox::hidePopup();
}
