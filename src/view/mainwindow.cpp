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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editor.h"
#include "editorfactorymanager.h"
#include "editableitemeditor.h"
#include "model/editableitem.h"
#include "model/editableitemmanager.h"
#include "model/editableitemfactory.h"
#include "model/editableitemmanagerfactory.h"

#include "control/app.h"

#include <QTabWidget>
#include <QDockWidget>
#include <QMessageBox>
#include <QToolBar>

namespace Aline {

const char* MainWindow::MENU_FILE_NAME = "file_menu";
const char* MainWindow::MENU_DISPLAY_NAME = "display_menu";

MainWindow* MainWindow::findWidgetMainWindow(QWidget* widget) {

	if (widget == nullptr) {
		return nullptr;
	}

	QWidget* current = widget;
	MainWindow* mw = qobject_cast<MainWindow*>(current);

	while(mw == nullptr) {

		if (current == nullptr) {
			return nullptr;
		}

		current = current->parentWidget();
		mw = qobject_cast<MainWindow*>(current);
	}

	return mw;

}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_currentProject(nullptr),
	_currentItemUrl(""),
	_editorFactoryManager(nullptr),
	_editorContextToolBar(nullptr),
	ui(new Ui::MainWindow),
	_currentEditorResetActionsConnection()
{
	ui->setupUi(this);

	//menu

	QMenu* fileMenu = menuBar()->addMenu(tr("fichier"));
	fileMenu->setObjectName(MENU_FILE_NAME);

	QMenu* viewMenu = menuBar()->addMenu(tr("affichage"));
	viewMenu->setObjectName(MENU_DISPLAY_NAME);

	_submenuDock = viewMenu->addMenu(tr("docks"));
	_submenuCreateProject = fileMenu->addMenu(tr("create project"));
	_submenuOpenProject = fileMenu->addMenu(tr("open project"));

	connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
			this, static_cast<void (MainWindow::*)(int)>(&MainWindow::closeEditor));

	connect(ui->tabWidget, &QTabWidget::currentChanged,
			this, &MainWindow::onCurrentEditorChanged);

	ui->tabWidget->setMovable(true);

	_editableItemManagerFactory = new Aline::EditableItemManagerFactory(this);
}

MainWindow::~MainWindow()
{
	setCurrentProject(nullptr);
	delete ui;
}

int MainWindow::nOpenedEditors() const {
	return ui->tabWidget->count();
}

Editor* MainWindow::editorAt(int index) {

	QWidget* widget = ui->tabWidget->widget(index);

	Editor* editor = qobject_cast<Editor*>(widget);

	return editor;

}

Editor* MainWindow::findEditorByName(QString const& name) {

	for (int i = 0; i < nOpenedEditors(); i++) {
		Editor* e = editorAt(i);

		if (e->objectName() == name) {
			return e;
		}
	}

	return nullptr;

}
QVector<Editor*> MainWindow::findAllEditorsOfType(QString const& type) {
	QVector<Editor*> eds = {};

	for (int i = 0; i < nOpenedEditors(); i++) {
		Editor* e = editorAt(i);

		if (e->getTypeId() == type) {
			eds.push_back(e);
		}
	}

	return eds;
}

Editor* MainWindow::currentEditor() {
	return qobject_cast<Editor*>(ui->tabWidget->currentWidget());
}


void MainWindow::switchToEditor(int index) {

	if (index >= 0 and index < ui->tabWidget->count()) {
		ui->tabWidget->setCurrentIndex(index);
	}
}

void MainWindow::switchToEditor(Editor* editor) {
	int index = ui->tabWidget->indexOf(editor);

	if (index >= 0) {
		ui->tabWidget->setCurrentIndex(index);
	}
}


EditableItemManager *MainWindow::currentProject() const
{
	return _currentProject;
}

void MainWindow::setCurrentProject(EditableItemManager *currentProject)
{

	EditorPersistentStateSaveInterface* editorsPersistenceInterface = nullptr;

	App* app = App::getAppInstance();

	if (app != nullptr) {
		QObject* tmp = app->getSpecialInterface(EditorPersistentStateSaveInterface::EditorPersistentStateSaveInterfaceCode);
		editorsPersistenceInterface = qobject_cast<EditorPersistentStateSaveInterface*>(tmp);
	}

	if (currentProject != _currentProject) {

		setCurrentItem("");

		if (_currentProject != nullptr) {

			if (editorsPersistenceInterface != nullptr) {
				editorsPersistenceInterface->saveEditorStates(this);
			}

			//close all editors before closing the previous project.
			for (int i = nOpenedEditors()-1; i >= 0; i--) {
				closeEditor(i);
			}

			_currentItemUrl = "";

			disconnect(this, &Aline::MainWindow::editedItemChanged,
					   _currentProject, &EditableItemManager::setActiveItem);

			Q_EMIT currentProjectUnloaded(_currentProject);

			_currentProject->setEditorManager(nullptr);
			_currentProject->deleteLater();
		}

		_currentProject = currentProject;

		if (_currentProject != nullptr) {
			connect(this, &Aline::MainWindow::editedItemChanged,
				_currentProject, &EditableItemManager::setActiveItem);

			_currentProject->setEditorManager(_editorFactoryManager);

			if (editorsPersistenceInterface != nullptr) {
				editorsPersistenceInterface->restoreEditorStates(this);
			}
		}

		Q_EMIT currentProjectChanged(_currentProject);
	}
}

QString MainWindow::currentItemRef() const {
	return _currentItemUrl;
}

QMenu* MainWindow::findMenuByName(QString const& name, bool createIfNotExist) {

	QMenu* menu = menuBar()->findChild<QMenu*>(name, Qt::FindDirectChildrenOnly);

	if (menu != nullptr) {
		return menu;
	}

	if (createIfNotExist) {
		return menuBar()->addMenu(name);
	}

	return nullptr;

}

QMenu* MainWindow::findMenuByName(const char* name, bool createIfNotExist) {
	QString nameStr(name);
	return findMenuByName(nameStr, createIfNotExist);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget) {

	_submenuDock->addAction(dockwidget->toggleViewAction());

	QMainWindow::addDockWidget(area, dockwidget);

}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget * dockwidget, Qt::Orientation orientation) {

	_submenuDock->addAction(dockwidget->toggleViewAction());

	QMainWindow::addDockWidget(area, dockwidget, orientation);

}

bool MainWindow::isEditingAnItem() const {

	if (ui->tabWidget->count() == 0) {
		return false;
	}

	QWidget* w = ui->tabWidget->currentWidget();

	Aline::EditableItemEditor* eie = qobject_cast<Aline::EditableItemEditor*>(w);

	if (eie != nullptr) {
		return true;
	}

	return false;
}

void MainWindow::registerCreateProjectFunction(QString const& fName,
											   QString const& projectType,
											   QString const& longDescr,
											   std::function<EditableItemManager*(QObject*, MainWindow*)> const& func) {

	registerProjectFunction(fName, projectType, longDescr, func, _submenuCreateProject);

}

void MainWindow::registerOpenProjectFunction(QString const& fName,
											 QString const& projectType,
											 QString const& longDescr,
											 std::function<EditableItemManager*(QObject*, MainWindow*)> const& func) {

	registerProjectFunction(fName, projectType, longDescr, func, _submenuOpenProject);

}

void MainWindow::registerProjectFunction(QString const& fName,
										 QString const& projectType,
										 QString const& longDescr,
										 std::function<EditableItemManager*(QObject*, MainWindow*)> const& func,
										 QMenu* target) {

	EditableItemManagerFactory::funcDescr d;
	d._projectDescr = projectType;
	d._longDescr = longDescr;

	_editableItemManagerFactory->registerGuiAwareFunction(fName, d, func);

	QAction* openProjectAction = new QAction(projectType, this);
	openProjectAction->setToolTip(longDescr);

	connect(openProjectAction, &QAction::triggered, this, [fName, this] () {
		setWindowProjectFromFunc(fName);
	});

	target->addAction(openProjectAction);

}

void MainWindow::setEditorFactoryManager(EditorFactoryManager *editorFactoryManager)
{
	_editorFactoryManager = editorFactoryManager;
}
EditorFactoryManager* MainWindow::editorManager() {
	return _editorFactoryManager;
}

bool MainWindow::addEditor(Editor* editor) {

	if (editor == nullptr) {
		return false;
	}

	for (int i = 0; i < nOpenedEditors(); i++) {
		Editor* oeiEditor = editorAt(i);

		if (oeiEditor == nullptr) {
			continue;
		}

		if (oeiEditor->getTypeId() == editor->getTypeId()) {
			if (oeiEditor->getEditorNoDuplicateClue() == editor->getEditorNoDuplicateClue()) {
				switchToEditor(i);
				return false;
			}
		}
	}

	ui->tabWidget->addTab(editor, editor->title());

	connect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);

	switchToEditor(editor);
	return true;
}


void MainWindow::closeEditor(int index) {

	QWidget* widg = ui->tabWidget->widget(index);

	Editor* editor = qobject_cast<Editor*>(widg);

	if (editor) {
		disconnect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);
	}

	Q_EMIT editorAboutToBeRemoved(editor);

	ui->tabWidget->removeTab(index);
}

void MainWindow::closeEditor(Editor* editor) {

	disconnect(editor, &Editor::titleChanged, this, &MainWindow::updateTitle);

	Q_EMIT editorAboutToBeRemoved(editor);

	ui->tabWidget->removeTab(ui->tabWidget->indexOf(editor));

}

void MainWindow::saveCurrentEditor() {

	Editor* editor = qobject_cast<Editor*>(ui->tabWidget->currentWidget());

	if (editor != nullptr) {
		editor->saveAction();
	}

}

void MainWindow::setCurrentItem(QString const& itemUrl) {

	if (_currentProject == nullptr) {
		return;
	}

	QStringList fullUrl = _currentProject->splitUrl(itemUrl);

	if (fullUrl.size() < 1 and !itemUrl.isEmpty()) {
		return;
	}

	QString parentItemUrl;

	if (itemUrl.isEmpty()) {
		parentItemUrl = itemUrl;
	} else {
		parentItemUrl = fullUrl.first();
	}

	if (!_currentProject->containItem(parentItemUrl) and !parentItemUrl.isEmpty()) { //works only for top level item, but this is expected.
		return;
	}

	if (_currentItemUrl != parentItemUrl) {
		_currentItemUrl = parentItemUrl;
		Q_EMIT currentItemChanged(_currentItemUrl);

		ui->statusbar->showMessage(tr("Current item: %1").arg(_currentItemUrl), 3000);
	}

}

void MainWindow::editItem(QString const& itemUrl) {

	EditableItem* item = _currentProject->loadItemByUrl(itemUrl);

	if (item == nullptr) {
		return; //not an item
	}

	if (_editorFactoryManager->hasSpecialEditFunctionInstalled(item->editAsTypeId())) {
		_editorFactoryManager->specialEditFunction(item->editAsTypeId())(this, item);
		return;
	}

	Editor* editor = nullptr;

	if (_editorFactoryManager != nullptr) {
		editor = _editorFactoryManager->createItemForEditableItem(item, this);
	}

	if (editor != nullptr) {
		addEditor(editor);
	}

}

void MainWindow::editItemWithEditorType(QString const& editorTypeRef, QString const& itemUrl) {

	EditableItem* item = _currentProject->loadItemByUrl(itemUrl);

	if (item == nullptr) {
		return; //not an item
	}

	Editor* editor = nullptr;

	if (_editorFactoryManager != nullptr) {
		editor = _editorFactoryManager->createItem(editorTypeRef, this);
	}

	if (editor != nullptr) {

		editor->setEditedItem(item);
		addEditor(editor);
	}

}

void MainWindow::saveAll() {

	if (_currentProject != nullptr) {
		_currentProject->saveAll();
	}

}

void MainWindow::setWindowProjectFromFunc(QString const& fName) {

	//TODO: look if it is possible to open a new main windows.
	EditableItemManager* manager = _editableItemManagerFactory->createManagerWithRegistredFunctionAndGuiContext(fName, this, this);

	if (manager != nullptr) {
		Q_EMIT projectLoaded(manager);
		setCurrentProject(manager);
	}
}


void MainWindow::updateTitle(Editor* editor, QString newTitle) {

	int index = ui->tabWidget->indexOf(editor);

	ui->tabWidget->setTabText(index, newTitle);

}

void MainWindow::onCurrentEditorChanged() {

	disconnect(_currentEditorResetActionsConnection);
	_currentEditorResetActionsConnection = QMetaObject::Connection();

	Editor* current = currentEditor();

	if (current == nullptr) {
		return;
	}

	_currentEditorResetActionsConnection = connect(current, &Editor::contextActionListChanged,
												   this, &MainWindow::reconfigureCurrentEditorActions);

	Aline::EditableItemEditor* eie = qobject_cast<Aline::EditableItemEditor*>(current);

	if (eie != nullptr) {

		setCurrentItem(eie->getEditedItemUrl());
		Q_EMIT editedItemChanged(eie->getEditedItemUrl());
	}

	reconfigureCurrentEditorActions();

    Q_EMIT currentEditorChanged();

}

void MainWindow::reconfigureCurrentEditorActions() {

	Editor* current = currentEditor();

	if (current == nullptr) {
		return;
	}

	QList<QAction*> contextActions = current->getContextActions();

	if (!contextActions.empty()) {

		if (_editorContextToolBar == nullptr) {
			_editorContextToolBar = new QToolBar(this);
			_editorContextToolBar->setObjectName("editor_context_toolbar");
			addToolBar(Qt::TopToolBarArea, _editorContextToolBar);
		}

		QList<QAction *> actions = _editorContextToolBar->actions();

		for (QAction* action : qAsConst(actions)) {
			action->deleteLater();
		}

		_editorContextToolBar->clear();

		for (QAction* action : qAsConst(contextActions)) {
			action->setParent(_editorContextToolBar);
			_editorContextToolBar->addAction(action);
		}

		_editorContextToolBar->show();

	} else {
		if (_editorContextToolBar != nullptr) {

			QList<QAction *> actions = _editorContextToolBar->actions();

			for (QAction* action : qAsConst(actions)) {
				action->deleteLater();
			}

			_editorContextToolBar->clear();

			_editorContextToolBar->hide();
		}
	}

}

QString MainWindow::defaultProjectOpener() const
{
	return _defaultProjectOpener;
}

void MainWindow::setDefaultProjectOpener(const QString &defaultProjectOpener)
{
	_defaultProjectOpener = defaultProjectOpener;
}

QString MainWindow::defaultProjectCreator() const
{
	return _defaultProjectCreator;
}

void MainWindow::setDefaultProjectCreator(const QString &defaultProjectCreator)
{
	_defaultProjectCreator = defaultProjectCreator;
}

void MainWindow::closeEvent(QCloseEvent *event) {

	Aline::EditableItemManager* project = currentProject();

	if (project == nullptr) {
		return QMainWindow::closeEvent(event);
	}

	if (project->hasUnsavedChanges()) {
		QMessageBox::StandardButton answer = QMessageBox::question(this, tr("Project has unsaved changes"), tr("Would you like to save the changes to the current project?"));

		if (answer == QMessageBox::StandardButton::Yes) {
			saveAll();
		}
	}

	return QMainWindow::closeEvent(event);

}

} // namespace Aline
