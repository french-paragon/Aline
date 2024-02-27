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

#include "editableitemfactory.h"

#include "editableitem.h"

namespace Aline {

EditableItemFactory::EditableItemFactory(QObject *parent) :
	QObject(parent),
	_typeId(""),
	_typeName(""),
	_typeIconUrl("")
{

}

QString EditableItemFactory::getItemTypeId() const {

	if (_typeId == "") {
		//get the type id.
		EditableItem* temp = createItem("", nullptr);
		_typeId = temp->getTypeId();

		delete temp;
	}

	return _typeId;
}

QString EditableItemFactory::getItemTypeName() const {

	if (_typeName == "") {
		//get the type name.
		EditableItem* temp = createItem("", nullptr);
		_typeName = temp->getTypeName();

		delete temp;
	}

	return _typeName;

}

QString EditableItemFactory::getItemTypeIconUrl() const {

	if (_typeIconUrl == "") {

		//get the type name.
		EditableItem* temp = createItem("", nullptr);
		_typeIconUrl = temp->iconInternalUrl();

		delete temp;

	}

	return _typeIconUrl;

}

bool EditableItemFactory::isSingleton() const {
	return false;
}

EditableSubItemFactory::EditableSubItemFactory(QObject *parent) :
	QObject(parent),
	_typeId(""),
	_typeName(""),
	_typeIconUrl("")
{

}

QString EditableSubItemFactory::getItemTypeId() const {

	if (_typeId == "") {
		//get the type id.
		EditableItem* temp = createItem("", nullptr);
		_typeId = temp->getTypeId();

		delete temp;
	}

	return _typeId;
}

QString EditableSubItemFactory::getItemTypeName() const {

	if (_typeName == "") {
		//get the type name.
		EditableItem* temp = createItem("", nullptr);
		_typeName = temp->getTypeName();

		delete temp;
	}

	return _typeName;

}

QString EditableSubItemFactory::getItemTypeIconUrl() const {

	if (_typeIconUrl == "") {

		//get the type name.
		EditableItem* temp = createItem("", nullptr);
		_typeIconUrl = temp->iconInternalUrl();

		delete temp;

	}

	return _typeIconUrl;

}



EditableItemFactoryManager::EditableItemFactoryManager(QObject *parent) : QAbstractListModel(parent)
{

}

void EditableItemFactoryManager::installFactory(EditableItemFactory* factory, bool takeOwnership) {

	if (hasFactoryInstalled(factory->getItemTypeId())) {
		if (takeOwnership) {
			factory->deleteLater();
		}
		return;
	}

	beginInsertRows(QModelIndex(), _installedFactoriesKeys.size(), _installedFactoriesKeys.size());

	_installedItemFactories.insert(factory->getItemTypeId(), factory);
	_installedFactoriesKeys.push_back(factory->getItemTypeId());

	if (takeOwnership) {
		factory->setParent(this);
	}

	endInsertRows();
}

void EditableItemFactoryManager::installSubItemFactory(EditableSubItemFactory* factory, bool takeOwnership) {

	if (hasSubItemFactoryInstalled(factory->getItemTypeId())) {
		if (takeOwnership) {
			factory->deleteLater();
		}
		return;
	}

	_installedSubItemFactories.insert(factory->getItemTypeId(), factory);
	_installedSubFactoriesKeys.push_back(factory->getItemTypeId());

	if (takeOwnership) {
		factory->setParent(this);
	}

}

bool EditableItemFactoryManager::hasFactoryInstalled(QString type_id) const {

	return _installedItemFactories.contains(type_id);

}
bool EditableItemFactoryManager::hasSubItemFactoryInstalled(QString type_id) const {

	return _installedSubFactoriesKeys.contains(type_id);

}

EditableItem* EditableItemFactoryManager::createItem(QString type_id, QString item_id, EditableItemManager* parent) const {

	const EditableItemFactory* f = _installedItemFactories.value(type_id, nullptr);

	if (f != nullptr) {
		return f->createItem(item_id, parent);
	}

	return nullptr;

}

EditableItem* EditableItemFactoryManager::createSubItem(QString type_id, QString item_id, EditableItem* parent) const {

	const EditableSubItemFactory* f = _installedSubItemFactories.value(type_id, nullptr);

	if (f != nullptr) {
		return f->createItem(item_id, parent);
	}

	return nullptr;

}

int EditableItemFactoryManager::rowCount(const QModelIndex &parent) const {

	if (parent != QModelIndex()) {
		return 0;
	}

	return _installedFactoriesKeys.size();

}

QVariant EditableItemFactoryManager::data(const QModelIndex &index,
										  int role) const {

	int r = index.row();
	QString associatedKey = _installedFactoriesKeys.at(r);

	switch(role) {
	case Qt::DisplayRole:
		return _installedItemFactories.value(associatedKey)->getItemTypeName();
	case Qt::ToolTipRole:
	case ItemRefRole:
		return associatedKey;
	case IsSingletonRole:
		return _installedItemFactories.value(associatedKey)->isSingleton();
	default:
		break;
	}

	return QVariant();

}

QString EditableItemFactoryManager::itemIconUrl(QString type_ref) {

	if (hasFactoryInstalled(type_ref)) {
		return _installedItemFactories.value(type_ref)->getItemTypeIconUrl();
	}

	if (hasSubItemFactoryInstalled(type_ref)) {
		return _installedSubItemFactories.value(type_ref)->getItemTypeIconUrl();
	}

	return "";
}

QString EditableItemFactoryManager::itemTypeName(QString type_ref) {

	if (hasFactoryInstalled(type_ref)) {
		return _installedItemFactories.value(type_ref)->getItemTypeName();
	}

	if (hasSubItemFactoryInstalled(type_ref)) {
		return _installedSubItemFactories.value(type_ref)->getItemTypeName();
	}

	return "";
}
bool EditableItemFactoryManager::itemTypeIsSingleton(QString type_ref) {

	if (hasFactoryInstalled(type_ref)) {
		return _installedItemFactories.value(type_ref)->isSingleton();
	}

	return false;
}

const QVector<QString> &EditableItemFactoryManager::installedFactoriesKeys() const
{
    return _installedFactoriesKeys;
}

} // namespace Cathia
