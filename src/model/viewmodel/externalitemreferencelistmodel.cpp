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

		_watchedPropertyName = new char[watchPropertyName.length() + 1];
		strcpy(_watchedPropertyName, watchPropertyName.toStdString().c_str());

		_watchedPropertyIndex = item->metaObject()->indexOfProperty(_watchedPropertyName);

		if (_watchedPropertyIndex >= 0) {

			QMetaProperty p = _trackedItem->metaObject()->property(_watchedPropertyIndex);

			if (p.type() == QVariant::StringList && p.hasNotifySignal()) {

				QMetaMethod signal = p.notifySignal();
				int index_of_slot = metaObject()->indexOfSlot("reset()");
				QMetaMethod slot = metaObject()->method(index_of_slot);

				_connection = connect(_trackedItem, signal, this, slot);

			} else {
				_watchedPropertyIndex = -1;
			}

		}

	}
}

ExternalItemReferenceListProxyModel::~ExternalItemReferenceListProxyModel() {
	delete _watchedPropertyName;
}

void ExternalItemReferenceListProxyModel::reset() {
	invalidateFilter();
}

QStringList ExternalItemReferenceListProxyModel::list() const {

	if (_trackedItem == nullptr || _watchedPropertyIndex < 0) {
		return QStringList();
	}

	return _trackedItem->property(_watchedPropertyName).toStringList();
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
