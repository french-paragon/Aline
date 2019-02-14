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

		_watchedPropertyName = new char[propName.length() + 1];
		strcpy(_watchedPropertyName, propName.toStdString().c_str());

		_watchedPropertyIndex = item->metaObject()->indexOfProperty(_watchedPropertyName);

		if (_watchedPropertyIndex >= 0) {

			QMetaProperty p = item->metaObject()->property(_watchedPropertyIndex);

			if (p.type() == QVariant::StringList && p.hasNotifySignal()) {

				QMetaMethod signal = p.notifySignal();
				int index_of_slot = metaObject()->indexOfSlot("checkForbidenItemRefs()");
				QMetaMethod slot = metaObject()->method(index_of_slot);

				_connection = connect(_trackedItem, signal, this, slot);

			} else {
				_watchedPropertyIndex = -1;
			}

		}

	}

}

EditableItemFilteredConnectionProxyModel::~EditableItemFilteredConnectionProxyModel() {
	delete _watchedPropertyName;
}

void EditableItemFilteredConnectionProxyModel::checkForbidenItemRefs() {
	setForbidenItemRefs(_trackedItem->property(_watchedPropertyName).toStringList());
}

bool EditableItemFilteredConnectionProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const {

	if (_forbidenItemRefs.contains(source_parent.data(_refRole).toString())) {
		return false;
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);

}

void EditableItemFilteredConnectionProxyModel::setForbidenItemRefs(const QStringList &forbidenItemRefs)
{
	if (forbidenItemRefs != _forbidenItemRefs) {
		beginResetModel();
		_forbidenItemRefs = forbidenItemRefs;
		endResetModel();
	}
}

int EditableItemFilteredConnectionProxyModel::refRole() const
{
	return _refRole;
}
