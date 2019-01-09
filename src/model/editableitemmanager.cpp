#include "editableitemmanager.h"

#include "editableitem.h"
#include "editableitemfactory.h"

#include "view/editorfactorymanager.h"

#include "model/labels/labelstree.h"

#include <QSet>
#include <QIcon>
#include <QMimeData>

namespace Aline {

const QChar EditableItemManager::RefSeparator = QChar('/');
const QString EditableItemManager::RefRoot = QString("root");

const QString EditableItemManager::RefMimeType = "text/editableitemref";

EditableItemManager::EditableItemManager(QObject *parent) :
	QAbstractItemModel(parent),
	_factoryManager(&EditableItemFactoryManager::GlobalEditableItemFactoryManager),
	_labels(nullptr),
	_activeItem(nullptr),
	_editorManager(nullptr)
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

		if (row >= _itemsByTypes.keys().size()) {
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
		return _itemsByTypes.keys().size();
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
			emit dataChanged(index, index, {Qt::DisplayRole});
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

		QString itemTypeRef = index.data(ItemTypeRefRole).toString();

		bool hasEditor = false;

		if (_editorManager != nullptr) {
			hasEditor = _editorManager->hasFactoryInstalledForItem(itemTypeRef);
		} else {
			hasEditor = Aline::EditorFactoryManager::GlobalEditorFactoryManager.hasFactoryInstalledForItem(itemTypeRef);
		}

		if (!hasEditor) {
			f |= Qt::ItemIsEditable;
		}

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

	connect(item, &EditableItem::visibleStateChanged, this, &EditableItemManager::itemVisibleStateChanged);

	treeStruct* node = _treeIndex.value(ref, nullptr);

	if (node == nullptr) {
		insertItem(item);
	} else {
		_loadedItems.insert(item->getRef(), {node, item});
	}

	return item;

}

QStringList EditableItemManager::loadedItems() const{
	return _loadedItems.keys();
}
void EditableItemManager::forceUnloadItem(QString ref) {

	if (!isItemLoaded(ref)) {
		return; //no need to unload a not present item.
	}

	emit itemAboutToBeUnloaded(ref);

	EditableItem* item = _loadedItems.value(ref)._item;
	_loadedItems.remove(ref);
	delete item; //effectively delete item.

	emit itemUnloaded(ref);

}

bool EditableItemManager::isItemLoaded(QString const& ref) const {
	return _loadedItems.contains(ref);
}

bool EditableItemManager::containItem(const QString & ref) const {
	return _treeIndex.keys().contains(ref);
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
			connect(item, &EditableItem::visibleStateChanged, this, &EditableItemManager::itemVisibleStateChanged);

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



}

bool EditableItemManager::clearItems(QStringList itemRefs) {

	for (QString ref : itemRefs) {
		clearItem(ref);
	}

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

	for (QString ref : _loadedItems.keys()) {
		saveItem(ref);
	}

	saveLabels();
	saveStruct();

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

	for (QString ref : _loadedItems.keys()) {

		emit itemAboutToBeUnloaded(ref); //at that point the items can still be saved or other operations can be carried on by the watchers.

		_loadedItems.remove(ref);

		emit itemUnloaded(ref);

	}

}

void EditableItemManager::setActiveItem(QString ref) {

	if (_activeItem == nullptr || _activeItem->getRef() != ref) {

		EditableItem* potential = qobject_cast<EditableItem*>(loadItem(ref));

		_activeItem = potential;
		emit activeItemChanged();

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

void EditableItemManager::itemVisibleStateChanged(QString ref) {

	if (_treeIndex.contains(ref)) {

		treeStruct* leaf = _treeIndex.value(ref);

		EditableItem* item = loadItem(ref);

		leaf->_name = item->objectName() + ((item->getHasUnsavedChanged()) ? " *" : "");

		QModelIndex index = indexFromLeaf(leaf);

		emit dataChanged(index, index, {Qt::DisplayRole});

	}

}

void EditableItemManager::cleanTreeStruct() {

	beginResetModel();

	for (treeStruct* leaf : _treeIndex.values()) {
		delete leaf;
	}

	_treeIndex.clear();
	_itemsByTypes.clear();

	for (QString ref : _factoryManager->installedFactoriesKeys()) {
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

		int s = _itemsByTypes.keys().size();

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

void EditableItemManager::setEditorManager(EditorFactoryManager *editorManager)
{
	_editorManager = editorManager;
}

void EditableItemManager::setFactoryManager(EditableItemFactoryManager *factoryManager, bool takeOwnership)
{
	_factoryManager = factoryManager;

	if (takeOwnership) {
		_factoryManager->setParent(this);
	}
}

} // namespace Aline
