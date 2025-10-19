#include "projecttreedockwidget.h"
#include "ui_projecttreedockwidget.h"

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

#include <QMenu>
#include <QInputDialog>

#include "editableitemtypespecializedaction.h"

#include "control/app.h"
#include "control/editableitemactionsmanager.h"

#include "../model/editableitemfactory.h"
#include "editorfactorymanager.h"

namespace Aline {

ProjectTreeDockWidget::ProjectTreeDockWidget(MainWindow *parent) :
	QDockWidget(parent),
	_mw_parent(parent),
	_newItemMenu(nullptr),
	_currentProject(nullptr),
	ui(new Ui::ProjectTreeDockWidget)
{
	_internalModel = new QSortFilterProxyModel(this);

	ui->setupUi(this);

	ui->treeView->setModel(_internalModel);

	projectChanged(_mw_parent->currentProject());

	connect(_mw_parent, &MainWindow::currentProjectChanged,
			this, &ProjectTreeDockWidget::projectChanged);

	ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	connect(ui->treeView, &QTreeView::clicked,
			this, &ProjectTreeDockWidget::receiveClick);

	connect(ui->treeView, &QTreeView::doubleClicked,
			this, &ProjectTreeDockWidget::receiveDoubleClick);

	connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
			this, &ProjectTreeDockWidget::selectionChanged);

	connect(ui->pushButtonSuppr, &QPushButton::clicked,
			this, &ProjectTreeDockWidget::supprButtonClicked);

	ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->treeView->setDragDropMode(QAbstractItemView::DragDrop);
	ui->treeView->setDragEnabled(true);
	ui->treeView->setAcceptDrops(true);
	ui->treeView->setDropIndicatorShown(true);

	ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->treeView, &QTreeView::customContextMenuRequested,
			this, &ProjectTreeDockWidget::buildTreeContextMenu);

	ui->treeView->setHeaderHidden(true);
}

ProjectTreeDockWidget::~ProjectTreeDockWidget()
{
	delete ui;
}

void ProjectTreeDockWidget::selectionChanged() {

	QModelIndexList selection = ui->treeView->selectionModel()->selectedIndexes();

	if (selection.size() > 0 && !_mw_parent->isEditingAnItem()) {

		QString ref = ui->treeView->model()->data(selection.first(), EditableItemManager::ItemRefRole).toString();

		_mw_parent->currentProject()->setActiveItem(ref);
	}
}

void ProjectTreeDockWidget::onItemCreationTriggered(QString itemTypeRef) {

	Q_EMIT itemCreationTriggered(itemTypeRef, itemTypeRef);

}

void ProjectTreeDockWidget::projectChanged(Aline::EditableItemManager* project) {

	if(_newItemFactoryWatcher) {
		disconnect(_newItemFactoryWatcher);
	}

	if (_itemCreationTrigger) {
		disconnect(_itemCreationTrigger);
	}

	if (_itemSuppresionTrigger) {
		disconnect(_itemSuppresionTrigger);
	}

	if (project == nullptr) {
        setEnabled(false);
	} else {
		setEnabled(true);
	}

	reselectProject(project);
	rebuildMenu(project);

	_currentProject = project;

	if (project != nullptr) {

		_newItemFactoryWatcher = connect(project->factoryManager(), &Aline::EditableItemFactoryManager::rowsInserted,
										 this, &ProjectTreeDockWidget::rebuildMenuWithoutProject);

		_itemCreationTrigger = connect(this, &ProjectTreeDockWidget::itemCreationTriggered,
									   project, static_cast<bool(Aline::EditableItemManager::*)(QString, QString)>(&Aline::EditableItemManager::createItem));

		_itemSuppresionTrigger = connect(this, &ProjectTreeDockWidget::itemSuppressionTriggered,
										 project, &Aline::EditableItemManager::clearItems);
	}
}

void ProjectTreeDockWidget::reselectProject(Aline::EditableItemManager *project) {

	_internalModel->setSourceModel(project);

}

void ProjectTreeDockWidget::rebuildMenuWithoutProject() {
	rebuildMenu(_mw_parent->currentProject());
}

void ProjectTreeDockWidget::rebuildMenu(Aline::EditableItemManager* project) {

	if (_newItemMenu != nullptr) {
		_newItemMenu->deleteLater();
	}

	if (project == nullptr) {
		return;
	}

	_newItemMenu = new QMenu(this);

	Aline::EditableItemFactoryManager* f = project->factoryManager();

	for (int i = 0; i < f->rowCount(); i++) {

		QString itemType = f->data(f->index(i), Aline::EditableItemFactoryManager::ItemRefRole).toString();
		bool isSingleton = f->data(f->index(i), Aline::EditableItemFactoryManager::IsSingletonRole).toBool();

		if (isSingleton) {
			continue; //cannot create singletons
		}

		EditableItemTypeSpecializedAction* action = new EditableItemTypeSpecializedAction(itemType,
																						  QIcon(f->itemIconUrl(itemType)),
																						  f->data(f->index(i),
																								  Qt::DisplayRole).toString(),
																						  _newItemMenu);

		connect(action, &EditableItemTypeSpecializedAction::triggered,
				this, &ProjectTreeDockWidget::onItemCreationTriggered);

		_newItemMenu->addAction(action);

	}

	ui->toolButton_new->setMenu(_newItemMenu);
	ui->toolButton_new->setPopupMode(QToolButton::MenuButtonPopup);

}


void ProjectTreeDockWidget::receiveClick(const QModelIndex &index) {

	QString ref = _internalModel->data(index, EditableItemManager::ItemRefRole).toString();

	Q_EMIT itemClicked(ref);

}

void ProjectTreeDockWidget::receiveDoubleClick(const QModelIndex &index) {

	QString ref = _internalModel->data(index, EditableItemManager::ItemRefRole).toString();

	Q_EMIT itemDoubleClicked(ref);

}

void ProjectTreeDockWidget::supprButtonClicked() {

	//TODO: options to have a warning about deletion.

	QModelIndexList smod = ui->treeView->selectionModel()->selectedIndexes();
	QStringList selectedItems;

	for (QModelIndex index : qAsConst(smod)) {
		selectedItems << _internalModel->data(index, Aline::EditableItemFactoryManager::ItemRefRole).toString();
	}

	Q_EMIT itemSuppressionTriggered(selectedItems);

}

void ProjectTreeDockWidget::buildTreeContextMenu(QPoint const& pos) {

	if (_currentProject == nullptr) {
		return;
	}

	QModelIndex index = ui->treeView->indexAt(pos);

	if (index.isValid()) {

		QMenu menu(this); //needs to make the current widget parent, so that the action manager can find the main windows

		QVariant data  = index.data(EditableItemManager::ItemTypeRefRole);
		QString itemTypeRef = data.toString();

		if (_internalModel->flags(index) & Qt::ItemIsEditable) {

			QAction* renameAction = menu.addAction(tr("Rename"));

			connect(renameAction, &QAction::triggered, this, [index, this] () {ui->treeView->edit(index);});

			menu.addSeparator();

		}

		App* app = App::getAppInstance();
		EditableItemActionsManager* actionsManager = app->getActionsManagerForType(itemTypeRef);

		QVariant itemRef = index.data(EditableItemManager::ItemRefRole);

		QList<QAction*> actions;

		if (itemRef.isValid()) { //we have an item
			QString ref = itemRef.toString();
			EditableItem* item = _currentProject->loadItem(ref);

			if (item != nullptr) {

				actions = actionsManager->factorizeItemContextActions(&menu, item);

			}
		} else {
			actions = actionsManager->factorizeClassContextActions(&menu, _currentProject, itemTypeRef);
		}

		for (QAction* action : qAsConst(actions)) {
			menu.addAction(action);
		}

		menu.addSeparator();

		Aline::EditableItemFactoryManager* f = _mw_parent->currentProject()->factoryManager();

		QString typeName = f->itemTypeName(itemTypeRef);

		EditableItemTypeSpecializedAction* action = new EditableItemTypeSpecializedAction(itemTypeRef,
																						  QIcon(f->itemIconUrl(itemTypeRef)),
																						  QString("Add a %1").arg(typeName),
																						  &menu);

		connect(action, &EditableItemTypeSpecializedAction::triggered,
				this, [this] (QString type_id) {

					Q_EMIT itemCreationTriggered(type_id, type_id);

		});

		menu.addAction(action);

		menu.exec(ui->treeView->viewport()->mapToGlobal(pos));

	}

}

} //namespace Sabrina
