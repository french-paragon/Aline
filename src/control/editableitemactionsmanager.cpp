/*This file is part of the project Aline

Copyright (C) 2023 Paragon <french.paragon@gmail.com>

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

#include "editableitemactionsmanager.h"

#include "model/labels/labelstree.h"
#include "view/editorfactorymanager.h"
#include "view/mainwindow.h"
#include "view/editableitemtypespecializedaction.h"

#include "model/editableitemmanagerfactory.h"
#include "model/editableitemfactory.h"
#include "model/editableitemmanager.h"
#include "model/editableitem.h"

#include <QAction>

namespace Aline {

EditableItemActionsManager::EditableItemActionsManager(QObject* parent) :
	QObject(parent)
{

}

QList<QAction*> EditableItemActionsManager::factorizeClassContextActions(QObject* parent, EditableItemManager* manager, QString editableItemType) const {

	EditableItemFactoryManager* factoryManager = manager->factoryManager();

	if (factoryManager == nullptr) {
		return {};
	}

	if (!factoryManager->hasFactoryInstalled(editableItemType)) {
		return {};
	}

	QString typeName = factoryManager->itemTypeName(editableItemType);

	EditableItemTypeSpecializedAction* actionCreate = new EditableItemTypeSpecializedAction(editableItemType,
																  QIcon(factoryManager->itemIconUrl(editableItemType)),
																  QString("%1").arg(typeName),
																  parent);

	connect(actionCreate, &EditableItemTypeSpecializedAction::triggered,
			this, [manager, this] (QString type_id) {
		manager->createItem(type_id, type_id);

	});

	return {qobject_cast<QAction*>(actionCreate)};

}

QList<QAction*> EditableItemActionsManager::factorizeItemContextActions(QObject* parent, EditableItem* item) const {

	MainWindow* mw = MainWindow::findWidgetMainWindow(qobject_cast<QWidget*>(parent));
	EditableItemManager* manager = item->getManager();

	if (manager == nullptr) {
		return {};
	}

	if (item->getParentItem() != nullptr) {
		return {}; //no actions for non root items;
	}

	QAction* actionRemove = new QAction(tr("Remove"), parent);

	connect(actionRemove, &QAction::triggered, this, [manager, item, this] () {
		manager->clearItem(item->getRef());
	});

	if (mw == nullptr) { //no main windows available
		return {actionRemove};
	}

	Aline::EditorFactoryManager* editorManager = mw->editorManager();

	if (editorManager->hasFactoryInstalledForItem(item->getTypeId())) {
		QAction* editAction = new QAction(tr("Edit"), parent);

		connect(editAction, &QAction::triggered, this, [this, item, mw] () {
			mw->editItem(item->getFullRefUrl());
		});

		return {editAction, actionRemove};
	}

	return {actionRemove};

}
QList<QAction*> EditableItemActionsManager::factorizeMultiItemsContextActions(QObject* parent, EditableItemManager* manager, QModelIndexList const& projectIndices) const {

	if (manager == nullptr) {
		return {};
	}

	QAction* actionRemove = new QAction(tr("Remove"), parent);

	QStringList itemsRefs;
	itemsRefs.reserve(projectIndices.size());

	for (QModelIndex idx : projectIndices) {
		itemsRefs.push_back(idx.data(EditableItemManager::ItemRefRole).toString());
	}

	connect(actionRemove, &QAction::triggered, this, [manager, itemsRefs, this] () {
		for (QString const& ref : itemsRefs) {
			manager->clearItem(ref);
		}
	});

	return {actionRemove};

}

} // namespace Aline
