#ifndef EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H
#define EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H

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

namespace Aline {

class EditableItem;

/*!
 * \brief The EditableItemFilteredConnectionProxyModel class provide a model filter capable of listing only a set of option available based on the options already set for a given editable item
 * (e.g. ask only for the mobile and home phone number if a contact already have a workplace number).
 */
class EditableItemFilteredConnectionProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:

	Q_PROPERTY(int refRole READ refRole CONSTANT)

	explicit EditableItemFilteredConnectionProxyModel(EditableItem* item, QString propName, int refRole, QObject* parent = nullptr);
	~EditableItemFilteredConnectionProxyModel();

	int refRole() const;

Q_SIGNALS:

public Q_SLOTS:

	void checkForbidenItemRefs();

protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

	void setForbidenItemRefs(const QStringList &forbidenItemRefs);

	EditableItem* _trackedItem;

	std::string _watchedPropertyName;
	int _watchedPropertyIndex;

	QStringList _forbidenItemRefs;

	QMetaObject::Connection _connection;

	int _refRole;
};

}

#endif // EDITABLEITEMFILTEREDCONNECTIONPROXYMODEL_H
