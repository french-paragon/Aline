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

#include "editorfactorymanager.h"

#include "editor.h"
#include "editableitemeditor.h"
#include "editorfactory.h"
#include "model/editableitem.h"
#include "qmleditorfactory.h"

namespace Aline {

EditorFactoryManager EditorFactoryManager::GlobalEditorFactoryManager;

EditorFactoryManager::EditorFactoryManager(QObject *parent) :
	QAbstractListModel(parent)
{

}

void EditorFactoryManager::installFactory(EditorFactory* factory, bool takeOwnership) {

	if (hasFactoryInstalled(factory->getItemTypeId())) {
		if (takeOwnership) {
			factory->deleteLater();
		}
		return;
	}

	beginInsertRows(QModelIndex(), _installedFactoriesKeys.size(), _installedFactoriesKeys.size());

	_installedFactories.insert(factory->getItemTypeId(), factory);
	_installedFactoriesKeys.push_back(factory->getItemTypeId());

	endInsertRows();

	for (QString ref : factory->editableTypes()) {

		_editableTypes.insert(ref, factory->getItemTypeId()); //insert the editable types.

	}

}

void EditorFactoryManager::installQmlEditorFactory(QString const& shadowTypeEditor,
												   QString const& shadowNameEditor,
												   QString const& qmlSource,
												   QStringList const& editableTypes) {

	if (!hasFactoryInstalled(shadowTypeEditor)) {

		QmlEditorFactory* factory = new QmlEditorFactory(shadowTypeEditor, shadowNameEditor, qmlSource, editableTypes, this);

		installFactory(factory);

	}

}

bool EditorFactoryManager::hasFactoryInstalled(QString type_id) const {

	return _installedFactories.contains(type_id);

}

bool EditorFactoryManager::hasFactoryInstalledForItem(QString type_id) const {

	QString factoryRef = _editableTypes.value(type_id, "");

	return hasFactoryInstalled(factoryRef);

}

Editor* EditorFactoryManager::createItem(QString type_id, QWidget *parent) const {

	const EditorFactory* f = _installedFactories.value(type_id, nullptr);

	if (f != nullptr) {
		return f->createItem(parent);
	}

	return nullptr;
}

Editor* EditorFactoryManager::createItemForEditableItem(EditableItem* item, QWidget* parent) const {

	QString factoryRef = _editableTypes.value(item->getTypeId(), "");

	if (factoryRef != "") {

		Editor* editor = createItem(factoryRef, parent);

		if (editor == nullptr) {
			return nullptr;
		}

		editor->setEditedItem(item);

		return editor;

	}

	return nullptr;

}

int EditorFactoryManager::rowCount(const QModelIndex &parent) const {

	if (parent != QModelIndex()) {
		return 0;
	}

	return _installedFactoriesKeys.size();

}
QVariant EditorFactoryManager::data(const QModelIndex &index, int role) const {

	int r = index.row();
	QString associatedKey = _installedFactoriesKeys.at(r);

	switch(role) {
	case Qt::DisplayRole:
		return _installedFactories.value(associatedKey)->getItemTypeName();
	case Qt::ToolTipRole:
	case ItemRefRole:
		return associatedKey;
	default:
		break;
	}

	return QVariant();

}

} // namespace Aline
