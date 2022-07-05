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

#include "editableitemfilteredconnectionproxymodel.h"

#include "../../model/editableitem.h"

#include <QMetaProperty>
#include <QMetaMethod>

#include <QDebug>

using namespace Aline;

EditableItemFilteredConnectionProxyModel::EditableItemFilteredConnectionProxyModel(EditableItem *item, QString propName, int refRole, QObject *parent) :
	QSortFilterProxyModel(parent),
	_refRole(refRole)
{

	if (item == nullptr) {
		_trackedItem = nullptr;
		setForbidenItemRefs({});
	} else {

		_trackedItem = item;

		_watchedPropertyName = propName.toStdString();

		_watchedPropertyIndex = item->metaObject()->indexOfProperty(_watchedPropertyName.c_str());

		if (_watchedPropertyIndex >= 0) {

			QMetaProperty p = item->metaObject()->property(_watchedPropertyIndex);

			if (p.type() == QVariant::StringList && p.hasNotifySignal()) {

				QMetaMethod signal = p.notifySignal();
				int index_of_slot = EditableItemFilteredConnectionProxyModel::staticMetaObject.indexOfSlot("checkForbidenItemRefs()");
				QMetaMethod slot = EditableItemFilteredConnectionProxyModel::staticMetaObject.method(index_of_slot);

				_connection = connect(_trackedItem, signal, this, slot);

				_forbidenItemRefs = item->property(_watchedPropertyName.c_str()).toStringList();

			} else {
				_watchedPropertyIndex = -1;
			}

		}

	}

}

EditableItemFilteredConnectionProxyModel::~EditableItemFilteredConnectionProxyModel() {

}

void EditableItemFilteredConnectionProxyModel::checkForbidenItemRefs() {
	setForbidenItemRefs(_trackedItem->property(_watchedPropertyName.c_str()).toStringList());
}

bool EditableItemFilteredConnectionProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {

	if (source_parent == QModelIndex()) {

		QModelIndex idR = sourceModel()->index(source_row, 0);

		if (_forbidenItemRefs.contains(idR.data(_refRole).toString())) {
			return false;
		}

	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);

}

void EditableItemFilteredConnectionProxyModel::setForbidenItemRefs(const QStringList &forbidenItemRefs)
{
	if (forbidenItemRefs != _forbidenItemRefs) {
		_forbidenItemRefs = forbidenItemRefs;
		invalidateFilter();
	}
}

int EditableItemFilteredConnectionProxyModel::refRole() const
{
	return _refRole;
}
