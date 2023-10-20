/*This file is part of the project Aline

Copyright (C) 2022-2023 Paragon <french.paragon@gmail.com>

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

#include "editableitemmanager.h"

#include "editableitem.h"
#include "editableitemfactory.h"

#include "view/editorfactorymanager.h"

#include "model/labels/labelstree.h"
#include "../utils/view_model/indexrebasedproxymodel.h"

#include "control/app.h"

#include <QSet>
#include <QIcon>
#include <QMimeData>

namespace Aline {

const QChar EditableItemManager::RefSeparator = QChar('/');
const QString EditableItemManager::RefRoot = QString("root");

const QString EditableItemManager::RefMimeType = "text/editableitemref";

EditableItemManager::EditableItemManager(QObject *parent) :
	QAbstractItemModel(parent),
	_labels(nullptr),
	_activeItem(nullptr),
	_factoryManager(App::getAppEditableItemFactoryManager()),
	_editorManager(nullptr),
	_refUrlSeparator("/")
{
	cleanTreeStruct();
}

QModelIndex EditableItemManager::indexFromRef(const QString &ref) const {

	treeStruct* leaf = _treeIndex.value(ref, nullptr);

	if (leaf == nullptr) {
		return QModelIndex();
	}

	return indexFromLeaf(leaf);
}

QModelIndex EditableItemManager::index(int row, int column, const QModelIndex &parent) const {

	if (column > 0) {
		return QModelIndex();
	}

	if (parent == QModelIndex()) {

		if (row >= _itemsByTypes.size()) {
			return QModelIndex();
		}

		return createIndex(row, column);
	}

	void * dataPtr = parent.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data == nullptr) {

		QString typeRef = parent.data(ItemTypeRefRole).toString();

		if (row < _itemsByTypes.value(typeRef).size()) {
			return createIndex(row, column, _itemsByTypes.value(typeRef).at(row));
		}
	}

	return QModelIndex();

}

QModelIndex EditableItemManager::parent(const QModelIndex &index) const {

	void * dataPtr = index.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data != nullptr) {
		return indexFromType(data->_type_ref);
	}

	return QModelIndex();

}
int EditableItemManager::rowCount(const QModelIndex &parent) const {

	if (parent == QModelIndex()) {
		return _itemsByTypes.size();
	}

	void * dataPtr = parent.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data == nullptr) {
		QString itemType = parent.data(ItemTypeRefRole).toString();
		return _itemsByTypes.value(itemType).size();
	}

	return 0;

}
int EditableItemManager::columnCount(const QModelIndex &parent) const {
	(void) parent;
	return 1;
}

QVariant EditableItemManager::data(const QModelIndex &index, int role) const {

	if (index == QModelIndex()) {
		return QVariant();
	}

	void * dataPtr = index.internalPointer();
	treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

	if (data == nullptr) { //We have a type instead of an item.

		QString typeId = _itemsByTypes.keys().at(index.row());

		switch (role) {

		case Qt::DisplayRole:

			return _factoryManager->itemTypeName(typeId);

		case ItemTypeRefRole:

			return typeId;

		case Qt::DecorationRole:

		{
			QString iconPath = _factoryManager->itemIconUrl(typeId);

			if (iconPath != "") {
				return QIcon(iconPath);
			}
		}
			return QVariant();

		default:
			return QVariant();
		}
	}

	switch (role) {

	case Qt::DisplayRole:

		return data->_name;

	case Qt::EditRole:

		return data->_name;

	case ItemRefRole:

		return data->_ref;

	case ItemAcceptChildrenRole:

		return data->_acceptChildrens;

	case ItemTypeRefRole:

		return data->_type_ref;

	case Qt::DecorationRole: //icon

		{
			QString iconPath = _factoryManager->itemIconUrl(data->_type_ref);

			if (iconPath != "") {
				return QIcon(iconPath);
			}
		}
		break;
	default:
		break;
	}

	return QVariant();
}

bool EditableItemManager::setData(const QModelIndex &index, const QVariant &value, int role) {

	switch (role) {

	case Qt::EditRole:

	{
		QString ref = data(index, ItemRefRole).toString();

		EditableItem* item = loadItem(ref);

		void * dataPtr = index.internalPointer();
		treeStruct* data = reinterpret_cast <treeStruct*> (dataPtr);

		if (item != nullptr) {
			item->setObjectName(value.toString());
			data->_name = value.toString();
			Q_EMIT dataChanged(index, index, {Qt::DisplayRole});
			return true;
		}
	}

	default:
		return false;
	}

	return false;

}

Qt::ItemFlags EditableItemManager::flags(const QModelIndex &index) const {

	if (index.isValid()) {

		Qt::ItemFlags f = Qt::ItemIsDragEnabled | QAbstractItemModel::flags(index);

		if (index.parent() == QModelIndex()) { //type instead of items.
			return f;
		}

		f |= Qt::ItemIsEditable;

		return f;
	}

	return QAbstractItemModel::flags(index);

}

QVariant EditableItemManager::headerData(int section, Qt::Orientation orientation, int role) const {

	Q_UNUSED(section);
	Q_UNUSED(orientation);

	if (role == Qt::DisplayRole) {
		return "";
	}

	return QVariant();

}

QStringList EditableItemManager::mimeTypes() const {

	QStringList mimes;
	mimes << RefMimeType;

	return mimes;

}

QMimeData* EditableItemManager::mimeData(const QModelIndexList &indexes) const {

	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	for (QModelIndex index : indexes) {
		if (index.isValid()) {
			QString text = data(index, ItemRefRole).toString();
			stream << text;
		}
	}

	mimeData->setData(RefMimeType, encodedData);
	return mimeData;
}

EditableItem* EditableItemManager::loadItem(QString const& ref) {

	if (ref == "") {
		return nullptr;
	}

	if (isItemLoaded(ref)) {
		return _loadedItems.value(ref)._item;
	}

	EditableItem* item = effectivelyLoadItem(ref);

	item->_hasBeenLoadedFromDisk = true;

	treeStruct* node = _treeIndex.value(ref, nullptr);

	if (node == nullptr) {
		insertItem(item);
	} else {
		_loadedItems.insert(item->getRef(), {node, item});
	}

	item->onLoadingDone(); //call the handler for the loading done!

	return item;

}

QStringList EditableItemManager::loadedItems(const QString &type) const{
	if (type == "") {
		return _loadedItems.keys();
	}

	if (_itemsByTypes.contains(type)) {

		QVector<treeStruct*> const& ref = _itemsByTypes[type];
		QStringList list;
		list.reserve(ref.size());

		for (treeStruct* s : ref) {
			list.push_back(s->_ref);
		}

		return list;
	}

	return {};
}

QStringList EditableItemManager::loadedItemsTypes() const {

	return _itemsByTypes.keys();

}

void EditableItemManager::forceUnloadItem(QString ref) {

	if (!isItemLoaded(ref)) {
		return; //no need to unload a not present item.
	}

	Q_EMIT itemAboutToBeUnloaded(ref);

	EditableItem* item = _loadedItems.value(ref)._item;
	_loadedItems.remove(ref);
	delete item; //effectively delete item.

	Q_EMIT itemUnloaded(ref);

}

bool EditableItemManager::isItemLoaded(QString const& ref) const {
	return _loadedItems.contains(ref);
}

bool EditableItemManager::containItem(const QString & ref) const {
	return _treeIndex.contains(ref);
}

bool EditableItemManager::createItem(QString typeRef, QString pref) {
	return createItem(typeRef, pref, nullptr);
}

bool EditableItemManager::createItem(QString typeRef, QString pref, QString *createdItemRef) {

	QString ref = pref;

	if (!_factoryManager->hasFactoryInstalled(typeRef)) {
		return false;
	}

	if (containItem(ref)) {
		makeRefUniq(ref);
	}

	EditableItem* item = _factoryManager->createItem(typeRef, ref, this);

	if (item != nullptr) {

		if (insertItem(item) ) {

			if (createdItemRef != nullptr) {
				*createdItemRef = ref;
			}

			return true;
		} else {
			return false;
		}

	}

	return false;

}

bool EditableItemManager::clearItem(QString itemRef) {

	EditableItem* item = loadItem(itemRef);

	if (item != nullptr) {
		item->suppress();
		item->deleteLater();
	}

	treeStruct* node = _treeIndex.value(itemRef, nullptr);

	QModelIndex index = indexFromLeaf(node);

	if (node != nullptr) {
		beginRemoveRows(index.parent(), index.row(), index.row());

		QString typeR = node->_type_ref;
		_itemsByTypes[typeR].removeOne(node);

		endRemoveRows();
	}

	_treeIndex.remove(itemRef);
	_loadedItems.remove(itemRef);

	delete node;

	clearItemData(itemRef);

	return true;

}

bool EditableItemManager::clearItems(QStringList itemRefs) {

	bool status = true;
	for (QString const& ref : itemRefs) {
		status = status and clearItem(ref);
	}

	return status;
}

EditableItem* EditableItemManager::loadItemByUrl(QStringList const& url) {

	EditableItem* current = nullptr;

	for (QString const& ref : url) {
		if (current == nullptr) {
			current = loadItem(ref);
		} else {
			current = current->getSubItemByRef(ref);
		}

		if (current == nullptr) {
			break;
		}
	}

	return current;

}


EditableItem* EditableItemManager::loadItemByUrl(QString const& url) {

	QStringList url_list = url.split(refUrlSeparator());
	return loadItemByUrl(url_list);
}

bool EditableItemManager::saveItem(QString ref) {
	if (isItemLoaded(ref)) {
		bool status = effectivelySaveItem(ref);

		if (status) {
			EditableItem* item = loadItem(ref);
			item->clearHasUnsavedChanges();

			return true;
		}
	}
	return false;
}

bool EditableItemManager::saveAll() {

	bool status = true;

	QList<QString> keys = _loadedItems.keys();

	for (QString const& ref : qAsConst(keys)) {
		status = status and saveItem(ref);
	}

	status = status and saveLabels();
	status = status and saveStruct();

	return status;

}

EditableItemFactoryManager *EditableItemManager::factoryManager() const
{
	return _factoryManager;
}

bool EditableItemManager::makeRefUniq(QString &ref) const {

	if (!containItem(ref)) {
		return true;
	}

	int i = 1;

	while(containItem(QString("%1_%2").arg(ref).arg(i))) {
		i++;

		if (i <= 0) {
			return false; //impossible to make the ref uniq.
		}
	}

	ref = QString("%1_%2").arg(ref).arg(i);

	return true;

}

bool EditableItemManager::hasDistantFile(QString fileName) {
	Q_UNUSED(fileName);
	return false; //distant files are not supported by defaults.
}

QString EditableItemManager::loadFile(QString fileName) {
	Q_UNUSED(fileName);

	return QString(); //by default remote file storing is not supported.
}

bool EditableItemManager::storeProjectFile(QString localFile) {
	Q_UNUSED(localFile);

	return false; //by default storing project files is not supported.
}

EditableItem* EditableItemManager::activeItem() const {
	return _activeItem;
}

LabelsTree* EditableItemManager::labelsTree() {

	if (!hasDataSource()) {
		return nullptr;
	}

	if (_labels == nullptr) {
		effectivelyLoadLabels();
	}

	return _labels;

}

void EditableItemManager::closeAll() {

	QList<QString> keys = _loadedItems.keys();

	for (QString const& ref : qAsConst(keys)) {

		Q_EMIT itemAboutToBeUnloaded(ref); //at that point the items can still be saved or other operations can be carried on by the watchers.

		_loadedItems.remove(ref);

		Q_EMIT itemUnloaded(ref);

	}

}

void EditableItemManager::setActiveItem(QString url) {

	if (_activeItem == nullptr || _activeItem->getFullRefUrl() != url) {

		EditableItem* potential = qobject_cast<EditableItem*>(loadItemByUrl(url));

		_activeItem = potential;
		Q_EMIT activeItemChanged();

	}

}

QModelIndex EditableItemManager::indexFromLeaf(treeStruct* leaf) const {

	int row = _itemsByTypes.value(leaf->_type_ref).indexOf(leaf);

	if (row >= 0) {
		return createIndex(row, 0, leaf);
	}

	return QModelIndex();
}

QModelIndex EditableItemManager::indexFromType(QString typeRef) const {

	int n = _itemsByTypes.keys().indexOf(typeRef);

	if (n < 0) {
		return QModelIndex();
	}

	return createIndex(n, 0);


}

void EditableItemManager::refreshItemDataDisplay(QString itemUrl) {

	if (_treeIndex.contains(itemUrl)) {

		treeStruct* leaf = _treeIndex.value(itemUrl);

		EditableItem* item = loadItem(itemUrl);

		leaf->_name = item->objectName() + ((item->getHasUnsavedChanged()) ? " *" : "");

		QModelIndex index = indexFromLeaf(leaf);

		Q_EMIT dataChanged(index, index, {Qt::DisplayRole});

	}

	Q_EMIT itemVisibleStateNeedRefresh(itemUrl);

}

void EditableItemManager::cleanTreeStruct() {

	beginResetModel();

	for (treeStruct* leaf : qAsConst(_treeIndex)) {
		delete leaf;
	}

	_treeIndex.clear();
	_itemsByTypes.clear();

	for (QString const& ref : _factoryManager->installedFactoriesKeys()) {
		_itemsByTypes.insert(ref, QVector<treeStruct*>());
	}

	endResetModel();

}

bool EditableItemManager::insertItem(EditableItem* item) {

	if (_treeIndex.contains(item->getRef())) {
		return false;
	}

	treeStruct* item_leaf = new treeStruct();

	item_leaf->_acceptChildrens = item->acceptChildrens();
	item_leaf->_name = item->objectName();
	item_leaf->_ref = item->getRef();
	item_leaf->_type_ref = item->getTypeId();

	if (!_itemsByTypes.contains(item_leaf->_type_ref)) {

		int s = _itemsByTypes.size();

		beginInsertRows(QModelIndex(), s, s);

		_itemsByTypes.insert(item_leaf->_type_ref, QVector<treeStruct*>());

		endInsertRows();
	}

	int s = _itemsByTypes.value(item_leaf->_type_ref).size();

	beginInsertRows(indexFromType(item_leaf->_type_ref), s, s);

	_itemsByTypes[item_leaf->_type_ref].push_back(item_leaf);
	_treeIndex.insert(item_leaf->_ref, item_leaf);

	_loadedItems.insert(item_leaf->_ref, {item_leaf, item});

	endInsertRows();

	return true;

}

const QString &EditableItemManager::refUrlSeparator() const
{
	return _refUrlSeparator;
}

void EditableItemManager::setRefUrlSeparator(const QString &newRefUrlSeparator)
{
	_refUrlSeparator = newRefUrlSeparator;
}

void EditableItemManager::setEditorManager(EditorFactoryManager *editorManager)
{
	_editorManager = editorManager;
}

QAbstractItemModel* EditableItemManager::getSubTreeFromItemType(QString typeRef, QObject* modelParent) {

	QModelIndex typeIndex = indexFromType(typeRef);

	if (typeIndex == QModelIndex()) {
		return nullptr;
	}

	ModelViewUtils::IndexRebasedProxyModel* model = new ModelViewUtils::IndexRebasedProxyModel(modelParent);

	model->setSourceModel(this, typeIndex);

	return model;

}

QHash<int, QByteArray> EditableItemManager::roleNames() const {

	QHash<int, QByteArray> ret = QAbstractItemModel::roleNames();

	ret.insert(ItemRefRole, "itemRef");
	ret.insert(ItemTypeRefRole, "itemTypeRef");

	return ret;

}

void EditableItemManager::setFactoryManager(EditableItemFactoryManager *factoryManager, bool takeOwnership)
{
	_factoryManager = factoryManager;

	if (takeOwnership) {
		_factoryManager->setParent(this);
	}
}

} // namespace Aline
