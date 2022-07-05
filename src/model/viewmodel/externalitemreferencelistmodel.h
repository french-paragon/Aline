#ifndef ALINE_EXTERNALITEMREFERENCELISTMODEL_H
#define ALINE_EXTERNALITEMREFERENCELISTMODEL_H

/*This file is part of the project Aline

Copyright (C) 2022 Paragon <french.paragon@gmail.com>

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

#include <QSortFilterProxyModel>
#include <QStringList>

namespace Aline {

class EditableItem;

/*!
 * \brief The ExternalItemReferenceListModel class Help to list the item refered by an editableitem
 * Listing references is a standard behaviour for editable items.
 * This model, given an editableitem with a property that represent a list of ref to other item (as a QStringList) allow to display it in widgets that uses Qt's model/view paradigm. Just provide the item and the name of the property.
 * Pay attention ! A change in the string list reset the model, so it's recommanded to use this solution only for items with small list of reference.
 */
class ExternalItemReferenceListProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:

	explicit ExternalItemReferenceListProxyModel(EditableItem *item, QString watchPropertyName, int refRole, QObject* parent = nullptr);
	virtual ~ExternalItemReferenceListProxyModel();

	int refRole() const;


Q_SIGNALS:

public Q_SLOTS:

	void reset();

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

	QStringList list() const;

	EditableItem * _trackedItem;

	std::string _watchedPropertyName;
	int _watchedPropertyIndex;

	QMetaObject::Connection _connection;

	int _refRole;
};

} // namespace Aline

#endif // ALINE_EXTERNALITEMREFERENCELISTMODEL_H
