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

#include "externalitemreferencelistmodel.h"

#include "../editableitem.h"
#include "../editableitemmanager.h"

#include <QMetaProperty>

#include <QDebug>

namespace Aline {

ExternalItemReferenceListProxyModel::ExternalItemReferenceListProxyModel(EditableItem *item, QString watchPropertyName, int refRole, QObject *parent) :
	QSortFilterProxyModel(parent),
	_trackedItem(item),
	_refRole(refRole)
{

	if (item == nullptr) {
		_trackedItem = nullptr;
	} else {

		_trackedItem = item;

		_watchedPropertyName = watchPropertyName.toStdString();

		_watchedPropertyIndex = item->metaObject()->indexOfProperty(_watchedPropertyName.c_str());

		if (_watchedPropertyIndex >= 0) {

			QMetaProperty p = _trackedItem->metaObject()->property(_watchedPropertyIndex);

			if (p.type() == QVariant::StringList && p.hasNotifySignal()) {

				QMetaMethod signal = p.notifySignal();
				int index_of_slot = ExternalItemReferenceListProxyModel::staticMetaObject.indexOfSlot("reset()");
				QMetaMethod slot = ExternalItemReferenceListProxyModel::staticMetaObject.method(index_of_slot);

				_connection = connect(_trackedItem, signal, this, slot);

			} else {
				_watchedPropertyIndex = -1;
			}

		}

	}
}

ExternalItemReferenceListProxyModel::~ExternalItemReferenceListProxyModel() {

}

void ExternalItemReferenceListProxyModel::reset() {
	invalidateFilter();
}

QStringList ExternalItemReferenceListProxyModel::list() const {

	if (_trackedItem == nullptr || _watchedPropertyIndex < 0) {
		return QStringList();
	}

	return _trackedItem->property(_watchedPropertyName.c_str()).toStringList();
}

bool ExternalItemReferenceListProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {

	if (source_parent == QModelIndex()) {

		QModelIndex idR = sourceModel()->index(source_row, 0);

		QString ref = idR.data(_refRole).toString();

		if (!list().contains(ref)) {
			return false;
		}
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

} // namespace Aline
