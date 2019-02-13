#include "externalitemreferencelistmodel.h"

#include "../editableitem.h"
#include "../editableitemmanager.h"

#include <QMetaProperty>

namespace Aline {

ExternalItemReferenceListModel::ExternalItemReferenceListModel(EditableItem *parent, QString watchPropertyName) :
	QAbstractListModel(parent),
	_parentItem(parent)
{

	_watchedPropertyName = new char[watchPropertyName.length() + 1];
	strcpy(_watchedPropertyName, watchPropertyName.toStdString().c_str());

	_watchedPropertyIndex = parent->metaObject()->indexOfProperty(_watchedPropertyName);

	if (_watchedPropertyIndex >= 0) {

		QMetaProperty p = parent->metaObject()->property(_watchedPropertyIndex);

		if (p.type() == QVariant::StringList && p.hasNotifySignal()) {

			QMetaMethod signal = p.notifySignal();
			int index_of_slot = metaObject()->indexOfSlot("reset()");
			QMetaMethod slot = metaObject()->method(index_of_slot);

			_connection = connect(_parentItem, signal, this, slot);

		} else {
			_watchedPropertyIndex = -1;
		}

	}
}

ExternalItemReferenceListModel::~ExternalItemReferenceListModel() {
	delete _watchedPropertyName;
}

int ExternalItemReferenceListModel::rowCount(const QModelIndex &parent) const {

	Q_UNUSED(parent)

	if (_watchedPropertyIndex < 0) {
		return 0;
	}

	return _parentItem->property(_watchedPropertyName).toStringList().length();

}

QVariant ExternalItemReferenceListModel::data(const QModelIndex &index, int role) const {

	if (_watchedPropertyIndex < 0) {
		return QVariant();
	}

	QStringList refs = _parentItem->property(_watchedPropertyName).toStringList();

	if (index.row() >= refs.length()) {
		return QVariant();
	}

	QString ref = refs.at(index.row());

	switch (role) {
	case Qt::DisplayRole:
	case ItemNameRole:
	{
		EditableItem* item = _parentItem->getManager()->loadItem(ref);

		if (item != nullptr) {

			return item->objectName();
		}
	}
	case ItemRefRole:
		return ref;
	case ItemRole:
		return QVariant(QMetaType::QObjectStar, _parentItem->getManager()->loadItem(ref) );
	default:
		break;
	}

	return QVariant();

}

QHash<int, QByteArray> ExternalItemReferenceListModel::roleNames() const {

	QHash<int, QByteArray> names = QAbstractListModel::roleNames();

	names.insert(ItemNameRole, "name");
	names.insert(ItemRefRole, "ref");
	names.insert(ItemRole, "item");

	return names;
}

void ExternalItemReferenceListModel::reset() {
	beginResetModel();
	endResetModel();
}

} // namespace Aline
