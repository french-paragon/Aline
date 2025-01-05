/*This file is part of the project Aline

Copyright (C) 2025 Paragon <french.paragon@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "editableitemlist.h"

#include "editableitem.h"

#include <QIcon>

namespace Aline {

EditableItemList::EditableItemList() :
    _model(nullptr)
{

}
EditableItemList::EditableItemList(QVector<EditableItem*> const& items) :
    _items(items),
    _model(nullptr)
{

}
EditableItemList::EditableItemList(QList<EditableItem*> const& items) :
    _items(items.toVector()),
    _model(nullptr)
{

}
EditableItemList::EditableItemList(EditableItemList const& other) :
    _items(other._items),
    _model(nullptr)
{

}

EditableItemList::~EditableItemList() {
    if (_model != nullptr) {
        delete _model;
    }
}

EditableItemList& EditableItemList::operator=(QVector<EditableItem*> const& items) {
    if (_model != nullptr) {
        _model->itemsAboutToBeReseted();
    }

    _items = items;

    if (_model != nullptr) {
        _model->itemsReseted();
    }

    return *this;
}
EditableItemList& EditableItemList::operator=(QList<EditableItem*> const& items) {
    return operator=(items.toVector());
}
EditableItemList& EditableItemList::operator=(EditableItemList const& other) {
    return operator=(other._items);
}

void EditableItemList::insertItem(EditableItem* item, int row) {
    if (_items.contains(item)) {
        return;
    }

    if (_model != nullptr) {
        _model->itemAboutToBeAdded(row);
    }

    _items.insert(row, item);

    if (_model != nullptr) {
        _model->itemAdded(row);
    }

}
void EditableItemList::removeItem(int row) {

    if (_model != nullptr) {
        _model->itemAboutToBeRemoved(row);
    }

    _items.removeAt(row);

    if (_model != nullptr) {
        _model->itemRemoved(row);
    }
}
void EditableItemList::clear() {

    if (_model != nullptr) {
        _model->itemsAboutToBeReseted();
    }

    _items.clear();

    if (_model != nullptr) {
        _model->itemsReseted();
    }
}

EditableItemListModel* EditableItemList::getViewModel() {
    if (_model == nullptr) {
        _model = new EditableItemListModel(*this);
    }
    return _model;
}

EditableItemListModel::EditableItemListModel(EditableItemList const& list,
                      QObject* parent) :
    _list(list)
{

    for (int i = 0; i < _list.size(); i++) {
        connectItemToModel(_list[i]);
    }

}

int EditableItemListModel::rowCount(const QModelIndex &parent) const {
    return _list.size();
}
QVariant EditableItemListModel::data(const QModelIndex & index, int role) const {

    if (index.row() < 0 or index.row() >= _list.size()) {
        return QVariant();
    }

    EditableItem* item = _list[index.row()];

    if (item == nullptr) {
        return QVariant();
    }

    QString name = item->objectName();

    switch (role) {
    case Qt::DisplayRole :
        if (name.isEmpty()) {
            return tr("Unnamed");
        }
    case Qt::EditRole :
        return name;
    case Qt::TextColorRole:
        if (name.isEmpty()) {
            return QColor(200,200,200);
        }
        break;
    case Qt::DecorationRole :
        if (!item->iconInternalUrl().isEmpty()) {
            return QIcon(item->iconInternalUrl());
        }
        break;
    case ItemRole :
        return QVariant::fromValue(item);
    default:
        break;
    }

    return QVariant();

}

Qt::ItemFlags EditableItemListModel::flags(QModelIndex const& index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}
bool EditableItemListModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    if (index.row() < 0 or index.row() >= _list.size()) {
        return false;
    }

    EditableItem* item = _list[index.row()];

    if (item == nullptr) {
        return false;
    }

    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        item->setObjectName(value.toString());
        return true;
    default:
        break;
    }

    return false;
}



void EditableItemListModel::connectItemToModel(EditableItem* item) {
    connect(item, &QObject::objectNameChanged, this, [this, item] () {
        itemNameChanged(item);
    });
}

void EditableItemListModel::itemAboutToBeAdded(int row) {
    beginInsertRows(QModelIndex(), row, row);
}
void EditableItemListModel::itemAdded(int row) {
    connectItemToModel(_list[row]);
    endInsertRows();
}
void EditableItemListModel::itemAboutToBeRemoved(int row) {
    beginRemoveRows(QModelIndex(), row, row);
    disconnect(_list[row],nullptr, this, nullptr);
}
void EditableItemListModel::itemRemoved(int row) {
    Q_UNUSED(row)
    endRemoveRows();
}
void EditableItemListModel::itemsAboutToBeReseted() {
    beginResetModel();
}
void EditableItemListModel::itemsReseted() {
    endResetModel();
}

void EditableItemListModel::itemNameChanged(EditableItem* item) {
    int row = _list.indexOf(item);

    if (row < 0 or row >= _list.size()) { //in case the item is no longer in the list
        disconnect(item, nullptr, this, nullptr);
    }

    Q_EMIT dataChanged(index(row), index(row), {Qt::DisplayRole});
}

} // namespace Aline
