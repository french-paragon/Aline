#ifndef EDITABLEITEMLIST_H
#define EDITABLEITEMLIST_H

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

#include <QVector>
#include <QAbstractListModel>

#include "../aline_global.h"

namespace Aline {

class EditableItem;
class EditableItemListModel;

/*!
 * \brief The EditableItemList class represent a list of editable items
 *
 * This class can be automatically serialized by serialization functions (e.g. json),
 * it can also be connected to a list model for easy display of the items it manages.
 */
class ALINE_EXPORT EditableItemList
{
public:
    EditableItemList();
    EditableItemList(QVector<EditableItem*> const& items);
    EditableItemList(QList<EditableItem*> const& items);
    EditableItemList(EditableItemList const& other);
    ~EditableItemList();

    EditableItemList& operator=(QVector<EditableItem*> const& items);
    EditableItemList& operator=(QList<EditableItem*> const& items);
    EditableItemList& operator=(EditableItemList const& other);

    inline void reserve(int size) {
        _items.reserve(size);
    }
    inline int size() const {
        return _items.size();
    }
    void insertItem(EditableItem* item, int row);
    inline void pushBackItem(EditableItem* item) {
        insertItem(item, size());
    }
    void removeItem(int row);
    void clear();

    inline EditableItem* operator[](int i) const {
        return _items[i];
    }

    inline int indexOf(EditableItem* item, int from = 0) const {
        return _items.indexOf(item, from);
    }

    inline QVector<EditableItem*>::ConstIterator begin() const {
        return _items.begin();
    }

    inline QVector<EditableItem*>::ConstIterator end() const {
        return _items.end();
    }

    EditableItemListModel* getViewModel();

protected:

    EditableItemListModel* _model;
    QVector<EditableItem*> _items;
};

class ALINE_EXPORT EditableItemListModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum SpecialRoles {
        ItemRole = Qt::UserRole+1
    };

    EditableItemListModel(EditableItemList const& list,
                          QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    virtual Qt::ItemFlags flags(QModelIndex const& index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

protected:
    friend class EditableItemList;

    void connectItemToModel(EditableItem* item);

    void itemAboutToBeAdded(int row);
    void itemAdded(int row);
    void itemAboutToBeRemoved(int row);
    void itemRemoved(int row);
    void itemsAboutToBeReseted();
    void itemsReseted();

    void itemNameChanged(EditableItem* item);

    EditableItemList const& _list;
};

} // namespace

Q_DECLARE_METATYPE(Aline::EditableItemList)

#endif // EDITABLEITEMLIST_H
